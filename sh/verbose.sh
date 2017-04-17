##
## FD for verbose output.  Use >&4 to output debug messages.  Enabled by
## setting $verbose to non-empty in the environment.  E.g.:
##     echo >&4 blabla      to output when verbose >= 1
##     echo >&5 blabla      to output when verbose >= 2
##

if [ -z "$verbose_sh" ] ; then
verbose_sh=t

if [ -z "$verbose" ] ; then verbose=0 ; fi

if [ "$verbose" -ge 1 ]
then
    exec 4>&2
else
    exec 4>/dev/null
fi

if [ "$verbose" -ge 2 ] ; then
    exec 5>&2
else
    exec 5>/dev/null
fi

fi ## [ -z "$verbose_sh" ] 
