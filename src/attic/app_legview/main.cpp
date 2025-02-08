/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: May 1st, 2005                                        */
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
#include "Leg_GUI.h"
#include "MBUtils.h"
#include "LegView_Info.h"

using namespace std;

void idleProc(void *);

//--------------------------------------------------------
// Procedure: main()

int main(int argc, char *argv[])
{
  string tif_file = "MIT_SP.tif";  // default

  Fl::add_idle(idleProc);
  Leg_GUI* gui = new Leg_GUI(1100, 800, "legview");

  for(int i=1; i<argc; i++) {
    string argi  = argv[i];
    
    bool handled = true;
    if((argi == "-v") || (argi == "--version"))
      showReleaseInfoAndExit();
    else if((argi == "-h") || (argi == "--help"))
      showHelpAndExit();
    else if((argi == "mit") || (argi=="charles"))
      tif_file = "MIT_SP.tif";
    else if((argi == "forrest-lake") || (argi=="fl"))
      tif_file = "forrest19.tif";
    else if(strEnds(argi, ".txt"))
      handled = gui->m_leg_viewer->readSwimFile(argi);      
    else 
      gui->m_leg_viewer->readGeomFile(argi);      
    
    if(!handled)
      return(1);
  }

  
  if(gui->m_leg_viewer->size() == 0) {
    cout << "No provided swimfile has been found" << endl;
    return(1);
  }
  
  gui->m_leg_viewer->setParam("tiff_file", tif_file);
  gui->updateXY();

  return Fl::run();
}

//--------------------------------------------------------
// Procedure: idleProc()

void idleProc(void *)
{
  Fl::flush();
  millipause(10);
}









