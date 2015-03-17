# Aliases for sh/dash/ash/bash/ksh/zsh in JOE shell window

joe_clear () {
	echo '{'shell_clear'}'
}

joe_math () {
	echo '{'shell_math,'"'$1'"',shell_rtn'}'
	echo '{'shell_rtn,shell_math,'"'ans:ins'"',shell_rtn'}'
}

joe_edit () {
	echo '{'shell_popedit,'"'$1'"',shell_rtn'}'
}

unalias cd 2>/dev/null

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
alias cd=joe_cd

joe_clear
