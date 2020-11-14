#!/bin/bash

echo "Setting LIBRARY PATHS!"

# define the SYSRT home directory
export SYSRT; SYSRT=`pwd`
echo $SYSRT

# extend the search path for dynamically-linked libraries
if [ -z "$LD_LIBRARY_PATH" ]; then
	export LD_LIBRARY_PATH; LD_LIBRARY_PATH=$SYSRT/lib;
else
	export LD_LIBRARY_PATH; LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SYSRT/lib;
fi
echo $LD_LIBRARY_PATH
