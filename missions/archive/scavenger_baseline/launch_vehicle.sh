#!/bin/bash 
#-------------------------------------------------------------- 
#   Script: launch_vehicle.sh    
#  Mission: ufld_encircle
#   Author: Michael Benjamin   
#   LastEd: January 2021
#-------------------------------------------------------------- 
#  Part 1: Define a convenience function for producing terminal
#          debugging/status output depending on the verbosity.
#-------------------------------------------------------------- 
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#-------------------------------------------------------------- 
#  Part 2: Set Global variables
#-------------------------------------------------------------- 
ME=`basename "$0"`
TIME_WARP=1
JUST_MAKE="no"
VERBOSE="no"
CONFIRM="no"
AUTO_LAUNCHED="no"
CMD_ARGS=""

IP_ADDR="localhost"
MOOS_PORT="9001"
PSHARE_PORT="9201"

SHORE_IP="localhost"
SHORE_PSHARE="9200"
VNAME="who"
INDEX="1"

#SHOREIP="localhost"
#SHORE_PSHARE="9300"

#VNAME="who"
START_POS="0,0"  
START_SPD="0.0"  

LOG_CLEAN="no"
CLOCK="false"
MAXSPD="5"
MTASC=""

#-------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    CMD_ARGS+=" ${ARGI}"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "$ME [OPTIONS] [time_warp]                        "
	echo "                                                 " 
	echo "Options:                                         "
	echo "  --help, -h                                     " 
	echo "    Print this help message and exit             "
	echo "  --just_make, -j                                " 
	echo "    Just make targ files, but do not launch      "
	echo "  --verbose, -v                                  " 
	echo "    Verbose output, confirm before launching     "
	echo "  --noconfirm, -nc                               " 
	echo "    No confirmation before launching             "
        echo "  --auto, -a                                     "
        echo "     Auto-launched by a script.                  "
        echo "     Will not launch uMAC as the final step.     "
	echo "                                                 "
	echo "  --ip=<localhost>                               " 
	echo "    Force pHostInfo to use this IP Address       "
	echo "  --mport=<9001>                                 "
	echo "    Port number of this vehicle's MOOSDB port    "
	echo "  --pshare=<9201>                                " 
	echo "    Port number of this vehicle's pShare port    "
	echo "                                                 "
	echo "  --shore=<localhost>                            " 
	echo "    IP address location of shoreside             "
	echo "  --vname=<who>                                  " 
	echo "    Name of the vehicle being launched           " 
	echo "  --index=<1>                                    " 
	echo "    Index for setting MOOSDB and pShare ports    "

	echo "  --maxspd=<m/s>    (default is 5)               "
        echo "    Max speed capability of the vehicle, passed  "
        echo "    to uSimMarine for setting its thrust map     "
	echo "  --start=<X,Y>     (default is 0,0)             " 
	echo "    Start position chosen by script launching    "
	echo "    this script (to ensure separation)           "
	echo "  --spd=<spd>       (default is 0)               " 
	echo "    Initial speed of vehicle being launched      " 

	echo "  --clock=true/false                                    " 
	echo "  --maxspd=meters/sec                                   " 
	echo "    The max speed capability of the vehicle, passed to  "
	echo "    uSimMarine for setting its thrust map               "
	echo "  --logclean, -l                                        " 
        echo "    Clean (remove) all log files prior to launch        "
        echo "  --mtasc, -m                                           "
        echo "     Mission is being remotely launched (e.g. MTASC)    "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE="yes"
        CONFIRM="yes"
    elif [ "${ARGI}" = "--noconfirm" -o "${ARGI}" = "-nc" ]; then
	CONFIRM="no"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        AUTO_LAUNCHED="yes" 

    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:7}" = "--mport" ]; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"

    elif [ "${ARGI:0:8}" = "--shore=" ]; then
        SHORE_IP="${ARGI#--shore=*}"
    elif [ "${ARGI:0:8}" = "--vname=" ]; then
        VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI:0:8}" = "--index=" ]; then
        INDEX="${ARGI#--index=*}"
	
    elif [ "${ARGI:0:9}" = "--maxspd=" ]; then
        MAXSPD="${ARGI#--maxspd=*}"
    elif [ "${ARGI:0:8}" = "--start=" ]; then
        START_POS="${ARGI#--start=*}"
    elif [ "${ARGI:0:6}" = "--spd=" ]; then
        START_SPD="${ARGI#--spd=*}"

    elif [ "${ARGI}" = "--logclean" -o "${ARGI}" = "-l" ]; then
	LOG_CLEAN="yes"
    elif [ "${ARGI}" = "--mtasc" -o "${ARGI}" = "-m" ]; then
	MTASC="yes"
    elif [ "${ARGI}" = "--clock" -o "${ARGI}" = "-c" ]; then
	CLOCK="true"

    else 
	echo "$ME: Bad Arg:[$ARGI]. Exit Code 1."
	exit 1
    fi
done

MOOS_PORT=`expr $INDEX + 9000`
PSHARE_PORT=`expr $INDEX + 9200`
     
#---------------------------------------------------------------
#  Part 4: If verbose, show vars and confirm before launching
#---------------------------------------------------------------
if [ "${VERBOSE}" = "yes" -o "${CONFIRM}" = "yes" ]; then 
    echo "$ME"
    echo "CMD_ARGS =      [${CMD_ARGS}]"
    echo "TIME_WARP =     [${TIME_WARP}]"
    echo "AUTO_LAUNCHED = [${AUTO_LAUNCHED}]"
    echo "----------------------------------"
    echo "MOOS_PORT =     [${MOOS_PORT}]"
    echo "PSHARE_PORT =   [${PSHARE_PORT}]"
    echo "IP_ADDR =       [${IP_ADDR}]"
    echo "----------------------------------"
    echo "SHORE_IP =      [${SHORE_IP}]"
    echo "SHORE_PSHARE =  [${SHORE_PSHARE}]"
    echo "VNAME =         [${VNAME}]"
    echo "INDEX =         [${INDEX}]"
    echo "----------------------------------"
    echo "MAXSPD =        [${MAXSPD}]"
    echo "START_POS =     [${START_POS}]"
    echo "START_SPD =     [${START_SPD}]"
    echo "----------------------------------"
    echo "LOG_CLEAN =     [${LOG_CLEAN}]"
    echo "MTASC =         [${MTASC}]"
    echo "CLOCK =         [${CLOCK}]"
    echo -n "Hit any key to continue with launching"
    read ANSWER
fi


#-------------------------------------------------------
#  Part 5: If Log clean before launch, do it now. 
#          In MTASC missions, remote cleaning is essential.
#-------------------------------------------------------
if [ "$LOG_CLEAN" = "yes" ]; then
    vecho "Cleaning local Log Files"
    rm -rf LOG* XLOG* MOOSLog*
fi

#-------------------------------------------------------
#  Part 6: Create the .moos and .bhv files. 
#-------------------------------------------------------
# interactive nsplug only when not being remotely launched.
NSARGS="--force"
if [ "${MTASC}" = "" ]; then
    NSARGS+=" --interactive"
fi

nsplug meta_vehicle.moos targ_$VNAME.moos $NSARGS WARP=$TIME_WARP  \
       PSHARE_PORT=$PSHARE_PORT     VNAME=$VNAME                   \
       START_POS=$START_POS         SHORE_IP=$SHORE_IP             \
       SHORE_PSHARE=$SHORE_PSHARE   MOOS_PORT=$MOOS_PORT           \
       IP_ADDR=$IP_ADDR             MAXSPD=$MAXSPD                 \
       START_SPD=$START_SPD
       
nsplug meta_vehicle.bhv targ_$VNAME.bhv $NSARGS  \
       START_POS=$START_POS   VNAME=$VNAME       \
       START_SPD=$START_SPD   CLOCKWISE=$CLOCK


if [ ${JUST_MAKE} = "yes" ]; then
    echo "Files assembled; nothing launched; exiting per request."
    exit 0
fi

#-------------------------------------------------------
#  Part 7: Launch the vehicle mission
#-------------------------------------------------------
vecho "Launching $VNAME MOOS Community. WARP="$TIME_WARP
pAntler targ_$VNAME.moos >& /dev/null &
vecho "Done Launching $VNAME MOOS Community"

#---------------------------------------------------------------
#  Part 8: If launched from script, we're done, exit now
#---------------------------------------------------------------
if [ "${AUTO_LAUNCHED}" = "yes" ]; then
    exit 0
fi

#---------------------------------------------------------------
# Part 9: Launch uMAC until the mission is quit
#---------------------------------------------------------------
uMAC targ_$VNAME.moos
kill -- -$$
