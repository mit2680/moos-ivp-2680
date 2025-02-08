/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GenSwimmers_Info.cpp                                 */
/*    DATE: Apr 2nd 2022                                         */
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
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "GenSwimmers_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  gen_swimmers is a utility for generating a swimmer file, i.e.,");
  blk("  a list of swimmers, and a unique name and location of each.   ");
  blk("  The user can provide one or more convex polygons within which ");
  blk("  a random swimmer location will be chosen. The user can also   "); 
  blk("  choose a minimum separation distance between swimmers.        ");

}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  cout << "=======================================================" << endl;
  cout << "Usage: gen_swimmers [OPTIONS]                          " << endl;
  cout << "=======================================================" << endl;
  cout << "                                                       " << endl;
  showSynopsis();
  cout << "                                                       " << endl;
  cout << "Options:                                               " << endl;
  cout << "  --help, -h                                           " << endl;
  cout << "      Display this help message.                       " << endl;
  cout << "  --version,-v                                         " << endl;
  cout << "      Display the release version of gen_swimmers.     " << endl;
  cout << "  --swimmers=<amt>                                     " << endl;
  cout << "      Specify number of randomly placed swimmers       " << endl;
  cout << "      Default is 1                                     " << endl;
  cout << "  --unreg=<amt>                                        " << endl;
  cout << "      Specify number of randomly placed unreg swimmers " << endl;
  cout << "      Default is 0                                     " << endl;
  cout << "  --buf=<value>                                        " << endl;
  cout << "      Min buffer between generated pts. Default is 10  " << endl;
  cout << "  --sep=<value>                                        " << endl;
  cout << "      Same as --buf=<amt>                              " << endl;
  cout << "  --poly=<poly>                                        " << endl;
  cout << "      Specify a polygon region of the form:            " << endl;
  cout << "      \"0,0 : 50,0 : 50,50 : 0,50\"                    " << endl;
  cout << "  --mit_small, --pav60                                 " << endl;
  cout << "      Short for:                                       " << endl;
  cout << "      --poly=\"60,10:-30.36,-32.84:-4.66,-87.05:85.7024,-44.21\" " << endl;
  cout << "  --mit_big, ---pav90                                  " << endl;
  cout << "      Short for:                                       " << endl;
  cout << "      --poly=\"60,10:-75.54,-54.26:-36.99,-135.58:98.55,-71.32\" " << endl;
  cout << "  --mit                                                " << endl;
  cout << "      Short for:                                       " << endl;
  cout << "      --poly=\"60,10:-30.36,-32.84:-4.66,-87.05:85.70,-44.22\" " << endl;
  cout << "                                                       " << endl;
  cout << "Example:                                               " << endl;
  cout << "  gen_swimmers --mit_big --swimmers=4 --unreg=6 --buf=5" << endl;
  cout << "  gen_swimmers --mit_small --swimmers=4 --buf=5        " << endl;
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("gen_swimmers", "gpl");
  exit(0);
}
