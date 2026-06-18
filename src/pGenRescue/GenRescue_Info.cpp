/****************************************************************/
/*   NAME: Mike Benjamin                                        */
/*   ORGN: MIT Cambridge MA                                     */
/*   FILE: GenRescue_Info.cpp                                   */
/*   DATE: April 18th, 2022                                     */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "GenRescue_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------                        
// Procedure: showSynopsis                                                                

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  A tool for                                                    ");
  blk("                                                                ");
  blk("                                                                ");
  blk("                                                                ");
}


//----------------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  blu("=============================================================== ");
  blu("Usage: pGenRescue file.moos [OPTIONS]                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pGenRescue with the given process                  ");
  blk("      name rather than pGenRescue.                              ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pNodeReporter.             ");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit()

void showExampleConfigAndExit()
{
  cout << "                                                   " << endl;
  cout << "===================================================" << endl;
  cout << "pGenRescue Example MOOS Configuration              " << endl;
  cout << "===================================================" << endl;
  cout << "                                                   " << endl;
  cout << "ProcessConfig = pGenRescue                         " << endl;
  cout << "{                                                  " << endl;
  cout << "  AppTick   = 4                                    " << endl;
  cout << "  CommsTick = 4                                    " << endl;
  cout << "}                                                  " << endl;
}


//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit()

void showReleaseInfoAndExit()
{
  showReleaseInfo("pGenRescue", "gpl");
  exit(0);
}
