if [ -z "$echo_n_sh" ] ; then
echo_n_sh=t

##
## Test how to 'echo' without a newline
##
## Use like that:
##        $echo_n "input > $echo_c"
##        echo $input
##

if echo "test\c" | grep c >/dev/null
then
    echo_n="echo -n"
    echo_c=
else
    echo_n=echo
    echo_c='\c'
fi

fi # [ -z "$echo_n_sh" ] 

