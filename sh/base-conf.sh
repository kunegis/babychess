if [ -z "$base_conf_sh" ] ; then
CXX=${CXX:-g++}
MANDATORY_CXXFLAGS="`pkg-config --cflags gtk+-2.0 `"
fi
