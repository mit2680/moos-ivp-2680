#!/bin/bash 
#-------------------------------------------------------
#   Script: launch_vehicle.sh                       
#  Mission: lab_08_baseline
#-------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
ME=`basename "$0"`
CMD_ARGS=""
TIME_WARP=1
VERBOSE=""
JUST_MAKE="no"
VNAME=$(id -un)

IP_ADDR="localhost"
MOOS_PORT="9001"
PSHARE_PORT="9201"
SHORE_PSHARE="9200"
SHORE_IP="localhost"

GUI="yes"
AUTO="no"

# Generate a random start position in range x=[0,180], y=[0,-50]
X_START_POS=$(($RANDOM % 180))
Y_START_POS=$((($RANDOM % 50)  - 50))
START_POS="$X_START_POS,$Y_START_POS" 

# Generate a random start position in range x=[0,180], y=[0,-125]
X_LOITER_POS=$(($RANDOM % 180))
Y_LOITER_POS=$((($RANDOM % 50) - 125))
LOITER_POS="x=$X_LOITER_POS,y=$Y_LOITER_POS" 

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    CMD_ARGS+=" ${ARGI}"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS] [time_warp]                        "
        echo "                                                 "
        echo "Options:                                         "
        echo "  --help, -h             Show this help message  "
        echo "  --just_make, -j        Only create targ files  "
        echo "  --verbose, -v          Verbose, confirm launch "
	echo "                                                 "
	echo "  --vname=<vname>                                " 
	echo "    Name of the vehicle being launched           " 
	echo "                                                 "
	echo "  --shore=<ipaddr>                               "
	echo "  --shoreip=<ipaddr>                             "
	echo "    IP address of the shoreside                  "
	echo "                                                 "
	echo "  --mport=<port>(9001)                           "
	echo "    Port number of this vehicle's MOOSDB port    "
	echo "  --pshare=<port>(9201)                          " 
	echo "    Port number of this vehicle's pShare port    "
	echo "  --ip=<ipaddr>                                  " 
	echo "    Force pHostInfo to use this IP Address       "
	echo "  --nogui                                        " 
	echo "    Do not launch pMarineViewer GUI with vehicle "
	echo "  --auto,-a                                      " 
	echo "    Exit after launching. Do not launch uMAC     "
	exit 0
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
        VERBOSE="yes"
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
	AUTO="yes"
    elif [ "${ARGI:0:8}" = "--shore=" ]; then
	SHORE_IP="${ARGI#--shore=*}"
    elif [ "${ARGI:0:10}" = "--shoreip=" ]; then
	SHORE_IP="${ARGI#--shoreip=*}"
    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        IP_ADDR="${ARGI#--ip=*}"
    elif [ "${ARGI:0:8}" = "--mport=" ]; then
	MOOS_PORT="${ARGI#--mport=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
	PSHARE_PORT="${ARGI#--pshare=*}"
    elif [ "${ARGI:0:8}" = "--vname=" ]; then
	VNAME="${ARGI#--vname=*}"
    elif [ "${ARGI}" = "--nogui" ]; then
	GUI="no"
    else
	echo "$ME: Bad Arg:" ${ARGI} " Exit Code 1"
	exit 1
    fi
done

# Set the FULL_NAME after possibly overriding VNAME in command args
FULL_VNAME=$VNAME

if [ "${VERBOSE}" = "yes" ]; then
    echo "============================================"
    echo "     launch_vehicle.sh SUMMARY        $VNAME"
    echo "============================================"
    echo "$ME                               "
    echo "CMD_ARGS =     [${CMD_ARGS}]      "
    echo "TIME_WARP =    [${TIME_WARP}]     "
    echo "JUST_MAKE =    [${JUST_MAKE}]     "
    echo "----------------------------------"
    echo "IP_ADDR =      [${IP_ADDR}]       "
    echo "MOOS_PORT =    [${MOOS_PORT}]     "
    echo "PSHARE_PORT =  [${PSHARE_PORT}]   "
    echo "SHORE_IP =     [${SHORE_IP}]      "
    echo "SHORE_PSHARE = [${SHORE_PSHARE}]  "
    echo "----------------------------------"
    echo "VNAME =        [${VNAME}]         "
    echo "GUI =          [${GUI}]           "
    echo "UMAC =         [${UMAC}]          "
    echo "FULL_NAME =    [${FULL_NAME}]     "
    echo "------------Custom----------------"
    echo "START_POS =    [${START_POS}]     "
    echo "LOITER_POS =   [${LOITER_POS}]    "
    echo -n "Hit any key to continue with launching"
    read ANSWER
fi

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------
nsplug meta_vehicle.moos targ_$FULL_VNAME.moos -f WARP=$TIME_WARP  \
    PSHARE_PORT=$PSHARE_PORT     VNAME=$FULL_VNAME                 \
    START_POS=$START_POS         SHORE_IP=$SHORE_IP                \
    SHORE_PSHARE=$SHORE_PSHARE   VPORT=$MOOS_PORT                  \
    IP_ADDR=$IP_ADDR             GUI=$GUI

nsplug meta_vehicle.bhv targ_$FULL_VNAME.bhv -f VNAME=$FULL_VNAME  \
    START_POS=$START_POS LOITER_POS=$LOITER_POS       
   
 
if [ ${JUST_MAKE} = "yes" ]; then
    exit 0
fi

#-------------------------------------------------------
#  Part 4: Launch the processes
#-------------------------------------------------------
printf "Launching $VNAME MOOS Community (WARP=%s) \n" $TIME_WARP
pAntler targ_$FULL_VNAME.moos >& /dev/null &

#-------------------------------------------------------
#  Part 5: If launched from script, we're done, exit now
#-------------------------------------------------------
if [ "${AUTO}" = "yes" ]; then
    exit 0
fi

uMAC targ_$FULL_VNAME.moos

#-------------------------------------------------------
#  Part 6: Exiting and/or killing the simulation
#-------------------------------------------------------
kill -- -$$
