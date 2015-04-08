
import array
import atexit
import collections
import fcntl
import os
import pty
import select
import shutil
import signal
import tempfile
import termios
import time

import pyte
from . import exceptions
from . import keys

coord = collections.namedtuple('coord', ['X', 'Y'])

class JoeController(object):
    """Controller class for JOE program.  Manages child process and sends/receives interaction.  Nothing specific to JOE here, really"""
    def __init__(self, joeexe, joeargs, joeenv, startup, pid, fd):
        self.joeexe = joeexe
        self.joeargs = joeargs
        self.joeenv = joeenv
        self.pid = pid
        self.fd = fd
        self.exited = None
        
        self.term = pyte.Screen(startup.columns, startup.lines)
        self.stream = pyte.ByteStream()
        self.stream.attach(self.term)
        
        self.timeout = 1
    
    def expect(self, func):
        """Waits for data from the child process and runs func until it returns true or a timeout has elapsed"""
        
        if not self.checkProcess():
            raise exceptions.ProcessExitedException
        
        if func(): return True
        
        timeout = self.timeout
        deadline = time.time() + timeout
        
        while timeout > 0:
            rready, wready, xready = select.select([self.fd], [], [self.fd], timeout)
            timeout = deadline - time.time()
            
            if len(rready) == 0:
                # Timeout expired.
                return False
            
            res = self._readData()
            if res <= 0:
                raise exceptions.ProcessExitedException
            
            if func():
                return True
        
        # Timeout expired.
        return False
    
    def write(self, b):
        """Writes specified data (string or bytes) to the process"""
        self.flushin()
        if hasattr(b, 'encode'):
            return os.write(self.fd, b.encode('utf-8'))
        else:
            return os.write(self.fd, b)
    
    def writectl(self, ctlstr):
        """Writes specified control sequences to the process"""
        return self.write(keys.toctl(ctlstr))
    
    def flushin(self):
        """Reads all pending input and processes through the terminal emulator"""
        if self.fd is None: return
        while True:
            ready, jnk, jnk = select.select([self.fd], [], [], 0)
            if len(ready) > 0:
                if self._readData() <= 0:
                    return
            else:
                return
    
    def _readData(self):
        """Reads bytes from program and sends them to the terminal"""
        try:
            result = os.read(self.fd, 1024)
        except OSError:
            return -1
        self.stream.feed(result)
        return len(result)
    
    def checkProcess(self):
        """Checks whether the process is still running"""
        if self.exited is not None:
            return True
        
        result = os.waitpid(self.pid, os.WNOHANG)
        if result == (0, 0):
            return True
        else:
            self.exited = result
            return False
    
    def getExitCode(self):
        """Get exit code of program, if it has exited"""
        if self.exited is not None:
            result = self.exited
        else:
            result = os.waitpid(self.pid, os.WNOHANG)
        
        if result != (0, 0):
            self.exited = result
            return result[1] >> 8
        else:
            return None
    
    def kill(self, code=9):
        """Kills the child process with the specified signal"""
        os.kill(self.pid, code)
    
    def close(self):
        """Waits for process to exit and close down open handles"""
        self.wait()
        os.close(self.fd)
        self.fd = None
    
    def readLine(self, line, col, length):
        """Reads the text found at the specified screen location"""
        def getChar(y, x):
            if len(self.term.buffer) <= y: return ''
            if len(self.term.buffer[y]) <= x: return ''
            return self.term.buffer[y][x].data
        
        return ''.join(getChar(line, i + col) for i in range(length))
    
    def checkText(self, line, col, text):
        """Checks whether the text at the specified position matches the input"""
        
        return self.readLine(line, col, len(text)) == text
    
    def wait(self):
        """Waits for child process to exit or timeout to expire"""
        
        if self.exited is not None:
            return self.exited[1] >> 8
        
        def ontimeout(signum, frame):
            raise exceptions.TimeoutException()
        
        signal.signal(signal.SIGALRM, ontimeout)
        signal.alarm(int(self.timeout))
        try:
            result = os.waitpid(self.pid, 0)
        except exceptions.TimeoutException:
            return None
        signal.alarm(0)
        self.exited = result
        return result[1] >> 8
    
    def resize(self, width, height):
        """Resizes terminal"""
        self.flushin()
        self.term.resize(height, width)
        buf = array.array('h', [height, width, 0, 0])
        fcntl.ioctl(self.fd, termios.TIOCSWINSZ, buf)
    
    @property
    def screen(self):
        """Returns contents of screen as a string"""
        return '\n'.join([self.readLine(i, 0, self.term.columns) for i in range(self.term.lines)])
    
    @property
    def cursor(self):
        """Returns cursor position"""
        return coord(self.term.cursor.x, self.term.cursor.y)
    
    @property
    def size(self):
        """Returns size of terminal"""
        return coord(self.term.columns, self.term.lines)

class StartupArgs(object):
    """Startup arguments for JOE"""
    def __init__(self):
        self.args = ()
        self.env = {}
        self.lines = 25
        self.columns = 80

class TempFiles:
    """Temporary file manager.  Creates temp location and ensures it's deleted at exit"""
    def __init__(self):
        self.tmp = None
    
    def _cleanup(self):
        if os.path.exists(self.tmp):
            # Make sure we're not in it.
            os.chdir('/')
            shutil.rmtree(self.tmp)
    
    def getDir(self, path):
        """Get or create temporary directory 'path', under root temporary directory"""
        if self.tmp is None:
            self.tmp = tempfile.mkdtemp()
            atexit.register(self._cleanup)
        
        fullpath = os.path.join(self.tmp, path)
        if not os.path.exists(fullpath):
            os.makedirs(fullpath)
        return fullpath
    
    @property
    def homedir(self):
        """Temporary directory pointed to by HOME"""
        return self.getDir("home")
    
    @property
    def workdir(self):
        """Temporary directory where JOE will be started"""
        return self.getDir("work")

tmpfiles = TempFiles()

def startJoe(joeexe, args=None):
    """Starts JOE in a pty, returns a handle to controller"""
    if not joeexe.startswith('/'):
        joeexepath = os.path.join(os.getcwd(), joeexe)
    else:
        joeexepath = joeexe
    
    if args is None:
        args = StartupArgs()
    
    env = {}
    #env.update(os.environ)
    
    env['HOME'] = tmpfiles.homedir
    env['LINES'] = str(args.lines)
    env['COLUMNS'] = str(args.columns)
    env['TERM'] = 'ansi'
    env['LANG'] = 'en_US.UTF-8'
    env['SHELL'] = os.environ['SHELL']
    
    env.update(args.env)
    
    cmdline = ('joe',) + args.args
    
    pid, fd = pty.fork()
    if pid == 0:
        os.chdir(tmpfiles.workdir)
        os.execve(joeexepath, cmdline, env)
        os._exit(1)
    else:
        buf = array.array('h', [args.lines, args.columns, 0, 0])
        fcntl.ioctl(fd, termios.TIOCSWINSZ, buf)
        return JoeController(joeexepath, cmdline, env, args, pid, fd)
