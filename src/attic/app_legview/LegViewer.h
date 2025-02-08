/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng, MIT Cambridge MA             */
/*    FILE: LegViewer.h                                          */
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

#ifndef LEG_VIEWER_HEADER
#define LEG_VIEWER_HEADER

// Defined to silence GL deprecation warnings in OSX 10.14+
#define GL_SILENCE_DEPRECATION

#include <string>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "MarineViewer.h"
#include "SwimmerSet.h"
#include "VPlug_GeoShapes.h"

class LegViewer : public MarineViewer
{
 public:
  LegViewer(int x,int y,int w,int h,const char *l=0);
  
  // Pure virtual that need to be defined
  void  modColorScheme() {};

  bool  readSwimFile(std::string);
  bool  readGeomFile(std::string);
  
  // Virtual defined
  void  draw();
  void  drawSwimSet(SwimmerSet);
  void  eraseSwimSet(SwimmerSet);
  int   handle(int);
  void  handle_left_mouse(int, int);
  void  handle_right_mouse(int, int);
  bool  setParam(std::string param, std::string value);
  bool  setParam(std::string param, double value);
  
  void  incCurrSwimSet();
  void  decCurrSwimSet();

  std::string  getColorByIndex() const;
  std::string  getCurrSwimFile() const;
  unsigned int getCurrReg() const;
  unsigned int getCurrUnreg() const;
  double       getCurrMinSep() const;

  void drawRegion();

  void toggleDrawGenRegions()   {m_draw_gen_regions=!m_draw_gen_regions;}
  void toggleDrawMarkerLabels();
  
  void drawGeneralRegions();
  void drawGeoObjects();

  unsigned int size() const {return(m_swimsets.size());}
  
public:
  std::string getPolySpec();

  double getSnap()                 {return(m_snap_val);}
  void   reApplySnapToCurrent();   

private: // config vars
  double  m_snap_val;

private: // state vars

  std::vector<SwimmerSet> m_swimsets;

  unsigned int m_curr_swimset;

  bool m_draw_gen_regions;

  std::string m_draw_mlabels;
  
  VPlug_GeoShapes  m_geoshapes;
};

#endif 










