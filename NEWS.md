## Release Notes

[Back to README file](http://sourceforge.net/p/joe-editor/mercurial/ci/default/tree/README.md)

### JOE 4.0

* JOE now has pop-up shell windows with full terminal emulation and shell commands
  that can control the editor.  Hit F1 - F4 to bring up a shell window.
  See [Pop-up shell feature](http://sourceforge.net/p/joe-editor/mercurial/ci/default/tree/docs/man.md#popup) for a full description.

* The status command (^K SPACE) can now be customized using the same syntax
  as the status bar.  Look for smsg and zmsg in joerc to see how to do this.

* parserr (the error parser) will parse only the highlighted block if it's set.  Before it always parsed the entire buffer.

* Now there is a per-buffer concept of current directory.  This was added to
  make the pop-up shell windows work better, but it's useful in general.

* At file prompts you can begin a new anchored path without having to delete
  the old one.  It means that ~jhallen/foo//etc/passwd is translated to /etc/passwd.
  Prompt windows are now highighted to indicate which parts of the path are
  being dropped.  There is a syntax file for this: filename.jsf

* The error parser now ignores ANSI sequences (some versions of grep
  color their results, now JOE can still parse it).

* Temporary messages are now dismissed by keyboard input only.  Before, they
  could also be dismissed by shell input.

* Tags search now supports multiple matches.  ^K ; can be configured to
  either provide a menu of the matches or to cycle through them.

* Tags search will now match on the member name part of member functions
  ('fred' will match 'myclass::fred').

* Tags search will prepend the path to the tags file file name in the tags
  file.  This is important when JOE finds the tags file via the TAGS
  environment variable.

* Remove ` as quote character from incremental search.

* Clean up documentation, convert much of it to Markdown.

### JOE 3.8

- Search JOE image for :include files referenced by the joerc file.
  Include ftyperc file in the JOE image.

- Change default indent from 2 to 4.  Add quick menu to change to common
  indent values: ^T = (1, 2, 4, or 8).  Switch to + and - for definitively
  setting or clearing options so that 0 and 1 can be use for quick select.

- Added option to suppress DEADJOE file

- Jump to matching delimiter (Ctrl-G) has been improved.  It can now use the
  syntax files to parse the document in order to identify strings and
  comments which should be skipped during the matching delimiter search.
  (patch by Charles Tabony).

- When 'notite' mode is enabled, JOE now emits linefeeds to preserve the
  screen contents in the terminal emulator's scrollback buffer.  This can be
  suppressed with a new flag: nolinefeeds.

- JOE now starts up quiet (prints no extra messages when starting). 
  Messages are collected in a startup log (view with ESC x showlog).

- There is a new flag 'noexmsg' which, when set, makes JOE quiet when it shuts
  down (suppresses "File not changed so no update needed" message).

- Use 80th column if terminal has xn capability (patch by pts and Egmont
  Koblinger).

- Support italic text (on some terminal emulators) with "\l" (patch by
  Egmont Koblinger)

- Support bracketed paste (patch by Egmont Koblinger)

- Fix line number in syntax highlighter error output 

- Prevent infinite loops caused by buggy syntax definitions.

- New and improved syntax definitions for:
    * Ant: contributed by Christian Nicolai
    * Batch files: contributed by John Jordan
    * C#: contributed by John Jordan
    * Debian apt sources.list: contributed by Christian Nicolai
    * Elixir: contributed by Andrew Lisin
    * Erlang: contributed by Christian Nicolai, Jonas Rosling, Andrew Lisin
    * git-commit messages: contributed by Christian Nicolai
    * Go: contributed by Matthias S. Benkmann
    * HAML: contributed by Christian Nicolai
    * INI: contributed by Christian Nicolai
    * iptables: contributed by Christian Nicolai
    * Javascript: contributed by Rebecca Turner, Christian Nicolai
    * json: contributed by Rebecca Turner
    * Markdown: contributed by Christian Nicolai, Jonas Rosling
    * Powershell: contributed by Oskar Liljeblad
    * Prolog: contributed by Christian Nicolai
    * Puppet: contributed by Christian Nicolai, Eric Eisenhart
    * Sieve: contributed by Christian Nicolai
    * YAML: contributed by Christian Nicolai
    
    (from github.com/cmur2/joe-syntax)

- Syntax definition fixes for: C, Python, Java, Lua, sh, Ruby, PHP, TeX,
  CSS, and XML

- Save/restore utf-8 and crlf modes when changing in/out of hex edit for
  better display

- Fix autocomplete for paths containing spaces

- Accept mouse events beyond column 208 (patch by Egmont Kobligner)

- Adjust guess_indent behavior based on user feedback

- Fix infinite loop in search and replace

- Add a new command 'timer' which executes a macro every n seconds.  I use
  this for periodically injecting a command into a shell window for
  overnight testing of some device.

- Convert double to long long (if we have it) when printing hexadecimal.

- Fix bug where undo was acting strangly in shell windows.

- Fix crash when hitting -----------.. wordwrap bug.

- Check for math functions

- Use *joerc if *fancyjoerc not there.

- fix segfault from -orphan

- fix window size detection bug: can't take out types.h
  from tty.c

- update status line immediately on resize.

- va_copy fix.

- don't smartbackspace when smartbacks is off.

### JOE 3.7

- backspace/DEL means 'n' in replace prompt for better emacs
  compatibility

- Menus are now made up of macros instead of options.

	New commands:

		* menu	Prompt for a menu to display with tab completion.

		* mode	Prompt for an option to change with tab completion.

	Menus are defined in joerc with ':defmenu name' followed
	by a set of menu entries.

	Menu entries are the pair: 'macro string'.  String is a
	format string displayed in the menu.  Macro is executed
	when then menu entry is selected.

	Use this to add your own macros to ^T.

- ^T is now a user definable menu system

- Treat \ as a quote character for file I/O.  Now you can edit
  files like !test with \\!test

- Print NULs in default search string.  Handle many \s properly.

- Allow backslashes in file names

- Fix %A to print unicode

- Charles Tabony's (vectorshifts's) highlighter stack patch

- ! is relace all in replace prompt

- Turn off UTF-8 when we enter hex mode

- Call ttsig on vfile I/O errors.

- Abort cleanly when malloc returns NULL

- Add reload command to reload file from disk

- Modify configure scrips to use docdir for extra documents and
  datadir/joe for syntax and i18n files.

- Don't use bold yellow, it's bad for white screens

- Fix TeX highlighter: don't highlight "

- Make mail.jsf more forgiving for those of us who still use old UNIX mail

- Fix file rename bugs

- Improve ubop: can reformat a block of paragraphs again.  Reformat
  of adjacent indented paragraphs working again.

- Improve XML highlighter: allow \r in whitespace

### JOE 3.6

- Preserve setuid bit

- Fix bug where backup file did not get modtime of original

- New diff highlighter

- Fix paragraph format when overtype is on

- Fix non-french spacing

- Fix bug with joe +2 on single line files

- Add syntax file for .jsf files

- Add ASCII table to joerc help

- ^KD renames file

- Improve HTML highlighter... if you see <? it's probably a script...

- Check for EINTR from ioctl

- > allowed in xml content

- Add -flowed option: adds a space after paragraph lines.

- Fix German and French .po files: they were cause search&replace to break.

- Look at LC_MESSAGES to get the language to use for editor messages.

- Added -no_double_quoted and -tex_comment for TeX

- Added -break_symlinks option and changed -break_links option to not
  break symbolic links.

- Paragraph format of single line paragraph is indented only if autoindent
  is enabled. (jqh)

- Guessindent no longer overrides istep if indendation is space.

- Fix low limit of lmargin

- Allow inserting file in rectangle mode even if selected rectangle is
  zero-width.

- .js is Javascript

- Fix ^G in perl mode when you hit it on second brace in:

	{\'
	\'}

- Fix LUA highlighter (dirk_s)

- Improved conf.jsf (dirk_s)

- Added local option (-nobackup) to suppress backup files (peje)

- Add Matlab syntax file (neer)

- Improve mail syntax highlighter (jqh)

- Fix crash when calling syntax file as subroutine (hal9042)

- Get "ctags" tag search to work again

- Fix crash when JOE tries to write to unwritable file

- Fix crash when entering blank macro ESC x <return>

- Improve Verilog highlighter

- Fix crash when typing ESC x !ls

- Add C++ keywords to highlighter (otterpop81)

- Added RPM spec file syntax spec.jsf

- Improve 'istring' (.jsf command) (hal9042)

- Update French .po file (jengelh)

- Fix infinite search/replace loop bug (yugk)

- New feature: insert status line format string using 'txt' (tykef)

- Update Russion .po file (yugk)

- Update Russian manpage (yugk)

- Update jicerc Russian rc file (yugk)

- Fix lock prompt message (yugk)

- Add Ukrainian .po file (yugk)

### JOE 3.5

- Query windows now expand to multiple lines if necessary

- Single key queries are now internationalized (set local versions in the
  .po file)

- Spell check language can be set in the editor

- New syntax files: m4, joerc

- New debug window for highlight file syntax errors.

- Macros can be typed at the ESC X prompt (before it used to accept only
  commands).

- Built-in joerc file allows joe to run even if /etc/joe directory is
  missing.

- Support for 'long long' allows editing parts of files larger than 4GB. 
  For example, you can say: joe /dev/hda,0x100000000,0x1000 to edit the 4KB
  block at offset 4GB of a hard drive.

- Option which allows you set how many undo records to keep.

- You can give a path to the tags file in the TAGS environment variable.

### JOE 3.4

- Paragraph reformatter and word wrap now handle '*' and '-' bullet lists.

- Better internationalization (i18n):

    JOE now uses gettext(), so that internal messages can be translated to
    the local language.  The /etc/joe directory now has a lang subdirectory
    for the .po files.

    Internationalized joerc files are now possible.  If LANG is en_GB, JOE
    tries successively to load joerc.en_GB, joerc.en and joerc.

- Multi-file search and replace:

    There are two new search and replace options:

      'a': the search covers all loaded buffers.  So you can say:

            joe *.c
            and then ^KF foo <return>
                     ra <return>
                     bar <return>
            to replace all instances of foo in all .c files.

      'e': the search covers all files in the error list.

            You can use grep-find to create a list of files:

            ESC g
             grep -n foo f*.c
            ^KF foo <return>
            re
            bar <return>

            You can also use 'ls' and 'find' instead of grep to
	    create the file list.

- JOE now restores cursor position in previously visited files.

- Build and grep window work more like Turbo-C: the messages window is
  forced onto the screen when you hit ^[ = and ^[ -.

- Syntax highlighter definition files (.jsf files) can now have subroutines. 
  This eases highlighter reuse: for example, Mason and PHP can share the HTML
  highlighter.

- I've changed the way JOE handles '-' and redirected input:

	joe < file		A shell process is started which 'cat's the
				file into the first buffer.

	tail -f log | joe	A shell process is started which 'cat's the
				output from 'tail -f' (watch a log file) into
				the first buffer.

	joe -			JOE does not try to read from stdin, but
				when the file is saved, it writes to stdout.

	echo "hi" | joe - | mail fred
				"hi" ends up in first buffer.  When you
				save, mail is sent.

- Many bugs have been fixed.  I've tried to address every issue in the bug
  tracker.  Hopefully I didn't create too many new ones :-)

- You can now define which characters can indent paragraphs.  Also the
  default list has been reduced so that formatting of TeX/LaTeX files works
  better.

- Highlighting now uses less CPU time and always parses from the beginning
  of the file (the number of sync lines option is deprecated).  Here is a
  CPU usage comparison for scrolling forwards and backwards through a 35K
  line C file:

	JOE	.58
	JED	.57
	NEDIT	3.26
	VIM	7.33
	EMACS	11.98

- JOE now matches Thomas Dickey's implementation of my xterm patch (but
  configure xterm with --paste64).

- File selection menu/completion-list is now above the prompt (which is more
  like bash).  Also it is transposed, so that it is sorted by columns
  instead of rows.

- "Bufed" (prompt for a buffer to edit), works like other file prompt
  commands: it's a real prompt with history and completion.

- Automatic horizontal left scroll jumps by 5-10 columns.

- New syntax files: troff, Haskell, Cadance SKILL, REXX, LUA, RUBY.  Many of
  the existing syntax files have been improved.

### JOE 3.3

- The default background color can now be set.

- JOE now supports 256 color xterm.

- The mouse can now resize windows and select menu entries.

- During selection with the mouse, the window will autoscroll when you go
  past the edge.

- An xterm-patch is included which makes "-mouse" mode work better.  (With
  the patch, also set "-joexterm").

- Syntax files are provided: ADA, AWK, COBOL, SED, Postscript, and SQL

- Improved jpico: search now looks more like real pico

- Grep find: use ESC g to grep.  Then use ESC space to jump to
  to indicated file/line.

- Cygwin setup.exe support

### JOE 3.2

- A Perforce SCM "p4 edit" macro has been supplied (along with the hooks
  within JOE which support it) so that when you make the first change to a
  read-only file, JOE runs "p4 edit".  (look in joerc file to enable the
  macro).

- Hex edit mode has been added.  For example: joe -hex /dev/hda,0,1024

- New '-break_links' option causes JOE to delete before writing files, to
  break hard links.  Useful for 'arch' SCM.

- JOE now has GNU-Emacs compatible file locks.  A symbolic link called
  .#name is created, "pointing" to "user@machine.pid" whenever the buffer
  goes from unmodified to modified.  If the lock can't be created, the user
  is allowed to steal or ignore the lock, or cancel the edit.  The lock is
  deleted when buffer goes from modified to unmodified (or you close the
  file).

- JOE now periodically checks the file on the disk and gives a warning if
  it changed when you try to modify the buffer.  (JOE already performed this
  test on file save).

- The built-in calculator (ESC m) is now a full featured scientific
  calculator (I'm shooting for Casio Fx-4000 level here :-), including
  hexadecimal and ability to sum (and perform statistics on) a highlighted
  (possibly rectangular) block of numbers.  Hit ^K H at the math prompt for
  documentation.

- You can now change the current directory in JOE (well, it prompts with
  the latest used directory).

- Colors can now be specified in the joerc file

- Macro language now has conditionals and modifiers for dealing with
  repeat arguments.  Jmacs now works better due to this.

- Tab completion works at tags search prompt ^K ;

- ^G now jumps between word delimiters (begin..end in Verilog, #if #else #endif
  in C, /* .. */ and XML tags).  If it doesn't know the word, it
  starts a search with the word seeding the prompt.  It is also much smarter
  about skipping over comments and quoted matter.

- TAB completion is now much more like bash (again :-).  The cursor stays
  at the file name prompt instead of jumping into the menu system.  Also
  ^D brings up the menu, as in tcsh.  Also, tab completion now works on user
  names for ~ expansion.

- Now there is a ~/.joe_state file which stores:
	all history buffers
	current keyboard macros
	yank records

- Joe now has xterm mouse support: when enabled, the mouse can position
  the cursor and select blocks.  The mouse wheel will scroll the screen. 
  When enabled, shift-click emulates old xterm mouse behavior (cut &
  paste between applications).

- More syntax files: TeX, CSS, OCaml, Delphi, SML and 4GL.  Thanks to
  all of the contributers.

- Vastly improved highlighting of Perl and Shell due to the highlighter now
  understanding word and balanced delimiters.

- Many bugs have been fixed (every bug which has been entered into the
  sourceforge project page has been addressed).  Hopefully I didn't add
  too many new ones :-)

### JOE 3.1

- Regex and incremental search (jmacs ^S) now work for UTF-8

- More and improved syntax highlighting files, including Mason

- Use ^T E to set character set of file (hit <tab> <tab> at the
  prompt for a list of available character sets).

- Can install custom "i18n" style byte oriented character set
  definition files.

- No longer depends on iconv() (easier to compile)

- Fix bug where right arrow was not doing right thing on last line

- Fix UTF-8 codes between 0x10000 - 0x1FFFF

- Now prints <XXXX> for unicode control characters

- Improved smart home, indent, etc.

- TAB completion is now more "bash"-like

- When multiple files are given on command line, they end up in
  same order on the screen in JOE (before they were shuffled).

- Menu size is now variable (40% of window size or smaller if
  it's not filled).

- Added -icase option for case insensitive search by default.

- Added -wrap option, which makes searches wrap

- Added status line sequence %x: shows current context (function
  name if you're editing C).

- Added tab completion at search prompts and ESC-Enter for tab
  completion within text windows.

- Warn if file changed on save.

- Added Ctrl-space block selection method

- Added Ctrl-arrow key block selection method

- ^K E asks if you want to load original version of the file

- jmacs bugs fixes: upperase word, transpose words, ^X ^C is
  more emacs-like., ^X k and ^X ^V more like emacs.

- Much improved compile system ^[ c

- Much improved jpico

- aspell support.

### JOE 3.0 (23 APR 2004)

- UTF-8
- Syntax highlighting
- Fixed ^C and ^D in shell windows
- Auto detect CR-LF (MS-DOS) files
- Fixed (or improved, anyway) shell windows for Cygwin
- During search & replace, the scroll found text on to screen
- File selection window is now 4 lines instead of 1
- David Phillips "smart home" key.
- Enhanced ^K , and ^K .
- Enhanced overtype mode
- Added picture drawing mode (can hit right arrow at ends of lines)
- Auto detect preferred indentation character TAB or SPACE

### Overview of changes in JOE 2.9.8 (5 May 2003)

- fixed signal handling
- return of the context help
- fixed segfault when moving rectangular block
- code clean up
- killed deadlock when reformatting paragraph
- fixed skiptop handling
- SECURITY: drop suid and sgid bits when creating backup files
- fixed segfaults in isalpha()-like functions

### Overview of changes in JOE 2.9.8-pre1 (14 Dec 2001)

- BUGFIX: don't exchange start and end point of the block in some cases
- defaulting to turn off -asis (locales take care of this; if joe doesn't
  print characters with 8th bit set properly, check if you have properly
  installed and set locales or simply turn on -asis option in joerc)
- fix to make joe compilable on *BSD
- fix to make joe compilable on systems without siginterrupt()
- added "support" for End key
- code cleanup: warnings removal (some still remaining, working on it)

### Overview of changes in JOE 2.9.7 (7 Nov 2001)

- BUGFIX: always save (even not-modified) file
- BUGFIX: solve problem with freezing after saving unmodified file
- small documentation update

### Overview of Changes in JOE 2.9.7-pre3 (29 Oct 2001)

- BUGFIX: wordwrap bug fixed (again and I'm sure forever)
- BUGFIX: don't change window when setting mark in multiwindow mode
- BUGFIX: use automake-1.5 to make joe compile on irix-6.5 with non-GNU make
- continuing code clean up: this code adds strict prototypes which raises
  a lot of warnings (they seem harmless) - we're working on their removal

### Overview of Changes in JOE 2.9.7-pre2 (10 Oct 2001)

- use automake and autoconf for configuration system (for now versions
  automake-1.4-p4 and autoconf-2.52)
- a lot of warnings of compiler were removed
- SECURITY:: use mkstemp() for temporary files if available
- code clean up
    
### Overview of Changes in JOE 2.9.7pre1 (19 Jul 2001)

- help system was slightly modified
- a lot of warnings of compiler were removed
- BUGFIX:: problem with freezing when save was solved (at least partially)
- BUGFIX:: undo after save of file (again same patch)
- FEATURE:: suffix of backup copy from SIMPLE_BACKUP_SUFFIX environment 
  variable

### Overview of Changes in JOE 2.9.7pre0 (02 Jul 2001)

- major BUGFIX:: wordwrap problem
- BUGFIX:: indentation
??* JOE can be compiled on Windows platform again
??* BUGFIX:: problem with ${sysconfdir}/joerc was solved

### Overview of Changes in JOE 2.9.6 (13 Apr 2001)

- BUGFIX:: resizing window
- JOE can be compiled on Windows platform again
* BUGFIX:: problem with ${sysconfdir}/joerc was solved
- BUGFIX:: security patch for sprintf
- BUGFIX:: partially solved problem on Solaris with SegFault
- BUGFIX:: patch joe-2.8-security (slightly changed)
- BUGFIX:: patch joe-2.8-port
- BUGFIX:: patch joe-2.8-mips
- BUGFIX:: patch joe-2.8-vsmk
- BUGFIX:: patch joe2.8-time
- *rc files where moved from $(prefix)/lib to $(prefix)/etc or $sysconfdir
- Makefile.in (and Makefile) was rewritten
   - special prefix for package (more in Makefile.in)
   - use of system independent 'mkinstalldirs'
   - rc files are not rewritten
- TEST FEATURE:: added autoconf support
		program can be installed by ./configure; make; make install
- BUGFIX (v2.9.4):: go to previous word problem solved
- JOE can be compiled without locale.h again
- BUGFIX:: patch joe2.8-axphack.patch
- BUGFIX:: patch joe2.8-resize2.patch
- BUGFIX:: fixed problem with :include in rc files
- BUGFIX (v2.9.5):: portability problem with is_blank on nonGNU systems

### Overview of Changes in JOE 2.9.5 (28 Mar 2001)

- new BUG:: can't be compiled on non-GNU systems (is_blank()) fixed in v2.9.6
- BUGFIX:: Fixed problem with resizing. 
- SECURITY:: .[joe|rjoe|jpico|..]rc in actual directory is ignored
             because in this file can be defined which program run.

### Overview of Changes in JOE 2.9.4 (27 Mar 2001)

- new BUG:: go to previous word; goes one character before this word 
            fixed in v2.9.6
- FEATURE:: locale (LC_CTYPE) is accepted when skipping/deleting/... words	

### Overview of Changes in JOE 2.9  (22 Mar 2001)

- version 2.8 with patches from RedHat/Suse

### Overview of Changes in JOE 2.8

- Fixed problem with TERMPATH string
- Added stupid two-letter names to termcap file
- Improved jmacs help and command set
- Improved README file

### Overview of Changes in JOE 2.7

- putenv() was not portable
- utime was not portable
- special utime handling for NeXT
- forgot to \\\\ the \s in the default termcap entry
- changed some key defaults in jpicorc
- add IXOFF in termio/termios list
- left margin limit was incorrect
- allow '.' and '/' in file names for error parsing
- Needed ptem.h and stream.h for SCO_UNIX window size structure (?)
- wordwrap no longer propogates indent prefix
- paragraph format was broken for tab indented paragraphs
- pipe through shell now goes through stderr too
- added '-crlf' option
- looks for termcap file in JOERC/termcap

### Overview of Changes in JOE 2.6

- Fixed stupid bug in termcap.c which prevented terminfo from working
- ESC h was missing from jpicorc
- Changes suggested by Dan Nelson:
   - backup files now attempt to have same permissions and times as original
   - Stat command now ands chars with 255 (don't know why this worked on my system
     without this...)
   - Maybe change shell invocation name- have to check this more

### Overview of Changes in JOE 2.5

- No longer use ^[ O, ^[ A, ^[ B, ^[ C, or ^[ D for anything because they
  interfere with arrow keys.
- Couldn't create new files because of bug in readonly setting
- fwrdc/bkwdc were crashing the editor except when called from wordstar
- 'tr' command was not called in a portable way in jmacs
- 'tr' was causing problems with the spell-check macros as well
- filter region was not working: had to add 'query' in ^[ | macro
- Changed incremental search so that whatever key the command is bound to
  can be used to repeat the search (used to only be able to use ^S)

### Overview of Changes in JOE 2.4

- Closing message was incorrect when exit macros (macros where the last
  command is abortbuf) were used.
- SuperPico rc file added
- Write block now writes the entire file if no block is set
- Added pico kill function for pico emulation
  (tried to do this with 'psh,markk,blkdel' where blkdel deletes lines if
   no block is set, but it didn't group the deletes right in the yank
   buffer)
- Filter block would leave the marks set
- Fixed ^@ in joe mode
- Fixed help screen glitches in wordstar mode
- If joe can't make a backup file it now prompts for you to save anyway
- Eliminated IDLEOUT compile option.  Now is the user gives - on the
  command line, joe uses /dev/tty.
- Added %o %O %a %A %X and %R status line messages
- Starts out in read only mode if loaded file can not be written to
- If joe can't find the termcap/terminfo entry, it instead uses the default
- termcap routines are now included even if you use terminfo.  If your
  terminal can't be found in the terminfo database, it will look in
  the termcap database too.
- The JOETERM environment variable can be used to bypass the TERM
  environment variable setting.

### Overview of Changes in JOE 2.3

- Search & Replace bugs fixed
   - replace would mess up the end mark of a marked block
   - a search would trash ^KB if only ^KB was set
   - regex problem with \\*
- Was using TCSANOW in posix driver.  Should have been using TCSADRAIN
- Format paragraph now correctly formats quoted news articles
- Attempted fix for SCO
- Fix for coherent
- Fix for old IRIX/SGI
- Fixed bug where if you used search & replace in a macro, and exiting the
  search & replace loop with right arrow key, then when you played the macro
  you got an extra ^[ in the input queue
- Added file hooks
- Added function to insert current keyboard macro into current file
- Added range checks to numeric option settings
- Restricted joe rc file added
- Added ':def' feature for rc files

### Overview of Changes in JOE 2.2

- First attempt at MS-DOS version of joe
   - Direct screen write
   - Modifications for dos file/drive names
   - Use TEMP variable to figure out where to store temporary file
   - Smaller virtual memory page size
   - Backslashes in file name problem
   - CR before an LF looks like an LF
- Backward search ignore-case was not working
- Scalable window height was not working fully
- Spaces in file-names gave a problem with backup file creation
- TILDE option is not available in all versions of BSD
- Allow : as seperate for termcap filename list
- Next error / Prev. error was not tracking right
- tabs not displayed right in nxterr/prverr messages
- Block moves where the cursor was to the right of the block was broken

### Overview of Changes in JOE 2.1

- rc file wasn't giving correct error messages for missing options
- the '-nobackups' options was mispelled: '- nobackups'
- editor was crashing because of problem in undo
- update bypass in utype has a problem when wordwrapping and scrolling
