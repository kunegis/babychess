if [ -z "$BABYCHESS_VERSION_NUMBER" ] ; then
    BABYCHESS_VERSION_NUMBER="`basename $PWD | sed 's/^[^ -]*-\([^ -]*\)$/\1/'`" ; fi
