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

flagsettings = [
    'asis', 'assume_256color', 'assume_color', 'autoindent', 'autoswap', 'beep', 'break_hardlinks', 'break_links',
    'c_comment', 'cpp_comment', 'crlf', 'csmode', 'dopadding', 'exask', 'floatmouse', 'flowed', 'force', 'french',
    'guess_crlf', 'guess_indent', 'guess_non_utf8', 'guess_utf8', 'help', 'help_is_utf8', 'hex', 'highlight', 'icase',
    'indentfirst', 'joe_state', 'joexterm', 'keepup', 'lightoff', 'linums', 'marking', 'menu_above', 'menu_explorer',
    'menu_jump', 'mid', 'mouse', 'no_double_quoted', 'nobackup', 'nobackups', 'nocurdir', 'nolocks', 'nomodcheck',
    'nonotice', 'nosta', 'notite', 'noxon', 'orphan', 'overwrite', 'picture', 'pound_comment', 'purify', 'rdonly',
    'restore', 'rtbutton', 'search_prompting', 'semi_comment', 'single_quoted', 'smarthome', 'smartbacks', 'spaces',
    'square', 'tex_comment', 'transpose', 'usetabs', 'vhdl_comment', 'wordwrap', 'wrap'
]

oneparamsettings = [
    'baud', 'columns', 'encoding', 'help_color', 'indentc', 'istep', 'keymap', 'lines', 'lmargin', 'menu_color',
    'msg_color', 'pg', 'prompt_color', 'rmargin', 'skiptop', 'status_color', 'syntax', 'tab', 'text_color',
    'undo_keep'
]

lineparamsettings = [
    'backpath', 'cpara', 'lmsg', 'mfirst', 'mnew', 'mold', 'msnew', 'msold', 'rmsg', 'text_delimiters'
]

specialkeys = [
    'SP', 'TO', 'MDOWN', 'MUP', 'MDRAG', 'M2DOWN', 'M2UP', 'M2DRAG', 'M3DOWN', 'M3UP', 'M3DRAG', 'MWDOWN', 'MWUP',
    'MRUP', 'MRDOWN', 'MRDRAG', 'MMUP', 'MMDOWN', 'MMDRAG'
]

class SqueezeState(object):
    inbrace = False
    insettings = True
    inmenus = False
    inkeys = False
    
    def clear(self):
        self.inbrace = self.insettings = self.inmenus = self.inkeys = False

def squeeze(infname, inf, outf, first=None):
    if first is None:
        first = [True]
    state = SqueezeState()
    
    for line in inf:
        nextln = squeezeline(line, state)
        if nextln:
            # Make sure includes are also loaded from builtin.
            if nextln.startswith(':include'):
                parts = nextln.split(' ', 1)
                nextln = parts[0] + ' *' + parts[1]
            
            if not first[0]:
                outf.write("\n")
            else:
                first[0] = False
            outf.write(nextln)

def squeezeline(line, state):
    # Blank lines, or lines starting with whitespace are ignored.
    if len(line) == 0: return None
    if line[0].isspace(): return None
    
    if line[0] == '{':
        state.inbrace = True
    if state.inbrace:
        if line[0] == '}':
            state.inbrace = False
        return line.strip()
    
    firstword = line.split()[0]
    
    if state.insettings:
        if line[0] == '-':
            # Setting
            cmd = firstword[1:]
            if cmd.startswith('-'):
                # This clears a setting, therefore we just want the name
                return '-' + cmd
            if cmd.lower() in flagsettings:
                # This is an on/off switch.  Only output the switch
                return '-' + cmd
            elif cmd.lower() in oneparamsettings:
                # This setting takes a single parameter.
                return '-%s %s' % (cmd, line.split()[1])
            else:
                if cmd.lower() not in lineparamsettings:
                    # We assume everything else is a lineparam, but show a warning
                    # if we encounter something we don't understand
                    sys.stderr.write("Don't know what to do with '%s' -- taking whole line\n" % cmd)
                # The entire line is significant
                return line.strip()
        if line[0] == '=':
            # Color definition: Take all tokens, minimize spaces
            return ' '.join(line.strip().split())
        if line[0] == "+":
            # File match definition: take whole thing.
            return line.strip()
    
    if firstword.lower() == ':defmenu':
        state.clear()
        state.inmenus = True
    elif firstword.lower() in [':windows', ':main', ':inherit', ':prompt', ':query', ':querya', ':querysr']:
        state.clear()
        state.inkeys = True
    
    result = ""     # Output string
    quote = False   # Inside quotation marks?
    esc = False     # Immediately after an escape character (\)?
    spaces = 0      # How many spaces have we buffered?
    maxspaces = 2   # How many spaces until a comment?
    
    if line.startswith("mode,") or line.startswith("menu,"):
        maxspaces = 100

    for i, c in enumerate(line):
        keep = True
        if quote:
            if c == '"':
                quote = False
            elif c == '\\':
                esc = True
            else:
                esc = False
        else:
            if c.isspace():
                spaces += 1
                if spaces == maxspaces:
                    break
                keep = False
            elif c == '"':
                quote = True
                esc = False
        
        if state.inkeys and spaces > 0 and not line.startswith(':'):
            # Divert here and handle key input section special.
            parts = line[i+1:].split()
            for p in parts:
                if len(p) == 1 or p.startswith('^') or p.startswith('.k') or p.lower().startswith('.f') or p.startswith('.@') or p in specialkeys:
                    result += ' ' + p
                else:
                    break
            break
        
        if keep:
            if spaces > 0:
                result += " "
                spaces = 0
                if state.inkeys:
                    # Divert here and handle key input section special.
                    # After the first set of spaces, we cut off at 2
                    maxspaces = 2
                        
            result += c
    
    return result

if __name__ == '__main__':
    if len(sys.argv) != 2 and len(sys.argv) != 3:
        print "JOERC file squeezer"
        print "Syntax: %s (in-file) (out-file)" % sys.argv[0]
        sys.exit(1)
    if len(sys.argv) == 3:
        main(sys.argv[1], sys.argv[2])
    elif len(sys.argv) == 2:
        main(sys.argv[1], None)

