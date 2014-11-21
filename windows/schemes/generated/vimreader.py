
class ColorDef:
	def __init__(this, fg, bg, bold, italic, underline, guiColors = True):
		this.fg = fg
		this.bg = bg
		this.bold = bold
		this.italic = italic
		this.underline = underline
		this.guiColors = guiColors

class ColorLink:
	def __init__(this, link):
		this.link = link

def readFile(lines):
	result = {}
	backupColors = {}
	ifstack = []
	for line in lines:
		if isBlank(line):
			pass
		elif isIf(line):
			ifstack.append(None)
		elif isElse(line):
			assert len(ifstack) > 0
			if ifstack[-1] is None:
				pass
			else:
				ifstack[-1] = not ifstack[-1]
		elif isEndif(line):
			ifstack.pop()
		elif isDo(line):
			ifstack.append(True)
		elif isDont(line):
			ifstack.append(False)
		elif len(ifstack) == 0 or ifstack[-1]:
			r = parseColor(line)
			if r is not None:
				if isinstance(r[1], ColorDef):
					if r[1].guiColors:
						result[r[0].lower()] = r[1]
					else:
						backupColors[r[0].lower()] = r[1]
	
	# Sub in cterm colors if no GUI colors available
	for k, v in backupColors.iteritems():
		if k not in result:
			result[k] = v
	
	# Process links
	for k in list(result.keys()):
		if k in result:
			resolveLink(result, k)
	
	postProcess(result)
	
	return result

def postProcess(dict):
	normfg = None
	normbg = None
	if 'normal' in dict:
		normfg = dict['normal'].fg
		normbg = dict['normal'].bg
	
	for cdef in dict.values():
		if cdef.fg == 'fg':   cdef.fg = normfg
		elif cdef.fg == 'bg': cdef.fg = normbg
		if cdef.bg == 'fg':   cdef.bg = normfg
		elif cdef.bg == 'bg': cdef.bg = normbg
	
	if 'cursor' not in dict:
		cursor = ColorDef(fg=normbg, bg=normfg, bold=False, italic=False, underline=False)
	else:
		cursor = dict['cursor']
	
	if cursor.fg is None:
		cursor.fg = normbg
	if cursor.bg is None:
		cursor.bg = normfg

def resolveLink(dict, key):
	v = dict[key]
	if isinstance(v, ColorLink):
		nk = v.link.lower()
		res = nk in dict and resolveLink(dict, nk)
		if res:
			dict[key] = res
			return res
		else:
			del dict[key]
			return None
	else:
		return v

def parseColor(line):
	pieces = list(splitWs(line))
	if len(pieces) < 3:
		return None
	stmt = pieces[0].lower()
	if stmt.startswith(':'): stmt = stmt[1:]
	if stmt.endswith('!'): stmt = stmt[:-1]
	if stmt not in ['hi', 'highlight']:
		return None
	
	if pieces[1].lower() == 'link':
		return pieces[2], ColorLink(pieces[3])
	
	if pieces[1].lower() == 'clear':
		return None
	
	nom = pieces[1]
	fg = None
	bg = None
	cfg = None
	cbg = None
	bold = False
	italic = False
	underline = False
	reverse = False
	
	for kv in pieces[2:]:
		pkv = kv.split('=')
		if len(pkv) == 2:
			k, v = pkv
			if k == 'guifg':
				fg = v
			elif k == 'guibg':
				bg = v
			elif k == 'gui':
				for spec in v.split(','):
					if spec == 'bold':
						bold = True
					elif spec == 'italic':
						italic = True
					elif spec == 'underline':
						underline = True
					elif spec == 'reverse':
						reverse = True
			elif k == 'ctermfg':
				cfg = v
			elif k == 'ctermbg':
				cbg = v

	guiColors = (fg or bg) and True
	
	if fg is None and cfg is not None:
		fg = parseCterm(cfg)
	if bg is None and cbg is not None:
		bg = parseCterm(cbg)
	
	if reverse:
		bg, fg = fg, bg
	
	if fg == 'fg' or (fg and fg.lower() == 'none'):
		# Stupid, ignore.
		fg = None
	if bg == 'bg' or (bg and bg.lower() == 'none'):
		bg = None
	
	if fg is None and bg is None and not (bold or italic or underline):
		return None
	
	if fg is not None and not fg.startswith('#'):
		# Name of some sort... pick it apart
		lwr = fg.lower()
		if lwr == 'fg' or lwr == 'bg':
			fg = lwr
		else:
			if lwr not in rgbmap:
				# Don't know.  Give up.
				return None
			fg = rgbmap[lwr]
	if bg is not None and not bg.startswith('#'):
		lwr = bg.lower()
		if lwr == 'fg' or lwr == 'bg':
			bg = lwr
		else:
			if lwr not in rgbmap:
				return None
			bg = rgbmap[lwr]
	
	return nom, ColorDef(fg=fg, bg=bg, bold=bold, underline=underline, italic=italic, guiColors=guiColors)

def isBlank(line):
	return len(line.strip()) == 0 or line.strip().startswith('"')

def isIf(line):
	return line.strip().startswith('if')

def isElse(line):
	return line.strip().startswith('else')

def isEndif(line):
	return line.strip().startswith('endif')

def isDo(line):
	return line.strip().startswith('do') and not line.strip().startswith('dont')

def isDont(line):
	return line.strip().startswith('dont')

def splitWs(line):
	inws = True
	tok = ""
	for i in range(0, len(line)):
		c = line[i]
		if c.isspace() and not inws:
			yield tok
			tok = ''
			inws = True
		elif not c.isspace():
			tok += c
			inws = False
	if len(tok) > 0:
		yield tok

rgbmap = {}
def loadRgbs(file):
	f = open(file, "r")
	for line in f:
		pieces = list(splitWs(line))
		if len(pieces) < 4:
			continue
		r, g, b = map(int, pieces[0:3])
		name = ' '.join(pieces[3:])
		rgbmap[name.lower()] = "#%02x%02x%02x" % (r, g, b)
	f.close()

ctermMap = None
ctermNames = {
	'black': 0,
	'darkred': 1,
	'darkgreen': 2,
	'darkyellow': 3,
	'brown': 3,
	'darkblue': 4,
	'darkmagenta': 5,
	'darkcyan': 6,
	'gray': 7,
	'grey': 7,
	'darkwhite': 7,
	'darkgray': 8,
	'darkgrey': 8,
	'red': 9,
	'green': 10,
	'yellow': 11,
	'blue': 12,
	'magenta': 13,
	'cyan': 14,
	'white': 15,
}

def loadCtermMap():
	global ctermMap
	
	def simpleColor(n, val):
		def chk(b):
			if n & b: return val
			return 0
		return (chk(1), chk(2), chk(4))
	
	colors = []
	
	# Basic ANSI colors (use xterm map)
	colors += [simpleColor(i, 0x80) for i in range(0, 7)]
	colors.append((0xc0, 0xc0, 0xc0))
	colors.append((0x80, 0x80, 0x80))
	colors += [simpleColor(i, 0xff) for i in range(9, 16)]
	
	# Extended colors
	extvals = [0x00, 0x5f, 0x87, 0xaf, 0xdf, 0xff]
        for r in extvals:
			for g in extvals:
				for b in extvals:
					colors.append((r, g, b))
	
	# Grayscale
	for v in [0x08, 0x12, 0x1c, 0x26, 0x30, 0x3a, 0x44, 0x4e, 0x58, 0x60, 0x66, 0x76,
	          0x80, 0x8a, 0x94, 0x9e, 0xa8, 0xb2, 0xbc, 0xc6, 0xd0, 0xda, 0xe4, 0xee]:
		colors.append((v, v, v))
	
	# Convert colors to #RRGGBB
	ctermMap = ["#%02x%02x%02x" % rgb for rgb in colors]

def parseCterm(nom):
	noml = nom.lower()
	if noml in ctermNames:
		n = ctermNames[noml]
	elif noml in rgbmap:
		return rgbmap[noml]
	elif noml.isdigit():
		n = int(nom)
	else:
		return None
	return ctermMap[n]

### Init
loadRgbs('rgb.txt')
loadCtermMap()
