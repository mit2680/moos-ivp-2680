#!/bin/bash 
#------------------------------------------------------------
#   Script: init_field.sh
#   Author: M.Benjamin
#   LastEd: March 2025
#------------------------------------------------------------
#  Part 1: A convenience function for producing terminal
#          debugging/status output depending on verbosity.
#------------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }

#------------------------------------------------------------
#  Part 2: Set global variable default values
#------------------------------------------------------------
ME=`basename "$0"`
VEHICLE_AMT="1"
VERBOSE=""
RAND_VPOS="no"

# custom
RAND_SWIMMERS=""
GAME_FORMAT="-r1"
SWIMMERS=15
UNREGERS=0
SWIM_FILE="mit_00.txt"
SWIM_REGION="60,10:-30.36,-32.84:-4.66,-87.05:85.7,-44.22"

#------------------------------------------------------------
#  Part 3: Check for and handle command-line arguments
#------------------------------------------------------------
for ARGI; do
    CMD_ARGS+=" ${ARGI}"
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME [OPTIONS] [time_warp]                      "
	echo "                                               "
	echo "Options:                                       "
	echo "  --amt=N            Num vehicles to launch    "
	echo "  --verbose, -v      Verbose, confirm values   "
	echo "  --rand, -r         Rand vehicle positions    "
	echo "                                               "
	echo "Options (custom):                              "
	echo "  --randswim, -rsl   Rand swim locations       " 
	echo "  --format<format>   Game format r1,r2,rs1,rs2 " 
	echo "                                               "
	echo "  --pav60            Gen rand swimmers in pav60" 
	echo "  --pav90            Gen rand swimmers in pav90" 
	echo "  --swim_file=<file> Set the swim file         " 
	echo "  --swimmers=<15>    Rand gen N reg swimmers   " 
	echo "  --unreg=<0>        Rand gen N unreg swimmers " 
	exit 0;
    elif [ "${ARGI:0:6}" = "--amt=" ]; then
        VEHICLE_AMT="${ARGI#--amt=*}"
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ]; then
	VERBOSE=$ARGI
    elif [ "${ARGI}" = "--rand" -o "${ARGI}" = "-r" ]; then
        RAND_VPOS="yes"

    elif [ "${ARGI}" = "--randswim" -o "${ARGI}" = "-rsl" ]; then
	RAND_SWIMMERS="true"
    elif [ "${ARGI:0:9}" = "--format=" ]; then
        GAME_FORMAT="${ARGI#--format=*}"

    elif [ "${ARGI}" = "--mit_small" -o "${ARGI}" = "--pav60" ]; then
	SWIM_REGION="60,10:-30.36,-32.84:-4.66,-87.05:85.70,-44.22"
	RAND_SWIMMERS="true"
    elif [ "${ARGI}" = "--mit_big" -o "${ARGI}" = "--pav90" ]; then
	SWIM_REGION="60,10:-75.54,-54.26:-36.99,-135.58:98.55,-71.32"
	RAND_SWIMMERS="true"
    elif [ "${ARGI:0:11}" = "--swimmers=" ]; then
        SWIMMERS="${ARGI#--swimmers=*}"
	RAND_SWIMMERS="true"
    elif [ "${ARGI:0:8}" = "--unreg=" ]; then
        UNREGERS="${ARGI#--unreg=*}"
	RAND_SWIMMERS="true"

    else 
	echo "$ME: Bad Arg: $ARGI. Exit Code 1."
	exit 1
    fi
done

#------------------------------------------------------------
#  Part 4: Source local coordinate grid if it exits
#------------------------------------------------------------

#------------------------------------------------------------
#  Part 5: Set starting positions, speeds, vnames, colors
#------------------------------------------------------------
vecho "Setting starting position, speeds, vnames, colors"

if [ "${RAND_VPOS}" = "yes" -o  ! -f "vpositions.txt" ]; then
    pickpos --poly="-2,-8 : 4,-13 : 60,13 : 57,18" --buffer=15 \
            --amt=$VEHICLE_AMT --hdg="170:190" > vpositions.txt
fi

# generate randomly placed swimmers
if [ "${RAND_SWIMMERS}" != "" -o  ! -f "mit_00.txt" ]; then
    gen_swimmers --poly=$SWIM_REGION --swimmers=$SWIMMERS   \
                 --unreg=$UNREGERS --sep=7 > mit_00.txt
fi

# Set the speeds and names
pickpos --amt=$VEHICLE_AMT --spd=1.2:1.2 > vspeeds.txt 
pickpos --amt=$VEHICLE_AMT --vnames  > vnames.txt

# Handle the chosen game format
if [ "${GAME_FORMAT}" = "r2" ]; then
    echo -e "rescue\nrescue" > vroles.txt
    echo -e "abe\nabe"       > vmates.txt
    echo -e "green\nblue"    > vcolors.txt
elif [ "${GAME_FORMAT}" = "rs1" ]; then
    echo -e "rescue\nscout" > vroles.txt
    echo -e "abe\nabe"      > vmates.txt
    echo -e "blue\nblue"    > vcolors.txt
elif [ "${GAME_FORMAT}" = "rs2" ]; then
    echo -e "rescue\nrescue\nscout\nscout" > vroles.txt
    echo -e "abe\nben\nabe\nben"           > vmates.txt
    echo -e "green\nblue\ngreen\nblue"     > vcolors.txt
else # format=r1
    echo "rescue" > vroles.txt
    echo "abe"    > vmates.txt
    echo "green"  > vcolors.txt
fi


#------------------------------------------------------------
#  Part 6: Set other aspects of the field, e.g., obstacles
#------------------------------------------------------------

#------------------------------------------------------------
#  Part 7: If verbose, show file contents
#------------------------------------------------------------
if [ "${VERBOSE}" != "" ]; then
    echo "--------------------------------------"
    echo "VEHICLE_AMT = $VEHICLE_AMT"
    echo "RAND_VPOS   = $RAND_VPOS"
    echo "--------------------------------------(pos/spd)"
    echo "vpositions.txt:"; cat  vpositions.txt
    echo "vspeeds.txt:";    cat  vspeeds.txt
    echo "--------------------------------------(vprops)"
    echo "vnames.txt:";     cat  vnames.txt
    echo "vcolors.txt:";    cat  vcolors.txt
    echo -n "Hit any key to continue"
    read ANSWER
fi

exit 0
