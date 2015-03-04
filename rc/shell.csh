# Aliases for csh/tcsh in JOE shell window

alias clear "echo \{psh,bof,markb,eof,markk,blkdel}"

alias math "echo \{math,"\\\"\!\*\\\"",rtn}; echo \{rtn,math,"\\\""ans:ins"\\\"",rtn}"

alias edit "echo \{edit,"\\\"\!\*\\\"",rtn}"

alias cd "cd "\!\*"; echo \{cd,dellin\\!,"\\\"`pwd`/\\\"",rtn}"

clear
