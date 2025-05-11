#!/bin/bash 
#-------------------------------------------------------
#   Script: get_scout.sh                                    
#   Author: Michael Benjamin  
#     Date: May 2025     
#-------------------------------------------------------
#  Part 1: Declare global var defaults
#-------------------------------------------------------
VAPP=""

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
	echo "get_scout.sh [SWITCHES]                           "
	echo "                                                  " 
	echo "Synopsis                                          " 
	echo "  Provisional script to determine the location of "
	echo "  BHV_Scout behavior based on the pGenRescue user " 
	exit 0;	
    elif [ "${VUSER}" = "" ]; then
	VAPP=$ARGI
    else 
	echo "get_scout.sh: Bad Arg:[$ARGI]. Exit Code 1."
	exit 1
    fi
done

#-------------------------------------------------------
#  Part 2: Do the cleaning!
#-------------------------------------------------------
ROOT_DIR="/Users/mikerb/Research/autotest/harnesses_2680"

if [[ "${VAPP}" == *"cdembski"* ]]; then
    echo "$ROOT_DIR/09-moos-ivp-mcschwar/lib/"

elif [[ "${VAPP}" == *"jwenger"* ]]; then
    echo "$ROOT_DIR/13-moos-ivp-jcbrandt/lib/"

elif [[ "${VAPP}" == *"chthigpen"* ]]; then
    echo "$ROOT_DIR/19-moos-ivp-hallsc83/lib/"

elif [[ "${VAPP}" == *"jcizu690"* ]]; then
    echo "$ROOT_DIR/16-moos-ivp-harkm/lib/"

elif [[ "${VAPP}" == *"thoma904"* ]]; then
    echo "$ROOT_DIR/05-moos-ivp-dmgilbe2/lib/"

elif [[ "${VAPP}" == *"bruno752"* ]]; then
    echo "$ROOT_DIR/18-moos-ivp-egajski/lib/"

elif [[ "${VAPP}" == *"cschnorr"* ]]; then
    echo "$ROOT_DIR/06-moos-ivp-cschnorr/lib/"

elif [[ "${VAPP}" == *"ews2011"* ]]; then
    echo "$ROOT_DIR/02-moos-ivp-ews2011/lib/"

elif [[ "${VAPP}" == *"bertucci"* ]]; then
    echo "$ROOT_DIR/01-moos-ivp-rjkang17/lib/"

elif [[ "${VAPP}" == *"ekwang"* ]]; then
    echo "$ROOT_DIR/07-moos-ivp-adamc710/lib/"
    
fi
