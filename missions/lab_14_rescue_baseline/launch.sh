#!/bin/bash 
#------------------------------------------------------------ 
#   Script: launch.sh    
#   Author: Michael Benjamin   
#   LastEd: March 2025
#------------------------------------------------------------
#  Part 1: Set convenience functions for producing terminal
#          debugging output, and catching SIGINT (ctrl-c).
#------------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }
on_exit() { echo; echo "$ME: Halting all apps"; kill -- -$$; }
trap on_exit SIGINT

#------------------------------------------------------------
#  Part 2: Set global variable default values
#------------------------------------------------------------
ME=`basename "$0"`
CMD_ARGS=""
TIME_WARP=1
VERBOSE=""
JUST_MAKE=""
LOG_CLEAN=""
VAMT="1"
MAX_VAMT="4"
RAND_VPOS=""
MAX_SPD="2"
MMOD=""

# Monte
XLAUNCHED="no"
NOGUI=""

# Custom: num vehicles/teams
GAME_FORMAT="r1"
COMPETE=""

# Custom: on-the-fly swimfile gen
RAND_SWIMMERS=""
SWIM_REGION=""
SWIMMERS=""
UNREGERS=""

# Custom: The main input file
SWIM_FILE=""

# Custom: Max competition time
MAX_TIME=""

#-------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    CMD_ARGS+="${ARGI} "
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME: [OPTIONS] [time_warp]                     "
	echo "                                               "
	echo "Options:                                       "
	echo "  --help, -h         Show this help message    " 
	echo "  --verbose, -v      Verbose, confirm launch   "
	echo "  --just_make, -j    Only create targ files    " 
	echo "  --log_clean, -lc   Run clean.sh bef launch   " 
	echo "  --amt=N            Num vehicles to launch    "
	echo "  --rand, -r         Rand vehicle positions    "
	echo "  --max_spd=N        Max helm/sim speed        "
        echo "  --mmod=<mod>       Mission variation/mod     "
	echo "                                               "
	echo "Options (monte):                               "
	echo "  --xlaunched, -x    Launched by xlaunch       "
	echo "  --nogui, -ng       Headless launch, no gui   "
	echo "                                               "
	echo "Options (custom: type of competition):         "
	echo "  --r1, -r1          1 rescue vehicle          " 
	echo "  --r2, -r2          2 rescue vehicles         " 
	echo "  --rs1, -rs1        1 rescue 1 scout          " 
	echo "  --rs2, -rs2        2 teams, resc/scout each  " 
	echo "  --compete, -c      Competition               "
	echo "                                               "
	echo "Options (custom: dynamic swim file):           "
	echo "  --rsl, -rsl        Rand swim locations       " 
	echo "  --pav60, -pav60    Gen rand swimmers in pav60" 
	echo "  --pav90, -pav90    Gen rand swimmers in pav90" 
	echo "  --swimmers=<15>    Rand gen N reg swimmers   " 
	echo "  --unreg=<0>        Rand gen N unreg swimmers " 
	echo "                                               "
	echo "Options (custom: selection of swim file):      "
	echo "  --swim_file=<file> Set the swim file         " 
	echo "  -1 :  Short for --swim_file=mit_01.txt       "
	echo "  -2 :  Short for --swim_file=mit_02.txt       "
	echo "  -3 :  Short for --swim_file=mit_03.txt       "
	echo "  -4 :  Short for --swim_file=mit_04.txt       "
	echo "  -5 :  Short for --swim_file=mit_05.txt       "
	echo "  -6 :  Short for --swim_file=mit_06.txt       "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ]; then
	JUST_MAKE=$ARGI
    elif [ "${ARGI}" = "--log_clean" -o "${ARGI}" = "-lc" ]; then
	LOG_CLEAN=$ARGI
    elif [ "${ARGI:0:6}" = "--amt=" ]; then
        VAMT="${ARGI#--amt=*}"
	if [ $VAMT -lt 1 -o $VAMT -gt $MAX_VAMT ]; then
	    echo "$ME: Veh amt range: [1, $MAX_VAMT]. Exit Code 2."
	    exit 2
	fi
    elif [ "${ARGI}" = "--rand" -o "${ARGI}" = "-r" ]; then
        RAND_VPOS=$ARGI
    elif [ "${ARGI:0:10}" = "--max_spd=" ]; then
        MAX_SPD="${ARGI#--max_spd=*}"
    elif [ "${ARGI:0:7}" = "--mmod=" ]; then
        MMOD=$ARGI

    elif [ "${ARGI}" = "--xlaunched" -o "${ARGI}" = "-x" ]; then
	XLAUNCHED="yes"
    elif [ "${ARGI}" = "--nogui" -o "${ARGI}" = "-ng" ]; then
	NOGUI="--nogui"
    elif [ "${ARGI}" = "--compete" -o "${ARGI}" = "-c" ]; then
	COMPETE=$ARGI


    elif [ "${ARGI}" = "--r1" -o "${ARGI}" = "-r1" ]; then
	GAME_FORMAT="r1"
    elif [ "${ARGI}" = "--r2" -o "${ARGI}" = "-r2" ]; then
	GAME_FORMAT="r2"
	VAMT="2"
    elif [ "${ARGI}" = "--rs1" -o "${ARGI}" = "-rs1" ]; then
	GAME_FORMAT="rs1"
	VAMT="2"
    elif [ "${ARGI}" = "--rs2" -o "${ARGI}" = "-rs2" ]; then
	GAME_FORMAT="rs2"
	VAMT="4"

    elif [ "${ARGI}" = "--rsl" -o "${ARGI}" = "-rsl" ]; then
	RAND_SWIMMERS=" ${ARGI}"
    elif [ "${ARGI}" = "--pav60" -o "${ARGI}" = "-pav60" ]; then
	SWIM_REGION=$ARGI
    elif [ "${ARGI}" = "--pav90" -o "${ARGI}" = "-pav90" ]; then
	SWIM_REGION=$ARGI
    elif [ "${ARGI:0:11}" = "--swimmers=" ]; then
        SWIMMERS=$ARGI
    elif [ "${ARGI:0:8}" = "--unreg=" ]; then
        UNREGERS=$ARGI

    elif [ "${ARGI:0:12}" = "--swim_file=" ]; then
        SWIM_FILE=" ${ARGI}"
    elif [ "${ARGI}" = "-1" -o "${ARGI}" = "-2" ]; then
        SWIM_FILE=" ${ARGI}"
    elif [ "${ARGI}" = "-3" -o "${ARGI}" = "-4" ]; then
        SWIM_FILE=" ${ARGI}"
    elif [ "${ARGI}" = "-5" -o "${ARGI}" = "-6" ]; then
        SWIM_FILE=" ${ARGI}"

    elif [ "${ARGI:0:11}" = "--max_time=" ]; then
        MAX_TIME=" ${ARGI}"

    else
	echo "$ME: Bad arg:" $ARGI "Exit Code 1."
        exit 1
    fi
done

#------------------------------------------------------------
#  Part 4: Set starting positions, speeds, vnames, colors
#------------------------------------------------------------
INIT_VARS=" --amt=$VAMT $RAND_VPOS $VERBOSE $RAND_SWIMMERS"
INIT_VARS+=" --format=$GAME_FORMAT $SWIM_REGION $SWIMMERS $UNREGERS "
./init_field.sh $INIT_VARS

VEHPOS=(`cat vpositions.txt`)
SPEEDS=(`cat vspeeds.txt`)
VNAMES=(`cat vnames.txt`)
VCOLOR=(`cat vcolors.txt`)
VROLES=(`cat vroles.txt`)  #custom
VMATES=(`cat vmates.txt`)  #custom
VAPPS=(`cat vapps.txt`)  #custom

VAMT=${#VROLES[@]}

# If a newly random swim_file was created, but the name was
# not specified, use the default name, "mit_rand.txt"
RAND_SWIM_FILE_MADE=""
if [ "${RAND_SWIMMERS}" != "" -o "${SWIMMERS}" != "" ]; then
    RAND_SWIM_FILE_MADE="yes"
elif [ "${SWIM_REGION}" != "" -o "${UNREGERS}" != "" ]; then
    RAND_SWIM_FILE_MADE="yes"
fi

if [ "${RAND_SWIM_FILE_MADE}" = "yes" -a "${SWIM_FILE}" = "" ]; then
    SWIM_FILE="--swim_file=mit_rand.txt"
fi

#------------------------------------------------------------
#  Part 5: If verbose, show vars and confirm before launching
#------------------------------------------------------------
if [ "${VERBOSE}" != "" ]; then
    echo "============================================"
    echo "  $ME SUMMARY                   (ALL)       "
    echo "============================================"
    echo "CMD_ARGS =      [${CMD_ARGS}]               "
    echo "TIME_WARP =     [${TIME_WARP}]              "
    echo "JUST_MAKE =     [${JUST_MAKE}]              "
    echo "LOG_CLEAN =     [${LOG_CLEAN}]              "
    echo "VAMT =          [${VAMT}]                   "
    echo "MAX_VAMT =      [${MAX_VAMT}]               "
    echo "RAND_VPOS =     [${RAND_VPOS}]              "
    echo "MAX_SPD =       [${MAX_SPD}]                "
    echo "MMOD =          [${MMOD}]                   "
    echo "--------------------------------(VProps)----"
    echo "VNAMES =        [${VNAMES[*]}]              "
    echo "VCOLORS =       [${VCOLOR[*]}]              "
    echo "START_POS =     [${VEHPOS[*]}]              "
    echo "--------------------------------(Monte)-----"
    echo "XLAUNCHED =     [${XLAUNCHED}]              "
    echo "NOGUI =         [${NOGUI}]                  "
    echo "--------------------------------(Custom)----"
    echo "GAME_FORMAT     [${GAME_FORMAT}]            "
    echo "COMPETE         [${COMPETE}]                "
    echo "MAX_TIME =      [${MAX_TIME}]               "
    echo "VROLES =        [${VROLES[*]}]              "
    echo "VMATES =        [${VMATES[*]}]              "
    echo "VAPPS =         [${VAPPS[*]}]               "
    echo "--------------------------------(Custom)----"
    echo "SWIM_REGION     [${SWIM_REGION}]            "
    echo "SWIM_FILE       [${SWIM_FILE}]              "
    echo "RAND_SWIMMERS   [${RAND_SWIMMERS}]          "
    echo "SWIMMERS        [${SWIMMERS}]               "
    echo "UNREGERS        [${UNREGERS}]               "
    echo -n "Hit any key to continue launch           "
    read ANSWER
fi

#-------------------------------------------------------------
# Part 6: Launch the vehicles
#-------------------------------------------------------------
VARGS=" --sim --auto --max_spd=$MAX_SPD $MMOD "
VARGS+=" $TIME_WARP $JUST_MAKE $VERBOSE "
for IX in `seq 1 $VAMT`;
do
    IXX=$(($IX - 1))
    IVARGS="$VARGS --mport=900${IX}  --pshare=920${IX} "
    IVARGS+=" --start_pos=${VEHPOS[$IXX]} "
    IVARGS+=" --stock_spd=${SPEEDS[$IXX]} "
    IVARGS+=" --vname=${VNAMES[$IXX]} "
    IVARGS+=" --color=${VCOLOR[$IXX]} "
    IVARGS+=" --vrole=${VROLES[$IXX]} "
    IVARGS+=" --tmate=${VMATES[$IXX]} "

    if [ "${COMPETE}" != "" ]; then
	VAPP="${VAPPS[$IXX]}"
	IVARGS+=" --pgr=${VAPP} "
	vecho "VAPP:[${VAPP}]"
	AWK6=`echo $VAPP | awk -F '/' '{print $6}'`
	AWK7=`echo $VAPP | awk -F '/' '{print $7}'`

	VUSER=`echo $AWK6 | awk -F '-' '{print $4}'`

	if [ "${VUSER}" = "" ]; then
	    VUSER=`echo $AWK7 | awk -F '-' '{print $4}'`
	fi
	vecho "AWK6:[${AWK6}]"
	vecho "AWK7:[${AWK7}]"
	vecho "VUSER:[${VUSER}]"
	IVARGS+=" --vuser=${VUSER} "
    fi

    vecho "Launching vehicle: $IVARGS"

    CMD="./launch_vehicle.sh $IVARGS"    
    eval $CMD
    sleep 0.5
done



#------------------------------------------------------------
#  Part 7: Launch the Shoreside mission file
#------------------------------------------------------------
SARGS=" --auto --mport=9000 --pshare=9200 $NOGUI "
SARGS+=" $TIME_WARP $JUST_MAKE $VERBOSE "
SARGS+=" $MMOD "
SARGS+=" $MAX_TIME $SWIM_FILE"
if [ "${XLAUNCHED}" = "yes" ]; then
    SARGS+=" --auto"
fi
vecho "Launching shoreside: $SARGS"
./launch_shoreside.sh $SARGS 

if [ "${JUST_MAKE}" != "" ]; then
    echo "$ME: Targ files made; exiting without launch."
    exit 0
fi

#------------------------------------------------------------
#  Part 8: Unless auto-launched, launch uMAC until mission quit
#------------------------------------------------------------
if [ "${XLAUNCHED}" != "yes" ]; then
    uMAC --paused targ_shoreside.moos
    trap "" SIGINT
    echo; echo "$ME: Halting all apps"
    kill -- -$$
fi

exit 0


