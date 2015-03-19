<h3>Coroutines in JOE</h3>

<p>JOE 3.6 now uses co-routines to help reduce the amount of event-driven
code which has to be written.  All previous versions of JOE were entirely in
the event driven coding style, which made user dialog code a chore to write. 
Now, with the help of co-routines, dialog code can be written as old
fashioned blocking code which is much cleaner, even though JOE can have
multiple un-finished dialogs on the screen.</p>

<p>The coroutine library has only three (now four) functions:</p>

<table width="100%" cellspacing=20 border=0 cellpadding=0> <colgroup> <col width="300"> <tbody>

<tr valign="top"><td>int co_call(int (*func)(va_list args),...);</td><td>Call a function
as a co-routine.</td></tr>

<tr valign="top"><td>int co_yield(Coroutine *t, int val);</td><td>Suspend current
co-routine and return to caller with given return value.</td></tr>

<tr valign="top"><td>int co_resume(Coroutine *t, int val);</td><td>Suspend current
co-routine and resume specified one with given return value.</td></tr>

<tr valign="top"><td>int co_suspend(Coroutine *u, int val);</td><td>Suspend
current co-routine (remembering its suspended invoking coroutines) and
resume the top-level.  The current coroutine is stored in u.  When u is
resumed and returns, the suspended chain is resumed (the resumer is
continued when the chain tries to continue the top level.</td></tr>

</tbody> </table>

<p>co_call() invokes 'func' as a co-routine- it will run on its own stack.
'func' is called immediately upon the call to co_call. All of the arguments
after func are passed as a va_list to the function.  co_call calls va_start
and va_end, 'func' should call va_arg once for each passed argument (see
stdarg.h).  The co-routine should not yield before it has finished reading
all of the arguments, otherwise va_end will be called early.  When 'func'
returns, the co-routine is destroyed and co_call() returns with the return
value of 'func'.  If the co-routine yields by calling co_yield, co_call
returns with the return value specified in the call to co_yield.  Note that
co_call only ever returns once for a particular call.</p>

<p>co_yield() suspends the current co-routine and returns with the specified
return value to the caller (either to co_call or co_resume).  co_yield()
writes information about the co-routine to the 'Coroutine' structure whose
address is passed to co_yield.  The Coroutine structure's address is used as
a handle for the co-routine and can be passed to co_resume().</p>

<p>co_resume() suspends the current co-routine (henceforth called the
original co-routine) and resumes the specified co-routine.  When the
specified co-routine returns (when the function given in the co_call that
created the co-routine returns), the original co-routine is continued and
co_resume() returns with the function's return value. If the specified
co-routine yields, the original co-routine is continued and co_resume()
returns with the return value specified in the call to co_yield.</p>

<p>co_suspend() is like co_resume, except that the top-level task is resumed
instead of the calling co-routine.  This has the effect of suspending the
entire chain of co-routines which invoked the caller of co_suspend. 
Co_suspend stores the suspended co_routine in another co_routine.  That
co_routine will resume this one when it finally returns.</p>

<p>The co-routine library is structured this way (which is different from
the usual way where you get a co-routine handle immediately upon creation,
for example see Knuth) to encourage proper stack un-winding.  Basically, a
co-routine can not be forcibly destroyed.  Instead, each created co-routine
is only destroyed when the initial function returns.</p>

<p>The co-routine library is based on ucontext.h and the getcontext,
swapcontext, makecontext and setcontext system calls.  If the system does
not have these calls, it uses setjmp/longjmp tricks in a highly portable
way: the stacks for each co-routine are allocated off the top of the main
stack.  The main stack grows after each allocation.  The stacks are never
freed, but they are reused.  This works for any machine with a single normal
stack.  It will not work on IA64 (two stacks) or Cray (the stack is really a
linked list).</p>

<p>I was originally going to use simple cooperative multi-threading, with
only two basic calls: sleep_on() and wake_up() (inspired by old versions of
the Linux kernel) and a top-level scheduler.  Sleep_on would suspend the
current thread, and create a new thread which runs just the scheduler. 
Wake_up would mark a thread as ready to run, and on the next return to the
scheduler, it would be run.</p>

<p>This turns out to not be powerful enough to implement JOE's keyboard
macro language, at least without much rewriting and inelegance. It could be
done, but the keyboard macro player would have to feed events into the
top-level scheduler or would have to be integrated with it, whereas in older
versions of JOE (even without co-routines) the macro player just calls the
edit functions (more or less) directly.  The problem is that sleep_on
(called by a function asking for user input) would suspend the task all the
way up to the scheduler including the macro player, thus preventing the
macro player from simulating user input.</p>

<p>On the other hand with real co-routines, the macro player will call each
edit function as a co-routine (which I like to think of as a task with a
very clear return point).  If the edit function has to yield, it returns
immediately to the macro player for the next macro step.  If the yield is
due to a request for input, the macro player supplies it by calling the
recorded edit functions.  If the macro wants to allow the user to provide
input (using the "query" function, also known as an interactive macro), it
can itself yield (since it is also running in a coroutine) back to the
top-level edit loop (in actuality, the macro player calls a function which
calls co_suspend() to do this).  The macro player is continued when the user
finishes supplying the input (hits the return key at a prompt).  Several
interactive macros can be running at once with this scheme.</p>

<h3>Semi-Automatic Variables</h3>
<p>JOE 3.6 uses "semi-automatic" variables for strings.  There is a global
"object-stack", accessed through these functions:</p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="300">
<tbody>
<tr valign="top"><td>void *obj_malloc(int size);</td><td>Allocate a block of memory.</td></tr>
<tr valign="top"><td>void *obj_realloc(void *ptr, int size);</td><td>Change allocation size.</td></tr>
<tr valign="top"><td>void obj_free(void *ptr);</td><td>Free a specified block and all newer blocks.</td></tr>
<tr valign="top"><td>void obj_perm(void *ptr);</td><td>Mark a block as permanent, which in effect moves the block from the stack to the heap.</td></tr>
<tr valign="top"><td>int obj_len(void *ptr);</td><td>Access hidden "length" part of block (string length).</td></tr>
<tr valign="top"><td>int obj_size(void *ptr);</td><td>Access hidden "size" part of block (allocation size).</td></tr>
</tbody>
</table>

<p>These functions remember the order of allocation.  When an allocated
block is freed, it and all newer blocks are freed as well.  For example, in:</p>

<pre>
unsigned char *a = obj_malloc(10);
unsigned char *b = obj_malloc(10);
unsigned char *c = obj_malloc(10);
unsigned char *d = obj_malloc(10);

obj_free(b);
</pre>

<p>b, c, and d are all freed, but a remains allocated.  The top-level edit
loop is structured to call obj_free after every editor command:</p>

<pre>
unsigned char *gc = obj_malloc(1);
execmd(...); /* Execute edit commands */
obj_free(gc); /* Collect garbage */
</pre>

<p>Thus, you rarely have to call obj_free- the only time you might want to do
it is if you are calling obj_malloc in a loop.  This could cause a large
amount of memory to be allocated, so obj_free() should be called at the
end of each loop iteration.</p>

<p>obj_perm() marks an object as a heap object instead of a stack object. 
When a heap object is freed, only itself is freed- the stack is not
modified.  Also when the stack is freed, heap objects are not touched.</p>

<h3>Variable Strings</h3>

<p>JOE uses dynamic strings built on top of the global object stack
functions. These strings automatically resize themselves to fit their
contents.  They also know their own length, which allows you to have NULs in
the middle of a string.  They also always have a terminating NUL to make
them compatible with C-strings.  In fact they have (almost) the same type as
C-strings: unsigned char *.  The length is hidden in the obj_malloc data
structure.  These functions are provided:</p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="400">
<tbody>
<tr valign="top"><td>unsigned char *vsmk(int prealloc);</td><td>Allocate a zero length string, but with enough space to grow to the specified length.</td></tr>
<tr valign="top"><td>int vslen(unsigned char *s);</td><td>Return string length or 0 if s is NULL.</td></tr>
<tr valign="top"><td>unsigned char *vsensure(unsigned char *s, int len);</td><td>Make sure there is enough space for a string of the specified length.</td></tr>
<tr valign="top"><td>unsigned char *vstrunc(unsigned char *s, int len);</td><td>Set length of string: the string is truncated or extended with spaces to the specified length.</td></tr>
<tr valign="top"><td>unsigned char *vsncpy(unsigned char *s,int offset,unsigned char *block, int block_size);</td><td>Copy a memory block to a string.  The string is extended with spaces if offset &gt; string length.</td></tr>
<tr valign="top"><td>unsigned char *vsdupz(unsigned char *s);</td><td>Duplicate a z-string to a variable string.</td></tr>
<tr valign="top"><td>unsigned char *vsdup(unsigned char *s);</td><td>Duplicate a variable string.</td></tr>
<tr valign="top"><td>unsigned char *vscpyz(unsigned char *s,unsigned char *z);</td><td>Copy a z-string to a variable string.</td></tr>
<tr valign="top"><td>unsigned char *vscpy(unsigned char *s,unsigned char *block,int size);</td><td>Copy a memory block to a variable string.</td></tr>
<tr valign="top"><td>unsigned char *vscatz(unsigned char *s,unsigned char *z);</td><td>Append a z-string to a variable string.</td></tr>
<tr valign="top"><td>unsigned char *vscat(unsigned char *s,unsigned char *block,int size);</td><td>Append a memory block to a variable string.</td></tr>
<tr valign="top"><td>unsigned char *vsadd(unsigned char *s,unsigned char c);</td><td>Append a single character to a string.</td></tr>
<tr valign="top"><td>unsigned char *vsfmt(unsigned char *s,int offset,const char *format,...);</td><td>Sprintf() to a variable string.</td></tr>
<tr valign="top"><td>unsigned char *vsgets(unsigned char **sp,FILE *f);</td><td>fgets() to a variable string (returns NULL on EOF, does not put '\n' in the string).</td></tr>
</tbody>
</table>

<p>Note that it is OK to pass NULL in place of 's' in any of the above
functions.  In this case, a new string is allocated.</p>

<p>Most of these function return the address of the variable string, in case
it was modified.  One exception is vsgets()- it takes an address of a
variable holding the string.  It returns the string address, but it also
writes it to the variable directly.</p>

<p>Many of the above functions take a memory block, which is specified by an
address and a length.  These macros can be used to help construct these
arguments:</p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="350">
<tbody>
<tr valign="top"><td>#define sc(s) (s), (sizeof(s)-1)</td><td>For string constants.</td></tr>
<tr valign="top"><td>#define sz(s) (s), zlen(s)</td><td>For C-strings (zlen is the same as strlen).</td></tr>
<tr valign="top"><td>#define sv(s) (s), vslen(s)</td><td>For variable strings.</td></tr>
</tbody>
</table>


<p>So for example, vscatz is defined as:</p>

<pre>
unsigned char *vscatz(unsigned char *s, unsigned char *z);
{
	return vsncpy(sv(s), sz(z));
}
</pre>

<h3>Variable arrays of variable strings</h3>

<p>JOE also semi-automatic variable arrays of variable strings.  These
functions are provided:</p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="400">
<tbody>
<tr valign="top"><td>unsigned char *vamk(int prealloc);</td><td>Allocate a zero length array, but with enough space to grow to the specified length.</td></tr>
<tr valign="top"><td>void varm(unsigned char **a);</td><td>Free an array.</td></tr>
<tr valign="top"><td>int valen(unsigned char *a);</td><td>Return array length or 0 if a is NULL.</td></tr>
<tr valign="top"><td>unsigned char *vaensure(unsigned char *s, int len);</td><td>Make sure there is enough space for an array of the specified length.</td></tr>
<tr valign="top"><td>unsigned char *vatrunc(unsigned char *s, int len);</td><td>Set length of array: the array is truncated or extended with NULLs to the specified length.</td></tr>
<tr valign="top"><td>unsigned char *vaadd(unsigned char **a,unsigned char *s);</td><td>Append a single string to an array.</td></tr>
<tr valign="top"><td>unsigned char *vasort(unsigned char **a,int len);</td><td>Sort an array.</td></tr>
<tr valign="top"><td>unsigned char *vawords(unsigned char **a,unsigned char *s,int len, unsigned char *sep, int seplen);</td><td>Convert a word list in s (words separated by characters given in sep/seplen) into an array of strings.</td></tr>
<tr valign="top"><td>void vaperm(unsigned char **a);</td><td>Mark an array as preferment (heap instead of stack).</td></tr>
</tbody>
</table>

<p>When an array is marked as permanent, any strings in the array also
marked as permanent, and any string later added to the array are also marked
as permanent.</p>


<h3>Edit Buffers</h3>
<p>API:
</p>
<p>  Look at the comments in b.h for more information.
</p>
<p>  B *bfind(unsigned char *name);
		Load disk file into memory buffer 'B'.
</p>
<p>  bsave(P *p,unsigned char *name,int size);
		Write size bytes from buffer beginning at p to disk file
</p>
<p>  brm(b);	Free data structure
</p>
<p>Once you have a B you can access the characters in it via P pointers (which
are like C++ STL iterators).
</p>
<p>  B *b = bfind("foo");	Load file into memory
</p>
<p>  P *p = pdup(b-&gt;bof);	Get pointer to beginning of file (duplicate
			b-&gt;bof which is a P).
</p>
<p>  prm(p);		Free p when we're done with it.
</p>
<p>  int c=brch(p);	Get character at p.</p>
<p>  int c=pgetc(p);	Get character at p and advance it.</p>
<p>  int c=prgetc(p);	Retract p, then return character at it.</p>
</p>
<p>    - These return -1 (NO_MORE_DATA) if you try to read end of file or
      before beginning of file.
</p>
<p>    - A pointer can point to any character of the file and right after the
      end of the file.
</p>
<p>    - For UTF-8 files, character can be between 0 and 0x7FFFFFFF
</p>
<p>  Publicly readable members of P:</p>
<p>	p-&gt;byte		The byte offset into the buffer</p>
<p>	p-&gt;line		The line number</p>
<p>	p-&gt;xcol		If P is the cursor, this is the column number
			where the cursor will be displayed on the screen
			(which does not have to match the column of the
			character at P).</p>
<p>  Some predicates:</p>
<p>	pisbof(p);	True if pointer is at beginning of buffer</p>
<p>	piseof(p);	True if pointer is at end of buffer</p>
<p>	pisbol(p);	True if pointer is at beginning of line</p>
<p>	piseol(p);	True if pointer is at end of line</p>
<p>	pisbow(p);	True if pointer is at beginning of a word</p>
<p>	piseow(p);	True if pointer is at end of a word</p>

<p>  More information about character at p:</p>
<p>	piscol(p);	Get column number of character at p.
</p>
<p>  Some other ways of moving a P through a B:
</p>
<p>	pnextl(p);	Go to beginning of next line</p>
<p>	pprevl(p);	Go to end of previous line</p>
<p>	pfwrd(p,int n);	Move forward n bytes</p>
<p>	pbkwd(p,int n);	Move backward n bytes</p>
<p>	p_goto_bof(p);</p>
<p>	p_goto_eof(p);</p>
<p>	p_goto_bol(p);</p>
<p>	p_goto_eol(p);</p>
<p>	pset(p,q);	Move p to same position as q.
</p>
<p>	pline(p,n);	Goto to beginning of a specific line.</p>
<p>	pgoto(p,n);	Goto a specific byte position.</p>
<p>	pfind(P,unsigned char *s,int len);
			Fast Boyer-Moore search forward.</p>
<p>	prfind(P,unsigned char *s,int len);
			Fast Boyer-Moore search backward.</p>
<p>		These are very fast- they look at low level
	data structure and don't go through pgetc().  Boyer-Moore
	allows you to skip over many characters without reading
	them, so you can get something like O(n/len).</p>
<p>  Some facts:</p>
<p>    Local operations are fast: pgetc(), prgetc().</p>
<p>    Copy is fast: pset().</p>
<p>    pline() and pgoto() are slower, but look for the closest existing
    P to start from.</p>
<p>    The column number is stored in P, but it is only updated if
    it is easy to do so.  If it's hard (like you crossed a line
    boundary backward) it's marked as invalid.  piscol() then has
    to recalculate it.
</p>
<p>  Modifying a buffer:
</p>
<p>    binsc(p,int c);		Insert character at p.</p>
<p>    bdel(P *from,P *to);	Delete character between two Ps.</p>
<p>  Note that when you insert or delete, all of the Ps after the insertion/
  deletion point are adjusted so that they continue to point to the same
  character before the insert or delete.
</p>
<p>  Insert and Delete create undo records.
</p>
<p>  Insert and Delete set dirty flags on lines which are currently being
  displayed on the screen, so that when you return to the edit loop, these
  lines automatically get redrawn.
</p>
<p>Internal:
</p>
<p>  An edit buffer is made up of a doubly-linked list of fixed sized (4 KB)
gap buffers.  A gap buffer has two parts: a ~16 byte header, which is always
in memory, and the actual buffer, which can be paged out to a swap file (a
vfile- see vfile.h).  A gap buffer consists of three regions: text before
the gap, the gap and text after the gap (which always goes all the way to
the end of buffer). (hole and ehole in the header indicate the gap
position).  The size of the gap may be 0 (which is the case when a file is
first loaded).  Gap buffers are fast for small inserts and deletes when the
cursor is at the gap (for delete you just adjust a pointer, for insert you
copy the data into gap).  When you reposition the cursor, you have to move
the gap before any inserts or deletes occur.  If you had only one window and
a single large gap buffer for the file, you could always keep the gap at the
cursor- the right-arrow key copies one character across the gap.
</p>
<p>  Of course for edits which span gap buffers or which are larger than a gap
buffer, you get a big mess of gap buffer splitting and merging plus
doubly-linked list splicing.
</p>
<p>  Still, this buffer method is quite fast: you never have to do large memory
moves since the gap buffers are limited in size.  To help search for line
numbers, the number of newlines '\n's contained in each gap buffer is stored
in the header.  Reads are fast as long as you have a P at the place you
want to read from, which is almost always the case.
</p>
<p>  It should be possible to quickly load files by mapping them directly into
memory (using mmap()) and treating each 4KB page as a gap buffer with 0 size
gap.  When page is modified, use copy-on-write to move the page into the
swap file (change pointer in header).  This is not done now.  Instead the
file is copied when loaded.
</p>
<h3>Windowing System</h3>
<p>There is a tiny object-oriented windowing system built into JOE.  This is
the class hierarchy:
</p>
<p>SCRN
  A optimizing terminal screen driver (very similar to 'curses').
    has a pointer to a CAP, which has the terminal capabilities read
    from termcap or terminfo.
</p>
<p>    writes output to screen with calls to the macro ttputc(). (tty.c is the
    actual interface to the tty device).
</p>
<p>    cpos()    - set cursor position</p>
<p>    outatr()  - draw a character at a screen position with attributes</p>
<p>    eraeol()  - erase from some position to the end of the line</p>
<p>SCREEN
  Contains list of windows on the screen (W *topwin).
</p>
<p>  Points to window with focus (W *curwin).
</p>
<p>  Contains pointer to a 'SCRN', the tty driver for the particular terminal
  type.
</p>
<p>W
  A window on a screen.
</p>
<p>  Has position and size of window.
</p>
<p>  Has:
    void *object- pointer to a structure which inherits window (W should
    really be a base class for these objects- since C doesn't have this
    concept, a pointer to the derived class is used instead- the derived
    class has a pointer to the base class: it's called 'parent').
</p>
<p>      Currently this is one of:</p>
<p>        BW *    a text buffer window (screen update code is here.)</p>
<p>        QW *    query window (single character yes/no questions)</p>
<p>        MENU *  file selection menu</p>
<p>      BW * is inherited by (in the same way that a BW inherits a W):</p>
<p>        PW *    a single line prompt window (file name prompts)</p>
<p>        TW *    a text buffer window (main editing window).</p>
<p>    WATOM *watom- Gives type of this window.  The WATOM structure has
    pointers to virtual member functions.</p>
<p>    KBD *kbd- The keyboard handler for this window.  When window has
    focus, keyboard events are sent to this object.  When key sequences
    are recognized, macros bound to them are invoked.</p>
<p>Some window are operators on others.  For example ^K E, load a file into a
window prompt operates on a text window.  If you hit tab, a file selection
menu which operates on the prompt window appears below this.  When a window
is the target of operator windows is killed, the operators are killed also.</p>
<p>Currently all windows are currently the width of the screen (should be fixed
in the future).  The windows are in a big circular list (think of a big loop
of paper).  The screen is small window onto this list.  So unlike emacs, you
can have windows which exist, but which are not on the screen.</p>
<p>^K N and ^K P move the cursor to next or previous window.  If the next
window is off the screen it is moved onto the screen, along with any
operator windows are target it.</p>
<h3>MACROS</h3>
<p>- add something here.
</p>
<h3>Screen update</h3>
<p>- add something here.
</p>
<h3>Syntax Highlighter</h3>

<p>There are two parts to the syntax highlighter: the parser (in syntax.c)
and the line attribute database (in lattr.c).  The parser is simple enough:
it is a finite state machine interpreter.  You supply a line of text and the
starting state, and it returns the starting state of the next line and an
array containing the color to be used for each character.  Each state has a
color.  The line is scanned left to right, character by character: the state
and the character index a table, which gives an action, which includes the
next state.</p>

<p>The action specifies the next state, and also indicates whether some
previous N characters (including the current) should be "recolored" with the
color of the next state, or whether the color of the current state should be
used for the character.  The action also indicates whether to move on to the
next character, or to stay put and re-evaluate the current character with
the new state.  This leads to simpler hand-coding of the state machine, but
can create an infinite loop.  The action also indicates if the character
should be added to a string buffer.  The string buffer can be used (when
specified by an action) as a key for a hash table look-up (typically filled
with key-words).  If there is a match, the hash-table entry supplies the
action instead of the state table.</p>

<p>This parser is fast and simple and powerful enough to lexically analyze
more than 40 languages.  However a few enhancements were made to improve
both the class of languages which can be highlighted and to improve the ease
of writing the state machine.  To support "here" documents in perl and sh,
as well as to simplify matching delimiters (like open with close
parenthesis), a string (from the string buffer) can be stored along with the
state.  This can be matched against the string buffer.  So first the
terminating word from the here document is stored in the string, and then
the state machine looks forward until it finds this string to determine the
end of the here document.</p>

<p>The state is only a single integer, not an entire stack.  The parser can
therefore not handle recursive grammars. Unfortunately, some languages have
recursive elements in their lexical structure.  One way to deal with this is
to support only a very limited recursion depth by tediously repeating a set
of states in the hand written state machine N times, where N is your maximum
recursion depth.  To ease writing this type of thing, and also to allow
state machine to be reused in cases where one language is embedded within
another (PHP in HTML, for example), a macro system was added to the state
machine loader.  Recursion is allowed in the macro calls, but is limited to
a depth of 5.</p>

<h3>Line attribute cache</h3>

<p>The second part of the syntax highlighter is the line attribute cache.
In the original implementation, the state of the first line of each window
is maintained and the parser is invoked during the screen update algorithm
to highlight all of the lines on the screen.  This works well when the
window remains in place or when it scrolls forward through the file. 
However, if the window scrolls backward you may have to reparse the entire
file or from the nearest previous window if there happens to be one.  This
can be slow for large files, and certainly wastes a lot of CPU time when
scrolling backward through a file.</p>

<p>One attempt to solve this was to instead of starting at the beginning of
the file, to instead assume that the line 200 lines (a configurable option)
back was at the initial state, parse forward from there and hope for the
best.  Sometimes the state machine would find itself back in the proper
state, but not always- to the point where most syntaxes had specified to
just always reparse from the beginning of the file.</p>

<p>Finally, the line attribute cache was added.  It stores the parser state
at the beginning of each line, from the beginning of the file to last line
actually displayed on the screen.  This solves the scrolling problem, but
introduces a new one: how to keep the line attribute cache up to date.  This
turns out to not be so difficult.  First of all, the states are stored in a
gap-buffer, which makes it easy to perform insertions and deletions. 
Whenever lines are inserted or deleted in the file, the line attribute cache
is expanded or contracted to keep in sync.  Now, when any change is made to
the file, we reparse from just before the start of the change until the
state of the beginning of a line again matches the corresponding entry in
the cache.  If a match is found, the rest of the cache is assumed to be
correct.  Some changes will require a large amount of reparsing, but many
will not.  In any case, the the highlighting on the screen is always
correct.</p>

<h3>Files</h3>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="100">
<tbody>
<tr valign="top"><td>b.c</td><td>Text buffer management</td></tr>
<tr valign="top"><td>blocks.c</td><td>Library: fast memcpy() functions (necessary on really old versions of UNIX).</td></tr>
<tr valign="top"><td>bw.c</td><td>A class: text buffer window (screen update code is here)</td></tr>
<tr valign="top"><td>charmap.c</td><td>UNICODE to 8-bit conversion functions</td></tr>
<tr valign="top"><td>cmd.c</td><td>Table of user edit functions</td></tr>
<tr valign="top"><td>coroutine.c</td><td>Coroutine library</td></tr>
<tr valign="top"><td>dir.c</td><td>Directory reading functions (for old UNIXs).</td></tr>
<tr valign="top"><td>hash.c</td><td>Library: automatically expanding hash table functions.</td></tr>
<tr valign="top"><td>help.c</td><td>Implement the on-line help window</td></tr>
<tr valign="top"><td>i18n.c</td><td>Unicode character type information database</td></tr>
<tr valign="top"><td>kbd.c</td><td>Keymap data structure (keysequence to macro bindings).</td></tr>
<tr valign="top"><td>lattr.c</td><td>Line attribute cache</td></tr>
<tr valign="top"><td>macro.c</td><td>Keyboard and joerc file macros</td></tr>
<tr valign="top"><td>main.c</td><td>Has main() and top level edit loop</td></tr>
<tr valign="top"><td>menu.c</td><td>A class: menu windows</td></tr>
<tr valign="top"><td>obj.c</td><td>Semi-automatic objects, plus dynamic string and array library</td></tr>
<tr valign="top"><td>path.c</td><td>Library: file name and path manipulation functions</td></tr>
<tr valign="top"><td>poshist.c</td><td>Cursor position history</td></tr>
<tr valign="top"><td>pw.c</td><td>A class: prompt windows</td></tr>
<tr valign="top"><td>queue.c</td><td>Library: doubly linked lists</td></tr>
<tr valign="top"><td>qw.c</td><td>A class: single-key query windows</td></tr>
<tr valign="top"><td>rc.c</td><td>Joerc file loader</td></tr>
<tr valign="top"><td>regex.c</td><td>Regular expressions</td></tr>
<tr valign="top"><td>scrn.c</td><td>Terminal update functions (like curses)</td></tr>
<tr valign="top"><td>selinux.c</td><td>Secure linux (for RedHat) functions</td></tr>
<tr valign="top"><td>syntax.c</td><td>Syntax highlighter</td></tr>
<tr valign="top"><td>tab.c</td><td>Tab completion for file selection prompt</td></tr>
<tr valign="top"><td>termcap.c</td><td>Load terminal capabilities from /etc/termcap file or terminfo database</td></tr>
<tr valign="top"><td>tw.c</td><td>A class: main text editing window</td></tr>
<tr valign="top"><td>ublock.c</td><td>User edit functions: block moves</td></tr>
<tr valign="top"><td>uedit.c</td><td>User edit functions: basic edit functions</td></tr>
<tr valign="top"><td>uerror.c</td><td>User edit functions: parse compiler error messages and goto next error, previous error</td></tr>
<tr valign="top"><td>ufile.c</td><td>User edit functions: load and save file</td></tr>
<tr valign="top"><td>uformat.c</td><td>User edit functions: paragraph formatting, centering</td></tr>
<tr valign="top"><td>uisrch.c</td><td>User edit functions: incremental search</td></tr>
<tr valign="top"><td>umath.c</td><td>User edit functions: calculator</td></tr>
<tr valign="top"><td>undo.c</td><td>Undo system</td></tr>
<tr valign="top"><td>usearch.c</td><td>User edit functions: search &amp; replace</td></tr>
<tr valign="top"><td>ushell.c</td><td>User edit functions: subshell</td></tr>
<tr valign="top"><td>utag.c</td><td>User edit functions: tags file search</td></tr>
<tr valign="top"><td>utf8.c</td><td>UTF-8 to unicode coding functions</td></tr>
<tr valign="top"><td>utils.c</td><td>Misc. utilities</td></tr>
<tr valign="top"><td>vfile.c</td><td>Library: virtual memory functions (allows you to edit files larger than memory)</td></tr>
<tr valign="top"><td>w.c</td><td>A class: base class for all windows</td></tr>
</tbody>
</table>
