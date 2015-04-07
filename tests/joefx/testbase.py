
import os
import unittest
import time

from . import controller
from . import fixtures

class JoeTestBase(unittest.TestCase):
    """Base class for joe tests.  Provides shortcuts for common editor interactions."""

    #
    # Test setup/teardown
    #
    
    def __init__(self, *a, **ka):
        super().__init__(*a, **ka)
        self.joe = None
        
        fixturesdir = os.path.join(os.path.dirname(__file__), "../fixtures")
        self.workdir = fixtures.FixtureDir(controller.tmpfiles.workdir, fixturesdir)
        self.homedir = fixtures.FixtureDir(controller.tmpfiles.homedir, fixturesdir)
        self.startup = controller.StartupArgs()
        self.timeout = 1 # Second
    
    def setUp(self):
        super().setUp()
    
    def tearDown(self):
        if self.joe is not None:
            ex = self.joe.getExitCode()
            if ex is None:
                self.joe.kill()
            elif ex < 0:
                self.assertTrue(False, "JOE exited with signal %d" % -ex)
        
        self.joe.flushin()
        self.joe.close()
        super(JoeTestBase, self).tearDown()
    
    def run(self, result=None):
        if result:
            orig_err_fail = result.errors + result.failures
        super(JoeTestBase, self).run(result)
        if result and result.errors + result.failures > orig_err_fail and self.joe is not None:
            print('\n----- screen at exit -----\n%s\n-----\n' % self.joe.screen)
    
    #
    # JOE startup
    #
    
    def startJoe(self, waitbanner=True):
        """Starts the editor"""
        self.workdir.setup()
        self.homedir.setup()
        self.joe = controller.startJoe("../joe/joe", self.startup)
        if waitbanner:
            self.assertTextAt("** Joe's Own Editor", x=0, y=self.joe.size.Y - 1)
    
    
    #
    # Assertions
    #
    
    def _posFromInput(self, x=None, y=None):
        return controller.coord(x if x is not None else self.joe.cursor.X, y if y is not None else self.joe.cursor.Y)
    
    def assertCursor(self, x=None, y=None):
        """Asserts cursor is at the specified position"""
        self.joe.expect(lambda: self._posFromInput(x, y) == self.joe.cursor)
        pos = self._posFromInput(x, y)
        self.assertEqual(pos.X, self.joe.cursor.X, "Cursor X pos")
        self.assertEqual(pos.Y, self.joe.cursor.Y, "Cursor Y pos")
    
    def assertTextAt(self, text, x=None, y=None):
        """Asserts text at specified coordinate matches the input. If either coordinate is omitted, use the cursor's coordinate"""
        def check():
            pos = self._posFromInput(x, y)
            return self.joe.checkText(pos.Y, pos.X, text)
        self.joe.expect(check)
        pos = self._posFromInput(x, y)
        s = self.joe.readLine(pos.Y, pos.X, len(text))
        self.assertEqual(s, text, "Text at line=%d col=%d" % (pos.Y, pos.X))
    
    def assertExited(self):
        """Asserts that the editor has exited"""
        result = self.joe.wait()
        self.assertIsNot(result, None, "Process was expected to have exited")
        self.assertEqual(result, 0, "Process exited with non-zero code")
    
    def assertFileContents(self, file, expected):
        """Asserts that file in workdir matches expected contents"""
        with open(os.path.join(controller.tmpfiles.workdir, file), 'r') as f:
            self.assertEqual(f.read(), expected)
    
    #
    # Editor functional helpers
    #
    
    def write(self, text):
        """Write specified text to editor"""
        return self.joe.write(text)
    
    def writeSlow(self, text):
        """Write specified text to editor with delay between keystrokes"""
        for c in text:
            self.joe.write(c)
            time.sleep(0.001)
            self.joe.flushin() # Short delay.
    
    def writectl(self, text):
        """Write control code to editor"""
        return self.joe.writectl(text)
    
    def rtn(self):
        """Press the ENTER key"""
        self.writectl("{enter}")
    
    def replace(self, findstr, replace, params=''):
        """Initiates find/replace with the specified options"""
        self.find(findstr, params + 'r')
        self.assertTextAt("Replace with", x=0)
        self.write(replace)
        self.rtn()
    
    def find(self, findstr, params=''):
        """Initiates find with the specified options"""
        self.cmd("ffirst")
        self.assertTextAt("Find (^C to abort)", x=0)
        self.findrow = self.joe.cursor.Y
        self.write(findstr)
        self.rtn()
        self.assertTextAt("(I)gnore (R)eplace", x=0)
        self.writectl(params + "{enter}")
    
    def answerReplace(self, responses):
        """Sends keystrokes as respones to ignore/replace prompt"""
        self.joe.flushin()
        cursor = None
        for response in responses:
            self.assertTrue(self.joe.expect(lambda: self.joe.cursor != cursor and self.joe.checkText(self.findrow, 0, "Replace (Y)es (N)o")))
            cursor = self.joe.cursor
            self.write(response)
    
    def cmd(self, s):
        """Executes a command or macro"""
        self.writectl("^[X")
        self.assertTextAt("Command:", x=0)
        self.write(s)
        self.rtn()
    
    def mode(self, mode):
        """Sets a mode (option menu)"""
        self.cmd("mode")
        self.assertTextAt("Options:", x=0)
        self.write(mode)
        self.rtn()
    
    #def option(self, menu):
    #    self.cmd("menu")
    #    self.assertTextAt("Menu:", x=0)
    #    self.writectl("root{enter}")
    #    self.selectMenu(lambda x: menu in x)
    
    def menu(self, menuname):
        """Brings up menuname menu"""
        self.cmd("menu")
        self.assertTextAt("Menu:", x=0)
        self.write(menuname)
        self.rtn()
        # Wait for cursor to be on inverse (selected) text
        self.assertTrue(self.joe.expect(lambda: len(self._readSelected(self.joe.cursor.X, self.joe.cursor.Y)) > 0))
    
    def encoding(self, encoding):
        """Changes the encoding of the current file to specified encoding"""
        self.mode("encoding")
        self.assertTextAt("Select file character", x=0)
        self.write(encoding)
        self.rtn()
    
    def save(self, filename=None):
        """Saves the current file as filename, or current name if not specified"""
        self.cmd("save")
        self.assertTextAt("Name of file to save", x=0)
        if filename is not None:
            self.writectl("^Y")
            self.write(filename)
        self.rtn()
    
    def selectMenu(self, f):
        """Selects a menu when the supplied function returns true for that menu's text"""
        # Need to detect when we're at the first entry
        while True:
            self.assertTrue(self.joe.expect(lambda: len(self._readSelected(self.joe.cursor.X, self.joe.cursor.Y)) > 0))
            cursor = self.joe.cursor
            txt = self._readSelected(cursor.X, cursor.Y)
            if f(txt):
                return True
            self.writectl("{right}")
            if not self.joe.expect(lambda: self.joe.cursor != cursor):
                self.assertTrue(False, "Could not find menu")
    
    def _readSelected(self, x, y):
        """Read all text in reverse around specified coordinate (cursor coordinates used in case one/both missing)"""
        self.joe.flushin()
        start = end = x
        for i in range(max(0, x - 1), -1, -1):
            if self.joe.term.buffer[y][i].reverse:
                start = i
            else:
                break
        for i in range(x, self.joe.size.X):
            if self.joe.term.buffer[y][i].reverse:
                end = i + 1
            else:
                break
        result = self.joe.readLine(y, start, end - start)
        return result
