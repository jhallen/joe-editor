
import sys
import os

import vimreader

# NOTE: see http://vimdoc.sourceforge.net/htmldoc/syntax.html search "NAMING CONVENTIONS"

tokenconvert = {
#	"Idle": ["normal"],
	"Comment": ["comment", "normal"],
	"PreProc": ["preproc", "precondit", "normal"],
	"Keyword": ["keyword", "statement", "normal"],
	"Type": ["type", "keyword", "statement", "normal"],
	"Define": ["define", "macro", "preproc", "normal"],
	"String": ["string", "constant", "normal"],
	"Constant": ["constant", "number", "float", "boolean", "character", "string"],
	"Literal": ["constant", "number", "float", "boolean", "character", "string"],
	"Brace": ["normal"],
	"Control": ["normal"],
	"Escape": ["specialchar", "special", "specialkey", "string", "constant", "normal"],
	
	"Bad": ["error", "normal"],
	
	"IncLocal": ["string", "preproc"],
	"IncSystem": ["constant", "preproc"],
	
	"diff.DelLine": ["diffdelete"],
	"diff.AddLine": ["diffadd"],
	"diff.ChgLine": ["diffchange"],
	"diff.Garbage": ["difftext"],
	
#	"python.Keyword": ["keyword", "statement", "normal"],
}

colorconvert = {
	"defaultfg": ["normal"],
	"defaultbg": ["/normal"],
	"green": ["comment"],
	"blue": ["preproc", "include", "normal"],
	"boldblue": ["define", "macro", "include", "preproc", "normal"],
	"cursorcolor": ["/cursor"],
	"cursortext": ["cursor"],
	"cyan": ["constant", "string", "number", "float", "boolean", "character", "string"],
	"bolddefaultfg": ["keyword", "statement", "normal"],
	"boldcyan": ["specialchar", "special", "specialkey", "normal"],
	"magenta": ["operator", "conditional", "repeat", "label"],
	"boldred": ["error"],
	"red": ["diffdelete", "error"],
	"yellow": ["storageclass", "function", "identifier", "normal"], # not used for much -- pick something we don't have a lot of
	"white": ["ignore", "normal"],
	"black": ["ignore", "normal"],

	"statusfg": ["statusline", "normal"],
	"statusbg": ["/statusline"],
	"selectionfg": ["visual"],
	"selectionbg": ["/visual"],
}

class JoeColorFile:
	def __init__(this):
		this.scheme = ConfigSection("Scheme")
		this.colors = ConfigSection("Colors")
		this.syntax = ConfigSection("Syntax")

class ConfigSection:
	def __init__(this, name):
		this._name = name
		this.kvp = []
	
	def getName(this):
		return this._name
	name = property(getName)
	
	def __getitem__(this, key):
		for k, v in this.kvp:
			if k.lower() == key.lower():
				return v
	
	def __setitem__(this, key, val):
		for i in range(len(this.kvp)):
			if this.kvp[i][0].lower() == key.lower():
				this.kvp[i] = (key, val)
				return
		this.kvp.append((key, val))
	
	def __iter__(this):
		for k, v in this.kvp:
			yield k

def convert(file, outfile, ovrfile = None):
	f = open(file, 'r')
	dict = vimreader.readFile(f)
	f.close()
	
	cfg = JoeColorFile()
	
	for k, v in tokenconvert.iteritems():
		for p in v: # p for possibility
			if p in dict:
				cfg.syntax[k] = convertColorSpec(dict[p])
				break
	
	for k, v in colorconvert.iteritems():
		for p in v:
			useback = False
			if p.startswith('/'):
				useback = True
				p = p[1:]
			if p in dict:
				col = dict[p]
				if (useback and col.bg) or (not useback and col.fg):
					cfg.colors[k] = convertColorSpec(col, bgonly=useback, fgonly=not useback, attrs=not useback)
					break
	
	cfg.scheme['Name'] = os.path.splitext(os.path.basename(file))[0]
	
	if ovrfile:
		f = open(ovrfile, 'r')
		ovrcfg = loadOverrides(f)
		f.close()
		populateOverrides(ovrcfg, dict)
		doOverrides(cfg, ovrcfg)
	
	f = open(outfile, 'w')
	writeColorFile(f, cfg)
	f.close()

def loadOverrides(f):
	cfg = JoeColorFile()
	sect = None
	for line in f:
		if ';' in line:
			line = line[0:line.index(';')]
		line = line.strip()
		
		if line.startswith('[') and line.endswith(']'):
			sectname = line[1:-1]
			if sectname.lower() == 'scheme':
				sect = cfg.scheme
			elif sectname.lower() == 'colors':
				sect = cfg.colors
			elif sectname.lower() == 'syntax':
				sect = cfg.syntax
			else:
				sect = NOne
		elif len(line) == 0:
			pass
		elif sect is not None:
			k, v = line.split('=', 1)
			k = k.strip()
			v = v.strip()
			sect[k] = v
	return cfg

def populateOverrides(cfg, syntax):
	# Build map of everything in the vim file for use with format
	fmtkeys = {}
	for k, v in syntax.iteritems():
		lk = k.lower()
		if v.fg:
			fmtkeys['fg_' + lk] = convertColorSpec(v, fgonly=True)
		if v.bg:
			fmtkeys['bg_' + lk] = convertColorSpec(v, bgonly=True)
		fmtkeys[lk] = convertColorSpec(v)
	
	for sect in (cfg.colors, cfg.syntax):
		for k in list(sect):
			ov = v = cfg.colors[k].lower()
			v = v.format(**fmtkeys)
			if v != ov:
				sect[k] = v

def doOverrides(cfg, ovrcfg):
	for csect, osect in ((cfg.scheme, ovrcfg.scheme), (cfg.colors, ovrcfg.colors), (cfg.syntax, ovrcfg.syntax)):
		for k in list(osect):
			csect[k] = osect[k]

def writeColorFile(f, cfg):
	# Sort colors
	def sckey(v):
	        return 0
	def ckey(v):
		order = ['defaultfg', 'bolddefaultfg', 'defaultbg', 'bolddefaultbg', 'cursortext', 'cursorcolor', 'black',
		         'boldblack', 'red', 'boldred', 'green', 'boldgreen', 'yellow', 'boldyellow', 'blue', 'boldblue',
		         'magenta', 'boldmagenta', 'cyan', 'boldcyan', 'white', 'boldwhite',
			 'statusfg', 'statusbg', 'selectionfg', 'selectionbg']
		return order.index(v.lower())
	def skey(v):
		if '.' in v:
		        syn, tok = v.split('.')
                else:
                        syn, tok = '', v
                return syn.lower(), tok.lower()
	
	for sect, skey in ((cfg.scheme, sckey), (cfg.colors, ckey), (cfg.syntax, skey)):
		f.write("[%s]\n" % sect.name)
		
		for k in sorted(sect, key=skey):
			f.write("%s=%s\n" % (k, sect[k]))
		f.write('\n')

def convertColorSpec(spec, fgonly=False, bgonly=False, attrs=True):
	result = ""
	if bgonly:
		if spec.bg:
			result = spec.bg
	elif fgonly:
		if spec.fg:
			result = spec.fg
	else:
		if spec.bg:
			result = (spec.fg or '') + '/' + spec.bg
		else:
			result = spec.fg or '/'
	
	if attrs:
		if spec.bold:
			result += ' bold'
		if spec.italic:
			result += ' italic'
		if spec.underline:
			result += ' underline'
	
	return result

### Main
if __name__ == '__main__':
	if len(sys.argv) == 3:
		convert(sys.argv[1], sys.argv[2])
	elif len(sys.argv) == 4:
		convert(sys.argv[1], sys.argv[2], sys.argv[3])
	else:
		print "Syntax: %s input.vim output.joecolor [output.override]" % sys.argv[0]
