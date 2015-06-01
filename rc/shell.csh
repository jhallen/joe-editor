# Aliases for csh/tcsh in JOE shell window

if ( $JOEWIN != "" && $OSTYPE == "cygwin" ) then
	# Read initial terminal size from environment
	stty rows $LINES cols $COLUMNS

	# Continually update terminal size from JOE's named pipe.
	set fn=`cygpath "$JOEDATA/vt/joewinpty.exe"`
	if ( -e "$fn" ) then
		set PTYHELPER="$fn"
	else
		set fn=`cygpath "$JOEHOME/vt/joewinpty.exe"`
		if ( -e "$fn" ) then
			set PTYHELPER="$fn"
		endif
	endif

	if ( "$PTYHELPER" != "" ) then
		"$PTYHELPER" winsize | sh -c 'while read w h ; do stty -F /dev/tty rows $h cols $w ; done' &
		unset PTYHELPER
	endif
	
	unset fn
endif

alias joehelp 'echo "clear         - erase buffer"; \\
echo "joe           - edit file"; \\
echo "math 1+2      - calculator"; \\
echo "pop           - dismiss shell"; \\
echo "parse cmd     - grep parse command"; \\
echo "parserr cmd   - compile parse command"; \\
echo "release       - drop parsed messages"; \\
echo "markb         - mark beginning of region"; \\
echo "markk         - mark end of region"; \\
echo "mark cmd      - mark output of command"'

alias clear "echo -n \{shell_clear}"

alias parse "echo -n \{shell_gparse}"

alias parserr "echo -n \{shell_parserr}"

alias release "echo -n \{shell_release}"

alias markb "echo -n \{shell_markb}"

alias markk "echo -n \{shell_markk}"

alias mark "echo -n \{shell_markb}; "\!\*"; echo -n \{shell_markk}"

alias math "echo -n \{shell_math,"\\\"\!\*\\\"",shell_rtn\\!,shell_typemath}; cat >/dev/null"

alias pop "echo -n \{shell_pop}"

if ( $JOEWIN != "" && $OSTYPE == "cygwin" ) then
	alias edit 'echo -n \{shell_edit,shell_dellin\\!,\"`cygpath -aw \!* | sed s/\\\\/\\//g`\",shell_rtn}'
	alias joe 'echo -n \{shell_edit,shell_dellin\\!,\"`cygpath -aw \!* | sed s/\\\\/\\//g`\",shell_rtn}'
	alias cd 'cd \!*; echo -n \{shell_cd,shell_dellin\\!,\"`cygpath -aw . | sed s/\\\\/\\//g `\",shell_rtn}'
else
	alias edit "echo -n \{shell_edit,"\\\"\!\*\\\"",shell_rtn}"
	alias joe "echo -n \{shell_edit,"\\\"\!\*\\\"",shell_rtn}"
	alias cd "cd "\!\*"; echo -n \{shell_cd,shell_dellin\\!,"\\\""; pwd | tr -d '\n'; echo -n /"\\\"",shell_rtn}"
endif

clear

echo
echo Type joehelp for editor shell commands
echo
