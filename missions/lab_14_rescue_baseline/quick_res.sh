#!/bin/bash 
#-------------------------------------------------------
#   Script: get_scout.sh                                    
#   Author: Michael Benjamin  
#     Date: May 2025     
#-------------------------------------------------------
#  Part 1: Declare global var defaults
#-------------------------------------------------------
ME=`basename "$0"`

user_array=("cdembski" "cschnorr" "ews2011" )
user_array+=("jwenger"  "bertucci" "ekwang"  )
user_array+=("thoma904" "bruno752" "jcizu690")
user_array+=("chthigpen")

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "$ME.sh [SWITCHES]                           "
	echo "                                                  " 
	echo "Synopsis                                          " 
	echo "  Provisional script to determine the location of "
	echo "  BHV_Scout behavior based on the pGenRescue user " 
	exit 0;	
    elif [ "${VUSER}" = "" ]; then
	VAPP=$ARGI
    else 
	echo "$ME.sh: Bad Arg:[$ARGI]. Exit Code 1."
	exit 1
    fi
done

for user in "${user_array[@]}"
do
    MATCHES=`fgrep $user results.txt | wc -l`
    WINS=`fgrep winner=$user results.txt | wc -l`
    echo "$user $WINS $MATCHES"
done

