#!/bin/bash 
#----------------------------------------------------------
#  Script: launch.sh
#  LastEd: Apr 11th 2021
#----------------------------------------------------------
#  Part 1: Set global var defaults
#----------------------------------------------------------
TIME_WARP=1
JUST_MAKE="no"
VNAME1="charlie"   
VNAME2="dana"      
START_POS1="0,0"         
START_POS2="25,0"        
LOITER_POS1="x=0,y=-75"
LOITER_POS2="x=125,y=-75"

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch.sh [SWITCHES] [time_warp] "
	echo "  --just_make, -j    " 
	echo "  --help, -h         " 
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_build" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    else 
	echo "launch.sh: Bad Arg:" $ARGI " Exit Code 1."
	exit 1
    fi
done

#-------------------------------------------------------
#  Part 3: Create the .moos and .bhv files. 
#-------------------------------------------------------
SHORE_LISTEN="9200"

nsplug meta_vehicle.moos targ_$VNAME1.moos -f WARP=$TIME_WARP \
       VNAME=$VNAME1          START_POS=$START_POS1           \
       MOOS_PORT="9001"       SHARE_LISTEN="9201"             \
       SHORE_PSHARE="9200"    SHORE_IP="localhost"

nsplug meta_vehicle.moos targ_$VNAME2.moos -f WARP=$TIME_WARP \
       VNAME=$VNAME2          START_POS=$START_POS2           \
       MOOS_PORT="9002"       SHARE_LISTEN="9202"             \
       SHORE_PSHARE="9200"    SHORE_IP="localhost"

nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
       SHARE_LISTEN="9200"  

nsplug meta_vehicle.bhv targ_$VNAME1.bhv -f VNAME=$VNAME1     \
       START_POS=$START_POS1 LOITER_POS=$LOITER_POS1       

nsplug meta_vehicle.bhv targ_$VNAME2.bhv -f VNAME=$VNAME2     \
       START_POS=$START_POS2 LOITER_POS=$LOITER_POS2       

if [ ${JUST_MAKE} = "yes" ] ; then
    echo "Files assembled; nothing launched; exiting per request."
    exit 0
fi

#-------------------------------------------------------
#  Part 3: Launch the processes
#-------------------------------------------------------
echo "Launching $SNAME MOOS Community. WARP is"  $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
echo "Launching $VNAME1 MOOS Community. WARP is" $TIME_WARP
pAntler targ_$VNAME1.moos >& /dev/null &
echo "Launching $VNAME2 MOOS Community. WARP is" $TIME_WARP
pAntler targ_$VNAME2.moos >& /dev/null &
echo "Done"

#-----------------------------------------------------------
#  Part 5: Launch uMAC and kill everything upon exiting uMAC
#-----------------------------------------------------------
uMAC targ_shoreside.moos

kill -- -$$
