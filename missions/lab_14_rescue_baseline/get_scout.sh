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

if [[ "${VAPP}" == *"o_sully"* ]]; then
    echo "$ROOT_DIR/14-moos-ivp-david355/lib"

elif [[ "${VAPP}" == *"amit2661"* ]]; then
    echo "$ROOT_DIR/03-moos-ivp-bflynn74/lib"

elif [[ "${VAPP}" == *"thomasts"* ]]; then
    echo "$ROOT_DIR/08-moos-ivp-mkier/lib"
#    echo "$ROOT_DIR/20-moos-ivp-thomasts/lib"

elif [[ "${VAPP}" == *"hoffman3"* ]]; then
    echo "$ROOT_DIR/06-moos-ivp-aaron24/lib"

elif [[ "${VAPP}" == *"adamphan"* ]]; then
    echo "$ROOT_DIR/18-moos-ivp-stoppani/lib"

elif [[ "${VAPP}" == *"akhargis"* ]]; then
    echo "$ROOT_DIR/01-moos-ivp-troyb528/lib"

elif [[ "${VAPP}" == *"jamie103"* ]]; then
    echo "$ROOT_DIR/09-moos-ivp-jamie103/lib"

elif [[ "${VAPP}" == *"karanmah"* ]]; then
    echo "$ROOT_DIR/17-moos-ivp-kshann/lib"

elif [[ "${VAPP}" == *"orlo"* ]]; then
    echo "$ROOT_DIR/12-moos-ivp-orlo/lib"

elif [[ "${VAPP}" == *"castroan"* ]]; then
    echo "$ROOT_DIR/02-moos-ivp-castroan/lib"
    
elif [[ "${VAPP}" == *"aarjav"* ]]; then
    echo "$ROOT_DIR/22-moos-ivp-aarjav/lib"
    
fi
