import os

def parseColorSpec(spec):
    parts = spec.split(' ')
    color = None
    bold = False
    for part in parts:
        if part.lower() == 'bold':
            bold = True
        elif part.lower() in ['underline', 'inverse', 'reverse']:
            pass
        else:
            color = part.lower()
    return (color, bold)

def loadFile(file):
    f = open(file, 'r')
    
    tokens = {}
    colors = {}
    
    for line in f:
        if line.startswith('='):
            line = line[1:]
            parts = line.split('\t', 1)
            if len(parts) == 1:
                parts = line.split(' ', 1)
                if len(parts) == 1:
                    name = line
                    color = ''
                else:
                    name, color = parts
            else:
                name, color = parts
            name = name.strip()
            color = color.strip()
            tokens[name.lower()] = color
            
            cspec = parseColorSpec(color)
            if cspec in colors:
                colors[cspec].append(name)
            else:
                colors[cspec] = [name]
    f.close()
    return tokens, colors

def loadFiles(dir):
    tokens = {}
    colors = {}
    
    for f in os.listdir(dir):
        if f.endswith('.jsf'):
            fpath = os.path.join(dir, f)
            nom = os.path.splitext(f)[0]
            t, c = loadFile(fpath)
            for key,val in t.iteritems():
                v = (nom, val)
                if key in tokens:
                    tokens[key].append(v)
                else:
                    tokens[key] = [v]
            
            for key,val in c.iteritems():
                v = (nom, val)
                if key in colors:
                    colors[key].append(v)
                else:
                    colors[key] = [v]
            
    return tokens, colors

def printTokens(r):
    for key,val in sorted(r.iteritems(), key=lambda x: -len(x[1])):
        print key, "(%d occurrences)" % len(val)
        for syn,col in val:
            print "\t%s\t%s" % (syn, col)
        print

def printColors(r):
    for key,val in sorted(r.iteritems(), key=lambda x: -len(x[1])):
        print "%s %s (%d occurrences)" % (key[0], (key[1] and 'bold') or False, len(val))
        for syn,col in val:
            print "\t%s\t%s" % (syn, col)
        print

tokens, colors = loadFiles('.\\bin\\release\\syntax')

print "*** Tokens:"
printTokens(tokens)

print
print "*** Colors:"
printColors(colors)
