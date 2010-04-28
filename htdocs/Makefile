

HTDOCS = history.html news.html changelog.html manpage.html index.html hints.html hacking.html list.html options.html

GIFS = capture.gif elaborate.gif cg2000.jpg trs80_16.jpg

all : ${HTDOCS}

publish : ${HTDOCS} ${GIFS}
	scp ${HTDOCS} ${GIFS} jhallen,joe-editor@web.sourceforge.net:htdocs

clean :
	/bin/rm -f ${HTDOCS}

index.html : header-src.html index-src.html footer-src.html
	sed "s,@title@,Joe's Own Editor," <header-src.html | cat - index-src.html footer-src.html > $@

manpage.html : header-src.html manpage-src.html footer-src.html
	sed "s,@title@,Joe's Own Editor - man page," <header-src.html | cat - manpage-src.html footer-src.html > $@

changelog.html : header-src.html changelog-src.html footer-src.html
	sed "s,@title@,Joe's Own Editor - Change Log," <header-src.html | cat - changelog-src.html footer-src.html > $@

news.html : header-src.html news-src.html footer-src.html
	sed "s,@title@,Joe's Own Editor - News," <header-src.html | cat - news-src.html footer-src.html > $@

history.html : header-src.html history-src.html footer-src.html
	sed "s,@title@,Joe's Own Editor - History," <header-src.html | cat - history-src.html footer-src.html > $@

hints.html : header-src.html hints-src.html footer-src.html
	sed "s,@title@,Joe's Own Editor - Hints," <header-src.html | cat - hints-src.html footer-src.html > $@

hacking.html : header-src.html hacking-src.html footer-src.html
	sed "s,@title@,Joe's Own Editor - Hacking," <header-src.html | cat - hacking-src.html footer-src.html > $@

list.html : header-src.html list-src.html footer-src.html
	sed "s,@title@,Joe's Own Editor - Commands," <header-src.html | cat - list-src.html footer-src.html > $@

options.html : header-src.html options-src.html footer-src.html
	sed "s,@title@,Joe's Own Editor - Options," <header-src.html | cat - options-src.html footer-src.html > $@
