if [ -z "$tmp_c_sh" ] ; then
tmp_c_sh=t

# Compute the name of temporary files for C compilation. 

. sh/tmp.sh

tmp_c=$TMP/$$.cc
tmp_e=$TMP/$$.p

export tmp_c tmp_e

fi ## [ "$tmp_c_sh" ]
