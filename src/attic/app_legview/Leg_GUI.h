/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng, MIT Cambridge MA             */
/*    FILE: Leg_GUI.h                                            */
/*    DATE: Jun 11th, 2023                                       */
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

#ifndef LEG_GUI_HEADER
#define LEG_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Output.H>
#include "LegViewer.h"
#include <FL/Fl_Hold_Browser.H>
#include "MarineGUI.h"

class Leg_GUI : public MarineGUI {
public:
  Leg_GUI(int w, int h, const char *l=0);
  ~Leg_GUI() {}
  
  void resize(int, int, int, int);
  void updateXY();
  int  handle(int);

protected:
  void augmentMenu();
  void initWidgets();
  void resizeWidgetsShape();
  void resizeWidgetsText();
   
public:
  LegViewer *m_leg_viewer;

  Fl_Output  *m_fld_sfile;
  Fl_Output  *m_fld_reg;
  Fl_Output  *m_fld_unreg;
  Fl_Output  *m_fld_minsep;

private:
  inline void cb_PavRegions_i();
  static void cb_PavRegions(Fl_Widget*);
  
  inline void cb_MarkerLabels_i();
  static void cb_MarkerLabels(Fl_Widget*);
  
  inline void cb_SwimFile_i(int);
  static void cb_SwimFile(Fl_Widget*, int);

  int m_start_hgt;
  int m_start_wid;

};
#endif
