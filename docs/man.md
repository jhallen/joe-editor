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
be entered without pressing shift (I.E., try __^-__, __^6__ and
__^2__).  Other keyboards may reassign these to other keys.  Try:
__^.__, __^,__ and __^/__.  __^SPACE__ can usually be used in place
of __^@__.  __^\__ and __^]__ are interpreted by many communication
programs, including telnet and kermit.  Usually you just hit the key twice
to get it to pass through the communication program.

Once you have typed __^K H__, the first help window appears at the top of
the screen.  You can continue to enter and edit text while the help window
is on.  To page through other topics, hit __^[,__ and __^[.__ (that is,
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
should.  See the section __Environment variables__ below.

## Command Line Options

The following global options may be specified on the command line:

* -asis

Characters with codes above 127 will be sent to the terminal as-is, instead
of as inverse of the corresponding character below 128.  If this does not
work, check your terminal server.

* -backpath path

If this option is given, backup files will be stored in the specified
directory instead of in each file's original directory.

* -baud nnn

Set the baud rate for the purposes of terminal screen optimization.  Joe
inserts delays for baud rates below 19200, which bypasses tty buffering so
that typeahead will interrupt the screen output.  Scrolling commands will
not be used for 38400 baud.  This is useful for X-terms and other console
ttys which really aren't going over a serial line.

* -beep

Joe will beep on command errors and when the cursor goes past
extremes.

* -columns nnn

Sets the number of screen columns.

* -csmode

Continued search mode: a search immediately following a search will repeat
the previous search instead of prompting for new string.  This is useful for
the the __^[S__ and __^[R__ commands and for when joe is trying to be emacs.

* -dopadding

Joe usually assumes that there is some kind of flow control between it and
the tty.  If there isn't, this option will make joe output extra ^@s to the
tty as specified by the termcap entry.  The extra ^@s allow the terminal to
catch up after long terminal commands.

* -exask

This option makes ^KX verify the file name that it's about to write.

* -force

This option makes sure that the last line of the file has a line-feed which
it's saved.

* -help

The editor will start with the help screen on if this option is
given.

* -keepup

Normally the column number and control-key prefix fields of the status
lines are on a one second delay to reduce CPU consumption, but with this
option they are updated after each key-stroke.

* -lightoff

The block highlighting will go away after any block command if this option
is given.

* -lines nnn

Sets the number of screen lines.

* -marking

Text between ^KB and the cursor is highlighted (use with -lightoff and a
modified joerc file to have drop-anchor style block selection).

* -mid

If this option is set and the cursor moves off the window, the window will
be scrolled so that the cursor is in the center.  This option is forced on
slow terminals which don't have scrolling commands.

* -nobackups

This option prevents backup files.

* -nonotice

This option prevent the copyright notice from being displayed when the
editor starts.

* -nosta

This option eliminates the top-most status line.  It's nice for when you
only want to see your text on the screen or if you're using a vt52.

* -noxon

Attempt to turn off ^S/^Q processing.  This is useful for when joe is trying
to be WordStar or EMACS.

* -orphan

When this option is active, extra files on the command line will be placed
in orphaned buffers instead of in extra windows.  This is useful for when
joe is trying to be emacs.

* -pg nnn

This specifies the number of lines to keep after PgUp/PgDn (^U/^V).  If -1
is given, half the window is kept.

* -skiptop nnn

Don't use the top nnn lines of the screen.  Useful for when joe is used as a
BBS editor.

Each of these options may be specified in the joerc file as well.  In
addition, the NOXON, BAUD, LINES, COLUMNS and DOPADDING options may be
specified with environment variables.

The JOETERM environment variable may be set to override the regular TERM
environment variable for specifying your terminal type.

The following options may be specified before each filename on the command
line:

* +nnn

The cursor starts on the specified line.

* -crlf

Joe uses CR-LF as the end of line sequence instead of just LF.  This is for
editing MS-DOS or VMS files.

* -wordwrap

Joe wraps the previous word when you type past the right margin.

* -autoindent

When you hit Return on an indented line, the indentation is duplicated onto
the new line.

* -overwrite

Typing overwrites existing characters instead of inserting before
them.

* -lmargin nnn

Sets the left margin.

* -rmargin nnn

Sets the right margin.

* -tab nnn

Sets the tab width.

* -indentc nnn

Sets the indentation character for ^K, and ^K. (32 for SPACE, 9 for
TAB).

* -istep nnn

Sets the indentation step for ^K, and ^K..

* -linums

Line numbers are displayed before each line.

* -rdonly

The file is read only.

* -keymap name

Use an alternate section of the joerc file for the key sequence
bindings.

These options can also be specified in the joerc file.  They can be set
depending on the file-name extension.  Programs (.c, .h or .p extension)
usually have autoindent enabled.  Wordwrap is enabled on other files, but rc
files have it disabled.

## Editing Tasks
### Basic Editing

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

If you make a mistake, you can hit __^___ to "undo" it.  On most keyboards
you hit just __^-__ to get __^___, but on some you might have to hold
both the __Shift__ and __Control__ keys down at the same time to get it.
If you "undo" too much, you can "redo" the changes back into existence by
hitting __^^__ (type this with just __^6__ on most keyboards).

If you were editing in one place within the file, and you then temporarily 
had to look or edit some other place within the file, you can get back to 
the original place by hitting __^K -__.  This command actually returns you 
to the last place you made a change in the file.  You can step through a 
history of places with __^K -__ and __^K =__, in the same way you can 
step through the history of changes with the "undo" and "redo" commands.

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

### Word wrap and formatting

If you type past the right edge of the screen in a C language or PASCAL 
file, the screen will scroll to the right to follow the cursor.  If you type 
past the right edge of the screen in a normal file (one whose name doesn't 
end in .c, .h or .p), JOE will automatically wrap the last word onto the 
next line so that you don't have to hit __Return__.  This is called 
word-wrap mode.  Word-wrap can be turned on or off with the __^T W__ 
command.  JOE's initialization file is usually set up so that this mode is 
automatically turned on for all non-program files.  See the section below on 
the __joerc__ file to change this and other defaults.

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

### Over-type mode

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

### Control and Meta characters

Each character is represented by a number.  For example, the number for 'A'
is 65 and the number for '1' is 49.  All of the characters which you
normally see have numbers in the range of 32 - 126 (this particular
arbitrary assignment between characters and numbers is called the ASCII
character set).  The numbers outside of this range, from 0 to 255, aren't
usually displayed, but sometimes have other special meanings.  The number
10, for example, is used for the line-breaks.  You can enter these special,
non-displayed __control characters__ by first hitting __`__ and then
hitting a character in the range __@ A B C ... X Y Z [ ^ ] \ ___ to get
the number 0 - 31, and ? to get 127.  For example, if you hit __` J__,
you'll insert a line-break character, or if you hit __` I__, you'll insert
a TAB character (which does the same thing the TAB key does).  A useful
control character to enter is 12 (__` L__), which causes most printers to
advance to the top of the page.  You'll notice that JOE displays this
character as an underlined L.  You can enter the characters above 127, the
__meta characters__, by first hitting __^\__.  This adds 128
to the next (possibly control) character entered.  JOE displays characters
above 128 in inverse-video.  Some foreign languages, which have more letters
than English, use the meta characters for the rest of their alphabet.  You
have to put the editor in __ASIS__ mode (described later) to have these
passed untranslated to the terminal.

### Prompts

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

### Where am I?


Hit __^K SPACE__ to have JOE report the line number, column number, and
byte number on the last line of the screen.  The number associated with the
character the cursor is on (its ASCII code) is also shown.  You can have the
line number and/or column number always displayed on the status line by
setting placing the appropriate escape sequences in the status line setup
strings.  Edit the joerc file for details.

### File operations


You can hit __^K D__ to save the current file (possibly under a different
name from what the file was called originally).  After the file is saved,
you can hit __^K E__ to edit a different file.

If you want to save only a selected section of the file, see the section on
__Blocks__ below.__

If you want to include another file in the file you're editing, use __^K
R__ to insert it.

### Temporarily suspending the editor


If you need to temporarily stop the editor and go back to the shell, hit 
__^K Z__.  You might want to do this to stop whatever you're editing and 
answer an e-mail message or read this man page, for example.  You have to 
type __fg__ or __exit__ (you'll be told which when you hit __^K Z__) 
to return to the editor.  

### Searching for text


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

### Regular Expressions

A number of special character sequences may be entered as search
text:

* __\*__

This finds zero or more characters.  For example, if you give __A\*B__ as
the search text, JOE will try to find an A followed by any number of characters
and then a B.

* __\?__

This finds exactly one character.  For example, if you give __A\?B__ as
the search text, JOE will find AXB, but not AB or AXXB.

* __\^ \$__

These match the beginning and end of a line.  For example, if you give
__\^test\$__, then JOE with find __test__ on a line by itself.

* __\< \>__

These match the beginning and end of a word.  For example, if you give
__\<\*is\*>__, then joe will find whole words which have the
sub-string __is__ within them.

* __\[...]__

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

* __\+__

This finds zero or more of the character which immediately follows the
__\+__.  For example, if you give __\[ ]\+\[ ]__, where the
characters within the brackets are both SPACE and TAB, then JOE will find
whitespace.

* __\\__

Matches a single \.

* __\n__

This finds the special end-of-line or line-break character.


A number of special character sequences may also be given in the replacement
string:

* __\&amp;__

This gets replaced by the text which matched the search string.  For
example, if the search string was __\&lt;\*\&gt;__, which matches words, and
you give __"\&amp;"__, then joe will put quote marks around words.

* __\0 - \9__

These get replaced with the text which matched the Nth __\*__, __\?__,
__\+__, __\c__, __\+__, or __\[...]__ in the search string.

* __\\__

Use this if you need to put a __\__ in the replacement string.

* __\n__

Use this if you need to put a line-break in the replacement string.


Some examples:

Suppose you have a list of addresses, each on a separate line, which starts 
with "Address:" and has each element separated by commas.  Like so:

Address: S. Holmes, 221b Baker St., London, England

If you wanted to rearrange the list, to get the country first, then the 
city, then the person's name, and then the address, you could do this:

Type __^K F__ to start the search, and type:

__Address:\*,\*,\*,\*\$__

to match "Address:", the four comma-separated elements, and then the end of 
the line.  When asked for options, you would type __r__ to replace the 
string, and then type:

__Address:\3,\2,\0,\1__

To shuffle the information the way you want it. After hitting return, the 
search would begin, and the sample line would be changed to:

Address: England, London, S. Holmes, 221b Baker St.


### Blocks


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

After you are finished with some block operations, you can just leave the 
highlighting on if you don't mind it (of course, if you accidently hit __^K 
Y__ without noticing...).  If it really bothers you, however, just hit 
__^K B ^K K__, to turn the highlighting off.  

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

### Windows

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

### Keyboard macros 


Macros allow you to record a series of keystrokes and replay them with the 
press of two keys.  This is useful to automate repetitive tasks.  To start a 
macro recording, hit __^K [__ followed by a number from 0 to 9.  The 
status line will display (Macro n recording...).  Now, type in the series of 
keystrokes that you want to be able to repeat.  The commands you type will 
have their usual effect. Hit __^K ]__ to stop recording the macro.  Hit 
__^K__ followed by the number you recorded the macro in to execute one 
iteration of the key-strokes.   

For example, if you want to put "**" in front of a number of lines, you can 
type:

__^K [ ^A ** __<down arrow> __^K ]__


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

You can use the repeat command, __^K \__, to repeat a macro, or any other
edit command or even a normal character, a specified number of times.  Hit
__^K \__, type in the number of times you want the command repeated and
press __Return__.  The next edit command you now give will be repeated
that many times.
For example, to delete the next 20 lines of text, type:

__^K \ 20__<return>__^Y__

### Rectangle mode

Type __^T X__ to have __^K B__ and __^K K__ select rectangular blocks
instead of stream-of-text blocks.  This mode is useful for moving, copying,
deleting or saving columns of text.  You can also filter columns of text
with the __^K /__ command- if you want to sort a column, for example.  The
insert file command, __^K R__ is also effected.

When rectangle mode is selected, over-type mode is also useful
(__^T T__).  When over-type mode is selected, rectangles will replace
existing text instead of getting inserted before it.  Also the delete block
command (__^K Y__) will clear the selected rectangle with SPACEs and TABs
instead of deleting it.  Over-type mode is especially useful for the filter
block command (__^K /__), since it will maintain the original width of the
selected column.

### Tag search

If you are editing a large C program with many source files, you can use the 
__ctags__ program to generate a __tags__ file.  This file contains a 
list of program symbols and the files and positions where the symbols are 
defined.  The __^K ;__ command can be used to lookup a symbol (functions, 
defined constants, etc.), load the file where the symbol is defined into the 
current window and position the cursor to where the symbol is defined.  
__^K ;__ prompts you for the symbol you want, but uses the symbol the 
cursor was on as a default.  Since __^K ;__ loads 
the definition file into the current window, you probably want to split the 
window first with __^K O__, to have both the original file and the 
definition file loaded.  

### Shell windows

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

## Environment variables 

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
terminal to use ^S/^Q handshaking (I.E., if the computer is sending
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

<a name="filenames"/>
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

* -

Use this to get input from the standard input or to write output to the
standard output.  For example, you can put joe in a pipe of commands:
__quota -v | joe - | mail root__, if you want to complain about your low
quota.


<br><br>
<a name="joerc">
## The joerc file

^T options, the help screens and the key-sequence to editor command
bindings are all defined in JOE's initialization file.  If you make a copy
of this file (which normally resides in __/home/jhallen/etc/joe/joerc__) to
__$HOME/.joerc__, you can customize these setting to your liking.  The
syntax of the initialization file should be fairly obvious and there are
further instruction in it.

## Acknowledgments

JOE was written by Joseph H. Allen.  If you have bug reports or questions,
e-mail them to jhallen@world.std.com.  Larry Foard (entropy@world.std.com)
and Gary Gray (ggray@world.std.com) also helped with the creation of
JOE.
