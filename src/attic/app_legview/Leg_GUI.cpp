/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng, MIT Cambridge MA             */
/*    FILE: Leg_GUI.cpp                                          */
/*    DATE: June 11th, 2023                                      */
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
#include <cstdio>
#include "Leg_GUI.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor()

Leg_GUI::Leg_GUI(int wid, int hgt, const char *label)
  : MarineGUI(wid, hgt, label) {

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  m_start_hgt = hgt;
  m_start_wid = wid;

  augmentMenu();
  initWidgets();
  resizeWidgetsShape();
  resizeWidgetsText();
  
  this->end();
  this->resizable(this);
  this->show();
}

//----------------------------------------------------------------
// Procedure: resize()

void Leg_GUI::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgetsShape();
  resizeWidgetsText();
}

//----------------------------------------------------------------
// Procedure: augmentMenu()

void Leg_GUI::augmentMenu() 
{
  m_menubar->add("SwimFile/Increment", ']', (Fl_Callback*)Leg_GUI::cb_SwimFile, (void*)2, 0);
  m_menubar->add("SwimFile/Decrement", '[', (Fl_Callback*)Leg_GUI::cb_SwimFile, (void*)1, 0);
  m_menubar->add("Regions/Toggle Pav Regions", 'p', (Fl_Callback*)Leg_GUI::cb_PavRegions,
		 (void*)0, 0);
  m_menubar->add("Markers/Toggle Labels", 'l', (Fl_Callback*)Leg_GUI::cb_MarkerLabels,
		 (void*)0, 0);
}

//----------------------------------------------------------
// Procedure: handle()
//     Notes: We want the various "Output" widgets to ignore keyboard
//            events (as they should, right?!), so we wrote a MY_Output
//            subclass to do just that. However the keyboard arrow keys
//            still seem to be grabbed by Fl_Window to change focuse
//            between sub-widgets. We over-ride that here to do the 
//            panning on the image by invoking the pan callbacks. By
//            then returning (1), we've indicated that the event has
//            been handled.

int Leg_GUI::handle(int event) 
{
  switch(event) {
  case FL_PUSH:
    Fl_Window::handle(event);
    updateXY();
    return(1);
    break;
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------- SwimFile
inline void Leg_GUI::cb_SwimFile_i(int v) {
  if(v == 1) 
    m_leg_viewer->decCurrSwimSet();
  if(v == 2) 
    m_leg_viewer->incCurrSwimSet();
  updateXY();
  m_leg_viewer->redraw();
}
void Leg_GUI::cb_SwimFile(Fl_Widget* o, int v) {
  ((Leg_GUI*)(o->parent()->user_data()))->cb_SwimFile_i(v);
}

//----------------------------------------- PavRegions
inline void Leg_GUI::cb_PavRegions_i() {
  m_leg_viewer->toggleDrawGenRegions();
  updateXY();
  m_leg_viewer->redraw();
}
void Leg_GUI::cb_PavRegions(Fl_Widget* o) {
  ((Leg_GUI*)(o->parent()->user_data()))->cb_PavRegions_i();
}

//----------------------------------------- MarkerLabels
inline void Leg_GUI::cb_MarkerLabels_i() {
  m_leg_viewer->toggleDrawMarkerLabels();
  updateXY();
  m_leg_viewer->redraw();
}
void Leg_GUI::cb_MarkerLabels(Fl_Widget* o) {
  ((Leg_GUI*)(o->parent()->user_data()))->cb_MarkerLabels_i();
}

//----------------------------------------- UpdateXY
void Leg_GUI::updateXY()
{
  string sfile = m_leg_viewer->getCurrSwimFile();
  m_fld_sfile->value(sfile.c_str());

  // Registered Swimmers
  unsigned int uint_reg = m_leg_viewer->getCurrReg();
  string str_reg = uintToString(uint_reg);
  m_fld_reg->value(str_reg.c_str());

  // UnRegistered Swimmers
  unsigned int uint_unreg = m_leg_viewer->getCurrUnreg();
  string str_unreg = uintToString(uint_unreg);
  m_fld_unreg->value(str_unreg.c_str());

  // Snap Value
  double dval = m_leg_viewer->getCurrMinSep();
  string sval = doubleToStringX(dval);
  m_fld_minsep->value(sval.c_str());
}
