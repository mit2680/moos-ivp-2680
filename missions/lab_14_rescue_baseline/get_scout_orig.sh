#!/bin/bash 
#-------------------------------------------------------
#   Script: get_scout.sh                                    
#   Author: Michael Benjamin  
#     Date: May 2025     
#-------------------------------------------------------
#  Part 1: Declare global var defaults
#-------------------------------------------------------
VUSER=""

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
	VUSER=$ARGI
    else 
	echo "get_scout.sh: Bad Arg:[$ARGI]. Exit Code 1."
	exit 1
    fi
done

#-------------------------------------------------------
#  Part 2: Do the cleaning!
#-------------------------------------------------------
ROOT_DIR="/Users/mikerb/Research/autotest/harnesses_2680"
if [ "${VUSER}" = "cdembski" ]; then
    echo "$ROOT_DIR/09-moos-ivp-mcschwar/lib/"

elif [ "${VUSER}" = "jwenger" ]; then
    echo "$ROOT_DIR/13-moos-ivp-jcbrandt/lib/"

elif [ "${VUSER}" = "chthigpen" ]; then
    echo "$ROOT_DIR/19-moos-ivp-hallsc83/lib/"

elif [ "${VUSER}" = "jcizu690" ]; then
    echo "$ROOT_DIR/16-moos-ivp-harkm/lib/"

elif [ "${VUSER}" = "thoma904" ]; then
    echo "$ROOT_DIR/05-moos-ivp-dmgilbe2/lib/"

elif [ "${VUSER}" = "bruno752" ]; then
    echo "$ROOT_DIR/18-moos-ivp-egajski/lib/"

elif [ "${VUSER}" = "cschnorr" ]; then
    echo "$ROOT_DIR/06-moos-ivp-cschnorr/lib/"

elif [ "${VUSER}" = "ews2011" ]; then
    echo "$ROOT_DIR/02-moos-ivp-ews2011/lib/"

elif [ "${VUSER}" = "bertucci" ]; then
    echo "$ROOT_DIR/01-moos-ivp-rjkang17/lib/"

elif [ "${VUSER}" = "ekwang" ]; then
    echo "$ROOT_DIR/07-moos-ivp-adamc710/lib/"
    
fi
