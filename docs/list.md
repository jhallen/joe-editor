<h3>JOE commands grouped by function:</h3>

<p>These commands can be entered at the ESC x prompt.</p>

<p><b>Background programs</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>bknd</td><td>Run a shell in a window</td></tr>
<tr valign="top"><td>killproc</td><td>Kill program in current window</td></tr>
<tr valign="top"><td>run</td><td>Run a UNIX command in a window</td></tr>
<tr valign="top"><td>sys</td><td>Run a UNIX command and return to editor when done (I/O does
not go through editor, be we get the command's return status).</td></tr>
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
<tr valign="top"><td>filt</td><td>Filter block or file through a unix command</td></tr>
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
<tr valign="top"><td>scratch</td><td>Load a scratch buffer into current window</td></tr>
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
<tr valign="top"><td>prverr</td><td>Go to previous parsed error</td></tr>
<tr valign="top"><td>showerr</td><td>Show current message</td></tr>
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

<tr valign="top"><td>killjoe</td><td>Exit joe immediately without checking for modified
buffers</td></tr>
</tbody>
</table>

<p><b>Files</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
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
</tbody>
</table>

<p><b>Misc</b></p>

<table width="100%" cellspacing=20 border=0 cellpadding=0>
<colgroup>
<col width="125">
<tbody>
<tr valign="top"><td>beep</td><td>Beep</td></tr>

<tr valign="top"><td>execmd</td><td>Execute a joe command</td></tr>

<tr valign="top"><td>math</td><td>Calculator</td></tr>

<tr valign="top"><td>mode</td><td>Mode prompt</td></tr>

<tr valign="top"><td>menu</td><td>Menu prompt</td></tr>

<tr valign="top"><td>msg</td><td>Display a message</td></tr>

<tr valign="top"><td>notmod</td><td>Clear the modified flag</td></tr>

<tr valign="top"><td>retype</td><td>Refresh screen</td></tr>

<tr valign="top"><td>shell</td><td>Suspend process or execute a sub-shell</td></tr>

<tr valign="top"><td>stat</td><td>Display cursor position</td></tr>

<tr valign="top"><td>tag</td><td>Tags file search</td></tr>

<tr valign="top"><td>txt</td><td>Insert text</td></tr>

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

<p>if,"char==65",then,"it's an A",else,"it's not an A",endif	^[ q</p>

<p>When you hit ^[ q, if the character under the cursor is an 'A': "it's a A"
is inserted into the buffer, otherwise "it's not an A" is inserted.</p>

<p>"if" creates a math prompt (like ESC m).  "then" is like "rtn"- it hits the
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

<tr valign="top"><td>defmdrag</td><td>(click to move cursor, drag to select
characters)</td></tr>

<tr valign="top"><td>defm2down</td><td>Default double-click handlers</td></tr>

<tr valign="top"><td>defm2drag</td><td>(drag to select words)</td></tr>

<tr valign="top"><td>defm3down</td><td>Default triple-click handlers</td></tr>

<tr valign="top"><td>defm3drag</td><td>(drag to select lines)</td></tr>

<tr valign="top"><td>paste</td><td>Insert base64 encoded text (for XTerm --enable-base64
option).</td></tr>
</tbody>
</table>


<p><b>Math</b></p>

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

<tr valign="top"><td>joe</td><td>..)	Execute a JOE macro (argument in same format
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

