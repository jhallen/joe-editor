# Various editing tasks

### TAGS file search

JOE will search for a tags file in the current directory, and if none is
found there, it will open the tags file name given in the TAGS environment
variable.

### Completion and selection menus

You can hit TAB in just about any prompt to request JOE to complete the
word you are typing.  If JOE beeps, there are either no completions or many. 
As with the "bash" shell, hit TAB twice to bring up a list of all the
possibilities.  This list is actually a menu, but by default, the cursor
does not jump into it since it is usually easier to just type in your
selection. You can, however, jump into the menu window with ^K P (move to
previous window) and use the arrow keys and &lt;Enter&gt; to make your
selection.  Also in a menu, you can hit the first letter of any of the items
to make the cursor jump directly to it.  The ^T option menu works like
this.

If the menu is too large to fit in the window, you can hit Page Up and
Page Down to scroll it (even if you have not jumped into it).

TAB completion works in the search and replace prompts as well.  In this
case, JOE tries to complete the word based on the contents of the buffer. 
If you need search for the TAB character itself, you can enter it with `
TAB

Also, you can hit ESC &lt;Enter&gt; in a text window to request JOE to
complete the word you are typing.  As with the search prompt, JOE tries to
complete the word based on the contents of the buffer.  It will bring up a
menu of possibilities if you hit ESC &lt;Enter&gt; twice.

At the ^K ; (tags search) prompt, TAB completion uses the ctags file to
get its data

### Xterm Mouse support

There are two levels of mouse support.  The -mouse option enables the
first level, which will work with any stock Xterm.  If -joexterm is also
set, mouse support is enhanced, but you need a recent version of XTerm,
and it needs to be ./configured with the --enable-paste64 option.

When -mouse is set, you can:

* Left-click in a text window to set the cursor position.  Left-click in a different window to move the cursor to a different window.

* Select text with the mouse.  Left-click and drag to select some text- it
will be as if you had used ^KB and ^KK to mark it. Left-click (but don't
drag) to position the cursor somewhere else.  Middle click to copy the
selected text to the cursor- it will be as if you had hit ^KC.  If you drag
past the edge of the text window, the window will auto-scroll to select more
text.  Unfortunately, Xterm does not send any codes when the cursor is
outside of the Xterm frame itself, so this only works if the mouse is still
contained within the Xterm frame.  I've sent a patch to the Xterm maintainer
to improve this, but he has not taken it yet.

* Resize windows with the mouse: click and hold on a status line
dividing two windows to move it.

* Select menu entries (such as any completion menu or the ^T options
menu): click on the menu item to position the cursor on it.  Double-click on
a menu item to select it (same as hitting return with cursor on it).</li>

* If your mouse has a wheel, turning the wheel will scroll the window with
the cursor.

Unfortunately, when -mouse is selected, cut and paste between X windows
does not work as it normally does in a shell window (left-click and drag to
select, middle click to paste).  Instead, you have to hold the shift key
down to do this: shift-left-click and drag to select, and shift-middle click
to paste.  Note that pasting text into JOE this way has problems: any `
characters will get messed up because ` means quote the following control
character.  Also if auto-indent is enabled, pasted text will not be indented
properly.

When -joexterm is set (and you have ./configured Xterm with
--enable-paste64):

* Cut &amp; paste are properly integrated with X.  Text selected with
left-click-drag is available for pasting into other X windows (even if the
selected text is larger than the text window).  Text selected in other X
windows can be pasted into JOE with middle-click.  There are no problems
pasting text containing ` or with auto-indent.

--enable-paste64 allows an application program to communicate Base-64
encoded selection data to and from the Xterm.  The program has full control
over what is in the selection data and when it is received or sent.

### Multi-file search and replace

There are two new search and replace options:

<ul>
<li>'a': The search covers all loaded buffers.  So you can say:
<ul>
<li>joe *.c</li>
<li>^K F foo &lt;Enter&gt;</li>
<li>ra &lt;Enter&gt;</li>
<li>bar &lt;Enter&gt;</li>
</ul>to replace all instances of "foo" with "bar" in all .c files.
</li>
<li>'e': The search covers all files in the grep or make error list.  You
can use a UNIX command to generate a list of files and search and replace
through the list:
<ul>
<li>ESC g</li>
<li>grep -n foo f*.c &lt;Enter&gt;</li>
<li>^K F foo &lt;Enter&gt;</li>
<li>re &lt;Enter&gt;</li>
<li>bar &lt;Enter&gt;</li>
to replace all instances of "foo" with "bar" in all .c files which begin
with f.  You can also use "ls" and "find" instead of grep to create the file
list.
</ul>
</li>
</ul>

You can hit ^K H at the search and replace options prompt
to bring up a list of all search and replace options.

### Grep-find

Hit ESC g to bring up the prompt.  Enter a command which results in file
names with line numbers, for example: 'grep -n fred *.c'.  This will list all
instances of 'fred' in the *.c files.  You need the '-n' to get the line
numbers.

Now you can hit ESC space on one of the lines to jump to the selected
file.  Also, you can use ESC = and ESC - to step through each line.

### How do I deselect a highlighted block?

Make the block zero size by hitting ^K B ^K K.

### What if I hit ^K by accident?

Hit the space bar.  This runs an innocuous command (it shows the line
number on the status bar).

### Color Xterm support

JOE can make use of monochrome Xterm, 8-color Xterm, 16-color Xterm,
88-color Xterm and 256-color Xterm.  The number of colors which Xterm
supports is determined by which "configure" script options are set before
the Xterm source code is compiled.  The termcap or terminfo entry must
support how your Xterm is configured.  On my Slackware Linux distribution,
you have to set the TERM environment variable to one of these:

<ul>
<li>xterm</li>
<li>xterm-color</li>
<li>xterm-16color</li>
<li>xterm-88color</li>
<li>xterm-256color</li>
</ul>

If the termcap/terminfo entry is missing, you can add the "-assume_256color"
option to the joerc file.  Note that this was broken for terminfo in
versions of JOE below 3.4.

When it is working, the command: "joe -bg_text bg_222" should have a gray
background.

### UTF-8

JOE handles two classes of character sets: UTF-8 and byte coded (like
ISO-8859-1).  It can not yet handle other major classes such as UTF-16 or
GB2312. There are other restrictions: character sets must use LF (0x0A) or
CR-LF (0x0D - 0x0A) as line terminators, space must be 0x20 and tab must be
0x09. Basically, the files must be UNIX or MS-DOS compatible text files.

This means EBCDIC will not work properly (but you would need to handle fixed
record length lines anyway) and character sets which use CR terminated lines
(MACs) will not yet work.

The terminal and the file can have different encodings.  JOE will translate
between the two.  Currently, one of the two must be UTF-8 for translation to
work.

The character set for the terminal and the default character set assumed for
files is determined by the 'LC_ALL' environment variable (and if that's not
set, LC_CTYPE and LANG are also checked).

For example, if LC_ALL is set to:

	de_DE

Then the character set will be ISO-8859-1.

If LC_ALL is set to:

	de_DE.UTF-8

The character set will be UTF-8.

Hit ^T E to change the coding for the file.  Hit &lt;tab&gt; &lt;tab&gt; at this prompt
to get a list of available codings.  There are a number of built-in
character sets, plus you can install character sets in the ~/.joe/charmaps
and /usr/local/etc/joe/charmaps directories.

Check: /usr/share/i18n/charmaps for example character set files.  Only
byte oriented character sets will work.  Also, the file should not be
gzipped (all of the charmap file in /usr/share/i18n/charmaps on my computer
were compressed).  The parser is very bad, so basically the file has to look
exactly like the example one in /usr/local/etc/joe/charmaps.

You can hit ^K &lt;space&gt; to see the current character set.

You can hit ` x to enter a Unicode character if the file coding is UTF-8.

### Indenting blocks

Try ^K , and ^K .  These keys select the current block (based on
indentation) and shift it left or right by the amount in -istep and with the
character in -indentc.

### Goto matching delimiter

Hit Ctrl-G to jump between matching delimiters.  This works on both
character delimiters (like '(' and ')') and word delimiters for languages
like Pascal and Verilog which use "begin" and "end" to delimit blocks.  If a
word is not known, Ctrl-G starts a search with the word moved into the
search prompt.

For Ctrl-G to work on word delimiters, the cursor must be positioned
on the first letter of the word.  So in XML, if the cursor is on the &lt; in
&lt;foo&gt;, it will jump to the &gt;.  But if it is one the 'f', it will
jump to the matching &lt;/foo&gt;.  Likewise, in C, ^G will jump between #if, #else
and #endif, but you need to position the cursor on the letter,
not the '#'.

Ctrl-G is smart enough to skip delimiters found in quoted or
commented-out matter.  You need to tell JOE how your language indicates
this: see the __ftyperc__ file for examples of how this is done.

### Cursor position history

Hit ^K - and ^K = to step through different places of the loaded
files that you've visited.  This makes it easy to get back to your starting
place when you make an excursion to some other area of the file you're
editing.

### Selecting blocks

The "classic" way is to hit ^K B at the beginning and ^K K at the
end.  These set pointers called markb and markk.  Once these are set you
can jump to markb with ^\[ b and jump to markk with \[ k.

New way: hit Ctrl-rtarw (right arrow) to start selecting rightward. 
Each time you hit Ctrl-rtarw, the block is extended one more to the right. 
This uses a simple macro: "begin_marking,rtarw,toggle_marking".

Unfortunately, there is no standard way to get the keysequence given
by the terminal emulator when you hit Ctrl-rtarw.  Instead you have to
determine this sequence yourself and enter it directly in the joerc file. 
Some examples are given for Xterm and gnome-terminal.  Hit ` rtarw within
JOE to have the sequence shown on your screen.  Note that Putty uses ^\[ ^\[ \[
C which will not appear with ` rtarw (also ^\[
^\[ is set book mark, so you need to unbind it to do this in Putty).

Also you can hit Ctrl-delete to cut and Ctrl-insert to paste if the
sequence for these keys are known.

### Shell Windows

If you use Bash, you can hit: ` UP-ARROW and ` DOWN-ARROW to scroll through
Bash's history buffer.  Other keys work as well: try ` A to go to beginning
of line or ` E to go to end of line.  Unfortunately JOE only emulates a dumb
terminal, so you have to use a lot of imagination to do any editing beyond
hitting backspace.

In general, any character quoted with ` is sent to the shell.

Also sent to the shell: TAB, Backspace, Enter, ^C and ^D.

### Macros

A macro is a comma separated list of commands or named macros.  When the
macro is executed, each command is executed until either the end of the list
is reached, or one of the commands fails (non-zero return value from the
command).  Failed commands beep if you have beeps enabled (^T B).

### Macro don't stop modifier

Sometimes, you expect commands to sometimes fail, but want the rest of the
commands in the list to be executed anyway.  To mark a command which is
allowed to fail, postfix it with '!'.  For example, here a macro which hits
down page in the window above:

	prevw,pgdn!,nextw

If prevw fails, the macro is aborted as usual. Even if pgdn fails (already
at end of buffer), nextw will be executed so that the cursor is returned to
the original window.

### Macro repeat argument modifiers

Repeat arguments can be specified with ^K \\.  When a command is executed
with a repeat argument, it is repeatedly executed the specified number of
times.  If the repeat argument is negative, an opposite command (if one
exists) is executed instead.  For example, if you repeat "rtarw" -3 times,
"ltarw" will be repeated 3 times.  If a negative argument is given for a
command which does not have an opposite, the repeat argument is ignored.

Normally, if a repeat argument is specified for a macro, the macro is simply
repeated the given number of times.  If a negative argument is given, the
argument is ignored.

Sometimes you want to allow negative arguments for macros and have their
behavior modified.  To do this, postfix each command within the macro which
should be switched to its opposite for negative arguments with '-'.  For
example, here is the page down other window macro:

	prevw,pgdn-!,nextw

Now if you execute this with an argument of -2, it will be repeated twice,
but pgup will be executed instead of pgdn.  (not that several postfix
modifiers can be placed after each command).

Sometimes when a repeat argument is given to macro, you want only one of the
commands in the list to be repeated, not the entire macro.  This can be
indicated as follows:

	prevw,pgdn#!,nextw

If this is executed with an argument of 2, prevw is executed once, pgdn is
executed twice, and nextw is executed once.

Finally, even more complex semantics can be expressed with the "if" command:

	if~,"arg<0",then,
		ltarw,
	else,
		rtarw,
	endif

When the macro is executed, the "arg" math variable is set to the given
repeat argument.  The "argset" variable is set to true if the user set an
argument, even if it's 1.  If no argument was given, argset is false.

If any command in the list is postfixed with ~ (if above), the macro is not
repeated, even if there is an argument.  'arg' is still set to the given
repeat count, however.

### 'psh'/'query' interaction

The 'psh' command saves the ^KB and ^KK positions on a stack.  When the
macro completes, (or when the 'pop' command is called) the positions are
restored.

The 'query' command suspends macro execution until the current dialog is
complete.  It also suspends the automatic 'pop' which happens at the end
of a macro- so if the macro ends in a dialog you often want to call 'query'
to prevent the ^KB ^KK positions from being restored too early.

### Hex edit mode

When this mode is selected (either put -hex on the command line, or look for
"Hex edit mode" after hitting ^T), the buffer is displayed as a hex dump,
but all of the editing commands operate the same way.  It is most useful to
select overtype mode in conjunction with hex dump (hit ^T T).  Then typing
will not insert.

- To enter the hex byte 0xF8 type ` x F 8

- You can use ^KC to copy a block as usual.  If overtype mode is selected,
  the block will overwrite the destination data without changing the size of
  the file.  Otherwise it inserts.

- Hit ESC x byte &lt;Enter&gt;, to jump to a particular byte offset.  Hex values
  can be entered into this prompt like this: 0x2000.

- Search, incremental search, and search &amp; replace all operate as usual.
