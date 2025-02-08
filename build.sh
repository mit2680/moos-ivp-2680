#!/bin/bash

#----------------------------------------------------------                               
#  Script: build.sh                                                                      
#  Author: Michael Benjamin                                                               
#---------------------------------------------------------- 

INVOCATION_ABS_DIR=`pwd`
BUILD_TYPE="None"
CMD_LINE_ARGS=""
BUILD_BOT_CODE_ONLY="OFF"

#-------------------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-------------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "build.sh [SWITCHES]                 " 
	echo "Switches:                           " 
	echo "  --help, -h                        " 
        echo "  --debug,   -d                     "
        echo "  --release, -r                     "
	echo "  --minrobot, -m                           "
	echo "    Only build minimal robot apps          "
	echo "  --minrobotx, -mx                         "
	echo "    Override min-robot default on Raspbian "
	echo "Notes:                              "
	echo " (1) All other command line args will be passed as args    "
	echo "     to \"make\" when it is eventually invoked.            "
	echo " (2) For example -k will continue making when/if a failure "
	echo "     is encountered in building one of the subdirectories. "
	echo " (3) For example -j2 will utilize a 2nd core in the build  "
	echo "     if your machine has two cores. -j4 etc for quad core. "
	exit 0;
    elif [ "${ARGI}" = "--debug" -o "${ARGI}" = "-d" ]; then
        BUILD_TYPE="Debug"
    elif [ "${ARGI}" = "--release" -o "${ARGI}" = "-r" ]; then
        BUILD_TYPE="Release"
    elif [ "${ARGI}" = "--minrobot" -o "${ARGI}" = "-m" ]; then
        BUILD_BOT_CODE_ONLY="ON"
    elif [ "${ARGI}" = "--minrobotx" -o "${ARGI}" = "-mx" ]; then
        FORCE_FULL_RASPI_BUILD="yes"
    else
	CMD_LINE_ARGS=$CMD_LINE_ARGS" "$ARGI
    fi
done

#-------------------------------------------------------------- 
#  Part 2: If this is Raspbian and minrobot not selected, and
#          no explicit override given with -mx, CONFIRM first
#-------------------------------------------------------------- 
command -v raspi-gpio
if [ "$?" = "0" -a "${BUILD_BOT_CODE_ONLY}" = "OFF" ]; then
    if [ ! "${FORCE_FULL_RASPI_BUILD}" = "yes" ]; then
	echo "Pi OS detected without --minrobotx or -mx selected."
	echo "[y] Continue with full build"
	echo "[M] Continue with minrobot build"
	echo -n "Continue? [y/M] "
	read ANSWER
	if [ ! "${ANSWER}" = "y" ]; then
	    BUILD_BOT_CODE_ONLY="ON"
	fi
    fi
fi
	
#---------------------------------------------------------
# Part 2: Set Compiler flags
#---------------------------------------------------------
CMAKE_CXX_FLAGS="-Wall -Wextra -Wno-unused-parameter "
CMAKE_CXX_FLAGS+="-Wno-missing-field-initializers -pedantic -fPIC "
CMAKE_CXX_FLAGS+="-Wno-deprecated-declarations "
CMAKE_CXX_FLAGS+="-Wno-c++11-extra-semi " 

#-------------------------------------------------------------------
#  Part 3: Invoke the call to make in the build directory
#-------------------------------------------------------------------

mkdir -p build
cd build

cmake -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS}"       \
      -DBUILD_BOT_CODE_ONLY=${BUILD_BOT_CODE_ONLY} \
      -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ../

make ${CMD_LINE_ARGS}
cd ${INVOCATION_ABS_DIR}


