#!/bin/bash 
#-------------------------------------------------------------- 
#   Script: launch.sh    
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
JUST_MAKE=""
VERBOSE=""
AUTO_LAUNCHED="no"
CMD_ARGS=""

RANDSTART="true"
SHORE_PSHARE="9300"
SHORE_IP="localhost"
AMT=1

MISSION="ufld_encircle"

MTASC="no"
MTASC_SUBNET="192.168.7"
USE_CACHE=""
VLAUNCH_ARGS="--auto "
SLAUNCH_ARGS="--auto "

#-------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME: [OPTIONS] [time_warp]                              "
	echo "                                                        "
        echo "Synopsis:                                               "
	echo "  A script for launching an arbitrary number of vehicles"
	echo "  either all in simulation on this machine, or with the "
	echo "  --mtasc flag, launched across multiple machines in the"
	echo "  same subnet.                                          "
	echo "                                                        "
	echo "Options:                                                "
        echo "  --help, -h                                            "
        echo "    Display this help message                           "
        echo "  --verbose, -v                                         "
        echo "    Increase verbosity                                  "
	echo "  --just_make, -j                                       " 
	echo "    Just make the targ files, but do not launch.        " 
	echo "  --amt=N                                               " 
	echo "    The number of vehicles to launch. The default is 1. "
	echo "  --norand                                              " 
	echo "    Do not randomly generate files vpositions.txt,      "
	echo "    vspeeds.txt. Just re-use the previous versions if   "
	echo "    they exist.                                         "
	echo "  --clock, -c                                           " 
	echo "    Set the encircle direction to clockwise.            "
	echo "                                                        "
	echo "Options For launches on MTASC machines:                 "
	echo "                                                        "
	echo "  --mtasc, -m                                           " 
	echo "    Launch vehicles in the MTASC cluster                " 
	echo "  --shore=<ipaddr>                                      " 
	echo "    IP address where nodes can expect to find shoreside "
	echo "  --cache, -c                                           " 
	echo "    Dont confirm IP address of a pablo before launching "
	echo "    a mission. Instead, rely on the ~/.pablos_ipfs file "
	echo "    for a cached mapping of pablo names to IP addresses."
	echo "    Without this flag, the pablo name/IP will first be  "
	echo "    confirmed by contacting the pablo. Longer to launch."
	echo "  --logclean, -l                                        " 
        echo "    Clean (remove) all log files prior to launch        "
	exit 0;
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE="--verbose"
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE="-j"
    elif [ "${ARGI}" = "--logclean" -o "${ARGI}" = "-l" ]; then
	VLAUNCH_ARGS+=" $ARGI"
    elif [ "${ARGI}" = "--norand" -o "${ARGI}" = "-r" ]; then
	RANDSTART="false"
    elif [ "${ARGI}" = "--mtasc" -o "${ARGI}" = "-m" ]; then
	MTASC="yes"
    elif [ "${ARGI:0:8}" = "--shore=" ]; then
        SHORE_IP="${ARGI#--shore=*}"
    elif [ "${ARGI}" = "--clock" -o "${ARGI}" = "-c" ]; then
	VLAUNCH_ARGS=" $ARGI"
    elif [ "${ARGI}" = "--cache" -o "${ARGI}" = "-c" ]; then
        USE_CACHE="--cache"
    elif [ "${ARGI:0:6}" = "--amt=" ]; then
        AMT="${ARGI#--amt=*}"
	if [ ! $AMT -ge 1 ] ; then
	    echo "$ME: Vehicle amount must be >= 1. Exit Code 1."
	    exit 1
	fi
    else 
	echo "$ME: Bad Arg: $ARGI. Exit Code 1."
	exit 1
    fi
done

#-------------------------------------------------------------
# Part 4: Generate random starting positions, speeds and vnames
#-------------------------------------------------------------
vecho "Picking starting positions and speeds"

if [ ! -f "$HOME/.pablo_names" -a "${MTASC}" = "yes" ]; then
    echo "$ME: Could not find ~/.pablo_names. Exit Code 2."
    exit 2
fi

if [ "${RANDSTART}" = "true" -o  ! -f "vpositions.txt" ]; then
    pickpos --poly="-2,-8 : 4,-13 : 60,13 : 57,18"   \
	    --amt=$AMT --hdg=75,-100,0  > vpositions.txt  
fi
if [ "${RANDSTART}" = "true" -o  ! -f "vspeeds.txt" ]; then
    pickpos --amt=$AMT --spd=0:0.1 > vspeeds.txt
fi

# vehicle names are always deterministic in alphabetical order
pickpos --amt=$AMT --vnames  > vnames.txt

VEHPOS=(`cat vpositions.txt`)
SPEEDS=(`cat vspeeds.txt`)
VNAMES=(`cat vnames.txt`)
PABLOS=(`cat ~/.pablo_names`)  # only need in mtasc mode

#-------------------------------------------------------------
# Part 5: In MTASC mode, the shore IP address must be: 
#         (1) a non-localhost IP address. 
#         (2) a currently active IP interface for this machine
#         (3) an IP address on the local MTASC network
#-------------------------------------------------------------
if [ "$MTASC" = "yes" ]; then
    vecho "Verifying Shoreside IP Address for MTASC mode"

    ADDR=""
    if [ "$SHORE_IP" = "localhost" ]; then
        ADDR=`ipmatch.sh --match=$MTASC_SUBNET`
    else
	ADDR=`ipmatch.sh --match=$SHORE_IP`
    fi
    
    
    if [ "$ADDR" != "" ]; then
	SHORE_IP="$ADDR"
    else
	echo "Provide active Shore IP addr on MTASC network. Exit Code 3."
	exit 3
    fi

    verify_ssh_key.sh --pablo
fi

vecho "SHORE IPAddress: $SHORE_IP"

#-------------------------------------------------------------
# Part 6: Launch the vehicles
#-------------------------------------------------------------
for INDEX in `seq 1 $AMT`;
do
    sleep 0.1
    ARRAY_INDEX=`expr $INDEX - 1`

    START=${VEHPOS[$ARRAY_INDEX]}
    VNAME=${VNAMES[$ARRAY_INDEX]}
    SPEED=${SPEEDS[$ARRAY_INDEX]}
    SPEED="${SPEED#speed=*}"

    IX_VLAUNCH_ARGS=$VLAUNCH_ARGS
    IX_VLAUNCH_ARGS+=" --index=$INDEX --start=$START "
    IX_VLAUNCH_ARGS+=" --vname=$VNAME --shore=$SHORE_IP "
    IX_VLAUNCH_ARGS+=" --spd=$SPEED " 
    IX_VLAUNCH_ARGS+=" $TIME_WARP $VERBOSE $JUST_MAKE"

    vecho "Launching: $VNAME"
    vecho "IX_VLAUNCH_ARGS: [$IX_VLAUNCH_ARGS]"

    if [ "${MTASC}" != "yes" ]; then
	./launch_vehicle.sh $IX_VLAUNCH_ARGS
    else
	SSH_OPTIONS="-o StrictHostKeyChecking=no -o LogLevel=QUIET "
	SSH_OPTIONS+="-o ConnectTimeout=1 -o ConnectionAttempts=1 "
	SSH_OPTIONS+="-o BatchMode=yes "
	
	PNAME=${PABLOS[$ARRAY_INDEX]}
	IP=`find_pablo_ip_by_name.sh $USE_CACHE $PNAME | tr -d '\r' `
	
	vecho "pablo[$PNAME] IP is: $IP"
	IX_VLAUNCH_ARGS+=" --mission=$MISSION"
	ssh student2680@$IP $SSH_OPTIONS mlaunch.sh $IX_VLAUNCH_ARGS &
    fi
done

#-------------------------------------------------------------
# Part 7: Launch the Shoreside mission file
#-------------------------------------------------------------
vecho "Launching the shoreside. Args: $SLAUNCH_ARGS $TIME_WARP"

SLAUNCH_ARGS=" --auto $JUST_MAKE"
./launch_shoreside.sh $SLAUNCH_ARGS $VERBOSE $TIME_WARP 


#---------------------------------------------------------------
#  Part 8: If launched from script, we're done, exit now
#---------------------------------------------------------------
if [ "${AUTO_LAUNCHED}" = "yes" -o "${JUST_MAKE}" != "" ]; then
    exit 0
fi

#-------------------------------------------------------------
# Part 9: Launch uMac until the mission is quit
#-------------------------------------------------------------
uMAC --paused targ_shoreside.moos

kill -- -$$

