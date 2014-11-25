#   This file is part of Joe's Own Editor for Windows.
#   Copyright (c) 2014 John J. Jordan.
# 
#   Joe's Own Editor for Windows is free software: you can redistribute it 
#   and/or modify it under the terms of the GNU General Public License as
#   published by the Free Software Foundation, either version 2 of the 
#   License, or (at your option) any later version.
# 
#   Joe's Own Editor for Windows is distributed in the hope that it will
#   be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
#   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
# 
#   You should have received a copy of the GNU General Public License
#   along with Joe's Own Editor for Windows.  If not, see
#   <http://www.gnu.org/licenses/>.

import sys
import os
import subprocess

def main(specfile, infileroot, outfile):
    rf = open(specfile, 'r')
    wf = open(outfile, 'w')
    
    specfileroot = os.path.dirname(specfile)
    result = processFile(rf, wf, infileroot, specfileroot)
    
    rf.close()
    wf.close()
    return result

def processFile(rf, wf, infileroot, specfileroot):
    for line in rf.readlines():
        sline = line.strip()
        if not sline: continue
        
        if sline.startswith('#include'):
            junk, incname = sline.split(' ')
            incfullname = os.path.join(specfileroot, incname)
            incf = open(incfullname, 'r')
            result = processFile(incf, wf, infileroot, specfileroot)
            incf.close()
            if not result: return False
            continue
        
        if '[' in sline:
            optstart = sline.index('[')
            optend = sline.index(']')
            optstr = sline[optstart + 1:optend]
            opts = optstr.split(' ')
            sline = sline[:optstart].strip()
        else:
            opts = []
        
        if '=>' in sline:
            parts = sline.split('=>')
            if len(parts) != 2:
                print "Invalid specification: " + sline
                return False
            
            srcfile = parts[0].strip()
            dstfile = parts[1].strip()
        else:
            srcfile = dstfile = sline
        
        actualfile = os.path.join(infileroot, srcfile)
        if not os.path.exists(actualfile):
            print "ERROR: File does not exist: " + actualfile
            return False
        
        if 'compress' in opts:
            lzf = os.path.join(os.path.dirname(__file__), 'lzf.exe')
            zname = actualfile + ".lzf"
            if os.path.exists(lzf):
                if not os.path.exists(zname) or os.path.getmtime(zname) < os.path.getmtime(actualfile):
                    # Make sure the lzf creation is atomic.
                    lockfname = zname + '.lock'
                    try:
                        lockfd = os.open(lockfname, os.O_CREAT | os.O_EXCL)
                        os.close(lockfd)
                        subprocess.call([lzf, '-ck', actualfile])
                        os.unlink(lockfname)
                    except:
                        # File is locked, generation must be going on *right now*.
                        # We'll trust that it finishes before the rc files get processed.
                        pass
                
                actualfile = zname
        
        wf.write("F:*" + dstfile + " RCDATA \"" + actualfile.replace("\\", "\\\\") + "\"\n")
    
    return True

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print "Syntax: %s (specfile) (infileroot) (outfile.rc)" % sys.argv[0]
    else:
        sys.exit(0 if main(sys.argv[1], sys.argv[2], sys.argv[3]) else 1)
