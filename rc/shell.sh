# Aliases for sh/dash/ash/bash/ksh/zsh in JOE shell window

joehelp () {
	echo "clear         - erase buffer"
	echo "joe           - edit file"
	echo "math 1+2      - calculator"
	echo "pop           - dismiss shell"
	echo "parse [cmd]   - grep parse command"
	echo "parserr [cmd] - compile parse command"
	echo "release       - drop parsed messages"
	echo "markb         - mark beginning of region"
	echo "markk         - mark end of region"
	echo "mark cmd      - mark output of command"
}

# Clear edit buffer
joe_clear () {
	echo -n '{'shell_clear'}'
}

# Release errors
joe_release () {
	echo -n '{'shell_release'}'
}

# Set marked region beginning
joe_markb () {
	echo -n '{'shell_markb'}'
}

# Set marked region end
joe_markk () {
	echo -n '{'shell_markk'}'
}


# Mark command result
joe_mark () {
	joe_markb
	$*
	joe_markk
}

# Parse command result (or whole buffer if no arg)
joe_parse () {
	if [ "$1" = "" ]; then
		echo -n '{'shell_gparse'}'
	else
		joe_markb
		$*
		joe_markk
		echo '{'shell_gparse'}'
	fi
}

# Parse command result (or whole buffer if no arg)
joe_parserr () {
	if [ "$1" = "" ]; then
		echo '{'shell_parse'}'
	else
		joe_markb
		$*
		joe_markk
		echo '{'shell_parse'}'
	fi
}

# Use JOE's calculator
joe_math () {
	echo -n '{'shell_math,'"'$1'"',shell_rtn!,shell_typemath'}'
	cat >/dev/null
}

# Edit a file
joe_edit () {
	echo -n '{'shell_edit,'"'$1'"',shell_rtn'}'
}

# Pop shell window
joe_pop () {
	echo -n '{'shell_pop'}'
}

unalias cd 2>/dev/null

# Change directory
joe_cd () {
	# cd $1    - does not work for directories with spaces in their names
	# cd "$1"  - breaks cd with no args (it's supposed to go to home directory)
	# So we have do this...
	if [ "$1" = "" ]; then
		cd
	else
		cd "$1"
	fi
	# Tell JOE our new directory
	echo -n '{'shell_cd,shell_dellin!,'"'`pwd`/'"',shell_rtn'}'
}

alias clear=joe_clear
alias math=joe_math
alias edit=joe_edit
alias joe=joe_edit
alias pop=joe_pop
alias cd=joe_cd
alias parse=joe_parse
alias parserr=joe_parserr
alias release=joe_release
alias markb=joe_markb
alias markk=joe_markk
alias mark=joe_mark

# Code to automatically mark and parse output from each command
# - This is bash specific code

#joe_markb_pre () {
#	joe_markb
#	MARK_FLAG=1
#}

#joe_markk_post () {
#	if [ "$MARK_FLAG" = "1" ]; then
#		joe_markk
#		MARK_FLAG=0
#		joe_parse
#	fi
#}

#preexec () { :; }

#preexec_invoke_exec () {
#    [ -n "$COMP_LINE" ] && return  # do nothing if completing
#    [ "$BASH_COMMAND" = "$PROMPT_COMMAND" ] && return # don't cause a preexec for $PROMPT_COMMAND
#    local this_command=`HISTTIMEFORMAT= history 1 | sed -e "s/^[ ]*[0-9]*[ ]*//"`;
#    joe_markb_pre
#    preexec "$this_command"
#}

#trap 'preexec_invoke_exec' DEBUG

#PROMPT_COMMAND=joe_markk_post

joe_clear

echo
echo Type joehelp for editor shell commands
echo
