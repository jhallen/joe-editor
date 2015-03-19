<h3>WordStar - 1984</h3>

<p>At age 15, I got my first high-tech job at a small company called Video
Data Systems in Hauppauge Long Island New York.  The founder of VDS was Bill
Leventer.  There I wrote 6800 and 6809 assembly language code for video
titling systems on a Motorola Exorciser development system.  The code was
pretty sophisticated- I wrote an multi-tasking operating system for 6809. 
The Exorciser had a line editor called 'E', an assembler called 'RASM' and
two 8 in. floppy drives.  I remember that it took half an hour to link 16K
of code and that we always printed the assembly language lists on an old GE
chain printer.</p>

<p>One program I wrote was the screen editor for the CG-2000 character
generator.  This was ridiculously simple- it had fixed width lines stored in
a big array.  But it did have formatting- specifically, you could set fonts,
colors and heights of lines.</p>

<p>Here is a CG-2000 I found on EBay: <img src="cg2000.jpg"
alt="CG-2000"/></p>

<a href="http://www.exorciser.net/excorciser_en.htm">Here</a> is the Motorola
Exorcizer it was developed on.  Remember: type MAID, followed by E800;G to
boot MDOS.</a>

<p>At some point, we upgraded to an IBM PC XT and switched to the AD2500
6809 assembler and used WordStar in "non-document mode" for the editor.  I
remember being annoyed with WordStar because it did not remember the column
position when you moved the cursor between lines.  On the other hand, I
thought the interruptible screen update algorithm was pretty cool- it was
certainly a total necessity on the 4.77 MHz 8088 based system.  I also learned
C on the IBM PC, using Microsoft's MS-DOS C compiler.</p>

<p>There is a nice history of WordStar <a href="http://www.wordstar.org/wordstar/history/history.htm">here.</a>
</p>

<p>I had gotten pretty familiar with many of the microcomputers available in
this time period, including: Commodore-64 (I wrote a remote terminal for VDS
for this, in half BASIC half 6502 assembly language), Atari-800 (it had a
real OS, unlike the C-64) and Radio Shack model 16.  My mom used the Radio
Shack model 16 for her home business: it had an 8 MB hard drive and ran
RM/COS (to run RealWorld accounting) or TRS-DOS (it could run UNIX as well,
but we didn't have it).  There was some assembly language editor that I
don't remember which ran on either RM/COS or TRS-DOS which moved the cursor
the same way that JOE does- it kept the column constant, even when moving
from a long line to a short line, and without actually adding any text to
the file.</p>

<img src="trs80_16.jpg" alt="TRS-80 Model 16"/>

<h3>The DecSystem 20 - 1986</h3>

<p>In 1986 I went to college at Worcester Polytechnic Institute, majoring in
electrical engineering.  WPI was one of the great hacker schools, with many
students writing programs both before and during my time there.  WPI had a
Decsystem-20 mainframe computer which ran TOPS-20.  This was a famous 36-bit
computer, especially in the LISP world, because each word could hold two
18-bit addresses and was therefore perfect for a LISP CONS cell.  It had an
awesome mail program, which was both mail and USENET news in one program. 
The shell had ESC-completion and question-mark help (if you hit ? at the end
of syntactically correct command, it said "(confirm)").  It had EDT, TECO,
EMACS written in TECO, and a bunch of other editors, many of them student
written.  Despite all that, I tended to write in macro using a line editor. 
I tried the C compiler, but it was really weird- it used 9-bit bytes!  Mind
you, characters were normally 7-bits on this machine, with 5 characters
packed into a 36-bit word.  I remember that the operating system manual
which was written by someone at Stanford (who?) was always missing from the
library- borrowed months at a time by hackers.</p>

<p>This <a href="http://www.columbia.edu/kermit/dec20.html">page</a> gives
you a good feel for what the DEC-20 was like.</p>

<h3>The Encore Multimax - 1988</h3>

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
