#!/bin/bash -e
#---------------------------------------------------------------  
#  Part 1: Declare global var defaults
#---------------------------------------------------------------
#   Script: launch.sh
#  Mission: lab_16
#   LastEd: 2021-May-10
#---------------------------------------------------------------
#  Part 1: Set global var defaults
#---------------------------------------------------------------
ME=`basename "$0"`
TIME_WARP=1
JUST_MAKE=""
VERBOSE=""
AUTO_LAUNCHED="no"

# parameters to vehicles
COOL_FAC="--cool=50"
COOL_STEPS="--steps=1000"
UNCONCURRENT=""
ADAPTIVE=""
DEGREES1=270
DEGREES2=0

SURVEY_X=70
SURVEY_Y=-100
HEIGHT1=150
HEIGHT2=150
WIDTH1=120
WIDTH2=120
LANE_WIDTH1=25
LANE_WIDTH2=25
LAUNCH_TWO="yes"

#---------------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#---------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then 
	echo "$ME [SWITCHES] [time_warp]                                   "
	echo "  --help, -h           Show this help message                "
	echo "  --just_make, -j      Just make targ files, no launch       "
	echo "  --verbose, -v        Verbose output, confirm before launch "
	echo "                                                             "
	echo "  --one, -1                      "
	echo "  --adaptive, -a                 "
	echo "  --unconcurrent, -uc            "
	echo "  --cool=COOL_FAC                "
	echo "  --steps=COOL_STEPS             "
	exit 0
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="-v"
	
    elif [ "${ARGI}" = "--one" -o "${ARGI}" = "-1" ]; then
        LAUNCH_TWO="no"
    elif [ "${ARGI:0:7}" = "--cool=" ]; then
        COOL_FAC=$ARGI
    elif [ "${ARGI:0:8}" = "--steps=" ]; then
        COOL_STEPS=$ARGI
    elif [ "${ARGI:0:8}" = "--angle=" ]; then
        DEGREES1="${ARGI#--angle=*}"
    elif [ "${ARGI:0:9}" = "--angle1=" ]; then
        DEGREES1="${ARGI#--angle1=*}"
    elif [ "${ARGI:0:9}" = "--angle2=" ]; then
        DEGREES2="${ARGI#--angle2=*}"
    elif [ "${ARGI}" = "--adaptive" -o "${ARGI}" = "-a" ]; then
        ADAPTIVE=$ARGI
    elif [ "${ARGI}" = "--unconcurrent" -o "${ARGI}" = "-uc" ]; then
        UNCONCURRENT=$ARGI
    else 
	echo "Bad Arg:[$ARGI]. Exit Code 1."
	exit 1
    fi
done

#---------------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#---------------------------------------------------------------
VNAME1="archie"   
VNAME2="betty"

START_POS1="0,0"  
START_POS2="10,-5"

VLAUNCH_ARGS=" --auto $COOL_FAC $COOL_STEPS $UNCONCURRENT $ADAPTIVE "
VLAUNCH_ARGS+=" --survey_x=$SURVEY_X --survey_y=$SURVEY_Y "


VLAUNCH_ARGS1="$VLAUNCH_ARGS --vname=$VNAME1 --index=1 --start=$START_POS1 "
VLAUNCH_ARGS1+="--angle=$DEGREES1 --width=$WIDTH1 "
VLAUNCH_ARGS1+="--height=$HEIGHT1 --lwidth=$LANE_WIDTH1"

echo "$ME: Launching $VNAME1 ..."
./launch_vehicle.sh $VLAUNCH_ARGS1 $VERBOSE $JUST_MAKE $TIME_WARP

if [ "${LAUNCH_TWO}" = "yes" ]; then

    VLAUNCH_ARGS2="$VLAUNCH_ARGS --vname=$VNAME2 --index=2 --start=$START_POS2 "
    VLAUNCH_ARGS2+="--angle=$DEGREES2 --width=$WIDTH2 "
    VLAUNCH_ARGS2+="--height=$HEIGHT2 --lwidth=$LANE_WIDTH2"
    
    echo "$ME: Launching $VNAME2 ..."
    ./launch_vehicle.sh $VLAUNCH_ARGS2 $VERBOSE $JUST_MAKE $TIME_WARP
fi

#---------------------------------------------------------------
#  Part 4: Launch the shoreside
#---------------------------------------------------------------
echo "$ME: Launching Shoreside ..."
SLAUNCH_ARGS="--vname1=$VNAME1 --vname2=$VNAME2 $VERBOSE "
./launch_shoreside.sh --auto $SLAUNCH_ARGS $JUST_MAKE $TIME_WARP

#---------------------------------------------------------------
#  Part 5: If launched from script, we're done, exit now
#---------------------------------------------------------------
if [ "${AUTO_LAUNCHED}" = "yes" -o "${JUST_MAKE}" != "" ]; then
    exit 0
fi

#---------------------------------------------------------------
# Part 6: Launch uMAC until the mission is quit
#---------------------------------------------------------------
uMAC targ_shoreside.moos
kill -- -$$

