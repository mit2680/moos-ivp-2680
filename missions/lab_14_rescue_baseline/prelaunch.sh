#!/bin/bash 
#------------------------------------------------------------ 
#   Script: prelaunch.sh    
#   Author: Michael Benjamin   
#   LastEd: April 2025
#------------------------------------------------------------
#  Part 1: Set convenience functions for producing terminal
#          debugging output, and catching SIGINT (ctrl-c).
#------------------------------------------------------------
vecho() { if [ "$VERBOSE" != "" ]; then echo "$ME: $1"; fi }
on_exit() { echo; echo "$ME: Halting all apps"; kill -- -$$; }
trap on_exit SIGINT

# --raepo=mikerb --repo=tpaine
# For each repo:
#   Confirm repo exists
#   Confirm repo builds
#   Confirm repo has pGenRescue
#   Get full path of pGenRescue 

#------------------------------------------------------------
#  Part 2: Set global variable default values
#------------------------------------------------------------
ME=`basename "$0"`
CMD_ARGS=""
VERBOSE=""
REPOS_DIR="/Users/mikerb/Research/autotest/harnesses_2680"
REPO1="moos-ivp-mrb"
REPO2="moos-ivp-mrb"

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
	echo "                                               "
	echo "                                               "
	echo "  --repos_dir=<dir>  Location of all repos     " 
	echo "  --repo1=<kerberos> Name of repo1 (kerberos)  " 
	echo "  --repo2=<kerberos> Name of repo2 (kerberos)  " 
	exit 0;
    elif [ $ARGI = "-v" ]; then
        VERBOSE="yes"
    elif [ "${ARGI:0:12}" = "--repos_dir=" ]; then
        REPOS_DIR="${ARGI#--repos_dir=*}"
    elif [ "${ARGI:0:8}" = "--repo1=" ]; then
        REPO1="${ARGI#--repo1=*}"
    elif [ "${ARGI:0:8}" = "--repo2=" ]; then
        REPO2="${ARGI#--repo2=*}"
    else
	echo "$ME: Bad arg:" $ARGI "Exit Code 1."
        exit 1
    fi
done

#------------------------------------------------------------
#  Part 4: Check if the parent repo_dir exists
#------------------------------------------------------------
if [ ! -d "${REPOS_DIR}" ]; then
    echo "Parent repos dir: [${REPOS_DIR}] not found. Exit 2."
    exit 2
fi

#------------------------------------------------------------
#  Part 5: Check if the repo1 exists and its pGenRescue
#------------------------------------------------------------
FULL_REPO1="${REPOS_DIR}/${REPO1}"
if [ ! -d "${FULL_REPO1}" ]; then
    echo "Repo1 dir: [${FULL_REPO1}] not found. Exit 3."
    exit 3
fi
vecho "Found Repo1 dir: [${FULL_REPO1}]"
    
if [ ! -f "${FULL_REPO1}/bin/pGenRescue" ]; then
    echo "A pGenRescue not found in Repo1 dir: [${FULL_REPO1}] Exit 4."
    exit 4
fi
vecho "Found: ${FULL_REPO1}/bin/pGenRescue"

#------------------------------------------------------------
#  Part 6: Check if the repo2 exists and its pGenRescue
#------------------------------------------------------------
FULL_REPO2="${REPOS_DIR}/${REPO2}"
if [ ! -d "${FULL_REPO2}" ]; then
    echo "Repo2 dir: [${FULL_REPO2}] not found. Exit 5."
    exit 5
fi
vecho "Found Repo2 dir: [${FULL_REPO2}]"

if [ ! -f "${FULL_REPO2}/bin/pGenRescue" ]; then
    echo "A pGenRescue not found in Repo2 dir: [${FULL_REPO2}] Exit 6."
    exit 6
fi
vecho "Found: ${FULL_REPO2}/bin/pGenRescue"


#------------------------------------------------------------
#  Part 7: Note to two versions of pGenRescue
#------------------------------------------------------------
echo "${FULL_REPO1}/bin/pGenRescue" > vapps.txt
echo "${FULL_REPO2}/bin/pGenRescue" >> vapps.txt

exit 0


