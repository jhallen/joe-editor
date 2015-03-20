# JOE for Windows tips

### Clipboard integration

^K C in JOE and JSTAR will paste text from the Windows clipboard if no block
is set.  JOE also maps ^S to copy the current block into the clipboard.  If
you use a different personality, you may use the context menu (right click
in the editor) to copy into/paste from the Windows clipboard.

### Explorer integration

You can drag files from Windows explorer onto JOE, and it will open those
files.

### Shell windows

Shell windows work, but with some limitations.  Programs that interact
directly with the console interface (e.g., PowerShell) do not work and will
appear to hang when run.  You may be able to stop them by moving the cursor
away from the end of file and pressing ^C to terminate.  If that doesn't
work, try task manager.

Some programs can be made to work, for example Python needs the "-i" switch
to function properly.  The shell windows interact with applications
similarly to the way that mintty does.  If there is a workaround for mintty,
it will likely work for JOE.

### Appearance

JOE for Windows comes with several builtin color schemes, which are more
suitable for a GUI environment.  They can be selected through the context
menu (right click on the editor window).  The font and cursor type may also
be changed there.

### Full screen

You can toggle fullscreen mode with Alt-Enter to relive the old days.  Even
better, find a CP437 font!

### File locations

JOE for Windows uses a different (but similar) file layout than JOE for
UNIXen.  Most files important to the editor are found at the install
location of the joe.exe file, usually C:\\Program Files (x86)\\JoeEditor. 
\*rc files are stored in the conf\\ subdirectory, \*.jsf files in syntax\\ and
color schemes in schemes\\.

You can override any of these files or settings by sticking a copy in your
"home" directory.  This directory is found in %LOCALAPPDATA%\\JoeEditor
(where %LOCALAPPDATA% is the value of that environment variable -- it's
usually C:\\Users\\(your user name)\\AppData\\Local\\JoeEditor).

* Place any *rc files in the root of that directory

* Place any *.jsf files in the syntax\\ subdirectory

* Place any *.joecolor (color scheme) files in the schemes\\ subdirectory
