# Aliases for csh/tcsh in JOE shell window

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

alias edit "echo -n \{shell_edit,"\\\"\!\*\\\"",shell_rtn}"

alias joe "echo -n \{shell_edit,"\\\"\!\*\\\"",shell_rtn}"

alias pop "echo -n \{shell_pop}"

alias cd "cd "\!\*"; echo -n \{shell_cd,shell_dellin\\!,"\\\""; pwd | tr -d '\n'; echo -n /"\\\"",shell_rtn}"

clear

echo
echo Type joehelp for editor shell commands
echo
