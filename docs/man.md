# JOE - Joe's Own Editor

[TOC]

## Syntax

__joe [global-options] [ [local-options] filename ]...__
<br>
__jstar [global-options] [ [local-options] filename ]...__
<br>
__jmacs [global-options] [ [local-options] filename ]...__
<br>
__rjoe [global-options] [ [local-options] filename ]...__
<br>
__jpico [global-options] [ [local-options] filename ]...__

## Description

JOE is a powerful console screen editor.  It has a "mode-less" user
interface which is similar to many user-friendly PC editors.  Users of
Micro-Pro's WordStar or Borland's "Turbo" languages will feel at home.  JOE
is a full featured UNIX screen-editor though, and has many features for
editing programs and text.

JOE also emulates several other editors.  JSTAR is a close imitation of
WordStar with many "JOE" extensions.  JPICO is a close imitation of the
Pine mailing system's PICO editor, but with many extensions and
improvements.  JMACS is a GNU-EMACS imitation.  RJOE is a restricted
version of JOE, which allows you to edit only the files specified on the
command line.

Although JOE is actually five different editors, it still requires only one
executable, but one with five different names.  The name of the editor with
an "rc" appended gives the name of JOE's initialization file, which
determines the personality of the editor.

JOE is free software;  you can distribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation.  JOE is available over the Internet from
<http://www.sourceforge.net/projects/joe-editor>.

## Usage

To start the editor, type __joe__ followed by zero or more names of files
you want to edit.  Each file name may be preceded by a local option setting
(see the local options table which follows).  Other global options, which
apply to the editor as a whole, may also be placed on the command line (see
the global options table which follows).  If you are editing a new file, you
can either give the name of the new file when you invoke the editor, or in
the editor when you save the new file.  A modified syntax for file names is
provided to allow you to edit program output, standard input/output, or
sections of files or devices.  See the section [Filenames](#filenames) below for
details.

Once you are in the editor, you can type in text and use special
control-character sequences to perform other editing tasks.  To find out
what the control-character sequences are, read the rest of this man page or
type __^K H__ for help in the editor.

Now for some obscure computer-lore:

The __^__ means that you hold down the __Control__ key while pressing
the following key (the same way the __Shift__ key works for uppercase
letters).  A number of control-key sequences are duplicated on other keys,
so that you don't need to press the control key: __ESC__ will work in
place of __^\[__, __Del__ will work in place of __^?__, __Backspace__
will work in place of __^H__, __Tab__ will work in place of __^I__,
__Return__ or __Enter__ will work in place of __^M__ and
__Linefeed__ will work in place of __^J__.  Some keyboards may give you
trouble with some control keys.  __^___, __^^__ and __^@__ can usually
be entered without pressing shift (i.e., try __^-__, __^6__ and
__^2__).  Other keyboards may reassign these to other keys.  Try:
__^.__, __^,__ and __^/__.  __^SPACE__ can usually be used in place
of __^@__.  __^\\__ and __^\]__ are interpreted by many communication
programs, including telnet and kermit.  Usually you just hit the key twice
to get it to pass through the communication program.

Once you have typed __^K H__, the first help window appears at the top of
the screen.  You can continue to enter and edit text while the help window
is on.  To page through other topics, hit __^\[,__ and __^\[.__ (that is,
__ESC ,__ and __ESC .__).  Use __^K H__ to dismiss the help window.

You can customize the keyboard layout, the help screens and a number of
behavior defaults by copying JOE's initialization file (usually
__/home/jhallen/etc/joe/joerc__) to __.joerc__ in your home directory and then
by modifying it.  See the section [joerc](#joerc) below.

To have JOE used as your default editor for e-mail and News, you need to set
the __EDITOR__ and __VISUAL__ environment variables in your shell
initialization file (__.cshrc__ or __.profile__) to refer to JOE (joe
usually resides as __/home/jhallen/bin/joe__).

There are a number of other obscure invocation parameters which may have to
be set, particularly if your terminal screen is not updating as you think it
should.  See the section [Environment variables](#evariables) below.

## Command Line Options

Only a subset of the options are shown here.  Please refer to this
[complete list of options](http://sourceforge.net/p/joe-editor/mercurial/ci/default/tree/docs/options.md).

The following options may be specified on the command line:

* -orphan

When this option is active, extra files on the command line will be placed
in orphaned buffers instead of in extra windows.  This is useful for when
joe is trying to be emacs.

* +nnn

The cursor starts on the specified line.

* -crlf

Joe uses CR-LF as the end of line sequence instead of just LF.  This is for
editing MS-DOS or VMS files.

* -rdonly

The file is read only.

These options can also be specified in the joerc file.  They can be set
depending on the file-name extension.  Programs (.c, .h or .p extension)
usually have autoindent enabled.  Wordwrap is enabled on other files, but rc
files have it disabled.

### Mode command

Many options can be controlled with the __^T__ menu.  This menu is defined
in the joerc file.  Each option in the __^T__ menu just executes a macro.  Usually
the macro is the mode command.  You can execute the mode command directly with:

	ESC x mode <enter>

Hit tab for a completion list of all options.

## Basic Editing

When you type characters into the editor, they are normally inserted into
the file being edited (or appended to the file if the cursor is at the end
of the file).  This is the normal operating mode of the editor.  If you want
to replace some existing text, you have to delete the old text before or
after you type in the replacement text.  The __Backspace__ key can be used
for deleting text: move the cursor to right after the text you want to
delete and hit __Backspace__ a number of times.

Hit the __Enter__ or __Return__ key to insert a line-break.  For 
example, if the cursor was in the middle of a line and you hit __Return__, 
the line would be split into two lines with the cursor appearing at the 
beginning of the second line.  Hit __Backspace__ at the beginning of a 
line to eliminate a line-break.

Use the arrow keys to move around the file.  If your keyboard doesn't have
arrow keys (or if they don't work for some reason), use __^F__ to move
forwards (right), __^B__ to move backwards (left), __^P__ to move to the
previous line (up), and __^N__ to move to the next line (down).  The right
and left arrow keys simply move forwards or backwards one character at a
time through the text: if you're at the beginning of a line and
you press left-arrow, you will end up at the end of the previous line.  The
up and down arrow keys move forwards and backwards by enough characters so
that the cursor appears in the same column that it was in on the original
line.

If you want to indent the text you enter, you can use the __TAB__ key. 
This inserts a special control character which makes the characters which
follow it begin at the next TAB STOP.  TAB STOPS normally occur every 8
columns, but this can be changed with the __^T D__ command.  PASCAL and C
programmers often set TAB STOPS on every 4 columns.

If for some reason your terminal screen gets messed up (for example, if
you receive a mail notice from biff), you can have the editor refresh the
screen by hitting __^R__.

There are many other keys for deleting text and moving around the file.  For 
example, hit __^D__ to delete the character the cursor is on instead of 
deleting backwards like __Backspace__.  __^D__ will also delete a 
line-break if the cursor is at the end of a line.  Type __^Y__ to delete 
the entire line the cursor is on or __^J__ to delete just from the cursor 
to the end of the line.

Hit __^A__ to move the cursor to the beginning of the line it's on.  Hit 
__^E__ to move the cursor to the end of the line.  Hit __^U__ or 
__^V__ for scrolling the cursor up or down 1/2 a screen's worth.  
"Scrolling" means that the text on the screen moves, but the cursor stays at 
the same place relative to the screen.  Hit __^K U__ or __^K V__ to move 
the cursor to the beginning or the end of the file.  Look at the help 
screens in the editor to find even more delete and movement commands.

If you make a mistake, you can hit __^\___ to "undo" it.  On most keyboards
you hit just __^-__ to get __^\___, but on some you might have to hold
both the __Shift__ and __Control__ keys down at the same time to get it.
If you "undo" too much, you can "redo" the changes back into existence by
hitting __^^__ (type this with just __^6__ on most keyboards).

## Cursor position history

If you were editing in one place within the file, and you then temporarily 
had to look or edit some other place within the file, you can get back to 
the original place by hitting __^K -__.  This command actually returns you 
to the last place you made a change in the file.  You can step through a 
history of places with __^K -__ and __^K =__, in the same way you can 
step through the history of changes with the "undo" and "redo" commands.

## Save and exit

When you are done editing the file, hit __^K X__ to exit the editor.  You
will be prompted for a file name if you hadn't already named the file you
were editing.

When you edit a file, you actually edit only a copy of the file.  So if you
decide that you don't want the changes you made to a file during a
particular edit session, you can hit __^C__ to exit the editor without
saving them.

If you edit a file and save the changes, a "backup" copy of that file is 
created in the current directory, with a ~ appended to the name, which 
contains the original version of the file.

## Word wrap and formatting

If you type past the right edge of the screen in a C language or PASCAL 
file, the screen will scroll to the right to follow the cursor.  If you type 
past the right edge of the screen in a normal file (one whose name doesn't 
end in .c, .h or .p), JOE will automatically wrap the last word onto the 
next line so that you don't have to hit __Return__.  This is called 
word-wrap mode.  Word-wrap can be turned on or off with the __^T W__ 
command.  JOE's initialization file is usually set up so that this mode is 
automatically turned on for all non-program files.  See the section below on 
the [joerc](#joerc) file to change this and other defaults.

Aside for Word-wrap mode, JOE does not automatically keep paragraphs 
formatted like some word-processors.  Instead, if you need a paragraph to be 
reformatted, hit __^K J__.  This command "fills in" the paragraph that the 
cursor is in, fitting as many words in a line as is possible.  A paragraph, 
in this case, is a block of text separated above and below by a blank line.

The margins which JOE uses for paragraph formatting and word-wrap can be set
with the __^T L__ and __^T R__ commands.  If the left margin is set to
a value other than 1, then when you start typing at the beginning of a line,
the cursor will immediately jump to the left margin.

If you want to center a line within the margins, use the __^K A__
command.

## Over-type mode

Sometimes it's tiresome to have to delete old text before or after you 
insert new text.  This happens, for example, when you are changing a table 
and you want to maintain the column position of the right side of the table.  
When this occurs, you can put the editor in over-type mode with __^T T__.  
When the editor is in this mode, the characters you type in replace existing 
characters, in the way an idealized typewriter would.  Also, __Backspace__ 
simply moves left instead of deleting the character to the left, when it's 
not at the end or beginning of a line.  Over-type mode is not the natural 
way of dealing with text electronically, so you should go back to 
insert-mode as soon as possible by typing __^T T__ again. 

If you need to insert while you're in over-type mode, hit __^@__.  This
inserts a single SPACE into the text.

## Control and Meta characters

Each character is represented by a number.  For example, the number for 'A'
is 65 and the number for '1' is 49.  All of the characters which you
normally see have numbers in the range of 32 - 126 (this particular
arbitrary assignment between characters and numbers is called the ASCII
character set).  The numbers outside of this range, from 0 to 255, aren't
usually displayed, but sometimes have other special meanings.  The number
10, for example, is used for the line-breaks.  You can enter these special,
non-displayed __control characters__ by first hitting __^\[ '__ and then
hitting a character in the range __@ A B C ... X Y Z [ ^ ] \\ \___ to get
the number 0 - 31, and ? to get 127.  For example, if you hit __^\[ ' J__,
you'll insert a line-break character, or if you hit __^\[ ' I__, you'll insert
a TAB character (which does the same thing the TAB key does).  A useful
control character to enter is 12 (__^\[ ' L__), which causes most printers to
advance to the top of the page.  You'll notice that JOE displays this
character as an underlined L.  You can enter the characters above 127, the
__meta characters__, by first hitting __^\___.  This adds 128
to the next (possibly control) character entered.  JOE displays characters
above 128 in inverse-video.  Some foreign languages, which have more letters
than English, use the meta characters for the rest of their alphabet.  You
have to put the editor in __ASIS__ mode (described later) to have these
passed untranslated to the terminal.

## Prompts

If you hit __TAB__ at any file name prompt, joe will attempt to complete
the name you entered as much as possible.  If it couldn't complete the
entire name, because there are more than one possible completions, joe
beeps.  If you hit __TAB__ again, joe list the completions.  You can use
the arrow keys to move around this directory menu and press RETURN or SPACE
to select an item.  If you press the first letter of one of the directory
entries, it will be selected, or if more than one entry has the same first
letter, the cursor will jump between those entries.  If you select a
subdirectory or .., the directory name is appended to the prompt and the new
directory is loaded into the menu.  You can hit Backspace to go back to the
previous directory.

Most prompts record a history of the responses you give them.  You can hit
up and down arrow to step through these histories.

Prompts are actually single line windows with no status line, so you can use
any editing command that you normally use on text within the prompts.  The
prompt history is actually just other lines of the same "prompt file".  Thus
you can can search backwards though the prompt history with the normal __^K
F__ command if you want.

Since prompts are windows, you can also switch out of them with __^K P__
and __^K N__.

## Completion and selection menus

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
If you need search for the TAB character itself, you can enter it with ESC '
TAB

Also, you can hit ESC &lt;Enter&gt; in a text window to request JOE to
complete the word you are typing.  As with the search prompt, JOE tries to
complete the word based on the contents of the buffer.  It will bring up a
menu of possibilities if you hit ESC &lt;Enter&gt; twice.


## Where am I?


Hit __^K SPACE__ to have JOE report the line number, column number, and
byte number on the last line of the screen.  The number associated with the
character the cursor is on (its ASCII code) is also shown.  You can have the
line number and/or column number always displayed on the status line by
setting placing the appropriate escape sequences in the status line setup
strings.  Edit the joerc file for details.

## What if I hit ^K by accident?

Hit the space bar.  This runs an innocuous command (it shows the line
number on the status bar).

## File operations


You can hit __^K D__ to save the current file (possibly under a different
name from what the file was called originally).  After the file is saved,
you can hit __^K E__ to edit a different file.

If you want to save only a selected section of the file, see the section on
[Blocks](#blocks) below.

If you want to include another file in the file you're editing, use __^K
R__ to insert it.

## Temporarily suspending the editor


If you need to temporarily stop the editor and go back to the shell, hit 
__^K Z__.  You might want to do this to stop whatever you're editing and 
answer an e-mail message or read this man page, for example.  You have to 
type __fg__ or __exit__ (you'll be told which when you hit __^K Z__) 
to return to the editor.  

## Searching for text


Hit __^K F__ to have the editor search forwards or backwards for a text 
fragment (__string__) for you.  You will be prompted for the text to 
search for.  After you hit __Return__, you are prompted to enter options.  
You can just hit __Return__ again to have the editor immediately search 
forwards for the text, or you can enter one or more of these options:

* __b__

Search backwards instead of forwards.

* __i__

Treat uppercase and lower case letters as the same when searching.  Normally
uppercase and lowercase letters are considered to be different.

* __nnn__

(where __nnn__ is a number) If you enter a number, JOE searches for the 
Nth occurrence of the text.  This is useful for going to specific places in 
files structured in some regular manner.

* __r__

Replace text.  If you enter the __r__ option, then you will be further
prompted for replacement text.  Each time the editor finds the search text,
you will be prompted as to whether you want to replace the found search text
with the replacement text.  You hit: __y__ to replace the text and then
find the next occurrence, __n__ to not replace this text, but to then find
the next occurrence, __r__ to replace all of the remaining occurrences of
the search text in the remainder of the file without asking for confirmation
(subject to the __nnn__ option above), or __^C__ to stop searching and
replacing.

You can hit __^L__ to repeat the previous search.

You can hit ^K H at the search and replace options prompt
to bring up a list of all search and replace options.

## Multi-file search and replace

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

## Regular Expressions

A number of special character sequences may be entered as search
text:

* __\\\*__

This finds zero or more characters.  For example, if you give __A\\\*B__ as
the search text, JOE will try to find an A followed by any number of characters
and then a B.

* __\?__

This finds exactly one character.  For example, if you give __A\?B__ as
the search text, JOE will find AXB, but not AB or AXXB.

* __\^ \$__

These match the beginning and end of a line.  For example, if you give
__\^test\$__, then JOE with find __test__ on a line by itself.

* __\< \\\>__

These match the beginning and end of a word.  For example, if you give
__\<\\\*is\\\*\\\>__, then joe will find whole words which have the
sub-string __is__ within them.

* __\\\[...]__

This matches any single character which appears within the brackets.  For
example, if __\[Tt]his__ is entered as the search string, then JOE finds
both __This__ and __this__.  Ranges of characters can be entered within
the brackets.  For example, __\[A-Z]__ finds any uppercase letter.  If
the first character given in the brackets is __^__, then JOE tries to find
any character not given in the the brackets.

* __\c__

This works like __\*__, but matches a balanced C-language expression. 
For example, if you search for __malloc(\c)__, then JOE will find all
function calls to __malloc__, even if there was a __)__ within the
parenthesis.

* __\\\+__

This finds zero or more of the character which immediately follows the
__\+__.  For example, if you give __\\\+\\\[ \]__, where the
characters within the brackets are both SPACE and TAB, then JOE will find
whitespace.

* __\\__

Matches a single \\.

* __\n__

This finds the special end-of-line or line-break character.


A number of special character sequences may also be given in the replacement
string:

* __\\&__

This gets replaced by the text which matched the search string.  For
example, if the search string was __\<\\\*\\\>__, which matches words, and
you give __"&"__, then joe will put quote marks around words.

* __\0 - \9__

These get replaced with the text which matched the Nth __\\\*__, __\?__,
__\\\+__, __\c__, or __\\\[...]__ in the search string.

* __\\\\__

Use this if you need to put a __\\__ in the replacement string.

* __\n__

Use this if you need to put a line-break in the replacement string.

Some examples:

Suppose you have a list of addresses, each on a separate line, which starts 
with "Address:" and has each element separated by commas.  Like so:

Address: S. Holmes, 221b Baker St., London, England

If you wanted to rearrange the list, to get the country first, then the 
city, then the person's name, and then the address, you could do this:

Type __^K F__ to start the search, and type:

__Address:\\\*,\\\*,\\\*,\\\*\$__

to match "Address:", the four comma-separated elements, and then the end of 
the line.  When asked for options, you would type __r__ to replace the 
string, and then type:

__Address:\3,\2,\0,\1__

To shuffle the information the way you want it. After hitting return, the 
search would begin, and the sample line would be changed to:

Address: England, London, S. Holmes, 221b Baker St.

## Goto matching delimiter

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



<a name="blocks"></a>

## Blocks

If you want to move, copy, save or delete a specific section of text, you 
can do it with highlighted blocks.  First, move the cursor to the start of 
the section of text you want to work on, and press __^K B__.  Then move 
the cursor to the character just after the end of the text you want to 
affect and press __^K K__.  The text between the __^K B__ and __^K K__ 
should become highlighted.  Now you can move your cursor to someplace else 
in your document and press __^K M__ to move the highlighted text there.  
You can press __^K C__ to make a copy of the highlighted text and insert 
it to where the cursor is positioned.  __^K Y__ to deletes the highlighted 
text.  __^K W__, writes the highlighted text to a file.  

A very useful command is __^K /__, which filters a block of text through a
unix command.  For example, if you select a list of words with __^K B__
and __^K K__, and then type __^K / sort__, the list of words will be
sorted.  Another useful unix command for __^K /__, is __tr__.  If you
type __^K / tr a-z A-Z__, then all of the letters in the highlighted block
will be converted to uppercase.

### How do I deselect a highlighted block?

After you are finished with some block operations, you can just leave the 
highlighting on if you don't mind it (but don't accidently hit __^K Y__).  If
it really bothers you, however, just hit  __^K B ^K K__, to turn the highlighting off.  

### Selecting blocks

The classic way is to hit ^K B at the beginning and ^K K at the
end.  These set pointers called markb and markk.  Once these are set you
can jump to markb with ^\[ b and jump to markk with \[ k.

New way: hit Ctrl-rtarw (right arrow) to start selecting rightward. 
Each time you hit Ctrl-rtarw, the block is extended one more to the right. 
This uses a simple macro: "begin_marking,rtarw,toggle_marking".

Unfortunately, there is no standard way to get the keysequence given
by the terminal emulator when you hit Ctrl-rtarw.  Instead you have to
determine this sequence yourself and enter it directly in the joerc file. 
Some examples are given for Xterm and gnome-terminal.  Hit ESC ' rtarw within
JOE to have the sequence shown on your screen.  Note that Putty uses ^\[ ^\[ \[
C which will not appear with ESC ' rtarw (also ^\[
^\[ is set book mark, so you need to unbind it to do this in Putty).

Also you can hit Ctrl-delete to cut and Ctrl-insert to paste if the
sequence for these keys are known.

### Indenting program blocks

Auto-indent mode toggled with the __^T I__ command.  The
__joerc__ is normally set up so that files with names ending with .p, .c
or .h have auto-indent mode enabled.  When auto-indent mode is enabled and
you hit __Return__, the cursor will be placed in the same column that the
first non-SPACE/TAB character was in on the original line.

You can use the __^K ,__ and __^K .__ commands to shift a block of text 
to the left or right.  If no highlighting is set when you give these 
commands, the program block the cursor is located in will be selected, and 
will be moved by subsequent __^K ,__ and __^K .__ commands.  The number 
of columns these commands shift by can be set through a __^T__
option.

## Windows

You can edit more than one file at the same time or edit two or more
different places of the same file.  To do this, hit __^K O__, to split the
screen into two windows.  Use __^K P__ or __^K N__ to move the cursor
into the top window or the lower window.  Use __^K E__ to edit a new
file in one of the windows.  A window will go away when you save the file
with __^K X__ or abort the file with __^C__.  If you abort a file which
exists in two windows, one of the window goes away, not the file.

You can hit __^K O__ within a window to create even more windows.  If you 
have too many windows on the screen, but you don't want to eliminate them, 
you can hit __^K I__.  This will show only the window the cursor is in, or 
if there was only one window on the screen to begin with, try to fit all 
hidden windows on the screen.  If there are more windows than can fit on 
the screen, you can hit __^K N__ on the bottom-most window or __^K P__ 
on the top-most window to get to them.  

If you gave more than one file name to JOE on the command line, each file 
will be placed in a different window.  

You can change the height of the windows with the __^K G__ and __^K T__
commands.

## Keyboard macros 


Macros allow you to record a series of keystrokes and replay them with the 
press of two keys.  This is useful to automate repetitive tasks.  To start a 
macro recording, hit __^K \[__ followed by a number from 0 to 9.  The 
status line will display (Macro n recording...).  Now, type in the series of 
keystrokes that you want to be able to repeat.  The commands you type will 
have their usual effectu. Hit __^K ]__ to stop recording the macro.  Hit 
__^K__ followed by the number you recorded the macro in to execute one 
iteration of the key-strokes.   

For example, if you want to put "**" in front of a number of lines, you can 
type:

__^K \[ 0 ^A \*\* __<down arrow\> __^K ]__


Which starts the macro recording, moves the cursor to the beginning of the 
line, inserts "**", moves the cursor down one line, and then ends the 
recording. Since we included the key-strokes needed to position the cursor 
on the next line, we can repeatedly use this macro without having to move 
the cursor ourselves, something you should always keep in mind when 
recording a macro.

If you find that the macro you are recording itself has a repeated set of
key-strokes in it, you can record a macro within the macro, as long as you
use a different macro number.  Also you can execute previously recorded
macros from within new macros. 


### Repeat

You can use the repeat command, __^K \\__, to repeat a macro, or any other
edit command or even a normal character, a specified number of times.  Hit
__^K \\__, type in the number of times you want the command repeated and
press __Return__.  The next edit command you now give will be repeated
that many times.
For example, to delete the next 20 lines of text, type:

__^K \\ 20__<return>__^Y__

## Macros and commands

A macro is a comma separated list of commands.  When the macro is executed,
each command is executed until either the end of the list is reached, or one
of the commands fails (non-zero return value from the command).  Failed
commands beep if you have beeps enabled (^T B).

Hit __^\[ D__ to insert the current set of keyboard macros as text into the
current buffer.

### Command prompt

Execute a macro directly at the command prompt.  Hit __ESC x__.  Hit tab
at this prompt for a completion list of all available commands.

Here is a [complete list of commands](http://sourceforge.net/p/joe-editor/mercurial/ci/default/tree/docs/list.md).

### Define your own

You can bind macros to key sequences or define your own named macros in the
joerc file.

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
but pgup will be executed instead of pgdn.  (note that several postfix
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



## Rectangle mode

Type __^T X__ to have __^K B__ and __^K K__ select rectangular blocks
instead of stream-of-text blocks.  This mode is useful for moving, copying,
deleting or saving columns of text.  You can also filter columns of text
with the __^K /__ command- if you want to sort a column, for example.  The
insert file command, __^K R__ is also affected.

When rectangle mode is selected, over-type mode is also useful
(__^T T__).  When over-type mode is selected, rectangles will replace
existing text instead of getting inserted before it.  Also the delete block
command (__^K Y__) will clear the selected rectangle with SPACEs and TABs
instead of deleting it.  Over-type mode is especially useful for the filter
block command (__^K /__), since it will maintain the original width of the
selected column.

## Tags search

If you are editing a large C program with many source files, you can use the 
__ctags__ program to generate a __tags__ file.  This file contains a 
list of program symbols and the files and positions where the symbols are 
defined. 


First, create the tags file with the "ctags" program.  For example:

	ctags *.c *.h

This will create a file called "tags" in the current directory.

JOE looks for the "tags" file in the current directory.  If there is none,
it will try to open the file specified by the TAGS environment variable.

Paths in the tags file are always relative to location of the tags file
itself.

The tags file contains a list of identifier definition locations in one of
these formats:

	identifier filename /search-expression/[;comments]

	identifier filename ?search-expression?[;comments]

	identifier filename line-number[;comments]

Some versions of ctags include class-names in the identifiers:

	class::member

In this case, JOE will match on any of these strings:

	member
	::member
	class::member

Some versions of ctags include a filename in the identifier:

	filename:identifier

In this case JOE will only find the identifier if the buffer name matches
the filename.

The search-expression is a vi regular expression, but JOE only supports the
following special characters:

	^ at the beginning means expression starts at beginning of line

	$ at the end means expression ends at end of line

	\x quote x (suppress meaning of /, ?, ^ or $)

Type ^K ; to bring up a tags search prompt.  If the cursor had been on an
identifier, the prompt is pre-loaded with it.  TAB completion works in this
prompt.

When you hit return, the tags search commences:

If there is one and only one match, JOE will jump directly to the
definition.

If there are multiple matches, then the behavior is controlled by the
notagsmenu option.  If notagsmenu is enabled JOE jumps to the first
definition.  If you hit ^K ; again before hitting any other keys, JOE jumps
to the next definition, and so on.  The "tagjump" command also performs this
function.

If notagsmenu is disabled, JOE brings up a menu of all the matches.  You
select the one you want and JOE jumps to it.  If you hit ^K ; again before
hitting any other keys, the same menu re-appears with the cursor left in the
original location.

You can hit ^K - to move the cusor back to the original location before the
tags search (often ^C will work as well).

Since __^K ;__ loads  the definition file into the current window, you
probably want to split the window first with __^K O__, to have both the
original file and the definition file loaded.

## Shell windows

Hit __^K '__ to run a command shell in one of JOE's windows.  When the
cursor is at the end of a shell window (use __^K V__ if it's not),
whatever you type is passed to the shell instead of the window.  Any output
from the shell or from commands executed in the shell is appended to the
shell window (the cursor will follow this output if it's at the end of the
shell window).  This command is useful for recording the results of shell
commands- for example the output of __make__, the result of __grep__ping
a set of files for a string, or directory listings from __FTP__ sessions. 
Besides typeable characters, the keys ^C, Backspace, DEL, Return and ^D are
passed to the shell.  Type the shell __exit__ command to stop recording
shell output.  If you press __^C__ in a shell window, when the cursor is
not at the end of the window, the shell is __kill__ed.

If you use Bash, you can hit: ESC ' UP-ARROW and ESC ' DOWN-ARROW to scroll through
Bash's history buffer.  Other keys work as well: try ESC ' ^A to go to beginning
of line or ESC ' ^E to go to end of line.  Unfortunately JOE only emulates a dumb
terminal, so you have to use a lot of imagination to do any editing beyond
hitting backspace.

In general, any character quoted with ESC ' is sent to the shell.

Also sent to the shell: TAB, Backspace, Enter, ^C and ^D.

<a name="popup"></a>
## Pop-up shell windows

Hit F1 - F4 to open and switch between shell windows.

The terminal emulator is improved so that when you type "man ls" it's
formatted correctly (it's improved enough so that some interactive programs
will work in it).  Even so, the shell window is still an edit buffer.

The old shell window (with no terminal emulation) still exists: use ^K ' to
invoke it as usual.  This is useful to see control sequences emitted by a
program.

More of the keys get passed to the running program in pop-up shell windows
compared with the older one.  There is a :shell section of the joerc file to
control which ones.  In particular arrow keys and Ctrl-C are passed to the
program.  It means you can easily step through bash history with the arrow
keys, or abort programs the normal way with Ctrl-C.

On the other hand, loss of Ctrl-C means it's less obvious how to close the
window.  One way is to move the cursor off of the shell data entry point
(with Ctrl-P), and then hit Ctrl-C.  Another is to hit ^K Q.  Finally, you
can type 'pop' at the command prompt.

If you need to pass a key to the shell that JOE normally uses, quote it.  For
example, if you invoke "emacs -nw" in the shell window, you can exit it with:

	ESC ' ^X ^C

or, if you use ` for the quote character:

	` ^X ^C


To quickly position the cursor back to the point where data is entered into
the shell, hit ^K V.

When you open a shell window, a JOE-specific startup-script is sourced. 
It's located in /etc/joe/shell.sh (also /etc/joe/shell.csh).  It contains
some aliases which allow you to control JOE with fake shell commands.  I
have these commands so far:

Command       | Action
--------------|--------
clear         | erase shell window (delete buffer contents)
joe file      | edit a file in joe
math 1+2      | evaluate equation using JOE's calculator
cd xyz        | change directory, keep JOE up to date
markb         | same as ^KB
markk         | same as ^KK
mark command  | execute shell command, mark it's output
parse command | execute shell command, parse it's output for file names and line numbers (for find or grep)
parser command| execute shell command, parse it's output for errors (for gcc)
release       | release parsed errors
pop           | dismiss shell window (same as ^K Q)

These work by emitting an escape sequence recognized by the terminal
emulator: ESC { joe_macro }.  When this is received, the macro is executed. 
For security, only macros defined in the joerc file which begin with
"shell_" can be executed this way.

### Use cases

Pop-up shell windows have a number of nice use cases:

* Use it to browse manual pages

	Hit F1 and type "man fopen".  Use 'b' ('u') and space to control
	_more_ (or _less_) while viewing the manual.  You can leave the manual
	on the screen in one window while editing in another window.

* Use it to switch directories

	Hit F1 and navigate to the directory while using _cd_.  Once
	you are in the right place, hit ^K E to load a file (or type "edit file"
	from the shell).

* Use it in conjuction with the error parser to find files

	Hit F1 and navigate to a directory.  Use grep or find (or both)
	to generate a list of files):

~~~~
		parse grep -n FIXME *.c
~~~~
	Or:

~~~~
		markb; find . | xargs grep -n FIXME; markk; parse
~~~~

	(Note that you can't say this:

~~~~
		parse find . | xargs grep -n FIXME
~~~~

	...the issue is that only the words to the left of the pipe symbol
	are passed as arguments to the parse command).

	Now use ^P to position the cursor on one of the lines of the list. 
	Hit ESC SPACE to have JOE edit the file and jump to the specified
	line (also you can use ESC - and ESC = to step through the list).

* Use it in conjuction with search and replace to edit many files

	Once JOE has a list of files (from above), use search and replace
	with the 'e' option to visit all of them:

~~~~
		^K F
		   Find: <text>
		   Options: re
		   Replace: <replacement text>
~~~~

* Build your project

	Easily capture errors from a build with:

~~~~
		parserr make
~~~~

	Hit ESC = and ESC - to step through the errors.


### How it works..

* There is a new mode "ansi".  (ESC x mode ansi).  When this mode is
enabled, the screen updater hides escape sequences which are in the
buffer.  Otherwise you get a big mess from the sequences surrounding
colored output from 'ls'.

* There is a new built-in syntax: "ansi".  (^T y ansi).  This syntax
parses the ANSI color control sequences so that text gets colored.

* There is a terminal emulator to interpret control sequences from the
shell program.  It emulates a terminal by modifying the contents of an
edit buffer.

* When the edit window is resized we tell the shell by issuing the
TIOCSSIZE or TIOCSWINSZ ioctl.  This way, the program running in the
shell knows the window size.

## Compiler and grep/find parsers

JOE has two parsers which can be used to generate the error list (list of
file names / line numbers).

The "parserr" command parses the entire buffer, or if the block is set, just
the highighted block for compiler error messages.  The messages should be in
this format:

	<junk> file.name <junk> line-number <junk> : <junk>

The file name needs to be made of numbers, letters, '/', '.' and '-'.  It
must have at leat one '.' in it.  There needs to be a colon somewhere after
the line number.  Lines not in this format are ignored.

THe "gparse' command parses the entire buffer, or if the block is set, just
the highlighted block for a list of filenames or filenames with line numbers
from "grep -n", "find" and similar programs.

	filename

	filename:<junk>

	filename:line-number:<junk>

Once JOE has the error list, there are a number of things you can do with
it:

* Visit the files/locations in the list with ESC - and ESC =

* Search and replace across all files in the list by using the 'e' search
  and replace option.

* Clear the list by using the "release" command.

Also, you can use ESC space ('jump' command) to parse the line the cursor is
on and jump to the parsed filename and line number.  'jump' uses the
grep/find parser unless 'parserr' had been previously issued in the buffer.

## Grep-find

Hit ESC g to bring up the prompt.  Enter a command which results in file
names with line numbers, for example: 'grep -n fred *.c'.  This will list all
instances of 'fred' in the *.c files.  You need the '-n' to get the line
numbers.

Now you can hit ESC space on one of the lines to jump to the selected
file.  Also, you can use ESC = and ESC - to step through each line.

## Compile

Hit ESC c to save all modified files and then bring up the compile prompt. 
Enter the command you want to use for the compiler (typically "make").  The
compiler will run in a shell window.  When it's complete, the results are
parsed.

If there are any errors or warnings from the compiler you can hit ESC space
on one of the lines to jump to the selected file.  Also, you can use ESC =
and ESC - to step through each line.



<a name="evariables"></a>
## Environment variables 

Please refer to this [complete list of environment variabls](http://sourceforge.net/p/joe-editor/mercurial/ci/default/tree/docs/options.md#env).

For JOE to operate correctly, a number of other environment settings must be 
correct.  The throughput (baud rate) of the connection between the computer 
and your terminal must be set correctly for JOE to update the screen 
smoothly and allow typeahead to defer the screen update.  Use the __stty 
nnn__ command to set this.  You want to set it as close as possible to 
actual throughput of the connection.  For example, if you are connected via 
a 1200 baud modem, you want to use this value for __stty__.  If you are 
connected via 14.4k modem, but the terminal server you are connected to 
connects to the computer a 9600 baud, you want to set your speed as 9600 
baud.  The special baud rate of 38400 or __extb__ is used to indicate that 
you have a very-high speed connection, such as a memory mapped console or an 
X-window terminal emulator.  If you can't use __stty__ to set the actual 
throughput (perhaps because of a modem communicating with the computer at a 
different rate than it's communicating over the phone line), you can put a 
numeric value in the __BAUD__ environment variable instead (use __setenv 
BAUD 9600__ for csh or __BAUD=9600; export BAUD__ for sh).  

The __TERM__ environment variable must be set to the type of terminal
you're using.  If the size (number of lines/columns) of your terminal is
different from what is reported in the TERMCAP or TERMINFO entry, you can
set this with the __stty rows nn cols nn__ command, or by setting the
__LINES__ and __COLUMNS__ environment variables.

JOE normally expects that flow control between the computer and your
terminal to use ^S/^Q handshaking (i.e., if the computer is sending
characters too fast for your terminal, your terminal sends ^S to stop the
output and ^Q to restart it).  If the flow control uses out-of-band or
hardware handshaking or if your terminal is fast enough to always keep up
with the computer output and you wish to map ^S/^Q to edit commands, you can
set the environment variable __NOXON__ to have JOE attempt to turn off
^S/^Q handshaking.  If the connection between the computer and your terminal
uses no handshaking and your terminal is not fast enough to keep up with the
output of the computer, you can set the environment variable __DOPADDING__
to have __JOE__ slow down the output by interspersing PAD characters
between the terminal screen update sequences.

<a name="filenames"></a>
## Filenames

Wherever JOE expects you to enter a file name, whether on the command line
or in prompts within the editor, you may also type:



* !command

Read or write data to or from a shell command.  For example,
use __joe '!ls'__ to get a copy of your directory listing to edit or from
within the editor use __^K D !mail jhallen@world.std.com__ to send the
file being edited to me.

* &gt;&gt;filename

Use this to have JOE append the edited text to the end of the file 
"filename."

* filename,START,SIZE

Use this to access a fixed section of a file or device.  __START__ and
__SIZE__ may be entered in decimal (ex.: 123) octal (ex.: 0777) or
hexadecimal (ex.: 0xFF).  For example, use __joe /dev/fd0,508,2__ to edit
bytes 508 and 509 of the first floppy drive in Linux.

* <blank>

Use this to get input from the standard input or to write output to the
standard output.  For example, you can put joe in a pipe of commands:
__quota -v | joe | mail root__, if you want to complain about your low
quota.


<br><br>
<a name="joerc"></a>
## The joerc file

^T options, the help screens and the key-sequence to editor command
bindings are all defined in JOE's initialization file.  If you make a copy
of this file (which normally resides in __/home/jhallen/etc/joe/joerc__) to
__$HOME/.joerc__, you can customize these setting to your liking.  The
syntax of the initialization file should be fairly obvious and there are
further instruction in it.

## Xterm Mouse support

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

## Color Xterm support

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

When it is working, the command: "joe -assume_256color -text_color bg_222"
should have a gray background.

## UTF-8

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

You can hit ESC ' x to enter a Unicode character if the file coding is UTF-8.

## Hex edit mode

When this mode is selected (either put -hex on the command line, or look for
"Hex edit mode" after hitting ^T), the buffer is displayed as a hex dump,
but all of the editing commands operate the same way.  It is most useful to
select overtype mode in conjunction with hex dump (hit ^T T).  Then typing
will not insert.

- To enter the hex byte 0xF8 type ESC ' x F 8

- You can use ^KC to copy a block as usual.  If overtype mode is selected,
  the block will overwrite the destination data without changing the size of
  the file.  Otherwise it inserts.

- Hit ESC x byte &lt;Enter&gt;, to jump to a particular byte offset.  Hex values
  can be entered into this prompt like this: 0x2000.

- Search, incremental search, and search &amp; replace all operate as usual.

## Using JOE in a shell script

Joe used to use /dev/tty to access the terminal.  This caused a problem with
idle-session killers (they would kill joe because the real tty device was
not being accessed for a long time), so now joe only uses /dev/tty if you
need to pipe a file into joe, as in:

	echo "hi" | joe

If you want to use joe in a shell script which has its stdin/stdout
redirected, but you do not need to pipe to it', you should simply redirect
joe's stdin/stdout to /dev/tty:

	joe filename  </dev/tty >/dev/tty

