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
is on.  To page through other topics, hit __ESC ,__ and __ESC .__ (that is,
__ESC ,__ and __ESC .__).  Use __^K H__ to dismiss the help window.

You can customize the keyboard layout, the help screens and a number of
behavior defaults by copying JOE's initialization file (usually
__/etc/joe/joerc__) to __.joerc__ in your home directory and then
by modifying it.  See the section [joerc](#joerc) below.

To have JOE used as your default editor for e-mail and News, you need to set
the __EDITOR__ and __VISUAL__ environment variables in your shell
initialization file (__.cshrc__ or __.profile__) to refer to JOE (JOE
usually resides as __/home/jhallen/bin/joe__).

There are a number of other obscure invocation parameters which may have to
be set, particularly if your terminal screen is not updating as you think it
should.  See the section [Environment variables](#evariables) below.

<a name="options"></a>
## Command Line Options

These options can also be specified in the joerc file.  Local options can be
set depending on the file-name extension.  Programs (.c, .h or .p extension)
usually have autoindent enabled.  Wordwrap is enabled on other files, but rc
files have it disabled.

An option is enabled when it's given like this:

* -wordwrap

An option is disabled when it's given like this:

* --wordwrap

Some options take arguments.  Arguments are given like this:

* -lmargin 5

The following global options may be specified on the command line:

* asis
Characters with codes above 127 will be sent to the terminal as-is, instead
of as inverse of the corresponding character below 128.  If this does not
work, check your terminal server.  This option has no effect if UTF-8
encoding is used.
<br>

* assume_256color
Assume ANSI-like terminal emulator supports 256 colors even if termcap entry
says it doesn't.
<br>

* assume_color
Assume ANSI-like terminal emulator supports color even if termcap entry says
it doesn't.
<br>

* text_color __color__
Set color for text.
<br>

* status_color __color__
Set color for status bar.
<br>

* help_color __color__
Set color for help.
<br>

* menu_color __color__
Set color for menus.
<br>

* prompt_color __color__
Set color for prompts.
<br>

* msg_color __color__
Set color for messages.
<br>

* autoswap
Automatically swap __^K B__ with __^K K__ if necessary to
mark a legal block during block copy/move commands.
<br>

* backpath path
Sets bath to a directory where all backup files are
to be stored.  If this is unset (the default) backup files are stored in the
directory containing the file.
<br>

* baud nnn
Set the baud rate for the purposes of terminal screen optimization
(overrides value reported by stty).  JOE inserts delays for baud rates below
19200, which bypasses tty buffering so that typeahead will interrupt the
screen output.  Scrolling commands will not be used for 38400 baud and
above.  This is useful for X-terms and other console ttys which really
aren't going over a serial line.
<br>

* beep
Enable beeps when edit commands return errors, for example when the cursor
goes past extremes.
<br>

* break_links
When enabled, JOE first deletes the file before
writing it in order to break hard-links and symbolic-links.
<br>

* break_hardlinks
When enabled, and the file is
not a symbolic links, JOE first deletes the file before
writing it in order to break hard-links.
<br>

* columns nnn
Set number of columns in terminal emulator (in case
termcap entry is wrong).  This is only useful on old system which don't have
the "get window size" ioctl.
<br>

* csmode
Enable continued search mode: Successive
__^K F__s repeat the current search instead of prompting for a new one.
<br>

* dopadding
Enable JOE to send padding NULs to the terminal (for very old terminals).
<br>

* exask
When set, __^K X__ prompts for a new name before saving the file.
<br>

* floatmouse
When set, mouse clicks can position the cursor
beyond the ends of lines.
<br>

* guess_crlf
When set, JOE tries to guess the file format
MS-DOS or UNIX.
<br>

* guess_indent
When set, JOE tries to guess the indentation character and indentation
step based on the contents of the file.  The algorithm is to find the
greatest common factor of the three most common indentations found in the
file.
<br>

* guess_non_utf8
When set, enable guessing of non-UTF-8 files
in UTF-8 locales.
<br>

* guess_utf8
When set, enable guessing of UTF-8 files in
non-UTF-8 locales.
<br>

* help
When set, start off with the on-line help enabled.
<br>

* help_is_utf8
When set, the help text in the joerc file is
assumed to be UTF-8.
<br>

* icase
Search is case insensitive by default when set.
<br>

* joe_state
Enable reading and writing of ~/.joe_state file
<br>

* joexterm
Set this if xterm was configured with --paste64
option for better mouse support.
<br>

* keepup
The column number on the status line is updated constantly when
this is set, otherwise it is updated only once a second.
<br>

* language __language__
Sets language for aspell.
<br>

* lightoff
Automatically turn off __^K B__ __^K K__ highlighting after a
block operation.
<br>

* lines nnn
Set number of lines in terminal emulator (in case termcap entry is wrong). 
This is only useful on old system which don't have the "get window size"
ioctl.
<br>

* marking
Enable marking mode: highlights between __^K B__ and cursor.
<br>

* menu_above
Put menus above prompt instead of below them.
<br>

* menu_explorer
Stay in menu when a directory is selected (otherwise the directory is added
to the path and the cursor jumps back to the prompt).
<br>

* menu_jump
Jump into the file selection menu when tab tab is hit.
<br>

* mid
If this option is set and the cursor moves off the window, the window will
be scrolled so that the cursor is in the center.  This option is forced on
slow terminals which don't have scrolling commands.
<br>

* mouse
Enable xterm mouse support.
<br>

* nobackups
Disable backup files.
<br>

* nocurdir
Disable current-directory prefix in prompts.
<br>

* noexmsg
Disable exiting message ("File not changed so no update needed")
<br>

* nolinefeeds
Disable sending linefeeds to
preserve screen history in terminal emulator's scroll-back buffer (only
relevant when notite mode is enabled).  
<br>

* nolocks
Disable EMACS compatible file locks.
<br>

* nomodcheck
Disable periodic file modification check.
<br>

* nonotice
This option prevent the copyright notice from being displayed when the
editor starts.
<br>

* nosta
This option eliminates the top-most status line.  It's nice for when you
only want to see your text on the screen or if you're using a vt52.
<br>

* notagsmenu
Disable selection menu for tags search with multiple results.
<br>

* notite
Disable ti and te termcap sequences which are usually
set up to save and restore the terminal screen contents when JOE starts and
exits.
<br>

* noxon
Disable __^S__ and __^Q__ flow control, possibly allowing __^S__ and __^Q__ to be used as
editor keys.
<br>

* orphan
Orphan extra files given on the command line instead of creating windows for
them (the files are loaded, but you need to use switch-buffer commands to
access them).
<br>

* pg nnn
Set number of lines to keep during Page Up and Page Down (use -1 for 1/2
window size).
<br>

* restore
Set to have cursor position restore to last position of previouly edited
files.
<br>

* rtbutton
Swap left and right mouse buttons.
<br>

* search_prompting
Show previous search string in search command (like in PICO).
<br>

* skiptop nnn
When set to N, the first N lines of the terminal screen are not used by JOE
and are instead left with their original contents.  This is useful for
programs which call JOE to leave a message for the user.
<br>

* square
Enable rectangular block mode.
<br>

* transpose
Transpose rows with columns in all menus.
<br>

* undo_keep nnn
Sets number of UNDO records to keep (0 means infinite).
<br>

* usetabs
Set to allow rectangular block operations to use
tabs.
<br>

* wrap
Enable search to wrap to beginning of file.
<br>

The following local options may be specified on the command line:

* +nnn
The cursor starts on the specified line.
<br>

* autoindent
Enable auto-indent mode.  When you hit Return on an indented line, the
indentation is duplicated onto the new line.
<br>

* c_comment
Enable __^G__ skipping of C-style comments /* ... */
<br>

* cpara __characters__
Sets list of characters which can indent paragraphs.
<br>

* cnotpara __characters__
Sets list of characters which begin lines which are definitely not part of
paragraphs.
<br>

* cpp_comment
Enable __^G__ skipping of C++-style comments // ...
<br>

* crlf
JOE uses CR-LF as the end of line sequence instead of just LF.  This is for
editing MS-DOS or VMS files.
<br>

* encoding __encoding__
Set file encoding (like utf-8 or 8859-1).
<br>

* flowed
Set to force an extra space after each line of a paragraph but the last.
<br>

* force
When set, a final newline is appended to the file if
there isn't one when the file is saved.
<br>

* french
When set, only one space is inserted after periods in paragraph reformats
instead of two.
<br>

* hex
Enable hex-dump mode.
<br>

* highlight
Enable syntax highlighting.
<br>

* highlighter_context
Enable use of syntax file to identify comments and strings which should be
skipped over during __^G__ matching.

* indentc nnn
Sets the indentation character for shift left and shift right (__^K ,__ and
__^K .__).  Use 32 for SPACE, 9 for TAB.
<br>

* indentfirst
When set, the smart home key jumps to the indentation point first, otherwise
it jumps to column 1 first.
<br>

* istep nnn
Sets indentation step.
<br>

* linums
Enable line number display.
<br>

* lmargin
Set left margin.
<br>

* lmsg
Define left-side status bar message.
<br>

* overwrite
Enable overtype mode.  Typing overwrites existing characters instead of
inserting before them.
<br>

* picture
Enable "picture" mode- allows cursor to go past ends of lines.
<br>

* pound_comment
__^G__ ignores # ... comments.
<br>

* purify
Fix indentation if necessary before shifting or smart backspace.  For
example, if indentation uses a mix of tabs and spaces, and indentc is
space, then indentation will be converted to all spaces before the shifting
operation.
<br>

* rdonly
Set read-only mode.
<br>

* rmargin nnn
Set right margin.
<br>

* rmsg __string__
Define right-side status bar message.
<br>

* semi_comment
__^G__ ignores ; ... comments.
<br>

* single_quoted
__^G__ ignores '...'
<br>

* smartbacks
Enable smart backspace and tab.  When this mode is set backspace and tab
indent or unindent based on the values of the istep and indentc options.
<br>

* smarthome
Home key first moves cursor to beginning of line, then if hit again, to
the first non-blank character.
<br>

* smsg __string__
Define status command format when cursor is on a character.
<br>

* spaces
Insert spaces when tab key is hit.
<br>

* syntax __syntax__
Set syntax for syntax highlighting.
<br>

* tab nnn
Set tab stop width.
<br>

* text_delimiters __word delimiter list__
Give list of word delimiters which __^G__ will step through.

For example, "begin=end:if=elif=else=endif" means that __^G__ will jump
between the matching if, elif, else and endif.

* vhdl_comment
__^G__ ignores -- ... comments
<br>

* wordwrap
JOE wraps the previous word when you type past the right margin.
<br>

* zmsg __string__
Define status command format when cursor is at end of file.
<br>

### Colors and attributes:

Combine attributes and up to one foreground color and one background color
to create arguments for color options like text_color.  For example:
bold+bg_green+blue

* Attributes: bold, inverse, blink, dim, underlined, and italic

* Foreground colors: white, cyan, magenta, blue, yellow, green, red, or black

* Background colors: bg_white, bg_cyan, bg_magenta, bg_blue, bg_yellow, bg_green, bg_red or bg_black

With a 16 color or 256 color terminal emulator (export TERM=xterm-16color), these
brighter than normal colors become available:

> Note that you need an xterm which was compiled to support 16 or 256 colors
> and a matching termcap/terminfo entry for it.

* Foreground: WHITE, CYAN, MAGENTA, BLUE, YELLOW, GREEN, RED or BLACK

* Background: bg_WHITE, bg_CYAN, bg_MAGENTA, bg_BLUE, bg_YELLOW, bg_GREEN, bg_RED or bg_BLACK

With a 256 color terminal emulator (export TERM=xterm-256color), these become available:

> Note that you need an xterm which was compiled to support 256 colors and a
> matching termcap/terminfo entry for it.

* fg_RGB and bg_RGB, where R, G and B rand from 0 - 5.  So: fg_500 is bright red.

* fg_NN and bg_NN give shades of grey, where the intensity, NN, ranges from 0 - 23.

### Status line definition strings

-lmsg defines the left-justified string and -rmsg defines the
right-justified string.  The first character of -rmsg is the background fill
character.

-smsg defines the status command (__^K SPACE__).  -zmsg defines it when the cursor
is at the end of the file.  The last character of smsg or zmsg is the fill character.

The following escape sequences can be used in these strings:

~~~~~
  %t  12 hour time
  %u  24 hour time
  %T  O for overtype mode, I for insert mode
  %W  W if wordwrap is enabled
  %I  A if autoindent is enabled
  %X  Rectangle mode indicator
  %n  File name
  %m  '(Modified)' if file has been changed
  %*  '*' if file has been changed
  %R  Read-only indicator
  %r  Row (line) number
  %c  Column number
  %o  Byte offset into file
  %O  Byte offset into file in hex
  %a  Ascii value of character under cursor
  %A  Ascii value of character under cursor in hex
  %w  Width of character under cursor
  %p  Percent of file cursor is at
  %l  No. lines in file
  %k  Entered prefix keys
  %S  '*SHELL*' if there is a shell running in window
  %M  Macro recording message
  %y  Syntax
  %e  Encoding
  %x  Context (first non-indented line going backwards)
  %dd day
  %dm month
  %dY year
  %Ename%  value of environment variable
  %Tname%  value of option (ON or OFF for boolean options)
~~~~~

These formatting escape sequences may also be given:
 
~~~~~
  \i  Inverse
  \u  Underline
  \b  Bold
  \d  Dim
  \f  Blink
  \l  Italic
~~~~~
<br>


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

## File operations


You can hit __^K D__ to save the current file (possibly under a different
name from what the file was called originally).  After the file is saved,
you can hit __^K E__ to edit a different file.

If you want to save only a selected section of the file, see the section on
[Blocks](#blocks) below.

If you want to include another file in the file you're editing, use __^K
R__ to insert it.

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
standard output.  For example, you can put JOE in a pipe of commands:
__quota -v | joe | mail root__, if you want to complain about your low
quota.

### Using JOE in a shell script

JOE used to use /dev/tty to access the terminal.  This caused a problem with
idle-session killers (they would kill JOE because the real tty device was
not being accessed for a long time), so now JOE only uses /dev/tty if you
need to pipe a file into JOE, as in:

	echo "hi" | joe

If you want to use JOE in a shell script which has its stdin/stdout
redirected, but you do not need to pipe to it', you should simply redirect
JOE's stdin/stdout to /dev/tty:

	joe filename  </dev/tty >/dev/tty


<br><br>

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

There are a number of options which control the paragraph reformatter and
word wrapper:

* The __cpara__ option provides a list of characters which can indent a
  paragraph.  For example, in e-mail quoted matter is indicated by __\>__ 
  at the beginnings of line, so this character should be in the cpara list.

k* The __cnotpara__ option provides a list of characters which, if they are
  the first non-whitespace character of a line, indicate the line is not
  included as part of a paragraph for formatting.  For example, lines
  beginning with '.' in nroff can not be paragraph lines.

* Autoindent mode effects the formatter.  If autoindent is disabled, only
  the first line will be indented.  If autoindent is enabled, the entire
  paragraph is indented.

* __french__ determines how many spaces are inserted after periods.

* When __flowed__ is enabled, a space is inserted after each line but the
  last of paragraph.  This indicates that the lines belong together as a
  single paragraph in some programs.

* When __overtype__ is enabled, the word wrapper will not insert lines.

## Centering

If you want to center a line within the margins, use the __^K A__
command.

## Spell checker

Hit __ESC N__ to check the spelling of the word the cursor is on using the
aspell program (or ispell program if you modify the joerc file).  Hit
__ESC L__ to check the highlighted block or the entire file if no block is
highlighted.

JOE passes the language and character enoding to the spell checker.  To
change the langauge, hit __^T V__.

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
non-displayed __control characters__ by first hitting __ESC '__ and then
hitting a character in the range __@ A B C ... X Y Z [ ^ ] \\ \___ to get
the number 0 - 31, and ? to get 127.  For example, if you hit __ESC ' J__,
you'll insert a line-break character, or if you hit __ESC ' I__, you'll insert
a TAB character (which does the same thing the TAB key does).  A useful
control character to enter is 12 (__ESC ' L__), which causes most printers to
advance to the top of the page.  You'll notice that JOE displays this
character as an underlined L.  You can enter the characters above 127, the
__meta characters__, by first hitting __^\___.  This adds 128
to the next (possibly control) character entered.  JOE displays characters
above 128 in inverse-video.  Some foreign languages, which have more letters
than English, use the meta characters for the rest of their alphabet.  You
have to put the editor in __ASIS__ mode to have these
passed untranslated to the terminal.

When UTF-8 encoding is used, characters above 255 may be inserted with __ESC
 '__.

## Character sets and UTF-8

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

Hit __^T E__ to change the coding for the file.  Hit &lt;tab&gt; &lt;tab&gt; at
this prompt to get a list of available codings.  There are a number of
built-in character sets, plus you can install character sets in the
~/.joe/charmaps and /usr/local/etc/joe/charmaps directories.

Check: /usr/share/i18n/charmaps for example character set files.  Only
byte oriented character sets will work.  Also, the file should not be
gzipped (all of the charmap file in /usr/share/i18n/charmaps on my computer
were compressed).  The parser is very bad, so basically the file has to look
exactly like the example one in /usr/local/etc/joe/charmaps.

You can hit __^K SPACE__ to see the current character set.

You can hit __ESC ' x__ to enter a Unicode character if the file coding is
UTF-8.

## Prompts

If you hit __TAB__ at any file name prompt, JOE will attempt to complete
the name you entered as much as possible.  If it couldn't complete the
entire name, because there are more than one possible completions, JOE
beeps.  If you hit __TAB__ again, JOE lists the completions.  You can use
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
selection. You can, however, jump into the menu window with __^K P__ (move to
previous window) and use the arrow keys and &lt;Enter&gt; to make your
selection.  Also in a menu, you can hit the first letter of any of the items
to make the cursor jump directly to it.  The __^T__ option menu works like
this.

If the menu is too large to fit in the window, you can hit Page Up and
Page Down to scroll it (even if you have not jumped into it).

TAB completion works in the search and replace prompts as well.  In this
case, JOE tries to complete the word based on the contents of the buffer. 
If you need search for the TAB character itself, you can enter it with __ESC '
TAB__.

Also, you can hit __ESC &lt;Enter&gt;__ in a text window to request JOE to
complete the word you are typing.  As with the search prompt, JOE tries to
complete the word based on the contents of the buffer.  It will bring up a
menu of possibilities if you hit __ESC &lt;Enter&gt;__ twice.


## Where am I?


Hit __^K SPACE__ to have JOE report the line number, column number, and
byte number on the last line of the screen.  The number associated with the
character the cursor is on (its ASCII code) is also shown.  You can have the
line number and/or column number always displayed on the status line by
setting placing the appropriate escape sequences in the status line setup
strings.  Edit the joerc file for details.

## What if I hit __^K__ by accident?

Hit the space bar.  This runs an innocuous command (it shows the line
number on the status bar).

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

* __a__

The search covers all loaded buffers.  So to replace all instances of "foo"
with "bar" in all .c files in the current directory:

	joe *.c
	   ^K F
	       foo <Enter>
	       ra <Enter>
	       bar <Enter>

* __e__

The search covers all files in the grep or make error list.  You can use a
UNIX command to generate a list of files and search and replace through the
list.  So to replace all instances of "foo" with "bar" in all .c files which
begin with f.  You can also use "ls" and "find" instead of grep to create
the file list.


	ESC G
	  grep -n foo f*.c <Enter>
	^K F
           foo <Enter>
	   re <Enter>
	   bar <Enter>

You can hit __^L__ to repeat the previous search.

You can hit __^K H__ at the search and replace options prompt to bring up a list
of all search and replace options.

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
__\<\\\*is\\\*\\\>__, then JOE will find whole words which have the
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
you give __"&"__, then JOE will put quote marks around words.

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

## Incremental search

Use __ESC S__ to start an increment search forwards, or __ESC R__ to start
an incremental search backwards.  As you type the search string, the cursor
will jump to the first text that matches the regular expression you have
entered so far.

Hit __ESC S__ or __ESC R__ again to find the next occurance of the text or
to switch the direction of the search.

__^S__, __^\\__ and __^L__ have the same effect as __ESC S__. __^R__ has the same
effect as __ESC R__.  These keys are to support JMACS.

Hit __Backspace__ to undo the last incremental search action.  The last
action could be a repeat of a previous search or the entering of a new
character.

Use __^Q__ to insert control characters into the search text.  Previously,
\` could also be used for this.

Hit any other key to exit the increment search. 

## Goto matching delimiter

Hit __^G__ to jump between matching delimiters.  This works on both
character delimiters (like '(' and ')') and word delimiters for languages
like Pascal and Verilog which use "begin" and "end" to delimit blocks.  It
also works for matching start and end tags in XML.  If a word is not known,
__^G__ starts a search with the word moved into the search prompt.

For __^G__ to work on word delimiters, the cursor must be positioned
on the first letter of the word.  So in XML, if the cursor is on the &lt; in
&lt;foo&gt;, it will jump to the &gt;.  But if it is one the 'f', it will
jump to the matching &lt;/foo&gt;.  Likewise, in C, __^G__ will jump between #if, #else
and #endif, but you need to position the cursor on the letter,
not the '#'.

__^G__ is smart enough to skip delimiters found in quoted or
commented-out matter.  You need to tell JOE how your language indicates
this: see the __ftyperc__ file for examples of how this is done.

The are a number of options which control the behavior of __^G__.  These
options control which kinds of comments __^G__ can skip over:

* c_comment
* cpp_comment
* pount_comment
* semi_comment
* vhdl_comment

These options determine which kinds of strings __^G__ can skip over:

* single_quoted
* double_quoted

This option allows an annotated syntax file to determine which text can be
counted as comments or strings which can be skipped over by __^G__:

* highlighter_context

This option enables the use of syntax files to identify comments and strings
which should be skipped over during __^G__ matching.  The syntax file states
should be annotated with the string and comment keywords for this to work.

* text_delimiters

This option provides a list of word delimiters to match.  For example,
"begin=end:if=elif=else=endif" means that __^G__ will jump between the
matching if, elif, else and endif.  It will also jump between begin and end.

__^G__ has a built-in table for matching character delimiters- it knows that
__(__ goes with __)__.

__^G__ has a built-in parser to handle start/end tag matching for XML.

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
UNIX command.  For example, if you select a list of words with __^K B__
and __^K K__, and then type __^K / sort__, the list of words will be
sorted.  Another useful UNIX command for __^K /__, is __tr__.  If you
type __^K / tr a-z A-Z__, then all of the letters in the highlighted block
will be converted to uppercase.

### How do I deselect a highlighted block?

After you are finished with some block operations, you can just leave the
highlighting on if you don't mind it (but don't accidently hit __^K Y__). 
If it really bothers you, however, just hit __^K B ^K K__, to turn the
highlighting off.

### Selecting blocks

The classic way is to hit __^K B__ at the beginning and __^K K__ at the
end.  These set pointers called markb and markk.  Once these are set you
can jump to markb with __ESC B__ and jump to markk with __ESC K__.

New way: hit Ctrl-rtarw (right arrow) to start selecting rightward. 
Each time you hit Ctrl-rtarw, the block is extended one more to the right. 
This uses a simple macro: "begin_marking,rtarw,toggle_marking".

Unfortunately, there is no standard way to get the keysequence given by the
terminal emulator when you hit Ctrl-rtarw.  Instead you have to determine
this sequence yourself and enter it directly in the joerc file.  Some
examples are given for Xterm and gnome-terminal.  Hit __ESC '__ ctrl-rtarw
within JOE to have the sequence shown on your screen.  Note that Putty uses
__ESC ESC \[ C__ which will not appear with __ESC ' rtarw__ (also
__ESC ESC__ is set book mark, so you need to unbind it to do this in Putty).

Also you can hit Ctrl-delete to cut and Ctrl-insert to paste if the
sequence for these keys are known.

### Indenting program blocks

Auto-indent mode is toggled with the __^T I__ command.  The
__joerc__ is normally set up so that files with names ending with .p, .c
or .h have auto-indent mode enabled.  When auto-indent mode is enabled and
you hit __Return__, the cursor will be placed in the same column that the
first non-SPACE/TAB character was in on the original line.

You can use the __^K ,__ and __^K .__ commands to shift a block of text 
to the left or right.  If no highlighting is set when you give these 
commands, the program block the cursor is located in will be selected, and 
will be moved by subsequent __^K ,__ and __^K .__ commands.

The number of columns these commands shift by and the character used for
shifting can be set through the istep and indentc options.  These options
are available in the __^T__ menu.  Also, __^T =__ can be used to quickly
select from a number of common values for indentation step and character.

JOE has a bunch of additional options related to indenting programs:

* smartbacks
Enable smart backspace and tab.  When this mode is set backspace and tab
indent or unindent based on the values of the istep and indentc options.
<br>

* smarthome
Home key first moves cursor to beginning of line, then if hit again, to
the first non-blank character.
<br>

* indentfirst
Smart home goes to first non-blank character first, instead of going
to the beginning of the line first.
<br>

* purify
Fix indentation if necessary before shifting or smart backspace.  For
example, if indentation uses a mix of tabs and spaces, and indentc is
space, then indentation will be converted to all spaces before the shifting
operation.
<br>

* guess_indent
When set, JOE tries to guess the indentation character and indentation
step based on the contents of the file.  The algorithm is to find the
greatest common factor of the three most common indentations found in the
file.
<br>

### Rectangle mode

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

### Picture mode

Use __^T P__ to enter or exit picture mode.  Picture mode helps with ASCII
drawings.

Picture mode controls how JOE handles the case where the cursor is past the
ends of lines.  This happens when you use the up or down arrow keys to move
the cursor from the end of a long line to a short line.

If you attempt to type a character in this case:

If picture mode is off, the cursor will jump to the end of the line and
insert it there.

If picture mode is on, the line is filled with spaces so that the character
can be inserted at the cursor position.

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

### Windowing system model

JOE has an unsual model for its windowing system.  Basically you have a ring
of windows, but only a section of this ring may fit on the screen.  The windows
not on the screen still exist, they are just scrolled off.  When you hit
__^K N__ on the bottom window of the screen, it scrolls further windows from
the ring onto the screen, possibly letting the top window scroll out of
view.

Native JOE tries to keep each loaded buffer in a window, so users can find
all of the buffers by scrolling through the windows.  The __explode__
command (__^K I__) either expands all windows to the size of the screen so
that only one window can fit on the screen, or shinks them all as much as
possible to fit many on the screen.

On the other hand, JOE supports "orphan" buffers- files loaded into the
editor, but which are not in a window.  __^C__ normally closes a window and
discards the buffer that was in it.  If you hit __^C__ on the last remaining
window, it will normally exit the editor.  However, if there are orphan
buffers, __^C__ will instead load them into this final window to give you
chance to explicitly discard them.  If the __orphan__ option is given on
the command line, as in __joe -orphan *.c__, then JOE only loads the first
file into a window and leaves all the rest as orphans.

__orphan__ also controls whether the edit command __^K E__ creates a new
window for a newly loaded file, or reuses the current window (orphaning its
previous occupant).

The __bufed__ command prompts for a name of a buffer to switch into a window. 
Its completion list will show all buffers, including orphans and buffers
which appear in other windows.  __ESC V__ and __ESC U__ (__nbuf__ and
__pbuf__ commands) allow you to cycle through all buffers within a single
window.

Windows maintain a stack of occupants to support the pop-up shell window
feature.  When a pop-up window is dismissed, the previous buffer is returned
to the window.

### Scratch buffers

Scratch buffers are buffers which JOE does not worry about trying to
preserve.  JOE will not ask to save modified scratch buffers.  Pop-up shell
windows, the startup log and compile and grep message windows are scratch
buffers.  You can create your own scratch buffer with the __scratch__
command.

The following commands load scratch buffers:

* __showlog__ Show startup log
* __mwind__ Show message window (compile / grep messages from __ESC C__ and
  __ESC G__ commands).

## Keyboard macros 

Macros allow you to record a series of keystrokes and replay them with the 
press of two keys.  This is useful to automate repetitive tasks.  To start a 
macro recording, hit __^K \[__ followed by a number from 0 to 9.  The 
status line will display (Macro n recording...).  Now, type in the series of 
keystrokes that you want to be able to repeat.  The commands you type will 
have their usual effects. Hit __^K ]__ to stop recording the macro.  Hit 
__^K__ followed by the number you recorded the macro in to execute one 
iteration of the key-strokes.   

For example, if you want to put "**" in front of a number of lines, you can 
type:

__^K \[ 0 ^A \*\* __<down arrow\> __^K ]__

Which starts the macro recording, moves the cursor to the beginning of the 
line, inserts "\*\*", moves the cursor down one line, and then ends the 
recording. Since we included the key-strokes needed to position the cursor 
on the next line, we can repeatedly use this macro without having to move 
the cursor ourselves, something you should always keep in mind when 
recording a macro.

### Keyboard macro subroutines

If you find that the macro you are recording itself has a repeated set of
key-strokes in it, you can record a macro within the macro, as long as you
use a different macro number.  Also you can execute previously recorded
macros from within new macros. 

### Query suspend

If your macro includes a prompt for user input, and you want the user to
fill in the prompt every time the macro is executed, hit __^K ?__ at the
point in the macro recording where the user action is required.  Keyboard
input will not be recorded at this point.  When the user completes the
prompt, macro recording will continue.

When the macro is executed, the macro player will pause at the point where
__^K ?__ was entered to allow user input.  When the user completes the
prompt, the player continues with the rest of the macro.

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
commands beep if you have beeps enabled (__^T B__).

Hit __ESC D__ to insert the current set of keyboard macros as text into the
current buffer.  For example, the "\*\*" insert macro above looks like this:

	home,"**",dnarw	^K 0	Macro 0

You could insert this into your .joerc file and change the key sequene (the
__K 0__) to something more permanent.

### Define your own

You can bind macros to key sequences or define your own named macros in the
joerc file.  For example, this will define a macro called __foo__:

	:def foo eof,bol

__foo__ will position the cursor at the beginning of the last line of the
file.  __eof__ jumps to the end of the file.  __bol__ jumps to the beginning
of a line.  Once a macro has been named this way it will show up in the
completion list of the __ESC X__ command prompt.

### Command prompt

You can execute a macro directly by typing it into the command prompt.  Hit
__ESC X__ to bring up the command prompt.  Hit tab at this prompt for a
completion list of all available commands.

Here is a [complete list of commands](http://sourceforge.net/p/joe-editor/mercurial/ci/default/tree/docs/list.md).

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

Repeat arguments can be specified with __^K \\__.  When a command is executed
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

The 'psh' command saves the __^K B__ and __^K K__ positions on a stack.  When the
macro completes, (or when the 'pop' command is called) the positions are
restored.

The 'query' command suspends macro execution until the current dialog is
complete.  It also suspends the automatic 'pop' which happens at the end
of a macro- so if the macro ends in a dialog you often want to call 'query'
to prevent the __^K B__ __^K K__ positions from being restored too early.



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

Type __^K ;__ to bring up a tags search prompt.  If the cursor had been on an
identifier, the prompt is pre-loaded with it.  TAB completion works in this
prompt.

When you hit return, the tags search commences:

If there is one and only one match, JOE will jump directly to the
definition.

If there are multiple matches, then the behavior is controlled by the
notagsmenu option.  If notagsmenu is enabled JOE jumps to the first
definition.  If you hit __^K ;__ again before hitting any other keys, JOE jumps
to the next definition, and so on.  The "tagjump" command also performs this
function.

If notagsmenu is disabled, JOE brings up a menu of all the matches.  You
select the one you want and JOE jumps to it.  If you hit __^K ;__ again before
hitting any other keys, the same menu re-appears with the cursor left in the
original location.

You can hit __^K -__ to move the cusor back to the original location before the
tags search (often __^C__ will work as well).

Since __^K ;__ loads  the definition file into the current window, you
probably want to split the window first with __^K O__, to have both the
original file and the definition file loaded.

## Calulator

JOE has a built-in calculator which can be invoked with __ESC M__.

<p><b>Math functions:</b></p>

<p>sin, cos, tan, exp, sqrt, cbrt, ln, log,
asin, acos, atan, sinh, cosh, tanh, asinh, acosh,
atanh, int, floor, ceil, abs, erf, erfc, j0,
j1, y0, y1</p>

<p><b>Variables:</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>e</td><td>Set to 'e'</td></tr>

<tr valign="top"><td>pi</td><td>Set to 'pi'</td></tr>

<tr valign="top"><td>top</td><td>Set to line number of top window line</td></tr>

<tr valign="top"><td>lines</td><td>Set to number of lines in file</td></tr>

<tr valign="top"><td>line</td><td>Set to current line number</td></tr>

<tr valign="top"><td>col</td><td>Set to current column number</td></tr>

<tr valign="top"><td>byte</td><td>Set to current byte number</td></tr>

<tr valign="top"><td>size</td><td>Set to buffer size</td></tr>

<tr valign="top"><td>height</td><td>Set to window height</td></tr>

<tr valign="top"><td>width</td><td>Set to window width</td></tr>

<tr valign="top"><td>char</td><td>Set to ASCII val of character under cursor	</td></tr>

<tr valign="top"><td>markv</td><td>True if there is a valid block set (^KB ...
^KK)</td></tr>

<tr valign="top"><td>rdonly</td><td>True if file is read-only</td></tr>

<tr valign="top"><td>arg</td><td>Current repeat argument</td></tr>

<tr valign="top"><td>argset</td><td>True if a repeat argument was given</td></tr>

<tr valign="top"><td>is_shell</td><td>True if executed in an active shell window</td></tr>

<tr valign="top"><td>no_windows</td><td>No. buffer windows on the screen</td></tr>

<tr valign="top"><td>ans</td><td>Result of previous expression</td></tr>
</tbody>
</table>

<p><b>Commands:</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>hex</td><td>Hex display mode</td></tr>

<tr valign="top"><td>dec</td><td>Decimal display mode</td></tr>

<tr valign="top"><td>ins</td><td>Insert 'ans' into buffer</td></tr>

<tr valign="top"><td>sum</td><td>Sum of numbers in block</td></tr>

<tr valign="top"><td>cnt</td><td>Count numbers in block</td></tr>

<tr valign="top"><td>avg</td><td>Average value of numbers in block</td></tr>

<tr valign="top"><td>dev</td><td>Standard deviation of numbers in block</td></tr>

<tr valign="top"><td>eval</td><td>Evaluate math expressions in block (or whole file
		if no block set).</td></tr>

<tr valign="top"><td>joe(...)</td><td>Execute a JOE macro (argument in same format
		as joerc file macros).  Return value of
		JOE macro is returned (for macro success,
		return true (non-zero)).</td></tr>
</tbody>
</table>

<p>For example:</p>

<p>joe(sys,"[ 1 == 1 ]",rtn)</p>

<p>([ 1 == 1 ]) is a shell command.  "[" is a synonym for
the "test" UNIX command.</p>

<p>Returns true.</p>

<p>Remember: argument for JOE macro command
"if" is a math expression.  So for example, the
macro:</p>

<p>if,"joe(sys,\"[ 1 == 1 ]\",rtn)",then,"TRUE",endif</p>

<p>Types TRUE into the buffer.</p>

<p><b>Operators:</b></p>
	
<p>^  * / %  + -  &lt; &lt;= &gt; &gt;= == !=  &amp;&amp;  ||  ? :  =  :</p>

<p>&amp;&amp;, || and ? : work as in C and sh as far as side effects: if the
left side of &amp;&amp; is false, the right side is not evaluated.</p>

<p>: is expression separator</p>

## Shell windows

Hit __^K '__ to run a command shell in one of JOE's windows.  When the
cursor is at the end of a shell window (use __^K V__ if it's not),
whatever you type is passed to the shell instead of the window.  Any output
from the shell or from commands executed in the shell is appended to the
shell window (the cursor will follow this output if it's at the end of the
shell window).  This command is useful for recording the results of shell
commands- for example the output of __make__, the result of __grep__ping
a set of files for a string, or directory listings from __FTP__ sessions. 
Besides typeable characters, the keys __^C__, Backspace, DEL, Return and
__^D__ are passed to the shell.  Type the shell __exit__ command to stop recording
shell output.  If you press __^C__ in a shell window, when the cursor is
not at the end of the window, the shell is __kill__ed.

If you use Bash, you can hit: __ESC ' UP-ARROW__ and __ESC ' DOWN-ARROW__ to
scroll through Bash's history buffer.  Other keys work as well: try
__ESC ' ^A__ to go to beginning of line or __ESC ' ^E__ to go to end of line.
Unfortunately JOE only emulates a dumb terminal, so you have to use a lot of
imagination to do any editing beyond hitting backspace.

In general, any character quoted with __ESC '__ is sent to the shell.

Also sent to the shell: TAB, Backspace, Enter, __^C__ and __^D__.

<a name="popup"></a>
## Pop-up shell windows

Hit F1 - F4 to open and switch between shell windows.

The terminal emulator is improved so that when you type "man ls" it's
formatted correctly (it's improved enough so that some interactive programs
will work in it).  Even so, the shell window is still an edit buffer.

The old shell window (with no terminal emulation) still exists: use __^K '__ to
invoke it as usual.  This is useful to see control sequences emitted by a
program.

More of the keys get passed to the running program in pop-up shell windows
compared with the older one.  There is a :shell section of the joerc file to
control which ones.  In particular arrow keys and Ctrl-C are passed to the
program.  It means you can easily step through bash history with the arrow
keys, or abort programs the normal way with Ctrl-C.

On the other hand, loss of Ctrl-C means it's less obvious how to close the
window.  One way is to move the cursor off of the shell data entry point
(with Ctrl-P), and then hit Ctrl-C.  Another is to hit __^K Q__.  Finally, you
can type 'pop' at the command prompt.

If you need to pass a key to the shell that JOE normally uses, quote it.  For
example, if you invoke "emacs -nw" in the shell window, you can exit it with:

	ESC ' ^X ^C

To quickly position the cursor back to the point where data is entered into
the shell, hit __^K V__.

When you open a shell window, a JOE-specific startup-script is sourced. 
It's located in /etc/joe/shell.sh (also /etc/joe/shell.csh).  It contains
some aliases which allow you to control JOE with fake shell commands.  I
have these commands so far:

Command       | Action
--------------|--------
clear         | erase shell window (delete buffer contents)
joe file      | edit a file in JOE
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
emulator: __ESC { joe_macro }__.  When this is received, the macro is executed. 
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
	you are in the right place, hit __^K E__ to load a file (or type "edit file"
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

	Now use __^P__ to position the cursor on one of the lines of the list. 
	Hit __ESC SPACE__ to have JOE edit the file and jump to the specified
	line (also you can use __ESC -__ and __ESC =__ to step through the list).

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

	Hit __ESC =__ and __ESC -__ to step through the errors.


### How it works..

* There is a new mode "ansi".  (__ESC X__ mode ansi).  When this mode is
enabled, the screen updater hides escape sequences which are in the
buffer.  Otherwise you get a big mess from the sequences surrounding
colored output from 'ls'.

* There is a new built-in syntax: "ansi".  (__^T Y__ ansi).  This syntax
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

* Visit the files/locations in the list with __ESC -__ and __ESC =__

* Search and replace across all files in the list by using the 'e' search
  and replace option.

* Clear the list by using the "release" command.

Also, you can use __ESC SPACE__ ('jump' command) to parse the line the cursor is
on and jump to the parsed filename and line number.  'jump' uses the
grep/find parser unless 'parserr' had been previously issued in the buffer.

## Grep-find

Hit __ESC G__ to bring up the prompt.  Enter a command which results in file
names with line numbers, for example: 'grep -n fred *.c'.  This will list all
instances of 'fred' in the *.c files.  You need the '-n' to get the line
numbers.

Now you can hit __ESC SPACE__ on one of the lines to jump to the selected
file.  Also, you can use __ESC =__ and __ESC -__ to step through each line.

## Compile

Hit __ESC C__ to save all modified files and then bring up the compile prompt. 
Enter the command you want to use for the compiler (typically "make").  The
compiler will run in a shell window.  When it's complete, the results are
parsed.

If there are any errors or warnings from the compiler you can hit
__ESC SPACE__ on one of the lines to jump to the selected file.  Also,
you can use __ESC =__ and __ESC -__ to step through each line.

## Syntax highlighting

To enable highlight use __^T H__.

To select the syntax, use __^T Y__.  You can hit TAB at the prompt for a
completion list.

JOE tries to determine the syntax to use based on the name and contents of
the file.  The configuration file /etc/joe/ftyperc contains the definitions.

Each syntax is defined by a file located /usr/share/joe/syntax/.

### How JOE syntax highlighting works

*from [c.jsf](http://joe-editor.hg.sourceforge.net/hgweb/joe-editor/joe-editor/file/tip/syntax/c.jsf.in),
slightly modified*

A deterministic state machine that performs lexical analysis of the target
language is provided in a syntax file.  (This is the "assembly language" of
syntax highlighting.  A separate program could in principal be used to
convert a regular expression NFA syntax into this format).

Each state begins with:

    :<name> <color-name> <context>

<name\> is the state's name.

<color-name\> is the color used for characters eaten by the state
(really a symbol for a user definable color).

<context\> tells JOE if the current character is part of a comment or a
string.  This allows JOE to skip over comments and strings when matching
characters such as parentheses.  To use this feature, the
highlighter_context option must be applied to the files highlighted by the
corresponding syntax.  To apply the option, add it to ftyperc for those file
entries.

The valid contexts are:

  * comment  This character is part of a comment.  Example:  /\* comment \*/

  * string   This character is part of a string.  Examples: "string" 'c' 'string'

The comment and string delimiters themselves should be marked with the
appropriate context.  The context is considered to be part of the color, so
the recolor=-N and recolormark options apply the context to previous
characters.

The first state defined is the initial state.

Within a state, define transitions (jumps) to other states.  Each
jump has the form:

        <character-list> <target-state-name> [<option>s]

There are three ways to specify <character-list\>s, either __\*__ for any
character not otherwise specified, __&__ to match the character in the
delimiter match buffer (opposite character like \( and \) automatically
match) or a literal list of characters within quotes (ranges and escape
sequences allowed).  When the next character matches any in the list, a jump
to the target-state is taken and the character is eaten (we advance to the
next character of the file to be colored).

The * transition should be the first transition specified in the state.

There are several options:

* __noeat__       - Do not eat the character, instead feed it to the next state
                    (this tends to make the states smaller, but be careful: you
                    can make infinite loops).  'noeat' implies 'recolor=-1'.

* __recolor=-N__  - Recolor the past N characters with the color of the
                    target-state.  For example once /\* is recognized as the
                    start of C comment, you want to color the /\* with the C
                    comment color with recolor=-2.

* __mark__        - Mark beginning of a region with current position.

* __markend__     - Mark end of region.

* __recolormark__ - Recolor all of the characters in the marked region with
                    the color of the target-state.  If markend is not given,
                    all of the characters up to the current position are recolored.
                    Note that the marked region can not cross line boundaries and
                    must be on the same line as recolormark.

* __buffer__      - Start copying characters to a string buffer, beginning with this
                    one (it's OK to not terminate buffering with a matching
                    'strings', 'istrings' or 'hold' option- the buffer is limited
                    to leading 23 characters).

* __save_c__      - Save character in delimiter match buffer.

* __save_s__      - Copy string buffer to delimiter match buffer.

* __strings__     - A list of strings follows.  If the buffer matches any of the
                    given strings, a jump to the target-state in the string list
                    is taken instead of the normal jump.

* __istrings__    - Same as strings, but case is ignored.
                    Note: strings and istrings should be the last option on the
                    line.  They cause any options which follow them to be ignored.

* __hold__        - Stop buffering string- a future 'strings' or 'istrings' will
                    look at contents of buffer at this point.  Useful for distinguishing
                    commands and function calls in some languages 'write 7' is a command
                    'write (' is a function call- hold lets us stop at the space and delay
                    the string lookup until the ( or 7.

The format of the string list is:

        "string"   <target-state> [<options>s]
        "string"   <target-state> [<options>s]
        "&"        <target-state> [<options>s]   # matches contents of delimiter match buffer
        done

(all of the options above are allowed except "strings", "istrings" and "noeat".  noeat is
always implied after a matched string).

Weirdness: only states have colors, not transitions.  This means that you
sometimes have to make dummy states with

        *    <next-state>    noeat

just to get a color specification.

Delimiter match buffer is for perl and shell: a regex in perl can be s<..>(...)
and in shell you can say: <<EOS ....... EOS.  The idea is that you capture
the first delimiter into the match buffer (the &lt; or first "EOS") and then
match it to the second one with "&" in a string or character list.

#### Subroutines

Highlighter state machines can now make subroutine calls.  This works by
template instantiation: the called state machine is included in your
current state machine, but is modified so that the return address points
to the called.  There is still no run-time stack (the state is represented
as a single integer plus the saved delimiter string).

Recursion is allowed, but is self limited to 5 levels.

__Note:__ this recursion limit is obsolete.  Subroutines now do use a stack
so the call-depth is limitless.

To call a subroutine, use the 'call' option:

        "\""    fred    call=string(dquote)

The subroutine called 'string' is called and the jump to 'fred' is
ignored.  The 'dquote' option is passed to the subroutine.

The subroutine itself returns to the caller like this:

        "\""    whatever    return

If we're in a subroutine, the return is made.  Otherwise the jump
to 'whatever' is made.

There are several ways of delimiting subroutines which show up in how it
is called.  Here are the options:

* __call=string()__         - A file called string.jsf is the subroutine.
                              The entire file is the subroutine.  The starting
                              point is the first state in the file.

* __call=library.string()__ - A file called library.jsf has the subroutine.
                              The subroutine within the file is called string.

* __call=.string()__        - There is a subroutine called string in the current file.

When a subroutine is within a file, but is not the whole file, it is delimited
as follows:

    .subr string

    . . . states for string subroutine . . .

    .end

Option flags can be passed to subroutines which control preprocessor-like
directives.  For example:

    .ifdef dquote
        "\""    idle    return
    .endif
    .ifdef squote
        "'"     idle    return
    .endif

.else if also available.  .ifdefs can be nested.


<a name="joerc"></a>
## The joerc file

__^T__ options, the help screens and the key-sequence to editor command
bindings are all defined in JOE's initialization file.  If you make a copy
of this file (which normally resides in __/etc/joe/joerc__) to
__$HOME/.joerc__, you can customize these setting to your liking.  The
syntax of the initialization file should be fairly obvious and there are
further instructions in it.

The __joerc__ file has a directive to include another file (:include).  This
facility is used to include a file called __ftyperc__ (usually located in
__/etc/joe/ftyperc__).  __ftyperc__ has the file type table which determines
which local options (including syntax for the highlighter) are applied to
each file type.

### Initialization file loading sequence

If the path for an initialization file begins with '/' (you can specify this
with the include directive), JOE only tries to load it from the absolute
path.  Otherwise, JOE tries to load initialization files (the joerc file and
any files included in it, typically ftyperc) from three places:

* "$HOME/.joerc" - The user's personalized joerc file.

* "/etc/joe/joerc" - The system's joerc file.
The exact path is fixed during the build, and is determined by the
--sysconfdir configure script option.

* "*joerc" - Built-in file
This means JOE searches for the file in a table of files linked in with the
JOE binary (they are in the builtins.c file).  A built-in joerc file is
provided so that the editor will run in cases where system's joerc is
inaccessible.

If the system's joerc file is newer than the user's joerc file, JOE will
print a warning in the startup log.  Previous versions of JOE would prompt
the user for this case- the idea was that JOE may be unusable with an out of
date initialization file.

### joerc file sections

The __joerc__ file is broken up into a number of sections:

* Global options
  Options which are not file specific, like __noxon__.

* File name and content dependent options
  Options which depend on the file type, such as __autoindent__.  The
  __ftyperc__ file is included in this section.

* __^T__ menu system definition
  Use :defmenu to define a named menu of macros.  The __menu__ command
brings up a specific named menu.  __^T__ is a macro which brings up the root
menu: __mode,"root",rtn__.

* Help screen contents
  Each help screen is named.  The name is used to implement context
  dependent help.

* Key bindings
  Key binding tables are defined.  You can define as many as you like (you
can switch to a specific one with the __keymap__ command), but
the following must be provided:

    * __main__ Editing windows
    * __prompt__ Prompt windows
    * __query__ Single-character query prompts
    * __querya__ Single-character query for quote
    * __querysr__ Single-character query for search and replace
    * __shell__ Shell windows
    * __vtshell__ Terminal emulator shell windows

Key binding tables can inherit bindings from already defined tables.  This
allows you to group common key bindings into a single table which is
inherited by the others.

### Mode command

Many options can be controlled with the __^T__ menu.  This menu is defined
in the joerc file.  Each option in the __^T__ menu just executes a macro. 
Usually the macro is the mode command.  You can execute the mode command
directly with:

	ESC X mode <enter>

Hit tab for a completion list of all options.

### Menu command

This command calls up a named menu of macros which was defined in the
__joerc__ file.

	ESC X menu <enter>

As usual, hit TAB at the prompt for a completion list of the menus which
exist.

__^T__ is bound to the simple macro __mode,"root",rtn__- it brings up the
root of the options menu system.

## Xterm Mouse support

There are two levels of mouse support.  The -mouse option enables the
first level, which will work with any stock Xterm.  If -joexterm is also
set, mouse support is enhanced, but you need a recent version of XTerm,
and it needs to be ./configured with the --enable-paste64 option.

When -mouse is set, you can:

* Left-click in a text window to set the cursor position.  Left-click in a
  different window to move the cursor to a different window.

* Select text with the mouse.  Left-click and drag to select some text- it
will be as if you had used __^K B__ and __^K K__ to mark it. Left-click (but don't
drag) to position the cursor somewhere else.  Middle click to copy the
selected text to the cursor- it will be as if you had hit __^K C__.  If you drag
past the edge of the text window, the window will auto-scroll to select more
text.  Unfortunately, Xterm does not send any codes when the cursor is
outside of the Xterm frame itself, so this only works if the mouse is still
contained within the Xterm frame.  I've sent a patch to the Xterm maintainer
to improve this, but he has not taken it yet.

* Resize windows with the mouse: click and hold on a status line
dividing two windows to move it.

* Select menu entries (such as any completion menu or the __^T__ options
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

## Hex edit mode

When this mode is selected (either put -hex on the command line, or look for
"Hex edit mode" after hitting __^T__), the buffer is displayed as a hex dump,
but all of the editing commands operate the same way.  It is most useful to
select overtype mode in conjunction with hex dump (hit __^T T__).  Then typing
will not insert.

- To enter the hex byte 0xF8 type __ESC ' x F 8__

- You can use __^K C__ to copy a block as usual.  If overtype mode is selected,
  the block will overwrite the destination data without changing the size of
  the file.  Otherwise it inserts.

- Hit __ESC X byte &lt;Enter&gt;__, to jump to a particular byte offset.  Hex values
  can be entered into this prompt like this: 0x2000.

- Search, incremental search, and search &amp; replace all operate as usual.

<a name="evariables"></a>
## Environment variables 

For JOE to operate correctly, a number of other environment settings must be 
correct.  The throughput (baud rate) of the connection between the computer 
and your terminal must be set correctly for JOE to update the screen 
smoothly and allow typeahead to defer the screen update.  Use the __stty nnn__
command to set this.  You want to set it as close as possible to 
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
__LINES__ and __COLUMNS__ environment variables.  The terminal size is
variable on modern systems and is determined by an ioctl, so these
parameters often have no effect.

JOE normally expects that flow control between the computer and your
terminal to use __^S__/__^Q__ handshaking (i.e., if the computer is sending
characters too fast for your terminal, your terminal sends __^S__ to stop the
output and __^Q__ to restart it).  If the flow control uses out-of-band or
hardware handshaking or if your terminal is fast enough to always keep up
with the computer output and you wish to map __^S__/__^Q__ to edit commands, you can
set the environment variable __NOXON__ to have JOE attempt to turn off
__^S__/__^Q__ handshaking.  If the connection between the computer and your terminal
uses no handshaking and your terminal is not fast enough to keep up with the
output of the computer, you can set the environment variable __DOPADDING__
to have __JOE__ slow down the output by interspersing PAD characters
between the terminal screen update sequences.

Here is a complete list of the environment variables:

* BAUD
Tell JOE the baud rate of the terminal (overrides value reported by stty).
<br>

* COLUMNS
Set number of columns in terminal emulator (in case
termcap entry is wrong).  This is only useful on old system which don't have
the "get window size" ioctl.
<br>

* DOPADDING
Enable JOE to send padding NULs to the terminal
when set (for very old terminals).
<br>

* HOME
Used to get path to home directory for ~
expansion and also to find ~/.joerc file ~/.joe directory.
<br>

* HOSTNAME
Used to get hostname to put in EMACS compatible locks.
<br>

* JOETERM
Gives terminal type: JOE will use this instead of TERM if it's set.
<br>

* LANG
Sets locale (like en_US.utf-8).  JOE uses
the first of these which is set: LC_ALL, LC_CTYPE, LANG.
<br>

* LC_ALL
Sets locale (like en_US.utf-8).  JOE
uses the first of these which is set: LC_ALL, LC_CTYPE, LANG.
<br>

* LC_CTYPE
Sets locale (like en_US.utf-8).  JOE
uses the first of these which is set: LC_ALL, LC_CTYPE, LANG.
<br>

* LINES
Set number of lines in terminal emulator (in case
termcap entry is wrong).  This is only useful on old system which don't have
the "get window size" ioctl.
<br>

* NOXON
Disable __^S__ and __^Q__ flow control, possibly
allowing __^S__ and __^Q__ to be used as editor keys.
<br>

* SHELL
Path to shell (like /bin/sh).  This is
used in several places: If you are on a system with no job control, this
shell is invoked when you hit __^K Z__.  Also this is shell which is run in shell
windows.  If SHELL is not set (Cygwin) or if it's set to /bin/sh, JOE
invokes the first of these which exists: /bin/bash, /usr/bin/bash, /bin/sh.
<br>

* SIMPLE_BACKUP_SUFFIX
If this is set, it is
appended to the file name instead of ~ to create the backup file name.
<br>

* TAGS
If set to a path to a file, JOE tries to
use this as the "tags" file if there is no "tags" file in the current
directory.
<br>

* TEMP
If set, gives path to directory to open
swapfile instead of /tmp
<br>

* TERMCAP
Used by JOE's built-in termcap file
parser (not used for terminfo).  A termcap entry can be placed directly in
this variable (which will be used if it matches TERM), or if it begins with
/, it gives a list of paths to termcap files to search.
<br>

* TERMPATH
Gives list of paths to termcap files to search when TERMCAP has a
termcap entry (otherwise it's ignored).  The default list of paths to
termcap files (when TERMCAP and TERMPATH do not have it) is: "~/.termcap
/etc/joe/termcap /etc/termcap"
<br>

* TERM
Gives terminal type, like "vt100" or "xterm".
<br>

* USER
Used to get user name for EMACS compatible file locks.
<br>

<a name="list"></a>
## JOE commands grouped by function

These commands can be entered at the __ESC X__ prompt.

<p><b>Background programs</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>bknd</td><td>Run a shell in a window</td></tr>
<tr valign="top"><td>vtbknd</td><td>Run a shell in a terminal emulator window</td></tr>
<tr valign="top"><td>killproc</td><td>Kill program in current window</td></tr>
<tr valign="top"><td>run</td><td>Run a UNIX command in a window</td></tr>
<tr valign="top"><td>sys</td><td>Run a UNIX command and return to editor when done (I/O does
not go through editor, but we get the command's return status).</td></tr>
</tbody>
</table>

<p><b>Blocks</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>blkcpy</td><td>Copy marked block to cursor</td></tr>
<tr valign="top"><td>blkdel</td><td>Delete marked block</td></tr>
<tr valign="top"><td>blkmove</td><td>Move marked block to cursor</td></tr>
<tr valign="top"><td>blksave</td><td>Save marked block into a file</td></tr>
<tr valign="top"><td>copy</td><td>Copy block to kill-ring</td></tr>
<tr valign="top"><td>drop</td><td>Set markb.  If it was already set, eliminate
it.</td></tr>
<tr valign="top"><td>dropon</td><td>Set markb.  If it was already set, eliminate it.  Turn on marking
mode.</td></tr>
<tr valign="top"><td>toggle_marking</td><td>If we're in a block: clear markb and markk. 
If marking is off: set markb and turn on marking.  If marking is on: set
markk (swap if necessary with markb) and turn marking off.</td></tr>
<tr valign="top"><td>begin_marking</td><td>If we're on an edge of a block: set markb to other edge
and turn on marking mode.  Otherwise set markb to cursor and turn on marking
mode.</td></tr>
<tr valign="top"><td>select</td><td>Set markb.  If it was already set, do
nothing.</td></tr>
<tr valign="top"><td>filt</td><td>Filter block or file through a UNIX command</td></tr>
<tr valign="top"><td>markb</td><td>Set beginning of block mark</td></tr>
<tr valign="top"><td>markk</td><td>Set end of block mark</td></tr>
<tr valign="top"><td>markl</td><td>Mark current line</td></tr>
<tr valign="top"><td>nmark</td><td>Eliminate markb and markk</td></tr>
<tr valign="top"><td>picokill</td><td>Delete line or block</td></tr>
<tr valign="top"><td>pop</td><td>Restore markb and markk values from stack</td></tr>
<tr valign="top"><td>psh</td><td>Push markb and markk values onto a stack</td></tr>
<tr valign="top"><td>swap</td><td>Switch cursor with markb</td></tr>
<tr valign="top"><td>tomarkb</td><td>Move cursor to markb</td></tr>
<tr valign="top"><td>tomarkbk</td><td>Move cursor to markb or markk</td></tr>
<tr valign="top"><td>tomarkk</td><td>Move cursor to markk</td></tr>
<tr valign="top"><td>yank</td><td>Insert top of kill ring</td></tr>
<tr valign="top"><td>yankpop</td><td>Scroll through kill ring</td></tr>
<tr valign="top"><td>yapp</td><td>Append next kill to top of kill ring</td></tr>
<tr valign="top"><td>upper</td><td>Convert everything in block to uppercase</td></tr>
<tr valign="top"><td>lower</td><td>Convert everything in block to lowercase</td></tr>
</tbody>
</table>

<p><b>Buffers</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>bufed</td><td>Buffer menu</td></tr>
<tr valign="top"><td>edit</td><td>Load file into window: asks to reload if buffer
exists</td></tr>
<tr valign="top"><td>switch</td><td>Load file into window: always uses buffer if it
exists</td></tr>
<tr valign="top"><td>scratch</td><td>Push a scratch buffer into current window</td></tr>
<tr valign="top"><td>popabort</td><td>Abort and pop window from stack (do nothing if stack empty)</td></tr>
<tr valign="top"><td>nbuf</td><td>Load next buffer into current window</td></tr>
<tr valign="top"><td>pbuf</td><td>Load previous buffer into current window</td></tr>
<tr valign="top"><td>reload</td><td>Re-read file into buffer (revert)</td></tr>
<tr valign="top"><td>reloadall</td><td>Re-read all unmodified buffers</td></tr>
</tbody>
</table>

<p><b>Cursor Motion</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>bof</td><td>Move cursor to beginning of file</td></tr>
<tr valign="top"><td>bol</td><td>Move cursor to beginning of line (always)</td></tr>
<tr valign="top"><td>bop</td><td>Move to beginning of a paragraph</td></tr>
<tr valign="top"><td>bos</td><td>Move to beginning of screen</td></tr>
<tr valign="top"><td>bkwdc</td><td>Search backwards for a character</td></tr>
<tr valign="top"><td>byte</td><td>Move cursor to specific byte offset into the
file.</td></tr>
<tr valign="top"><td>col</td><td>Move cursor to specific column number.</td></tr>
<tr valign="top"><td>dnarw</td><td>Move cursor down one line</td></tr>
<tr valign="top"><td>eof</td><td>Move cursor to end of file</td></tr>
<tr valign="top"><td>eol</td><td>Move cursor to end of line</td></tr>
<tr valign="top"><td>eop</td><td>Move cursor to end of paragraph</td></tr>
<tr valign="top"><td>fwrdc</td><td>Search forward for matching character</td></tr>
<tr valign="top"><td>gomark</td><td>Move cursor to a bookmark</td></tr>
<tr valign="top"><td>home</td><td>Move cursor to beginning of line</td></tr>
<tr valign="top"><td>line</td><td>Move cursor to specified line</td></tr>
<tr valign="top"><td>ltarw</td><td>Move cursor left</td></tr>
<tr valign="top"><td>nedge</td><td>Move cursor to next edge</td></tr>
<tr valign="top"><td>nextpos</td><td>Move cursor to next position in cursor position
history</td></tr>
<tr valign="top"><td>nextword</td><td>Move cursor to end of next word</td></tr>
<tr valign="top"><td>pedge</td><td>Move cursor to previous edge</td></tr>
<tr valign="top"><td>prevpos</td><td>Move cursor to previous position in cursor position
history</td></tr>
<tr valign="top"><td>prevword</td><td>Move cursor to beginning of previous word</td></tr>
<tr valign="top"><td>rtarw</td><td>Move cursor right</td></tr>
<tr valign="top"><td>setmark</td><td>Set a bookmark</td></tr>
<tr valign="top"><td>tomatch</td><td>Move cursor to matching delimiter</td></tr>
<tr valign="top"><td>tos</td><td>Move cursor to top of screen</td></tr>
<tr valign="top"><td>uparw</td><td>Move cursor up</td></tr>
</tbody>
</table>

<p><b>Deletion</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>backs</td><td>Backspace</td></tr>
<tr valign="top"><td>backw</td><td>Backspace a word</td></tr>
<tr valign="top"><td>delbol</td><td>Delete to beginning of line</td></tr>
<tr valign="top"><td>delch</td><td>Delete character under cursor</td></tr>
<tr valign="top"><td>deleol</td><td>Delete to end of line</td></tr>
<tr valign="top"><td>dellin</td><td>Delete entire line</td></tr>
<tr valign="top"><td>delw</td><td>Delete word to right</td></tr>
</tbody>
</table>

<p><b>Error parsing</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>nxterr</td><td>Goto next parsed error</td></tr>
<tr valign="top"><td>parserr</td><td>Parse errors in current file</td></tr>
<tr valign="top"><td>gparse</td><td>Parse grep list in current file</td></tr>
<tr valign="top"><td>jump</td><td>Parse current line and jump to it</td></tr>
<tr valign="top"><td>prverr</td><td>Go to previous parsed error</td></tr>
<tr valign="top"><td>showerr</td><td>Show current message</td></tr>
<tr valign="top"><td>grep</td><td>Execute grep command, parse when done</td></tr>
<tr valign="top"><td>build</td><td>Execute build command, parse when done</td></tr>
<tr valign="top"><td>release</td><td>Release error/grep records</td></tr>
</tbody>
</table>

<p><b>Exit</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>cancel</td><td>Like abort, but doesn't return failure: useful in macros
		to escape out of a prompt.</td></tr>

<tr valign="top"><td>abort</td><td>Abort current buffer/window.  Prompt if it is
changed.</td></tr>

<tr valign="top"><td>abortbuf</td><td>Like above, but just fail if it would have to prompt because
		it's the last window on a modified buffer.</td></tr>

<tr valign="top"><td>ask</td><td>Prompt to save current file: user says yes return, user says
		no: run 'abort'.  Use in a macro:
"ask,query,exsave"</td></tr>

<tr valign="top"><td>exsave</td><td>Save file and exit</td></tr>

<tr valign="top"><td>lose</td><td>emacs kill buffer.  The buffer is deleted- any windows with
		it get a replacement scratch buffer.</td></tr>

<tr valign="top"><td>querysave</td><td>Prompt to save each modified buffer
		Use in a macro: "querysave,query,killjoe"</td></tr>

<tr valign="top"><td>killjoe</td><td>Exit JOE immediately without checking for modified
buffers</td></tr>
</tbody>
</table>

<p><b>Files</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>cd</td><td>Set directory prefix</td></tr>

<tr valign="top"><td>save</td><td>Save file</td></tr>

<tr valign="top"><td>savenow</td><td>Save immediately, unless file name is not
known</td></tr>

<tr valign="top"><td>insf</td><td>Insert a file</td></tr>

</tbody>
</table>

<p><b>Formatting</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>center</td><td>Center line</td></tr>

<tr valign="top"><td>fmtblk</td><td>Format all paragraphs in a block</td></tr>

<tr valign="top"><td>format</td><td>Format current paragraph</td></tr>

<tr valign="top"><td>lindent</td><td>Indent to the left</td></tr>

<tr valign="top"><td>rindent</td><td>Indent to the right</td></tr>
</tbody>
</table>

<p><b>Help</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>help</td><td>Turn help on or off</td></tr>

<tr valign="top"><td>hnext</td><td>Switch to next help screen</td></tr>

<tr valign="top"><td>hprev</td><td>Switch to previous help screen</td></tr>
</tbody>
</table>

<p><b>Inserting</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>ctrl</td><td>Type next key</td></tr>

<tr valign="top"><td>finish</td><td>Complete word in text window</td></tr>

<tr valign="top"><td>insc</td><td>Insert a space </td></tr>

<tr valign="top"><td>open</td><td>Insert newline</td></tr>

<tr valign="top"><td>quote</td><td>Insert a control character</td></tr>

<tr valign="top"><td>quote8</td><td>Insert a meta character</td></tr>

<tr valign="top"><td>rtn</td><td>Return key</td></tr>

<tr valign="top"><td>type</td><td>Insert typed character</td></tr>

<tr valign="top"><td>secure_type</td><td>Insert typed character, but only allowed in prompt windows (not allowed in shell windows)</td></tr>
</tbody>
</table>

<p><b>Macros</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>macros</td><td>Insert keyboard macros into current file</td></tr>

<tr valign="top"><td>play</td><td>Execute a macro</td></tr>

<tr valign="top"><td>query</td><td>Suspend macro recording for user query</td></tr>

<tr valign="top"><td>record</td><td>Record a macro</td></tr>

<tr valign="top"><td>stop</td><td>Stop recording macro</td></tr>
</tbody>
</table>

<p><b>Menu</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>backsmenu</td><td>Undo in file completion menu</td></tr>

<tr valign="top"><td>bofmenu</td><td>Move to beginning of menu</td></tr>

<tr valign="top"><td>bolmenu</td><td>Move to beginning of line in a menu</td></tr>

<tr valign="top"><td>dnarwmenu</td><td>Move down one line in a menu</td></tr>

<tr valign="top"><td>eolmenu</td><td>Move cursor to end of line in a menu</td></tr>

<tr valign="top"><td>eofmenu</td><td>Move cursor to end of menu</td></tr>

<tr valign="top"><td>ltarwmenu</td><td>Move cursor left in a menu</td></tr>

<tr valign="top"><td>rtarwmenu</td><td>Move cursor right in menu</td></tr>

<tr valign="top"><td>uparwmenu</td><td>Move cursor up in menu</td></tr>

<tr valign="top"><td>dnslidemenu</td><td>Scroll menu down one line</td></tr>

<tr valign="top"><td>upslidemenu</td><td>Scroll menu up one line</td></tr>

<tr valign="top"><td>pgupmenu</td><td>Scroll menu up</td></tr>

<tr valign="top"><td>pgdnmenu</td><td>Scroll menu down</td></tr>

<tr valign="top"><td>tabmenu</td><td>Tab through menu</td></tr>
</tbody>
</table>

<p><b>Misc</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>beep</td><td>Beep</td></tr>

<tr valign="top"><td>execmd</td><td>Execute a JOE command</td></tr>

<tr valign="top"><td>debug_joe</td><td>Insert debug information into buffer</td></tr>

<tr valign="top"><td>math</td><td>Calculator</td></tr>

<tr valign="top"><td>maths</td><td>Secure Calculator (no way to run joe() macros)</td></tr>

<tr valign="top"><td>mode</td><td>Mode prompt</td></tr>

<tr valign="top"><td>menu</td><td>Menu prompt</td></tr>

<tr valign="top"><td>msg</td><td>Display a message</td></tr>

<tr valign="top"><td>notmod</td><td>Clear the modified flag</td></tr>

<tr valign="top"><td>retype</td><td>Refresh screen</td></tr>

<tr valign="top"><td>shell</td><td>Suspend process or execute a sub-shell</td></tr>

<tr valign="top"><td>stat</td><td>Display cursor position</td></tr>

<tr valign="top"><td>tag</td><td>Tags file search</td></tr>

<tr valign="top"><td>tagjump</td><td>Jump to next tags file search match (only if notagsmenu is set)</td></tr>

<tr valign="top"><td>timer</td><td>Execute a macro periodically</td></tr>

<tr valign="top"><td>txt</td><td>Insert text.  If first character is \`, then text is assumed to be a format string (that is, the string
used to define the status line for the rmsg and lmsg options) and is formatted before the insertion.</td></tr>

<tr valign="top"><td>name</td><td>Insert current file name</td></tr>

<tr valign="top"><td>language</td><td>Insert current language</td></tr>

<tr valign="top"><td>charset</td><td>Insert current character set</td></tr>

<tr valign="top"><td>keymap</td><td>Switch to another keymap</td></tr>
</tbody>
</table>

<p><b>Prompts</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>

<tr valign="top"><td>complete</td><td>Complete a file-name in a prompt</td></tr>

<tr valign="top"><td>if</td><td>Only run following cmds if expr is true
(non-zero)</td></tr>

<tr valign="top"><td>then</td><td>Same as rtn but only works in prompt windows</td></tr>

<tr valign="top"><td>elsif</td><td>Try a new condition</td></tr>

<tr valign="top"><td>else</td><td>Toggle truth flag</td></tr>

<tr valign="top"><td>endif</td><td>Start running cmds again</td></tr>
</tbody>
</table>

<p>Here is an example 'if' macro:</p>

<p>if,"char==65",then,"it's an A",else,"it's not an A",endif	__^[ q__</p>

<p>When you hit __^[ q__, if the character under the cursor is an 'A': "it's a A"
is inserted into the buffer, otherwise "it's not an A" is inserted.</p>

<p>"if" creates a math prompt (like __ESC M__).  "then" is like "rtn"- it hits the
return key for this prompt.</p>

<p>Within the math prompt, the following variables are available:</p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>char</td><td>ASCII value of character under cursor</td></tr>

<tr valign="top"><td>width</td><td>Width of screen</td></tr>

<tr valign="top"><td>height</td><td>Height of screen</td></tr>

<tr valign="top"><td>byte</td><td>byte number</td></tr>

<tr valign="top"><td>col</td><td>column number</td></tr>

<tr valign="top"><td>line</td><td>line number</td></tr>

<tr valign="top"><td>lines</td><td>no. lines in file</td></tr>

<tr valign="top"><td>top</td><td>line number of top line of window</td></tr>
</tbody>
</table>


<p><b>Repeat</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>arg</td><td>Prompt for repeat argument</td></tr>

<tr valign="top"><td>uarg</td><td>Universal argument</td></tr>
</tbody>
</table>

<p><b>Scrolling</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>crawll</td><td>Pan screen left</td></tr>

<tr valign="top"><td>crawlr</td><td>Pan screen right</td></tr>

<tr valign="top"><td>dnslide</td><td>Scroll screen down 1 line</td></tr>

<tr valign="top"><td>pgdn</td><td>Scroll screen down</td></tr>

<tr valign="top"><td>pgup</td><td>Scroll screen up</td></tr>

<tr valign="top"><td>upslide</td><td>Scroll up one line</td></tr>
</tbody>
</table>

<p><b>Search and replace</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>ffirst</td><td>Find text</td></tr>

<tr valign="top"><td>fnext</td><td>Repeat previous search</td></tr>

<tr valign="top"><td>isrch</td><td>Incremental search forward</td></tr>

<tr valign="top"><td>qrepl</td><td>Search and replace</td></tr>

<tr valign="top"><td>rfirst</td><td>Search backwards for text</td></tr>

<tr valign="top"><td>rsrch</td><td>Reverse incremental search</td></tr>
</tbody>
</table>

<p><b>Windows</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>explode</td><td>Display one window or display all windows</td></tr>

<tr valign="top"><td>dupw</td><td>Duplicate current window</td></tr>

<tr valign="top"><td>groww</td><td>Increase size of window</td></tr>

<tr valign="top"><td>nextw</td><td>Move cursor to next window</td></tr>

<tr valign="top"><td>prevw</td><td>Go to previous window</td></tr>

<tr valign="top"><td>shrinkw</td><td>Shrink window</td></tr>

<tr valign="top"><td>splitw</td><td>Split window into two</td></tr>

<tr valign="top"><td>tw0</td><td>Eliminate this window</td></tr>

<tr valign="top"><td>tw1</td><td>Show only one window</td></tr>

<tr valign="top"><td>mwind</td><td>Get error messages window on the screen and put cursor in
it.</td></tr>

<tr valign="top"><td>showlog</td><td>Get startup log scratch buffer into window.</td></tr>

<tr valign="top"><td>mfit</td><td>Fit two windows on the screen: make current window 6 lines,
		and give rest of space to window above.  The window above is
		either the existing previous window, a newly created one if
		there wasn't one.</td></tr>
</tbody>
</table>

<p><b>Undo</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>redo</td><td>Re-execute the latest undone change</td></tr>

<tr valign="top"><td>undo</td><td>Undo last change</td></tr>
</tbody>
</table>

<p><b>Mouse</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>tomouse</td><td>Move the cursor to where the mouse was
clicked/dragged</td></tr>

<tr valign="top"><td>defmdown</td><td>Default single-click handlers</td></tr>

<tr valign="top"><td>defmup</td><td>Default single-click release handlers</td></tr>

<tr valign="top"><td>defmdrag</td><td>(click to move cursor, drag to select
characters)</td></tr>

<tr valign="top"><td>defm2down</td><td>Default double-click handlers</td></tr>

<tr valign="top"><td>defm2up</td><td>Default double-click release handlers</td></tr>

<tr valign="top"><td>defm2drag</td><td>(drag to select words)</td></tr>

<tr valign="top"><td>defm3down</td><td>Default triple-click handlers</td></tr>

<tr valign="top"><td>defm3up</td><td>Default triple-click release handlers</td></tr>

<tr valign="top"><td>defm3drag</td><td>(drag to select lines)</td></tr>

<tr valign="top"><td>xtmouse</td><td>Handle xterm mouse events</td></tr>

<tr valign="top"><td>extmouse</td><td>Handle extended xterm mouse events</td></tr>

<tr valign="top"><td>paste</td><td>Insert base64 encoded text (for XTerm --enable-base64
option).</td></tr>

<tr valign="top"><td>brpaste</td><td>Insert text until __ESC [ 2 0 1 ~__ has been received. 
This is for bracketed paste support.</td></tr>

</tbody>
</table>


