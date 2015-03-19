<p>In 1988 WPI replaced the DEC-20 with an Encore Multimax which ran a
version of UNIX called Umax.  Originally, the system lacked USENET, so a
friend of mine, Larry Foard, wrote a local BBS system for it called bboard. 
Bboard had a terrible line editor, so I wrote a screen editor called 'e' for
writing BBS messages.  This is really the first version of JOE.  Other
hackers wrote 's'- an instant messaging program, and 'superwho'- a program
which drew a map show which terminal each student was logged in on.</p>

<p><a href="http://en.wikipedia.org/wiki/Encore_Computer">Here</a> is some
history of Encore.  The company's "Annex" terminal servers had longer history
than its computers.</p>

<p><a href="http://www.farviolet.com">Larry Foard</a> went on to write <a
href="http://en.wikipedia.org/wiki/MUSH">TinyMush</a>.

<p>Why not emacs or vi?  I couldn't stand emacs- it was always saying
"garbage collecting...  done.", used awkward weird keys, formatted my source
code the wrong way and basically just had an arrogant attitude.  It was
especially frustrating that Ctrl-H was not backspace.  I was certainly too
impatient to conform to it when I could just write my own editor.</p>

<p>JOE might never have been had I understood how nice vi
was at the time.  Luckily, the commands were not documented in the 'man'
page and both it and the UNIX line editors 'ex' and 'ed' did not work like
the line editors I was familiar with.  Specifically, I thought it was stupid
that these editor did not allow you to position the cursor past the ends of
lines (or files), forcing you to use the append command instead of the
insert command for these cases.</p>

<h3>e - 1988</h3>

<p>I wrote 'e' before I knew about the gap buffer data structure, but the
method I came up with was pretty close: the file is stored in a large array. 
A small array (maybe 1K bytes) has a copy of a small part of the larger
array.  Most edits occurred in the small array to keep the insert/delete cost
low.  When an edit is requested outside of the range of what is in the small
array, it is swapped into the large array and new part of large array
(centered around the cursor) is copied into the small array.</p>

<p>'e' was released to the general public in the USENET newsgroup
alt.sources in 1989 or 1990 (I don't remember which year, and Google's
alt.sources records are spotty for this period).  An announcement was posted
to comp.editors.  This version of JOE had an interruptible screen update
algorithm and worked pretty well over my 1200 baud modem.  Only VT100 and
VT52 terminals are supported.</p>

<h3>j</h3>

<p>People complained that 'e' was a synonym for vi, so I re-named the editor
'j' and released it on alt.sources again.  Somewhere around this time, I
replaced my buffer data structure with a gap buffer.  My gap buffer code
appeared in a Dr. Dobbs Journal article [find reference].</p>

<h3>joe0.0.0</h3>

<p>'j' turned out to be the name of a programming language, so someone
(either on alt.sources or comp.editors) suggested I call the editor JOE, for
Joe's Own Editor.</p>

<h3>joe1.0</h3>

<p>I rewrote the editor: now the data structure is a linked-list of gap
buffers which can be swapped to a file, so that the maximum file size is not
limited by the amount of virtual memory.  Also, the key-binding code was
created which allowed for the joerc file.  Also, this version of JOE
supports any terminal type with /etc/termcap support.</p>

<p>A lot of this work took place while I was living on the top floor of the
5 story "Griggs" building on Pleasant st., Worcester.  This was a bad area
of Worcester MA, but we learned that thieves are lazy: the first floor got
robbed all the time, but nobody would bother climbing 4 flights of stairs,
so we could basically leave the door unlocked.  There were 7 people and 7
cats living together.  My share of the rent was about $130 a month.  I
inherited two of the cats: Puck and Zack.</p>

<h3>joe2.0</h3>

<h3>joe2.2</h3>
<p>This version is ported to MS-DOS.</p>

<h3>joe2.8</h3>

<p>Last version from me until Joe 3.0.  In the intervening 9 years, I got
married, had two kids, got a real full time job, and bought a house.  Others
patched this version, fixing minor bugs and security holes.</p>


<h3>joe2.9</h3>

<p>Marec Grac and Vitezslav Samel put JOE on SourceForge.  They reformat the
code to the same standard as the Linux kernel, put it under source
control and switch to the standard 'configure' script.</p>

<h3>joe3.0 - April 2004</h3>

<p>I start working on the editor again.  I start by adding syntax
highlighting to the editor- this has been the most requested feature since
JOE 2.8.  Also I add UTF-8 support, with much guidance from Egmont
Koblinger (who is Hungarian).</p>

<p><a
href="http://sourceforge.net/project/showfiles.php?group_id=23475&package_id=192549">Here</a>
is the source to some very old versions of JOE, including 'E' and 'J'.</p>
