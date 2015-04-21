
import codecs
import sys

import markdown2

def main(infile, outfile):
    fp = codecs.open(infile, 'r', 'utf-8')
    text = fp.read()
    fp.close()
    
    result = markdown2.markdown(text, extras={'toc':None})
    
    lines = result.split('\n')
    
    # Insert the TOC
    for i in range(len(lines)):
        if "[TOC]" in lines[i]:
            lines[i] = result.toc_html
            break
    
    fp = codecs.open(outfile, 'w', 'utf-8')
    fp.write('<link rel="stylesheet" href="markdown.css"></link>\r\n')
    fp.write('\n'.join(lines))
    fp.close()

if __name__ == '__main__':
    main(*sys.argv[1:])
