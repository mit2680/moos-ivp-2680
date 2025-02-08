/**********************************************************/
/*   NAME: Michael Benjamin                               */
/*   ORGN: MIT, Cambridge MA                              */
/*   FILE: ManOverboard_Info.cpp                          */
/*   DATE: Feb 19th, 2022                                 */
/**********************************************************/

#include <cstdlib>
#include <iostream>
#include "ManOverboard_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The uFldManOverboard application is used for               ");
  blk("                                                                ");
  blk("                                                                ");
  blk("                                                                ");
  blk("                                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uFldManOverboard file.moos [OPTIONS]                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uFldManOverboard with the given process name         ");
  blk("      rather than uFldManOverboard.                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uFldManOverboard.        ");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uFldManOverboard Example MOOS Configuration                     ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldManOverboard                                ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  swimmers    = 4                                               ");
  blk("  swimmer_sep = 10                                              ");
  blk("  drop_region =  pts={20,-50 : 20,-100 : 75,-100 : 75,-50},     ");
  blk("                 label =dropzone                                ");
  blk("  drop_circle =  x=23, y=45, rad=20, pts=12                     ");
  blk("  show_region =  true  (Default is false)                       ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uFldManOverboard INTERFACE                                    ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  FOUND_SWIMMER = swimmer_name=s1, finder=abe                   ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  // Published initially, shared with rescue vehicles           ");
  blk("  MOB_ALERT  = swimmer_name=s2, utc=time, x=23, y=33            ");
  blk("                                                                ");
  blk("  // Published regularly, shared swim sensor                    ");
  blk("  MOB_UPDATE = swimmer_name=s2, utc=time, x=24, y=35            ");
  blk("                                                                ");
  blk("                                                                ");
  blk("  MOB_SUMMARY = total_found=2, total_unfound=1, ftime=23.1,     ");
  blk("                ftime=456,2                                     ");
  blk("  MOB_REPORT  = finder=abe, total_found=1, ftime=23.1           ");
  blk("                                                                ");
  blk("  NODE_REPORT = NAME=alpha,TYPE=UUV,TIME=1252348077.59,         ");
  blk("                COLOR=red,X=51.71,Y=-35.50, LAT=43.824981,      ");
  blk("                LON=-70.329755,SPD=2.0,HDG=118.8,               ");
  blk("                YAW=118.8,DEPTH=4.6,LENGTH=3.8,                 ");
  blk("                MODE=MODE@ACTIVE:LOITERING,                     ");
  blk("                THRUST_MODE_REVERSE=true                        ");
  blk("                                                                ");
  blk("  VIEW_POLYGON                                                  ");

  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldManOverboard", "gpl");
  exit(0);
}

