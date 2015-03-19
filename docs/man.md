<h1>JOE</h1>
<h2>Name</h2>
joe - Joe's Own Editor
<h2>Syntax</h2>
<b>joe [global-options] [ [local-options] filename ]...</b>
<br><br>
<b>jstar [global-options] [ [local-options] filename ]...</b>
<br><br>
<b>jmacs [global-options] [ [local-options] filename ]...</b>
<br><br>
<b>rjoe [global-options] [ [local-options] filename ]...</b>
<br><br>
<b>jpico [global-options] [ [local-options] filename ]...</b>
<h2>Description</h2>
<p>JOE is a powerful ASCII-text screen editor.  It has a "mode-less" user
interface which is similar to many user-friendly PC editors.  Users of
Micro-Pro's WordStar or Borland's "Turbo" languages will feel at home.  JOE
is a full featured UNIX screen-editor though, and has many features for
editing programs and text.</p>
<p>JOE also emulates several other editors.  JSTAR is a close imitation of
WordStar with many "JOE" extensions.  JPICO is a close imitation of the
Pine mailing system's PICO editor, but with many extensions and
improvements.  JMACS is a GNU-EMACS imitation.  RJOE is a restricted
version of JOE, which allows you to edit only the files specified on the
command line.</p>
<p>Although JOE is actually five different editors, it still requires only one
executable, but one with five different names.  The name of the editor with
an "rc" appended gives the name of JOE's initialization file, which
determines the personality of the editor.</p>
<p>JOE is free software;  you can distribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation.  I have no plans for turning JOE into a commercial or share-ware
product.  JOE is available over the Internet from
<b>www.sourceforge.net/projects/joe-editor</b>.</p>

<h2>Usage</h2>

<p>To start the editor, type <b>joe</b> followed by zero or more names of files
you want to edit.  Each file name may be preceded by a local option setting
(see the local options table which follows).  Other global options, which
apply to the editor as a whole, may also be placed on the command line (see
the global options table which follows).  If you are editing a new file, you
can either give the name of the new file when you invoke the editor, or in
the editor when you save the new file.  A modified syntax for file names is
provided to allow you to edit program output, standard input/output, or
sections of files or devices.  See the section <b>Filenames</b> below for
details.</p>
<p>Once you are in the editor, you can type in text and use special
control-character sequences to perform other editing tasks.  To find out
what the control-character sequences are, read the rest of this man page or
type <b>^K H</b> for help in the editor.</p>
<p>Now for some obscure computer-lore:</p>
<p>The <b>^</b> means that you hold down the <b>Control</b> key while pressing
the following key (the same way the <b>Shift</b> key works for uppercase
letters).  A number of control-key sequences are duplicated on other keys,
so that you don't need to press the control key: <b>ESC</b> will work in
place of <b>^[</b>, <b>Del</b> will work in place of <b>^?</b>, <b>Backspace</b>
will work in place of <b>^H</b>, <b>Tab</b> will work in place of <b>^I</b>,
<b>Return</b> or <b>Enter</b> will work in place of <b>^M</b> and
<b>Linefeed</b> will work in place of <b>^J</b>.  Some keyboards may give you
trouble with some control keys.  <b>^_</b>, <b>^^</b> and <b>^@</b> can usually
be entered without pressing shift (I.E., try <b>^-</b>, <b>^6</b> and
<b>^2</b>).  Other keyboards may reassign these to other keys.  Try:
<b>^.</b>, <b>^,</b> and <b>^/</b>.  <b>^SPACE</b> can usually be used in place
of <b>^@</b>.  <b>^\</b> and <b>^]</b> are interpreted by many communication
programs, including telnet and kermit.  Usually you just hit the key twice
to get it to pass through the communication program.</p>
<p>Once you have typed <b>^K H</b>, the first help window appears at the top of
the screen.  You can continue to enter and edit text while the help window
is on.  To page through other topics, hit ^[, and ^[. (that is, ESC , and
ESC .).  Use <b>^K H</b> to dismiss the help window.</p>
<p>You can customize the keyboard layout, the help screens and a number of
behavior defaults by copying JOE's initialization file (usually
<b>/home/jhallen/etc/joe/joerc</b>) to <b>.joerc</b> in your home directory and then
by modifying it.  See the section <b>joerc</b> below.</p>
<p>To have JOE used as your default editor for e-mail and News, you need to set
the <b>EDITOR</b> and <b>VISUAL</b> environment variables in your shell
initialization file (<b>.cshrc</b> or <b>.profile</b>) to refer to JOE (joe
usually resides as <b>/home/jhallen/bin/joe</b>).</p>
<p>There are a number of other obscure invocation parameters which may have to
be set, particularly if your terminal screen is not updating as you think it
should.  See the section <b>Environment variables</b> below.</p>

<h2>Command Line Options</h2>
<p>The following global options may be specified on the command line:</p>
<ul>
<li>-asis</li>
<p>Characters with codes above 127 will be sent to the terminal as-is, instead
of as inverse of the corresponding character below 128.  If this does not
work, check your terminal server.</p>
<li>-backpath path</li>
<p>If this option is given, backup files will be stored in the specified
directory instead of in each file's original directory.</p>
<li>-baud nnn</li>
<p>Set the baud rate for the purposes of terminal screen optimization.  Joe
inserts delays for baud rates below 19200, which bypasses tty buffering so
that typeahead will interrupt the screen output.  Scrolling commands will
not be used for 38400 baud.  This is useful for X-terms and other console
ttys which really aren't going over a serial line.</p>
<li>-beep</li>
<p>Joe will beep on command errors and when the cursor goes past
extremes.</p>
<li>-columns nnn</li>
<p>Sets the number of screen columns.</p>
<li>-csmode</li>
<p>Continued search mode: a search immediately following a search will repeat
the previous search instead of prompting for new string.  This is useful for
the the ^[S and ^[R commands and for when joe is trying to be emacs.</p>
<li>-dopadding</li>
<p>Joe usually assumes that there is some kind of flow control between it and
the tty.  If there isn't, this option will make joe output extra ^@s to the
tty as specified by the termcap entry.  The extra ^@s allow the terminal to
catch up after long terminal commands.</p>
<li>-exask</li>
<p>This option makes ^KX verify the file name that it's about to write.</p>
<li>-force</li>
<p>This option makes sure that the last line of the file has a line-feed which
it's saved.</p>
<li>-help</li>
<p>The editor will start with the help screen on if this option is
given.</p>
<li>-keepup</li>
<p>Normally the column number and control-key prefix fields of the status
lines are on a one second delay to reduce CPU consumption, but with this
option they are updated after each key-stroke.</p>

<li>-lightoff</li>
<p>The block highlighting will go away after any block command if this option
is given.</p>

<li>-lines nnn</li>
<p>Sets the number of screen lines.</p>

<li>-marking</li>
<p>Text between ^KB and the cursor is highlighted (use with -lightoff and a
modified joerc file to have drop-anchor style block selection).</p>

<li>-mid</li>
<p>If this option is set and the cursor moves off the window, the window will
be scrolled so that the cursor is in the center.  This option is forced on
slow terminals which don't have scrolling commands.</p>

<li>-nobackups</li>
<p>This option prevents backup files.</p>

<li>-nonotice</li>
<p>This option prevent the copyright notice from being displayed when the
editor starts.</p>

<li>-nosta</li>
<p>This option eliminates the top-most status line.  It's nice for when you
only want to see your text on the screen or if you're using a vt52.</p>

<li>-noxon</li>
<p>Attempt to turn off ^S/^Q processing.  This is useful for when joe is trying
to be WordStar or EMACS.</p>

<li>-orphan</li>
<p>When this option is active, extra files on the command line will be placed
in orphaned buffers instead of in extra windows.  This is useful for when
joe is trying to be emacs.</p>

<li>-pg nnn</li>
<p>This specifies the number of lines to keep after PgUp/PgDn (^U/^V).  If -1
is given, half the window is kept.</p>

<li>-skiptop nnn</li>
<p>Don't use the top nnn lines of the screen.  Useful for when joe is used as a
BBS editor.</p>
</ul>
<p>Each of these options may be specified in the joerc file as well.  In
addition, the NOXON, BAUD, LINES, COLUMNS and DOPADDING options may be
specified with environment variables.</p>

<p>The JOETERM environment variable may be set to override the regular TERM
environment variable for specifying your terminal type.</p>

<p>The following options may be specified before each filename on the command
line:</p>
<ul>
<li>+nnn</li>
<p>The cursor starts on the specified line.</p>

<li>-crlf</li>
<p>Joe uses CR-LF as the end of line sequence instead of just LF.  This is for
editing MS-DOS or VMS files.</p>

<li>-wordwrap</li>
<p>Joe wraps the previous word when you type past the right margin.</p>

<li>-autoindent</li>
<p>When you hit Return on an indented line, the indentation is duplicated onto
the new line.</p>

<li>-overwrite</li>
<p>Typing overwrites existing characters instead of inserting before
them.</p>

<li>-lmargin nnn</li>
<p>Sets the left margin.</p>

<li>-rmargin nnn</li>
<p>Sets the right margin.</p>

<li>-tab nnn</li>
<p>Sets the tab width.</p>

<li>-indentc nnn</li>
<p>Sets the indentation character for ^K, and ^K. (32 for SPACE, 9 for
TAB).</p>

<li>-istep nnn</li>
<p>Sets the indentation step for ^K, and ^K..</p>

<li>-linums</li>
<p>Line numbers are displayed before each line.</p>

<li>-rdonly</li>
<p>The file is read only.</p>

<li>-keymap name</li>
<p>Use an alternate section of the joerc file for the key sequence
bindings.</p>
</ul>
<p>These options can also be specified in the joerc file.  They can be set
depending on the file-name extension.  Programs (.c, .h or .p extension)
usually have autoindent enabled.  Wordwrap is enabled on other files, but rc
files have it disabled.</p>

<h2>Editing Tasks</h2>
<h3>Basic Editing</h3>
<p>When you type characters into the editor, they are normally inserted into
the file being edited (or appended to the file if the cursor is at the end
of the file).  This is the normal operating mode of the editor.  If you want
to replace some existing text, you have to delete the old text before or
after you type in the replacement text.  The <b>Backspace</b> key can be used
for deleting text: move the cursor to right after the text you want to
delete and hit <b>Backspace</b> a number of times.</p>
<p>Hit the <b>Enter</b> or <b>Return</b> key to insert a line-break.  For 
example, if the cursor was in the middle of a line and you hit <b>Return</b>, 
the line would be split into two lines with the cursor appearing at the 
beginning of the second line.  Hit <b>Backspace</b> at the beginning of a 
line to eliminate a line-break.</p>
<p>Use the arrow keys to move around the file.  If your keyboard doesn't have
arrow keys (or if they don't work for some reason), use <b>^F</b> to move
forwards (right), <b>^B</b> to move backwards (left), <b>^P</b> to move to the
previous line (up), and <b>^N</b> to move to the next line (down).  The right
and left arrow keys simply move forwards or backwards one character at a
time through the text: if you're at the beginning of a line and
you press left-arrow, you will end up at the end of the previous line.  The
up and down arrow keys move forwards and backwards by enough characters so
that the cursor appears in the same column that it was in on the original
line.</p>
<p>If you want to indent the text you enter, you can use the <b>TAB</b> key. 
This inserts a special control character which makes the characters which
follow it begin at the next TAB STOP.  TAB STOPS normally occur every 8
columns, but this can be changed with the <b>^T D</b> command.  PASCAL and C
programmers often set TAB STOPS on every 4 columns.</p>
<p>If for some reason your terminal screen gets messed up (for example, if
you receive a mail notice from biff), you can have the editor refresh the
screen by hitting <b>^R</b>.</p>
<p>There are many other keys for deleting text and moving around the file.  For 
example, hit <b>^D</b> to delete the character the cursor is on instead of 
deleting backwards like <b>Backspace</b>.  <b>^D</b> will also delete a 
line-break if the cursor is at the end of a line.  Type <b>^Y</b> to delete 
the entire line the cursor is on or <b>^J</b> to delete just from the cursor 
to the end of the line.</p>
<p>Hit <b>^A</b> to move the cursor to the beginning of the line it's on.  Hit 
<b>^E</b> to move the cursor to the end of the line.  Hit <b>^U</b> or 
<b>^V</b> for scrolling the cursor up or down 1/2 a screen's worth.  
"Scrolling" means that the text on the screen moves, but the cursor stays at 
the same place relative to the screen.  Hit <b>^K U</b> or <b>^K V</b> to move 
the cursor to the beginning or the end of the file.  Look at the help 
screens in the editor to find even more delete and movement commands.</p>
<p>If you make a mistake, you can hit <b>^_</b> to "undo" it.  On most keyboards
you hit just <b>^-</b> to get <b>^_</b>, but on some you might have to hold
both the <b>Shift</b> and <b>Control</b> keys down at the same time to get it.
If you "undo" too much, you can "redo" the changes back into existence by
hitting <b>^^</b> (type this with just <b>^6</b> on most keyboards).</p>
<p>If you were editing in one place within the file, and you then temporarily 
had to look or edit some other place within the file, you can get back to 
the original place by hitting <b>^K -</b>.  This command actually returns you 
to the last place you made a change in the file.  You can step through a 
history of places with <b>^K -</b> and <b>^K =</b>, in the same way you can 
step through the history of changes with the "undo" and "redo" commands.</p>
<p>When you are done editing the file, hit <b>^K X</b> to exit the editor.  You
will be prompted for a file name if you hadn't already named the file you
were editing.</p>
<p>When you edit a file, you actually edit only a copy of the file.  So if you
decide that you don't want the changes you made to a file during a
particular edit session, you can hit <b>^C</b> to exit the editor without
saving them.</p>
<p>If you edit a file and save the changes, a "backup" copy of that file is 
created in the current directory, with a ~ appended to the name, which 
contains the original version of the file.</p>

<h3>Word wrap and formatting</h3>

<p>If you type past the right edge of the screen in a C language or PASCAL 
file, the screen will scroll to the right to follow the cursor.  If you type 
past the right edge of the screen in a normal file (one whose name doesn't 
end in .c, .h or .p), JOE will automatically wrap the last word onto the 
next line so that you don't have to hit <b>Return</b>.  This is called 
word-wrap mode.  Word-wrap can be turned on or off with the <b>^T W</b> 
command.  JOE's initialization file is usually set up so that this mode is 
automatically turned on for all non-program files.  See the section below on 
the <b>joerc</b> file to change this and other defaults.</p>
<p>Aside for Word-wrap mode, JOE does not automatically keep paragraphs 
formatted like some word-processors.  Instead, if you need a paragraph to be 
reformatted, hit <b>^K J</b>.  This command "fills in" the paragraph that the 
cursor is in, fitting as many words in a line as is possible.  A paragraph, 
in this case, is a block of text separated above and below by a blank line.</p>
<p>The margins which JOE uses for paragraph formatting and word-wrap can be set
with the <b>^T L</b> and <b>^T R</b> commands.  If the left margin is set to
a value other than 1, then when you start typing at the beginning of a line,
the cursor will immediately jump to the left margin.</p>
<p>If you want to center a line within the margins, use the <b>^K A</b>
command.</p>

<h3>Over-type mode</h3>
<p>Sometimes it's tiresome to have to delete old text before or after you 
insert new text.  This happens, for example, when you are changing a table 
and you want to maintain the column position of the right side of the table.  
When this occurs, you can put the editor in over-type mode with <b>^T T</b>.  
When the editor is in this mode, the characters you type in replace existing 
characters, in the way an idealized typewriter would.  Also, <b>Backspace</b> 
simply moves left instead of deleting the character to the left, when it's 
not at the end or beginning of a line.  Over-type mode is not the natural 
way of dealing with text electronically, so you should go back to 
insert-mode as soon as possible by typing <b>^T T</b> again. </p>
<p>If you need to insert while you're in over-type mode, hit <b>^@</b>.  This
inserts a single SPACE into the text.</p>

<h3>Control and Meta characters</h3>
<p>Each character is represented by a number.  For example, the number for 'A'
is 65 and the number for '1' is 49.  All of the characters which you
normally see have numbers in the range of 32 - 126 (this particular
arbitrary assignment between characters and numbers is called the ASCII
character set).  The numbers outside of this range, from 0 to 255, aren't
usually displayed, but sometimes have other special meanings.  The number
10, for example, is used for the line-breaks.  You can enter these special,
non-displayed <b>control characters</b> by first hitting <b>`</b> and then
hitting a character in the range <b>@ A B C ... X Y Z [ ^ ] \ _</b> to get
the number 0 - 31, and ? to get 127.  For example, if you hit <b>` J</b>,
you'll insert a line-break character, or if you hit <b>` I</b>, you'll insert
a TAB character (which does the same thing the TAB key does).  A useful
control character to enter is 12 (<b>` L</b>), which causes most printers to
advance to the top of the page.  You'll notice that JOE displays this
character as an underlined L.  You can enter the characters above 127, the
<b>meta characters</b>, by first hitting <b>^\</b>.  This adds 128
to the next (possibly control) character entered.  JOE displays characters
above 128 in inverse-video.  Some foreign languages, which have more letters
than English, use the meta characters for the rest of their alphabet.  You
have to put the editor in <b>ASIS</b> mode (described later) to have these
passed untranslated to the terminal.</p>
<h3>Prompts</h3>
<p>If you hit <b>TAB</b> at any file name prompt, joe will attempt to complete
the name you entered as much as possible.  If it couldn't complete the
entire name, because there are more than one possible completions, joe
beeps.  If you hit <b>TAB</b> again, joe list the completions.  You can use
the arrow keys to move around this directory menu and press RETURN or SPACE
to select an item.  If you press the first letter of one of the directory
entries, it will be selected, or if more than one entry has the same first
letter, the cursor will jump between those entries.  If you select a
subdirectory or .., the directory name is appended to the prompt and the new
directory is loaded into the menu.  You can hit Backspace to go back to the
previous directory.</p>
<p>Most prompts record a history of the responses you give them.  You can hit
up and down arrow to step through these histories.</p>
<p>Prompts are actually single line windows with no status line, so you can use
any editing command that you normally use on text within the prompts.  The
prompt history is actually just other lines of the same "prompt file".  Thus
you can can search backwards though the prompt history with the normal <b>^K
F</b> command if you want.</p>
<p>Since prompts are windows, you can also switch out of them with <b>^K P</b>
and <b>^K N</b>.</p>

<h3>Where am I?</h3>

<p>Hit <b>^K SPACE</b> to have JOE report the line number, column number, and
byte number on the last line of the screen.  The number associated with the
character the cursor is on (its ASCII code) is also shown.  You can have the
line number and/or column number always displayed on the status line by
setting placing the appropriate escape sequences in the status line setup
strings.  Edit the joerc file for details.</p>

<h3>File operations</h3>

<p>You can hit <b>^K D</b> to save the current file (possibly under a different
name from what the file was called originally).  After the file is saved,
you can hit <b>^K E</b> to edit a different file.</p>
<p>If you want to save only a selected section of the file, see the section on
<b>Blocks</b> below.</b>
<p>If you want to include another file in the file you're editing, use <b>^K
R</b> to insert it.</p>

<h3>Temporarily suspending the editor</h3>

<p>If you need to temporarily stop the editor and go back to the shell, hit 
<b>^K Z</b>.  You might want to do this to stop whatever you're editing and 
answer an e-mail message or read this man page, for example.  You have to 
type <b>fg</b> or <b>exit</b> (you'll be told which when you hit <b>^K Z</b>) 
to return to the editor.  </p>

<h3>Searching for text</h3>

<p>Hit <b>^K F</b> to have the editor search forwards or backwards for a text 
fragment (<b>string</b>) for you.  You will be prompted for the text to 
search for.  After you hit <b>Return</b>, you are prompted to enter options.  
You can just hit <b>Return</b> again to have the editor immediately search 
forwards for the text, or you can enter one or more of these options:</p>
<ul>
<li><b>b</b></li>
<p>Search backwards instead of forwards.</p>

<li><b>i</b></li>
<p>Treat uppercase and lower case letters as the same when searching.  Normally
uppercase and lowercase letters are considered to be different.</p>

<li><b>nnn</b></li>
<p>(where <b>nnn</b> is a number) If you enter a number, JOE searches for the 
Nth occurrence of the text.  This is useful for going to specific places in 
files structured in some regular manner.</p>

<li><b>r</b></li>
<p>Replace text.  If you enter the <b>r</b> option, then you will be further
prompted for replacement text.  Each time the editor finds the search text,
you will be prompted as to whether you want to replace the found search text
with the replacement text.  You hit: <b>y</b> to replace the text and then
find the next occurrence, <b>n</b> to not replace this text, but to then find
the next occurrence, <b>r</b> to replace all of the remaining occurrences of
the search text in the remainder of the file without asking for confirmation
(subject to the <b>nnn</b> option above), or <b>^C</b> to stop searching and
replacing.</p>
</ul>

<p>You can hit <b>^L</b> to repeat the previous search.</p>

<h3>Regular Expressions</h3>

<p>A number of special character sequences may be entered as search
text:</p>
<ul>
<li><b>\*</b></li>
<p>This finds zero or more characters.  For example, if you give <b>A\*B</b> as
the search text, JOE will try to find an A followed by any number of characters
and then a B.</p>

<li><b>\?</b></li>
<p>This finds exactly one character.  For example, if you give <b>A\?B</b> as
the search text, JOE will find AXB, but not AB or AXXB.</p>

<li><b>\^ \$</b></li>
<p>These match the beginning and end of a line.  For example, if you give
<b>\^test\$</b>, then JOE with find <b>test</b> on a line by itself.</p>

<li><b>\&lt; \&gt;</b></li>
<p>These match the beginning and end of a word.  For example, if you give
<b>\&lt;\*is\*\&gt;</b>, then joe will find whole words which have the
sub-string <b>is</b> within them.</p>

<li><b>\[...]</b></li>
<p>This matches any single character which appears within the brackets.  For
example, if <b>\[Tt]his</b> is entered as the search string, then JOE finds
both <b>This</b> and <b>this</b>.  Ranges of characters can be entered within
the brackets.  For example, <b>\[A-Z]</b> finds any uppercase letter.  If
the first character given in the brackets is <b>^</b>, then JOE tries to find
any character not given in the the brackets.</p>

<li><b>\c</b></li>
<p>This works like <b>\*</b>, but matches a balanced C-language expression. 
For example, if you search for <b>malloc(\c)</b>, then JOE will find all
function calls to <b>malloc</b>, even if there was a <b>)</b> within the
parenthesis.</p>

<li><b>\+</b></li>
<p>This finds zero or more of the character which immediately follows the
<b>\+</b>.  For example, if you give <b>\[ ]\+\[ ]</b>, where the
characters within the brackets are both SPACE and TAB, then JOE will find
whitespace.</p>

<li><b>\\</b></li>
<p>Matches a single \.</p>

<li><b>\n</b></li>
<p>This finds the special end-of-line or line-break character.</p>
</ul>
<p>A number of special character sequences may also be given in the replacement
string:</p>
<ul>
<li><b>\&amp;</b></li>
<p>This gets replaced by the text which matched the search string.  For
example, if the search string was <b>\&lt;\*\&gt;</b>, which matches words, and
you give <b>"\&amp;"</b>, then joe will put quote marks around words.</p>

<li><b>\0 - \9</b></li>
<p>These get replaced with the text which matched the Nth <b>\*</b>, <b>\?</b>,
<b>\+</b>, <b>\c</b>, <b>\+</b>, or <b>\[...]</b> in the search string.</p>

<li><b>\\</b></li>
<p>Use this if you need to put a <b>\</b> in the replacement string.</p>

<li><b>\n</b></li>
<p>Use this if you need to put a line-break in the replacement string.</p>
</ul>
<p>Some examples:</p>
<p>Suppose you have a list of addresses, each on a separate line, which starts 
with "Address:" and has each element separated by commas.  Like so:</p>
<p>Address: S. Holmes, 221b Baker St., London, England</p>
<p>If you wanted to rearrange the list, to get the country first, then the 
city, then the person's name, and then the address, you could do this:</p>
<p>Type <b>^K F</b> to start the search, and type:</p>
<p><b>Address:\*,\*,\*,\*\$</b></p>
<p>to match "Address:", the four comma-separated elements, and then the end of 
the line.  When asked for options, you would type <b>r</b> to replace the 
string, and then type:</p>
<p><b>Address:\3,\2,\0,\1</b></p>
<p>To shuffle the information the way you want it. After hitting return, the 
search would begin, and the sample line would be changed to:</p>
<p>Address: England, London, S. Holmes, 221b Baker St.</p>


<h3>Blocks</h3>

<p>If you want to move, copy, save or delete a specific section of text, you 
can do it with highlighted blocks.  First, move the cursor to the start of 
the section of text you want to work on, and press <b>^K B</b>.  Then move 
the cursor to the character just after the end of the text you want to 
affect and press <b>^K K</b>.  The text between the <b>^K B</b> and <b>^K K</b> 
should become highlighted.  Now you can move your cursor to someplace else 
in your document and press <b>^K M</b> to move the highlighted text there.  
You can press <b>^K C</b> to make a copy of the highlighted text and insert 
it to where the cursor is positioned.  <b>^K Y</b> to deletes the highlighted 
text.  <b>^K W</b>, writes the highlighted text to a file.  </p>
<p>A very useful command is <b>^K /</b>, which filters a block of text through a
unix command.  For example, if you select a list of words with <b>^K B</b>
and <b>^K K</b>, and then type <b>^K / sort</b>, the list of words will be
sorted.  Another useful unix command for <b>^K /</b>, is <b>tr</b>.  If you
type <b>^K / tr a-z A-Z</b>, then all of the letters in the highlighted block
will be converted to uppercase.</p>
<p>After you are finished with some block operations, you can just leave the 
highlighting on if you don't mind it (of course, if you accidently hit <b>^K 
Y</b> without noticing...).  If it really bothers you, however, just hit 
<b>^K B ^K K</b>, to turn the highlighting off.  </p>

<h3>Indenting program blocks</h3>
<p>Auto-indent mode toggled with the <b>^T I</b> command.  The
<b>joerc</b> is normally set up so that files with names ending with .p, .c
or .h have auto-indent mode enabled.  When auto-indent mode is enabled and
you hit <b>Return</b>, the cursor will be placed in the same column that the
first non-SPACE/TAB character was in on the original line.</p>
<p>You can use the <b>^K ,</b> and <b>^K .</b> commands to shift a block of text 
to the left or right.  If no highlighting is set when you give these 
commands, the program block the cursor is located in will be selected, and 
will be moved by subsequent <b>^K ,</b> and <b>^K .</b> commands.  The number 
of columns these commands shift by can be set through a <b>^T</b>
option.</p>
<h3>Windows</h3>
<p>You can edit more than one file at the same time or edit two or more
different places of the same file.  To do this, hit <b>^K O</b>, to split the
screen into two windows.  Use <b>^K P</b> or <b>^K N</b> to move the cursor
into the top window or the lower window.  Use <b>^K E</b> to edit a new
file in one of the windows.  A window will go away when you save the file
with <b>^K X</b> or abort the file with <b>^C</b>.  If you abort a file which
exists in two windows, one of the window goes away, not the file.</p>
<p>You can hit <b>^K O</b> within a window to create even more windows.  If you 
have too many windows on the screen, but you don't want to eliminate them, 
you can hit <b>^K I</b>.  This will show only the window the cursor is in, or 
if there was only one window on the screen to begin with, try to fit all 
hidden windows on the screen.  If there are more windows than can fit on 
the screen, you can hit <b>^K N</b> on the bottom-most window or <b>^K P</b> 
on the top-most window to get to them.  </p>
<p>If you gave more than one file name to JOE on the command line, each file 
will be placed in a different window.  </p>
<p>You can change the height of the windows with the <b>^K G</b> and <b>^K T</b>
commands.</p>

<h3>Keyboard macros </h3>

<p>Macros allow you to record a series of keystrokes and replay them with the 
press of two keys.  This is useful to automate repetitive tasks.  To start a 
macro recording, hit <b>^K [</b> followed by a number from 0 to 9.  The 
status line will display (Macro n recording...).  Now, type in the series of 
keystrokes that you want to be able to repeat.  The commands you type will 
have their usual effect. Hit <b>^K ]</b> to stop recording the macro.  Hit 
<b>^K</b> followed by the number you recorded the macro in to execute one 
iteration of the key-strokes.   </p>
<p>For example, if you want to put "**" in front of a number of lines, you can 
type:</p>

<b>^K [ ^A ** </b><down arrow> <b>^K ]</b>

<p>Which starts the macro recording, moves the cursor to the beginning of the 
line, inserts "**", moves the cursor down one line, and then ends the 
recording. Since we included the key-strokes needed to position the cursor 
on the next line, we can repeatedly use this macro without having to move 
the cursor ourselves, something you should always keep in mind when 
recording a macro.</p>
<p>If you find that the macro you are recording itself has a repeated set of
key-strokes in it, you can record a macro within the macro, as long as you
use a different macro number.  Also you can execute previously recorded
macros from within new macros. </p>


<h3>Repeat</h3>
<p>You can use the repeat command, <b>^K \</b>, to repeat a macro, or any other
edit command or even a normal character, a specified number of times.  Hit
<b>^K \</b>, type in the number of times you want the command repeated and
press <b>Return</b>.  The next edit command you now give will be repeated
that many times.</p>
</p>For example, to delete the next 20 lines of text, type:</p>
<p><b>^K \ 20</b><return><b>^Y</b></p>

<h3>Rectangle mode</h3>
<p>Type <b>^T X</b> to have <b>^K B</b> and <b>^K K</b> select rectangular blocks
instead of stream-of-text blocks.  This mode is useful for moving, copying,
deleting or saving columns of text.  You can also filter columns of text
with the <b>^K /</b> command- if you want to sort a column, for example.  The
insert file command, <b>^K R</b> is also effected.</p>
<p>When rectangle mode is selected, over-type mode is also useful
(<b>^T T</b>).  When over-type mode is selected, rectangles will replace
existing text instead of getting inserted before it.  Also the delete block
command (<b>^K Y</b>) will clear the selected rectangle with SPACEs and TABs
instead of deleting it.  Over-type mode is especially useful for the filter
block command (<b>^K /</b>), since it will maintain the original width of the
selected column.</p>

<h3>Tag search</h3>
<p>If you are editing a large C program with many source files, you can use the 
<b>ctags</b> program to generate a <b>tags</b> file.  This file contains a 
list of program symbols and the files and positions where the symbols are 
defined.  The <b>^K ;</b> command can be used to lookup a symbol (functions, 
defined constants, etc.), load the file where the symbol is defined into the 
current window and position the cursor to where the symbol is defined.  
<b>^K ;</b> prompts you for the symbol you want, but uses the symbol the 
cursor was on as a default.  Since <b>^K ;</b> loads 
the definition file into the current window, you probably want to split the 
window first with <b>^K O</b>, to have both the original file and the 
definition file loaded.  </p>

<h3>Shell windows</h3>
<p>Hit <b>^K '</b> to run a command shell in one of JOE's windows.  When the
cursor is at the end of a shell window (use <b>^K V</b> if it's not),
whatever you type is passed to the shell instead of the window.  Any output
from the shell or from commands executed in the shell is appended to the
shell window (the cursor will follow this output if it's at the end of the
shell window).  This command is useful for recording the results of shell
commands- for example the output of <b>make</b>, the result of <b>grep</b>ping
a set of files for a string, or directory listings from <b>FTP</b> sessions. 
Besides typeable characters, the keys ^C, Backspace, DEL, Return and ^D are
passed to the shell.  Type the shell <b>exit</b> command to stop recording
shell output.  If you press <b>^C</b> in a shell window, when the cursor is
not at the end of the window, the shell is <b>kill</b>ed.</p>

<h2>Environment variables </h2>
<p>For JOE to operate correctly, a number of other environment settings must be 
correct.  The throughput (baud rate) of the connection between the computer 
and your terminal must be set correctly for JOE to update the screen 
smoothly and allow typeahead to defer the screen update.  Use the <b>stty 
nnn</b> command to set this.  You want to set it as close as possible to 
actual throughput of the connection.  For example, if you are connected via 
a 1200 baud modem, you want to use this value for <b>stty</b>.  If you are 
connected via 14.4k modem, but the terminal server you are connected to 
connects to the computer a 9600 baud, you want to set your speed as 9600 
baud.  The special baud rate of 38400 or <b>extb</b> is used to indicate that 
you have a very-high speed connection, such as a memory mapped console or an 
X-window terminal emulator.  If you can't use <b>stty</b> to set the actual 
throughput (perhaps because of a modem communicating with the computer at a 
different rate than it's communicating over the phone line), you can put a 
numeric value in the <b>BAUD</b> environment variable instead (use <b>setenv 
BAUD 9600</b> for csh or <b>BAUD=9600; export BAUD</b> for sh).  </p>
<p>The <b>TERM</b> environment variable must be set to the type of terminal
you're using.  If the size (number of lines/columns) of your terminal is
different from what is reported in the TERMCAP or TERMINFO entry, you can
set this with the <b>stty rows nn cols nn</b> command, or by setting the
<b>LINES</b> and <b>COLUMNS</b> environment variables.</p>
<p>JOE normally expects that flow control between the computer and your
terminal to use ^S/^Q handshaking (I.E., if the computer is sending
characters too fast for your terminal, your terminal sends ^S to stop the
output and ^Q to restart it).  If the flow control uses out-of-band or
hardware handshaking or if your terminal is fast enough to always keep up
with the computer output and you wish to map ^S/^Q to edit commands, you can
set the environment variable <b>NOXON</b> to have JOE attempt to turn off
^S/^Q handshaking.  If the connection between the computer and your terminal
uses no handshaking and your terminal is not fast enough to keep up with the
output of the computer, you can set the environment variable <b>DOPADDING</b>
to have <b>JOE</b> slow down the output by interspersing PAD characters
between the terminal screen update sequences.</p>

<h2>Filenames</h2>
<p>Wherever JOE expects you to enter a file name, whether on the command line
or in prompts within the editor, you may also type:</p>

<ul>

<li>!command</li>
<p>Read or write data to or from a shell command.  For example,
use <b>joe '!ls'</b> to get a copy of your directory listing to edit or from
within the editor use <b>^K D !mail jhallen@world.std.com</b> to send the
file being edited to me.</p>

<li>&gt;&gt;filename</li>
<p>Use this to have JOE append the edited text to the end of the file 
"filename."</p>

<li>filename,START,SIZE</li>
<p>Use this to access a fixed section of a file or device.  <b>START</b> and
<b>SIZE</b> may be entered in decimal (ex.: 123) octal (ex.: 0777) or
hexadecimal (ex.: 0xFF).  For example, use <b>joe /dev/fd0,508,2</b> to edit
bytes 508 and 509 of the first floppy drive in Linux.</p>

<li>-</li>
<p>Use this to get input from the standard input or to write output to the
standard output.  For example, you can put joe in a pipe of commands:
<b>quota -v | joe - | mail root</b>, if you want to complain about your low
quota.</p>
</ul>

<br><br>
<h2>The joerc file</h2>
<p>^T options, the help screens and the key-sequence to editor command
bindings are all defined in JOE's initialization file.  If you make a copy
of this file (which normally resides in <b>/home/jhallen/etc/joe/joerc</b>) to
<b>$HOME/.joerc</b>, you can customize these setting to your liking.  The
syntax of the initialization file should be fairly obvious and there are
further instruction in it.</p>

<h2>Acknowledgments</h2>
<p>JOE was written by Joseph H. Allen.  If you have bug reports or questions,
e-mail them to jhallen@world.std.com.  Larry Foard (entropy@world.std.com)
and Gary Gray (ggray@world.std.com) also helped with the creation of
JOE.</p>
