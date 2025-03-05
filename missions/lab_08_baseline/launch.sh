#!/bin/bash 
#-------------------------------------------------------
#   Script: launch.sh                       
#  Mission: lab_08_baseline
#-------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
ME=`basename "$0"`
CMD_ARGS=""
TIME_WARP=1
VERBOSE=""
JUST_MAKE=""

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS] [time_warp]                         " 
	echo "                                                  " 
	echo "Options:                                          " 
	echo "  --help, -h         Display this help message    "
	echo "  --verbose, -v      Verbose, confirm launch      "
	echo "  --just_make, -j    Only create targ files       "
	echo "                                                  "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
        JUST_MAKE="-j"
    else 
	echo "ME: Bad Arg:" $ARGI "Exit Code 1."
	exit 1
    fi
done

if [ "${VERBOSE}" != "" ]; then
    echo "============================================"
    echo "  $ME SUMMARY                   (ALL)       "
    echo "============================================"
    echo "CMD_ARGS =      [${CMD_ARGS}]               "
    echo "TIME_WARP =     [${TIME_WARP}]              "
    echo "JUST_MAKE =     [${JUST_MAKE}]              "
    echo "                                            "
    echo -n "Hit any key to continue launch           "
    read ANSWER
fi

#-------------------------------------------------------
#  Part 3: Launch the vehicles
#-------------------------------------------------------
VARGS=" $JUST_MAKE $VERBOSE $TIME_WARP "
echo "Launching henry...."
./launch_vehicle.sh --vname=henry --mport=9001 --pshare=9201  \
		    --nogui --auto $VARGS &

echo "Launching gilda...."
./launch_vehicle.sh --vname=gilda --mport=9002 --pshare=9202  \
		    --nogui --auto $VARGS &

#-------------------------------------------------------
#  Part 4: Launch the shoreside
#-------------------------------------------------------
echo "Launching shoreside...."
./launch_shoreside.sh $VARGS --ip=localhost

exit 0
