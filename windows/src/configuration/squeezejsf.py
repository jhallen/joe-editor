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

###
### Reduces size of jsf files, so that they are more suitable for packing directly
### into the binary.  Removes comments, whitespace, and shortens state names to
### achieve roughly 40-50% reduction in file size, while remaining readable by JOE's
### jsf parser and without any change in function.
###
### Sample set: batch, csharp, css, diff, html, ini, java, perl, php, python, ruby, sql, xml
###
###                       Uncompressed: 79,689 bytes
###    Whitespace and comments removed: 67,369 bytes (-12,320 bytes or 15.4%)
###                  **squeezejsf.py**: 43,039 bytes (-36,650 bytes or 45.9%)
###                            zipfile: 22,067 bytes (-57,622 bytes or 72.3%)
###                    squeeze+zipfile: 15,746 bytes (-63,943 bytes or 80.2%)
###
### Subroutines, color names, and subroutine parameters are left untouched as they are
### used across files and by color schemes.  Shortening them would be tricky and prone
### to error for a only modest gain.
###

import sys

class SyntaxReader(object):
    def __init__(this):
        this.seen_colors = False
        this.seen_backtrack = False
        this.seen_states = False
    
    def accept_line(this, line):
        line = this.strip_line(line)
        if len(line) == 0:
            return
        elif line.startswith("-"):
            if this.seen_colors or this.seen_backtrack or this.seen_states:
                raise "Got a backtrack specification too late"
            this.on_backtrack(line)
            this.seen_backtrack = True
        elif line.startswith("="):
            if this.seen_states:
                raise "Got a color specification too late"
            this.seen_colors = True
            colorspec = line.split(' ', 1)
            if len(colorspec) == 1:
                this.on_color_def(colorspec[0][1:], None)
            elif len(colorspec) == 2:
                this.on_color_def(colorspec[0][1:], colorspec[1])
            else:
                raise "Invalid color specification"
        elif line.startswith(":"):
            this.seen_states = True
            statespec = line.split(' ', 1)
            if len(statespec) == 1:
                this.on_start_state(statespec[0][1:], None)
            elif len(statespec) == 2:
                this.on_start_state(statespec[0][1:], statespec[1])
            else:
                raise "Invalid state specification"
        elif line.startswith("."):
            this.on_condition(line)
        elif line == 'done':
            this.on_done()
        else:
            if not this.seen_states:
                raise "Don't know what I just got"
            # Have to manually split because this likely contains a string
            firstarg = ''
            rest = ''
            q = False
            esc = False
            for i in range(0, len(line)):
                c = line[i]
                if c == '"':
                    firstarg += c
                    q = esc or not q
                    esc = False
                elif q and c == '\\':
                    esc = not esc
                    firstarg += c
                elif not q and c == ' ':
                    rest = line[i+1:]
                    break
                else:
                    firstarg += c
                    esc = False
            
            if q:
                raise "Unterminated string"
            
            # Pull apart target state and options
            trans = rest.split(' ', 1)
            if len(trans) == 1:
                this.on_transition(firstarg, trans[0], '')
            elif len(trans) == 2:
                this.on_transition(firstarg, trans[0], trans[1])
            else:
                raise "Invalid state transition"
    
    def strip_line(this, line):
        line = line.strip()
        result = ''
        q = False
        esc = False
        sp = True
        for c in line:
            if c == '"':
                result += c
                sp = False
                q = esc or not q
                esc = False
            elif q and c == '\\':
                esc = not esc
                result += c
            elif not q and (c == ' ' or c == '\t'):
                if not sp:
                    result += ' '
                    sp = True
            elif c == '#' and not q:
                # Comment: stop!
                break
            else:
                result += c
                sp = False
                esc = False
        if q:
            raise "Unterminated string"
        # Probably trailing whitespace
        return result.strip()
    
    def on_backtrack(this, bt): pass
    def on_start_state(this, state, color): pass
    def on_color_def(this, name, colors): pass
    def on_transition(this, case, state, options): pass
    def on_done(this): pass
    def on_condition(this, text): pass

class StateAnalyzer(SyntaxReader):
    def __init__(this):
        super(StateAnalyzer, this).__init__()
        this.state_usage = {}
    
    def on_start_state(this, state, color):
        this.add_use(state)
    
    def on_transition(this, case, state, options):
        this.add_use(state)
    
    def add_use(this, state):
        if state in this.state_usage:
            this.state_usage[state] += 1
        else:
            this.state_usage[state] = 0
    
    def assign_names(this):
        def atoz():
            for i in range(0, 26): yield chr(ord('a') + i)
            for i in range(0, 26): yield chr(ord('A') + i)
        def get_names():
            for c in atoz(): yield c
            for p in get_names():
                for c in atoz(): yield p + c
        
        result = {}
        prilist = [(-v, k) for k, v in this.state_usage.iteritems()]
        prilist.sort()
        for i, name in enumerate(get_names()):
            if i >= len(prilist): break
            result[prilist[i][1]] = name # = prilist[i][1] to only remove comments/whitespace
            #print "%d:\t%s -> %s" % (-prilist[i][0], prilist[i][1], name)
        return result

class SyntaxTranslator(SyntaxReader):
    def __init__(this, translation, f):
        super(SyntaxTranslator, this).__init__()
        this.translation = translation
        this.outf = f
    
    def on_backtrack(this, bt):
        this.outf.write(bt + "\n")
    
    def on_start_state(this, state, color):
        this.outf.write(":" + this.translation[state])
        if color: this.outf.write(" " + color)
        this.outf.write("\n")
    
    def on_color_def(this, name, colors):
        this.outf.write("=" + name)
        if colors: this.outf.write(" " + colors)
        this.outf.write("\n")
    
    def on_transition(this, case, state, options):
        this.outf.write(case + ' ' + this.translation[state])
        if options: this.outf.write(' ' + options)
        this.outf.write('\n')
    
    def on_done(this):
        this.outf.write('done\n')
    
    def on_condition(this, text):
        this.outf.write(text + '\n')

def squeeze(infile, outfile):
    a = StateAnalyzer()
    fin = open(infile, 'r')
    for line in fin:
        a.accept_line(line)
    fin.close()
    
    fin = open(infile, 'r')
    fout = open(outfile, 'wb') # b - write out just LF instead of CRLF
    t = SyntaxTranslator(a.assign_names(), fout)
    for line in fin:
        t.accept_line(line)
    fin.close()
    fout.close()

def main(argv):
    if len(argv) != 3:
        print "Syntax: %s (in-file) (out-file)" % argv[0]
    else:
        squeeze(argv[1], argv[2])

if __name__ == '__main__':
    main(sys.argv)
