<h3>JOE options (environment variables at the bottom):</h3>


<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
</colgroup>
<tbody>

<tr valign="top"><td>asis</td><td>Pass characters above 127 directly to
terminal instead of showing them as inverse of corresponding character below
128.</td></tr>

<tr valign="top"><td>assume_256color</td><td>Assume ANSI-like terminal emulator supports 256 colors even if termcap entry says it doesn't.</td></tr>

<tr valign="top"><td>assume_color</td><td>Assume ANSI-like terminal emulator supports color even if termcap entry says it doesn't.</td></tr>

<tr valign="top"><td>autoindent</td><td>Enable auto-indent mode.</td></tr>

<tr valign="top"><td>autoswap</td><td>Automatically swap ^KB with ^KK if necessary to
mark a legal block during block copy/move commands.</td></tr>

<tr valign="top"><td>backpath</td><td>Sets bath to a directory where all backup files are
to be stored.  If this is unset (the default) backup files are stored in the
directory containing the file.</td></tr>

<tr valign="top"><td>baud</td><td>Tell JOE the baud rate of the terminal (overrides value reported by stty).</td></tr>

<tr valign="top"><td>beep</td><td>Enable beeps when edit commands return errors.</td></tr>

<tr valign="top"><td>break_links</td><td>When enabled, JOE first deletes the file before
writing it in order to break hard-links and symbolic-links.</td></tr>

<tr valign="top"><td>break_hardlinks</td><td>When enabled, and the file is
not a symbolic links, JOE first deletes the file before
writing it in order to break hard-links.</td></tr>

<tr valign="top"><td>c_comment</td><td>Enable ^G skipping of C-style comments /* ... */</td></tr>

<tr valign="top"><td>columns</td><td>Set number of columns in terminal emulator (in case
termcap entry is wrong).  This is only useful on old system which don't have
the "get window size" ioctl.</td></tr>

<tr valign="top"><td>cpara</td><td>Sets list of characters which can indent paragraphs.</td></tr>

<tr valign="top"><td>cpp_comment</td><td>Enable ^G skipping of C++-style comments // ...</td></tr>

<tr valign="top"><td>crlf</td><td>Set MS-DOS line endings.</td></tr>

<tr valign="top"><td>csmode</td><td>Enable continued search mode: Successive
^KFs repeat the current search instead of prompting for a new one.</td></tr>

<tr valign="top"><td>dopadding</td><td>Enable JOE to send padding NULs to the terminal (for very old terminals).</td></tr>

<tr valign="top"><td>encoding</td><td>Set file encoding (like utf-8 or 8859-1).</td></tr>

<tr valign="top"><td>exask</td><td>When set, ^KX prompts for a new name before saving the file.</td></tr>

<tr valign="top"><td>floatmouse</td><td>When set, mouse clicks can position the cursor
beyond the ends of lines.</td></tr>

<tr valign="top"><td>force</td><td>When set, a final newline is appended to the file if
there isn't one when the file is saved.</td></tr>

<tr valign="top"><td>french</td><td>When set, only one space is inserted after periods in
paragraph reformats instead of two.</td></tr>

<tr valign="top"><td>guess_crlf</td><td>When set, JOE tries to guess the file format
MS-DOS or UNIX.</td></tr>

<tr valign="top"><td>guess_indent</td><td>When set, JOE tries to guess the indentation
character and indentation step based on how a file is already indented.</td></tr>

<tr valign="top"><td>guess_non_utf8</td><td>When set, enable guessing of non-UTF-8 files
in UTF-8 locales.</td></tr>

<tr valign="top"><td>guess_utf8</td><td>When set, enable guessing of UTF-8 files in
non-UTF-8 locales.</td></tr>

<tr valign="top"><td>help</td><td>When set, start off with the on-line help enabled.</td></tr>

<tr valign="top"><td>help_is_utf8</td><td>When set, the help text in the joerc file is
assumed to be UTF-8.</td></tr>

<tr valign="top"><td>hex</td><td>Enable hex-dump mode.</td></tr>

<tr valign="top"><td>highlight</td><td>Enable syntax highlighting.</td></tr>

<tr valign="top"><td>icase</td><td>Search is case insensitive by default when set.</td></tr>

<tr valign="top"><td>indentc</td><td>Sets the indentation character for shift left and
shift right commands.</td></tr>

<tr valign="top"><td>indentfirst</td><td>When set, the smart home key jumps to the
indentation point first, otherwise it jumps to column 1 first.</td></tr>

<tr valign="top"><td>istep</td><td>Sets indentation step.</td></tr>

<tr valign="top"><td>joe_state</td><td>Enable reading and writing of ~/.joe_state file</td></tr>

<tr valign="top"><td>joexterm</td><td>Set this if xterm was configured with --paste64
option for better mouse support.</td></tr>

<tr valign="top"><td>keepup</td><td>The column number on the status line is updated constantly when
this is set, otherwise it is updated only once a second.</td></tr>

<tr valign="top"><td>language</td><td>Sets language for aspell.</td></tr>

<tr valign="top"><td>lightoff</td><td>Automatically turn off ^KB ^KK highlighting after a
block operation.</td></tr>

<tr valign="top"><td>lines</td><td>Set number of lines in terminal emulator (in case
termcap entry is wrong).  This is only useful on old system which don't have
the "get window size" ioctl.</td></tr>

<tr valign="top"><td>linums</td><td>Enable line number display.</td></tr>

<tr valign="top"><td>lmargin</td><td>Set left margin.</td></tr>

<tr valign="top"><td>marking</td><td>Enable marking mode: highlights between ^KB and
cursor.</td></tr>

<tr valign="top"><td>menu_above</td><td>Put menus above prompt instead of below them.</td></tr>

<tr valign="top"><td>menu_explorer</td><td>Stay in menu when a directory is selected
(otherwise the directory is added to the path and the cursor jumps back to
the prompt).</td></tr>

<tr valign="top"><td>menu_jump</td><td>Jump into the file selection menu when tab tab is
hit.</td></tr>

<tr valign="top"><td>mid</td><td>Recenter window around cursor when you scroll (like
EMACS).</td></tr>

<tr valign="top"><td>mouse</td><td>Enable xterm mouse support.</td></tr>

<tr valign="top"><td>nobackups</td><td>Disable backup files.</td></tr>

<tr valign="top"><td>nocurdir</td><td>Disable current-directory prefix in prompts.</td></tr>

<tr valign="top"><td>nolocks</td><td>Disable EMACS compatible file locks.</td></tr>

<tr valign="top"><td>nomodcheck</td><td>Disable periodic file modification check.</td></tr>

<tr valign="top"><td>nonotice</td><td>Disable JOE copyright notice.</td></tr>

<tr valign="top"><td>nosta</td><td>Disable top most status line (so that when you edit a
file, only your text is on the screen).</td></tr>

<tr valign="top"><td>notite</td><td>Disable ti and te termcap sequences which are usually
set up to save and restore the terminal screen contents when JOE starts and
exits.</td></tr>

<tr valign="top"><td>noxon</td><td>Disable ^S and ^Q flow control, possibly
allowing ^S and ^Q to be used as editor keys.</td></tr>

<tr valign="top"><td>orphan</td><td>Orphan extra files given on the command line instead
of creating windows for them (the files are loaded, but you need to use
switch-buffer commands to access them).</td></tr>

<tr valign="top"><td>overwrite</td><td>Enable overtype mode.</td></tr>

<tr valign="top"><td>pg</td><td>Set number of lines to keep during Page Up and Page Down
(use -1 for 1/2 window size).</td></tr>

<tr valign="top"><td>picture</td><td>Enable "picture" mode- allows cursor to go past ends
of lines.</td></tr>

<tr valign="top"><td>pound_comment</td><td>^G ignores # ... comments.</td></tr>

<tr valign="top"><td>purify</td><td>Clean up indentation when enabled (during shift-left
and shift-right commands).</td></tr>

<tr valign="top"><td>rdonly</td><td>Set read-only mode.</td></tr>

<tr valign="top"><td>restore</td><td>Set to have cursor position restore to last position
of previouly edited files.</td></tr>

<tr valign="top"><td>rmargin</td><td>Set right margin.</td></tr>

<tr valign="top"><td>rtbutton</td><td>Swap left and right mouse buttons.</td></tr>

<tr valign="top"><td>search_prompting</td><td>Show previous search string in search command (like in PICO).</td></tr>

<tr valign="top"><td>semi_comment</td><td>^G ignores ; ... comments.</td></tr>

<tr valign="top"><td>single_quoted</td><td>^G ignores '...'</td></tr>

<tr valign="top"><td>skiptop</td><td>When set to N, the first N lines of the terminal
screen are not used by JOE and are instead left with their original
contents.  This is useful for programs which call JOE to leave a message for
the user.</td></tr>

<tr valign="top"><td>smartbacks</td><td>Enable smart backspace (backspace unindents).</td></tr>

<tr valign="top"><td>smarthome</td><td>Enable smart home (home key jumps to indentation
point on first or second press).</td></tr>

<tr valign="top"><td>spaces</td><td>Insert spaces when tab key is hit.</td></tr>

<tr valign="top"><td>square</td><td>Enable rectangular block mode.</td></tr>

<tr valign="top"><td>syntax</td><td>Set syntax for syntax highlighting.</td></tr>

<tr valign="top"><td>tab</td><td>Set tab stop width.</td></tr>

<tr valign="top"><td>text_delimiters</td><td>Give list of word delimiters which ^G will
step through.</td></tr>

<tr valign="top"><td>transpose</td><td>Transpose rows with columns in all menus.</td></tr>

<tr valign="top"><td>undo_keep</td><td>Sets number of UNDO records to keep (0 means
infinite).</td></tr>

<tr valign="top"><td>usetabs</td><td>Set to allow rectangular block operations to use
tabs.</td></tr>

<tr valign="top"><td>vhdl_comment</td><td>^G ignores -- ... comments</td></tr>

<tr valign="top"><td>wordwrap</td><td>Enable wordwrap mode.</td></tr>

<tr valign="top"><td>wrap</td><td>Enable search to wrap to beginning of file.</td></tr>

</tbody>
</table>

<h3>Environment variables:</h3>
<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
</colgroup>
<tbody>

<tr valign="top"><td>BAUD</td><td>Tell JOE the baud rate of the terminal (overrides value reported by stty).</td></tr>

<tr valign="top"><td>COLUMNS</td><td>Set number of columns in terminal emulator (in case
termcap entry is wrong).  This is only useful on old system which don't have
the "get window size" ioctl.</td></tr>

<tr valign="top"><td>DOPADDING</td><td>Enable JOE to send padding NULs to the terminal
when set (for very old terminals).</td></tr>

<tr valign="top"><td>HOME</td><td>Used to get path to home directory for ~
expansion and also to find ~/.joerc file ~/.joe directory.</td></tr>

<tr valign="top"><td>HOSTNAME</td><td>Used to get hostname to put in EMACS
compatible locks.</td></tr>

<tr valign="top"><td>JOETERM</td><td>Gives terminal type: JOE will use this instead of TERM if it's set.</td></tr>

<tr valign="top"><td>LANG</td><td>Sets locale (like en_US.utf-8).  JOE uses
the first of these which is set: LC_ALL, LC_CTYPE, LANG.</td></tr>

<tr valign="top"><td>LC_ALL</td><td>Sets locale (like en_US.utf-8).  JOE
uses the first of these which is set: LC_ALL, LC_CTYPE, LANG.</td></tr>

<tr valign="top"><td>LC_CTYPE</td><td>Sets locale (like en_US.utf-8).  JOE
uses the first of these which is set: LC_ALL, LC_CTYPE, LANG.</td></tr>

<tr valign="top"><td>LINES</td><td>Set number of lines in terminal emulator (in case
termcap entry is wrong).  This is only useful on old system which don't have
the "get window size" ioctl.</td></tr>

<tr valign="top"><td>NOXON</td><td>Disable ^S and ^Q flow control, possibly
allowing ^S and ^Q to be used as editor keys.</td></tr>

<tr valign="top"><td>SHELL</td><td>Path to shell (like /bin/sh).  This is
used in several places: If you are on a system with no job control, this
shell is invoked when you hit ^KZ.  Also this is shell which is run in shell
windows.  If SHELL is not set (Cygwin) or if it's set to /bin/sh, JOE
invokes the first of these which exists: /bin/bash, /usr/bin/bash, /bin/sh.</td></tr>

<tr valign="top"><td>SIMPLE_BACKUP_SUFFIX</td><td>If this is set, it is
appended to the file name instead of ~ to create the backup file name.</td></tr>

<tr valign="top"><td>TAGS</td><td>If set to a path to a file, JOE tries to
use this as the "tags" file if there is no "tags" file in the current
directory.</td></tr>

<tr valign="top"><td>TEMP</td><td>If set, gives path to directory to open
swapfile instead of /tmp</td></tr>

<tr valign="top"><td>TERMCAP</td><td>Used by JOE's built-in termcap file
parser (not used for terminfo).  A termcap entry can be placed directly in
this variable (which will be used if it matches TERM), or if it begins with
/, it gives a list of paths to termcap files to search.</td></tr>

<tr valign="top"><td>TERMPATH</td><td>Gives list of paths to termcap files
to search when TERMCAP has a termcap entry (otherwise it's ignored).</td></tr>

<tr valign="top"><td></td><td>The default list of paths to termcap files
(when TERMCAP and TERMPATH do not have it) is: "~/.termcap /etc/joe/termcap
/etc/termcap"</td></tr>

<tr valign="top"><td>TERM</td><td>Gives terminal type, like "vt100" or "xterm".</td></tr>

<tr valign="top"><td>USER</td><td>Used to get user name for EMACS compatible
file locks.</td></tr>

</tbody>
</table>
