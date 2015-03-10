# Aliases for sh/dash/ash/bash/ksh/zsh in JOE shell window

joe_clear () {
	echo '{'psh,bof,markb,eof,markk,blkdel'}'
}

joe_math () {
	echo '{'math,'"'$1'"',rtn'}'
	echo '{'rtn,math,'"'ans:ins'"',rtn'}'
}

joe_edit () {
	echo '{'edit,'"'$1'"',rtn'}'
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
	echo '{'cd,dellin!,'"'`pwd`/'"',rtn'}'
}

alias clear=joe_clear
alias math=joe_math
alias edit=joe_edit
alias joe=joe_edit
alias cd=joe_cd

joe_clear
