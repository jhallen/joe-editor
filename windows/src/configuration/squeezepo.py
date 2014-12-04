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
import io
import os

def main(infname, outfname):
    inf = open(infname, 'r')
    if outfname:
        outf = open(outfname, 'wb')
    else:
        outf = sys.stdout
    squeeze(infname, inf, outf)
    inf.close()
    if outfname:
        outf.close()

def squeeze(infname, inf, outf):
    runningstr = None
    for line in inf:
        lstr = line.strip()
        if runningstr is not None:
            if lstr.startswith('"'):
                end = lstr.rindex('"')
                runningstr += lstr[1:end]
                continue
            else:
                outf.write('"' + runningstr + '"\n')
                runningstr = None
        if lstr.startswith('#') or len(lstr) == 0:
            continue
        if '"' in lstr:
            q = lstr.index('"')
            rq = lstr.rindex('"')
            outf.write(lstr[:q])
            runningstr = lstr[q+1:rq]
        else:
            outf.write(lstr + '\n')

if __name__ == '__main__':
    if len(sys.argv) != 2 and len(sys.argv) != 3:
        print "PO file squeezer"
        print "Syntax: %s (in-file) (out-file)" % sys.argv[0]
        sys.exit(1)
    if len(sys.argv) == 3:
        main(sys.argv[1], sys.argv[2])
    elif len(sys.argv) == 2:
        main(sys.argv[1], None)

