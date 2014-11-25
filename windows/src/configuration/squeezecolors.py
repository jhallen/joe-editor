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

BUILTIN_NAMES = [
    'defaultfg',
    'bolddefaultfg',
    'defaultbg',
    'bolddefaultbg',
    'cursortext',
    'cursorcolor',
    'black',
    'boldblack',
    'red',
    'boldred',
    'green',
    'boldgreen',
    'yellow',
    'boldyellow',
    'blue',
    'boldblue',
    'magenta',
    'boldmagenta',
    'cyan',
    'boldcyan',
    'white',
    'boldwhite',
    'selectionfg',
    'selectionbg',
    'statusfg',
    'statusbg',
]

def main(infile, outfile):
    inf = open(infile, 'r')
    if outfile is not None:
        outf = open(outfile, 'wb')
    else:
        outf = sys.stdout
    squeeze(inf, outf)
    inf.close()
    outf.close()

def squeeze(inf, outf):
    data = readspec(inf)
    shrink(data)
    writeout(data, outf)

class ColorScheme:
    def __init__(self):
        self.sections = []

class ColorSection:
    def __init__(self):
        self.name = None
        self.entries = []
    
    def isScheme(self):
        return self.name and self.name.lower() == 'scheme'
    
    def isColors(self):
        return self.name and self.name.lower() == 'colors'

class ColorSpec:
    def __init__(self):
        self.name = None
        self.fg = None
        self.bg = None
        self.attrs = []
        self.text = None

def readspec(inf):
    scheme = ColorScheme()
    sect = None
    for line in inf.readlines():
        line = stripline(line)
        if line.startswith('['):
            sect = ColorSection()
            sep = line.index(']')
            sect.name = line[1:sep]
            scheme.sections.append(sect)
            continue
        
        if '=' in line:
            spec = ColorSpec()
            kv = line.split('=')
            spec.name = kv[0].strip()
            if sect.isScheme():
                spec.text = kv[1]
            else:
                fgbg = kv[1].split('/')
                if len(fgbg) == 1:
                    parts = kv[1].split()
                    spec.fg = parts[0].strip()
                    spec.attrs = parts[1:]
                else:
                    parts = fgbg[1].split()
                    spec.fg = fgbg[0].strip()
                    spec.bg = parts[0].strip()
                    spec.attrs = parts[1:]
            
            sect.entries.append(spec)
    
    return scheme

def stripline(line):
    if ';' in line:
        parts = line.split(';')
        return parts[0].strip()
    return line.strip()

def shorten(color):
    if color and color.startswith('#'):
        shorter = '#' + color[1:].lstrip('0')
        if shorter == '#': shorter = '#0'
        return shorter.lower()
    else:
        return color

class NameCounter:
    def __init__(self):
        self.cur = 0
    
    def peek(self):
        first = self.cur // 26
        second = self.cur % 26
        
        result = ''
        if first > 0:
            result += chr(ord('a') + first - 1)
        result += chr(ord('a') + second)
        return result
    
    def next(self):
        result = self.peek()
        self.cur += 1
        return result

def shrink(data):
    # Count uses of unique colors
    usecount = {}
    for sect in data.sections:
        for ent in sect.entries:
            ent.fg = shorten(ent.fg)
            ent.bg = shorten(ent.bg)
            for c in (ent.fg, ent.bg):
                if c and c.startswith('#'):
                    color = c.lower()
                    if color in usecount:
                        usecount[color] += 1
                    else:
                        usecount[color] = 1
    
    # Replace colors with short variable names where it saves us bytes
    ctr = NameCounter()
    coloralias = {}
    newitems = []
    for k, v in sorted(usecount.iteritems(), key=lambda x: -x[1]):
        normsz = len(k) * v
        namesz = len(ctr.peek())
        replacesz = namesz + 2 + len(k) + (v * (namesz + 1)) # "X=k\n" + v * "$X"
        if replacesz < normsz:
            name = ctr.next()
            coloralias[k] = "$" + name
            spec = ColorSpec()
            spec.name = name
            spec.fg = k
            newitems.append(spec)
    
    # Replace custom variables with new, shorter ones.
    for sect in data.sections:
        if sect.isColors():
            for ent in sect.entries:
                if ent.name.lower() not in BUILTIN_NAMES:
                    newname = ctr.next()
                    coloralias['$' + ent.name.lower()] = '$' + newname
                    ent.name = newname
    
    for sect in data.sections:
        if sect.isScheme(): continue
        for ent in sect.entries:
            if ent.fg and ent.fg.lower() in coloralias:
                ent.fg = coloralias[ent.fg.lower()]
            if ent.bg and ent.bg.lower() in coloralias:
                ent.bg = coloralias[ent.bg.lower()]
    
    for sect in data.sections:
        if sect.isColors():
            sect.entries = newitems + sect.entries
            break

def writeout(data, outf):
    for sect in data.sections:
        outf.write('[' + sect.name + ']\n')
        for ent in sect.entries:
            outf.write(ent.name + '=')
            if ent.text is not None:
                outf.write(ent.text)
            else:
                if ent.fg and ent.bg:
                    outf.write(ent.fg + '/' + ent.bg)
                elif ent.fg:
                    outf.write(ent.fg)
                elif ent.bg:
                    outf.write('/' + ent.bg)
                if ent.attrs:
                    outf.write(' ' + ' '.join(ent.attrs))
            outf.write('\n')

if __name__ == '__main__':
    import sys
    main(sys.argv[1], sys.argv[2])
