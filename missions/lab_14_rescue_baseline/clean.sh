#!/bin/bash 
#--------------------------------------------------------------
#   Script: clean.sh                                    
#   Author: Michael Benjamin  
#     Date: June 2020     
#----------------------------------------------------------
#  Part 1: Declare global var defaults
#----------------------------------------------------------
VERBOSE=""

#-------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#-------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "clean.sh [SWITCHES]        "
	echo "  --verbose                " 
	echo "  --help, -h               " 
	exit 0;	
    elif [ "${ARGI}" = "--verbose" -o "${ARGI}" = "-v" ] ; then
	VERBOSE="-v"
    else 
	echo "clean.sh: Bad Arg:[$ARGI]. Exit Code 1."
	exit 1
    fi
done

#-------------------------------------------------------
#  Part 2: Do the cleaning!
#-------------------------------------------------------
if [ "${VERBOSE}" = "-v" ]; then
    echo "Cleaning: $PWD"
fi
rm -rf  $VERBOSE   MOOSLog_*  XLOG_* LOG_* 
rm -f   $VERBOSE   *~  *.moos++
rm -f   $VERBOSE   targ_*
rm -f   $VERBOSE   .LastOpenedMOOSLogDirectory
rm -f   $VERBOSE   .mem_info*
rm -f   $VERBOSE   vgroups.txt vnames.txt vpositions.txt vapps.txt
rm -f   $VERBOSE   vcolors.txt vspeeds.txt vroles.txt vmates.txt
