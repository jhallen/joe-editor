# Aliases for sh/dash/ash/bash/ksh/zsh in JOE shell window

# Clear edit buffer
joe_clear () {
	echo '{'shell_clear'}'
}

# Release errors
joe_release () {
	echo '{'shell_release'}'
}

# Set marked region beginning
joe_markb () {
	echo '{'shell_markb'}'
}

# Set marked region end
joe_markk () {
	echo '{'shell_markk'}'
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
	echo '{'shell_math,'"'$1'"',shell_rtn!,shell_typemath'}'
	cat >/dev/null
}

# Edit a file
joe_edit () {
	echo '{'shell_popedit,'"'$1'"',shell_rtn'}'
}

# Pop shell window
joe_pop () {
	echo '{'shell_pop'}'
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
	echo '{'shell_cd,shell_dellin!,'"'`pwd`/'"',shell_rtn'}'
}

alias clear=joe_clear
alias math=joe_math
alias edit=joe_edit
alias joe=joe_edit
alias pop=joe_pop
alias cd=joe_cd
alias parse=joe_parse
alias release=joe_release
alias markb=joe_markb
alias markk=joe_markk
alias mark=joe_mark

joe_clear
