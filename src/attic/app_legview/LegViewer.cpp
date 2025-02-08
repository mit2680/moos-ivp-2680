/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng, MIT Cambridge MA             */
/*    FILE: LegViewer.cpp                                        */
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
#include <string>
#include <cmath>
#include "LegViewer.h"
#include "MBUtils.h"
#include "GeomUtils.h"
#include "CircularUtils.h"
#include "AngleUtils.h"
#include "LMV_Utils.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//------------------------------------------------------------
// Constructor()

LegViewer::LegViewer(int x, int y, int w, int h, const char *l)
  : MarineViewer(x,y,w,h,l)
{
  m_snap_val    = 10.0;
  //m_active_poly = 0;

  m_curr_swimset = 0;

  m_draw_gen_regions = true;

  m_draw_mlabels = "name";  // name/id/off
  

  m_zoom     = 1.2; 
  m_vshift_x = 60;    // Nice start values for MIT SP
  m_vshift_y = -280;
}

//-------------------------------------------------------------
// Procedure: handle()

int LegViewer::handle(int event)
{
  int vx, vy;
  switch(event) {
  case FL_PUSH:
    vx = Fl::event_x();
    vy = h() - Fl::event_y();
    if(Fl_Window::handle(event) != 1) {
      if(Fl::event_button() == FL_LEFT_MOUSE)
	handle_left_mouse(vx, vy);
      if(Fl::event_button() == FL_RIGHT_MOUSE)
	handle_right_mouse(vx, vy);
    }
    return(1);
    break;
  default:
    return(Fl_Gl_Window::handle(event));
  }
}

//-------------------------------------------------------------
// Procedure: draw()

void LegViewer::draw()
{
  MarineViewer::draw();
  drawGeoObjects();
  drawGeneralRegions();
  drawSwimSet(m_swimsets[m_curr_swimset]);

  // Draw pan/zoom right on the screen
  ColorPack cpack("yellow");
  string msg = "--zoom=" + doubleToString(m_zoom,3) + "  ";
  msg += "--panx=" + doubleToString(m_vshift_x,2) + " ";
  msg += "--pany=" + doubleToString(m_vshift_y,2);
  drawTextX(10, 10, msg, cpack, 12);
}

//-------------------------------------------------------------
// Procedure: drawGeoObjects()

void LegViewer::drawGeoObjects()
{
  if(m_geo_settings.viewable("hash_viewable")) {
    double xl = m_geoshapes.getXMin() - 1000;
    double xh = m_geoshapes.getXMax() + 1000;
    double yl = m_geoshapes.getYMin() - 1000;
    double yh = m_geoshapes.getYMax() + 1000;
    drawHash(xl, xh, yl, yh);
  }

  vector<XYPolygon> polys = m_geoshapes.getPolygons();  
  vector<XYGrid>    grids = m_geoshapes.getGrids();
  vector<XYSegList> segls = m_geoshapes.getSegLists();
  //  vector<XYMarker>  markers = m_geoshapes.getMarkers();

  const map<string, XYPoint> points = m_geoshapes.getPoints();

  drawSegLists(segls);
  drawPoints(points);
  drawPolygons(polys);
  // drawMarkers(markers);
}

//-------------------------------------------------------------
// Procedure: toggleDrawMarkerLabels()

void LegViewer::toggleDrawMarkerLabels()
{
  if(m_draw_mlabels == "name")
    m_draw_mlabels = "id";
  else if(m_draw_mlabels == "id")
    m_draw_mlabels = "off";
  else if(m_draw_mlabels == "off")
    m_draw_mlabels = "name";
  
}


//-------------------------------------------------------------
// Procedure: drawGeneralRegions()

void LegViewer::drawGeneralRegions()
{
  string pav60 = "pts={60,10:-30.3602,-32.8374:-4.6578,-87.0535:85.7024,-44.2161}";
  string pav90 = "pts={60,10:-75.5402,-54.2561:-36.9866,-135.58:98.5536,-71.3241}";
  
  XYPolygon region1 = string2Poly(pav60);
  XYPolygon region2 = string2Poly(pav90);

  region1.set_label("reg1");
  region2.set_label("reg2");

  region1.set_edge_color("lime");
  region2.set_edge_color("yellow");

  region1.set_label_color("off");
  region2.set_label_color("off");

  if(!m_draw_gen_regions) {
    region1.set_active(false);
    region2.set_active(false);
  }
  
  drawPolygon(region1);
  drawPolygon(region2);
}

//-------------------------------------------------------------
// Procedure: handle_left_mouse()

void LegViewer::handle_left_mouse(int vx, int vy)
{
#if 0
  int vsize = m_geoshapes.sizePolygons();

  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  double sx = snapToStep(mx, m_snap_val);
  double sy = snapToStep(my, m_snap_val);
  
  if(m_drop_mode == 0) {
    if(vsize == 0) {
      m_active_poly = 0;
      XYPolygon newpoly;
      string new_label;
      int vvsize = vsize;
      while(vvsize >= 0) {
	int rem = vvsize % 26;
	char next_char = 65 + rem;
	new_label += next_char;
	vvsize = vvsize - 26;
      }
      newpoly.set_param("edge_size", "1");
      newpoly.set_label(new_label);
      newpoly.add_vertex(sx, sy);
      m_geoshapes.addPolygon(newpoly);
    }
    else
      m_geoshapes.poly(m_active_poly).add_vertex(sx, sy);
  }
  if(m_drop_mode == 1) {
    if(vsize > 0)
      m_geoshapes.poly(m_active_poly).alter_vertex(sx, sy);
  }
  if(m_drop_mode == 2) {
    if(vsize > 0)
      m_geoshapes.poly(m_active_poly).delete_vertex(mx, my);
  }
  if(m_drop_mode == 3) {
    if(vsize > 0)
      m_geoshapes.poly(m_active_poly).insert_vertex(sx, sy);
  }
#endif
  redraw();
}

//-------------------------------------------------------------
// Procedure: handle_right_mouse()

void LegViewer::handle_right_mouse(int vx, int vy)
{
#if 0
  unsigned int vsize = m_geoshapes.sizePolygons(); 
  if(vsize == 0)
    return;

  double ix = view2img('x', vx);
  double iy = view2img('y', vy);
  double mx = img2meters('x', ix);
  double my = img2meters('y', iy);
  //double sx = snapToStep(mx, m_snap_val);
  //double sy = snapToStep(my, m_snap_val);
  
  m_active_poly = 0;
  bool found = false;
  
  for(unsigned int i=0; i<vsize; i++) {
    if(m_geoshapes.poly(i).contains(mx, my)) {
      m_active_poly = i;
      found = true;
    }
  }
  #endif
  redraw();
}


//-------------------------------------------------------------
// Procedure: setParam()

bool LegViewer::setParam(string param, string value)
{
  param = tolower(stripBlankEnds(param));
  value = stripBlankEnds(value);
  
  if(MarineViewer::setParam(param, value))
    return(true);

  bool handled = false;
  if(param == "view_polygon")
    handled = m_geoshapes.addPolygon(value);
  else if(param == "view_seglist")
    handled = m_geoshapes.addSegList(value);
  else if(param == "view_point") 
    handled = m_geoshapes.addPoint(value);

  else if(param == "view_vector")
    handled = m_geoshapes.addVector(value);
  else if(param == "view_circle")
    handled = m_geoshapes.addCircle(value);
  else if((param == "view_marker") || (param == "marker"))
    handled = m_geoshapes.addMarker(value);
  else if(param == "grid_config")
    handled = m_geoshapes.addGrid(value);
  else if(param == "grid_delta")
    handled = m_geoshapes.updateGrid(value);
  else
    handled = handled || m_vehi_settings.setParam(param, value);

  return(handled);
}

//-------------------------------------------------------------
// Procedure: setParam()

bool LegViewer::setParam(string param, double pval)
{
  if(MarineViewer::setParam(param, pval))
    return(true);

  else if(param == "snap") {
    m_snap_val = pval;
    return(true);
  }
  else
    return(false);
}

// ----------------------------------------------------------
// Procedure: getPolySpec()

string LegViewer::getPolySpec()
{
  return("");  
#if 0
  if(m_geoshapes.sizePolygons() == 0) 
    return("");
  else
    return(m_geoshapes.poly(m_active_poly).get_spec());
#endif
}

// ----------------------------------------------------------
// Procedure: reApplySnapToCurrent()

void LegViewer::reApplySnapToCurrent()
{
#if 0
  if(m_active_poly >= m_geoshapes.sizePolygons())
    return;
  
  m_geoshapes.poly(m_active_poly).apply_snap(m_snap_val);
#endif
}


// ----------------------------------------------------------
// Procedure: drawSwimSet()

void LegViewer::drawSwimSet(SwimmerSet swimset)
{
  vector<Swimmer> swimmers = swimset.getSwimmers();

  string mcolor = getColorByIndex();
  
  XYMarker marker;
  marker.set_color("primary_color", mcolor);
  marker.set_transparency(0.4);
  marker.set_width(3);
 
  if(m_draw_mlabels == "name")
    marker.set_label_color("gray70");
  else if(m_draw_mlabels == "id") 
    marker.set_label_color("gray70");
  else
    marker.set_label_color("off");
  
  for(unsigned int i=0; i<swimmers.size(); i++) {

    marker.set_type("triangle");
    if(swimmers[i].getType() == "unreg") {
      marker.set_type("efield");
      marker.set_color("primary_color", "gray50");
      marker.set_color("secondary_color", "gray60");
    }      

    marker.set_label(swimmers[i].getName());
    marker.set_msg("");
    if(m_draw_mlabels == "id")
      marker.set_msg(swimmers[i].getID());
    
    marker.set_vx(swimmers[i].getStartX());
    marker.set_vy(swimmers[i].getStartY());
    ColorPack cpack = marker.get_color("label");
    drawMarker(marker);
  }

  XYPolygon region = swimset.getRescueRegion();
  if(!region.is_convex()) {
    string swim_file = swimset.getSwimFile();
    cout << "Empty/non-convex region for file: " << swim_file << endl;
    return;
  }
 
  region.set_label("rescue_region");
  region.set_label_color("off");
  drawPolygon(region);  
}

// ----------------------------------------------------------
// Procedure: eraseSwimSet()

void LegViewer::eraseSwimSet(SwimmerSet swimset)
{
  vector<Swimmer> swimmers = swimset.getSwimmers();

  XYMarker marker;
  marker.set_type("triangle");

  for(unsigned int i=0; i<swimmers.size(); i++) {
    marker.set_label(swimmers[i].getID());
    marker.set_active(false);
    drawMarker(marker);
  } 
}

// ----------------------------------------------------------
// Procedure: incCurrSwimSet()

void LegViewer::incCurrSwimSet()
{
  if(m_swimsets.size() <= 1) {
    m_curr_swimset = 0;
    return;
  }
  eraseSwimSet(m_swimsets[m_curr_swimset]);
  
  m_curr_swimset++;
  if(m_curr_swimset >= m_swimsets.size())
    m_curr_swimset = 0;
}

// ----------------------------------------------------------
// Procedure: decCurrSwimSet()

void LegViewer::decCurrSwimSet()
{
  if(m_swimsets.size() <= 0) {
    m_curr_swimset = 0;
    return;
  }
  eraseSwimSet(m_swimsets[m_curr_swimset]);

  if(m_curr_swimset == 0)
    m_curr_swimset = m_swimsets.size()-1;
  else
    m_curr_swimset--;
}


// ----------------------------------------------------------
// Procedure: readSwimFile()

bool LegViewer::readSwimFile(string filestr)
{
  SwimmerSet swimset;

  string warning;
  bool handled = swimset.handleSwimFile(filestr, 0, warning);

  if(!handled) {
    cout << "Unhandled swim_file: [" << filestr << "]" << endl;
    cout << "Warning:" << warning << endl;
    return(false);
  }

  cout << "Added SwimFile: " << filestr << endl;
  
  m_swimsets.push_back(swimset);
  
  return(true);
}

// ----------------------------------------------------------
// Procedure: readGeomFile()

bool LegViewer::readGeomFile(string filestr)
{
  vector<string> svector;

  svector = readEntriesFromFile(filestr, "poly:polygon");
  for(unsigned int j=0; j<svector.size(); j++)
    setParam("view_polygon", svector[j]);
  
  svector = readEntriesFromFile(filestr, "segl:seglist:points");
  for(unsigned int j=0; j<svector.size(); j++)
    setParam("view_seglist", svector[j]);
  
  svector = readEntriesFromFile(filestr, "point");
  for(unsigned int j=0; j<svector.size(); j++)
    setParam("view_point", svector[j]);

  svector = readEntriesFromFile(filestr, "circle");
  for(unsigned int j=0; j<svector.size(); j++)
    setParam("view_circle", svector[j]);

  svector = readEntriesFromFile(filestr, "marker");
  for(unsigned int j=0; j<svector.size(); j++)
    setParam("view_marker", svector[j]);

  svector = readEntriesFromFile(filestr, "op_vertex");
  for(unsigned int j=0; j<svector.size(); j++)
    setParam("op_vertex", svector[j]);

  return(true);
}


// ----------------------------------------------------------
// Procedure: getCurrSwimFile()

string LegViewer::getCurrSwimFile() const
{
  if(m_curr_swimset >= m_swimsets.size())
    return("");

  return(m_swimsets[m_curr_swimset].getSwimFile());
}

// ----------------------------------------------------------
// Procedure: getCurrReg()

unsigned int LegViewer::getCurrReg() const
{
  if(m_curr_swimset >= m_swimsets.size())
    return(0);

  return(m_swimsets[m_curr_swimset].getTotalReg());
}

// ----------------------------------------------------------
// Procedure: getCurrUnreg()

unsigned int LegViewer::getCurrUnreg() const
{
  if(m_curr_swimset >= m_swimsets.size())
    return(0);

  return(m_swimsets[m_curr_swimset].getTotalUnreg());
}

// ----------------------------------------------------------
// Procedure: getCurrMinSep()

double LegViewer::getCurrMinSep() const
{
  if(m_curr_swimset >= m_swimsets.size())
    return(0);

  return(m_swimsets[m_curr_swimset].getMinSep());
}

// ----------------------------------------------------------
// Procedure: getColorByIndex()

string LegViewer::getColorByIndex() const
{
  if(m_curr_swimset == 0)
    return("lime");
  else if(m_curr_swimset == 1)
    return("red");
  else if(m_curr_swimset == 2)
    return("dodger_blue");
  else if(m_curr_swimset == 3)
    return("yellow");
  else if(m_curr_swimset == 4)
    return("olive");
  else if(m_curr_swimset == 5)
    return("orange");
  else if(m_curr_swimset == 6)
    return("pink");
  else if(m_curr_swimset == 7)
    return("magenta");

  return("white");
}
