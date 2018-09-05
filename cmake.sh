function echo_exec(){
	lcmd=$1
	echo $lcmd
	$lcmd
	if [ $? -ne 0 ]; then
		exit
	fi
}
echo_exec "mkdir -p bld"
echo_exec "cd bld"
echo_exec "rm -f CMakeCache.txt"
echo_exec "cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/"
echo_exec "make VERBOSE=1"
echo_exec "make install"
echo_exec "cd .."
