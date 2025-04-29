/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RescueMgr_Info.cpp                                   */
/*    DATE: Feb 18th 2022                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#include <cstdlib>
#include <iostream>
#include "RescueMgr_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                        ");
  blk("------------------------------------                             ");
  blk("  The uFldRescueMgr app is the shoreside arbiter for the         ");
  blk("  Autonomous Rescue Lab sequence and competition. It holds the   ");
  blk("  ground truth information of swimmer locations and rescue       ");
  blk("  status. Ground truth locations are read in from a swim_file, or");
  blk("  may be added dynamically on the shoreside by an operator. The  ");
  blk("  status of the swimmers (rescued/unrescued, found/unfound) are  ");
  blk("  maintained by the rescue manager but change status based on    ");
  blk("  locations of the rescue vehicles or scout vehicles. The rescue ");
  blk("  manager manages the games state in terms of team tallies and   ");
  blk("  declaration of a winning team.                                 ");
  blk("  The rescue manager is available to participants when running   ");
  blk("  test simulations, but will be run on a shoreside computer by   ");
  blk("  a competition manager during an competition, simulated or      ");
  blk("  in-water.                                                      ");
}

//----------------------------------------------------------------
// Procedure: showHelp

void showHelpAndExit()
{
  blk("                                                          ");
  blu("==========================================================");
  blu("Usage: uFldRescueMgr file.moos [OPTIONS]                  ");
  blu("==========================================================");
  blk("                                                          ");
  showSynopsis();
  blk("                                                          ");
  blk("Options:                                                  ");
  mag("  --alias","=<ProcessName>                                ");
  blk("      Launch uFldRescueMgr with the given process         ");
  blk("      name rather than uFldRescueMgr.                     ");
  mag("  --example, -e                                           ");
  blk("      Display example MOOS configuration block.           ");
  mag("  --help, -h                                              ");
  blk("      Display this help message.                          ");
  mag("  --interface, -i                                         ");
  blk("      Display MOOS publications and subscriptions.        ");
  mag("  --version,-v                                            ");
  blk("      Display release version of uFldRescueMgr.           ");
  mag("  --web,-w (not yet implemented)                          ");
  blk("      Open browser to: https://oceanai.mit.edu/apps/uFldRescueMgr");
  blk("                                                          ");
  blk("Note: If argv[2] does not otherwise match a known option, ");
  blk("      then it will be interpreted as a run alias. This is ");
  blk("      to support pAntler launching conventions.           ");
  blk("                                                          ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blu("=============================================================== ");
  blu("uFldRescueMgr Example MOOS Configuration                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldRescueMgr                                   ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Common to all appcasting MOOSApps                          ");
  blk("  term_report_interval = 0.4                  // default        ");
  blk("  max_appcast_events   = 8                    // default        ");
  blk("  max_appcast_run_warnings = 10               // default        ");
  blk("                                                                ");
  blk("  swim_file     = file.txt                                      ");
  blk("  swimmer_color = dodger_blue                                   ");
  blk("                                                                ");
  blk("  // Configuring visual preferences                             ");
  blk("  show_rescue_rng = true                      // default        ");
  blk("  rescue_rng_transparency = 0.2               // default        ");
  blk("                                                                ");
  blk("  // Sensor Config                                              ");
  blk("  rescue_rng_min   = 25                       // default        ");
  blk("  rescue_rng_max   = 40                       // default        ");
  blk("  rescue_rng_pd    = 0.5                      // default        ");
  blk("                                                                ");
  blk("  // Event Flags                                                ");
  blk("  leader_flag = NEW_LEADER=$[LEADER]                            ");
  blk("  winner_flag = NEW_WINNER=$[WINNER]                            ");
  blk("  finish_flag = MISSION_COMPLETE=true                           ");
  blk("  finish_flag = RETURN_ALL=true                                 ");
  blk("                                                                ");
  blk("  finish_upon_win = false                     // default        ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blu("=============================================================== ");
  blu("uFldRescueMgr INTERFACE                                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  NODE_REPORT                                                   ");
  blk("  NODE_REPORT_LOCAL   = NAME=alpha,TYPE=UUV,TIME=1252348077.59, ");
  blk("                        X=51.71,Y=-35.50, LAT=43.824981,        ");
  blk("                        LON=-70.329755,SPD=2.0,HDG=118.8,       ");
  blk("                        YAW=118.8,DEPTH=4.6,LENGTH=3.8,         ");
  blk("                        MODE=MODE@ACTIVE:LOITERING              ");
  blk("                                                                ");
  blk("  XSWIMMER_ALERT      = type=unreg, x=92, y=31                  ");
  blk("  XFOUND_SWIMMER      = x=-16.5, y=4.3                          ");
  blk("                                                                ");
  blk("  RESCUE_REQUEST      = vname=abe                               ");
  blk("  SCOUT_REQUEST       = vname=ben, tmate=abe                    ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  RESCUED_SWIMMER     = id=07, finder=cal                       ");
  blk("  SCOUTED_SWIMMER_DEB = id=25, x=-48.20, y=-55.50               ");
  blk("                                                                ");
  blk("  SWIMMER_ALERT_ABE = x=23, y=148, id=21                        ");
  blk("                                                                ");
  blk("  VIEW_MARKER       = x=217.3,y=-16.7,width=8,label=6,          ");
  blk("                      primary_color=green,type=triangle         ");
  blk("                                                                ");
  blk("  VIEW_CIRCLE       = x=-150.3,y=-117.5,radius=10,edge_size=1   ");
  blk("                      edge_color=white,fill_color=white,        ");
  blk("                      vertex_size=0,fill_transparency=0.3       ");
  blk("                      type=circle,width=4                       ");
  blk("                                                                ");
  blk("  VIEW_POLYGON      = pts={-65,-49:-22,-139:119,-73:76,17},     ");
  blk("                      edge_color=gray90,vertex_color=blue,      ");
  blk("                      vertex_size=5                             ");
  blk("                                                                ");
  blk("  RESCUE_REGION     = pts={-65,-49:-22,-139:119,-73:76,17}      ");
  blk("                                                                ");
  blk("  UFRM_WINNER       = abe    (or \"pending\" before winner set  ");
  blk("  UFRM_LEADER       = ben    (or \"tie\" if no leader now       ");
  blk("  UFRM_FINISHED     = true                                      ");
  blk("                                                                ");
  blk("  PLOGGER_CMD       = COPY_FILE_REQUEST=<swim_file.txt>         ");
  blk("                                                                ");
  blk("  NODE_COLOR_CHANGE_ABE = green                                 ");
  blk("                                                                ");
  blk("PUBLICATIONS (flags):                                           ");
  blk("------------------------------------                            ");
  blk("  leader_flag    Posted when leader changes                     ");
  blk("  winner_flag    Posted when winner established                 ");
  blk("  finish_flag    Posted when competition is finished            ");
  blk("                                                                ");
  blk("  $[LEADER]      Current leader (or \"tie\")                    ");
  blk("  $[WINNER]      Current winner (or \"pending\"                 ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldRescueMgr", "gpl");
  exit(0);
}
