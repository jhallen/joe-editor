
import vimreader
import sys
import os


template = """
<html>
  <head>
    <title>{name} Color palette</title>
  </head>
  <body bgcolor="{defaultbg}" text="{defaultfg}">
    <font face="Consolas" size=2><center>
      <table cellspacing=10>
        <tr>
          <th colspan=2><u>Color swatch</u></th>
          <th><u>FG code</u></th>
          <th><u>BG code</u></th>
          <th><u>Uses</u></th>
        </tr>
        {rows}
      </table>
    </center></font>
  </body>
</html>"""

singleColorRowTemplate = """
<tr>
  <td colspan=2 bgcolor="{color}">&nbsp;</td>
  <td><center>{color}</center></td>
  <td><center>None</center></td>
  <td><font color="{color}">{uses}</font></td>
</tr>
"""

fgbgRowTemplate = """
<tr>
  <td bgcolor="{bg}">&nbsp;</td>
  <td bgcolor="{fg}">&nbsp;</td>
  <td><center>{fgname}</center></td>
  <td><center>{bg}</center></td>
  <td bgcolor="{bg}"><font color="{fg}">{uses}</font></td>
</tr>
"""

def main(infile, outfile):
  f = open(infile, 'r')
  tokens = vimreader.readFile(f)
  f.close()
  
  colors = reverseMap(tokens)
  name = os.path.splitext(os.path.basename(infile))[0]
  
  f = open(outfile, 'w')
  f.write(generatePalette(colors, tokens, name))
  f.close()

def generatePalette(colors, tokens, name):
  # Break out and sort color dict
  rowdata = [(coltup[0], coltup[1], toks) for coltup, toks in colors.iteritems()]
  rowdata.sort(key=lambda x: (-len(x[2]), x[2][0]))
  
  # Build row strings
  rows = []
  for fg, bg, toks in rowdata:
    tokstring = ", ".join(toks)
    if bg is None or len(bg) == 0:
      rows.append(singleColorRowTemplate.format(color=fg, uses=tokstring))
    else:
      fgname = fg
      if not fg:
        fg = tokens['normal'].fg
      rows.append(fgbgRowTemplate.format(fg=fg, fgname=fgname, bg=bg, uses=tokstring))
  
  rowstring = "\n".join(rows)
  
  return template.format(name=name, defaultfg=tokens['normal'].fg, defaultbg=tokens['normal'].bg, rows=rowstring)

def reverseMap(tokens):
  colors = {}
  
  for tok, col in tokens.iteritems():
    fg = col.fg
    bg = col.bg
    if fg: fg = fg.lower()
    if bg: bg = bg.lower()
    
    if not fg and not bg: continue
    
    coltup = fg, bg
    
    if coltup in colors:
      colors[coltup].append(tok)
    else:
      colors[coltup] = [tok]
  
  for clist in colors.values():
    clist.sort()
  
  return colors

if __name__ == '__main__':
  main(sys.argv[1], sys.argv[2])
