# Aliases for sh/dash/ash/bash/ksh/zsh in JOE shell window

# Clear edit buffer
joe_clear () {
	echo '{'shell_clear'}'
}

# Parse errors
joe_parse () {
	echo '{'shell_parse'}'
}

# Release errors
joe_release () {
	echo '{'shell_release'}'
}

# Use JOE's calculator
joe_math () {
	echo '{'shell_math,'"'$1'"',shell_rtn'}'
	echo '{'shell_rtn,shell_math,'"'ans:ins'"',shell_rtn'}'
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

joe_clear
