/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
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

#include <iostream>
#include "MBUtils.h"
#include "SwimFldGenerator.h"
#include "GenSwimmers_Info.h"

using namespace std;

int main(int argc, char *argv[])
{
  SwimFldGenerator generator;

  string arg_summary = argv[0];

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    arg_summary += " " + argi;

    bool handled = false;
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
     showReleaseInfoAndExit();
    else if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if(strBegins(argi, "--swimmers="))
      handled = generator.setSwimmerAmt(argi.substr(11));
    else if(strBegins(argi, "--unreg="))
      handled = generator.setUnregAmt(argi.substr(8));
    else if(strBegins(argi, "--poly="))
      handled = generator.addPolygon(argi.substr(7));
    else if(strBegins(argi, "--polygon="))
      handled = generator.addPolygon(argi.substr(10));
    else if(strBegins(argi, "--sep="))
      handled = generator.setBufferDist(argi.substr(6));
    else if(strBegins(argi, "--buf="))
      handled = generator.setBufferDist(argi.substr(6));
    else if(argi == "--mit") {
      string poly = "60,10:-30.36,-32.84:-4.66,-87.05:85.70,-44.22";
      handled = generator.addPolygon(poly);
    }
    else if((argi == "--mit_small") || (argi == "--pav60")) {
      string poly = "60,10:-30.3602,-32.8374:-4.6578,-87.0535:85.7024,-44.2161";
      handled = generator.addPolygon(poly);
    }
    else if((argi == "--mit_big") || (argi == "--pav90")) {
      string poly = "60,10:-75.5402,-54.2561:-36.9866,-135.58:98.5536,-71.3241";
      handled = generator.addPolygon(poly);
    }
    
    if(!handled) {
      cout << "Unhandled arg: " << argi << endl;
      return(1);
    }
  }

  cout << "// " << arg_summary << endl;

  generator.generate();
  return(0);
}
