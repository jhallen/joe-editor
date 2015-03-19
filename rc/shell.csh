# Aliases for csh/tcsh in JOE shell window

alias clear "echo \{shell_clear}"

alias parse "echo \{shell_parse}"

alias release "echo \{shell_release}"

alias markb "echo \{shell_markb}"

alias markk "echo \{shell_markk}"

alias math "echo \{shell_math,"\\\"\!\*\\\"",shell_rtn}; echo \{shell_rtn,shell_math,"\\\""ans:ins"\\\"",shell_rtn}"

alias edit "echo \{shell_popedit,"\\\"\!\*\\\"",shell_rtn}"

alias joe "echo \{shell_edit,"\\\"\!\*\\\"",shell_rtn}"

alias pop "echo \{shell_pop}"

alias cd "cd "\!\*"; echo \{shell_cd,shell_dellin\\!,"\\\"`pwd`/\\\"",shell_rtn}"

clear
