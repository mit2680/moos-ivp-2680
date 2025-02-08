/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SwimmerSet.cpp                                       */
/*    DATE: March 3rd 2022                                       */
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

#include <iterator>
#include <algorithm>
#include <cmath>
#include "SwimmerSet.h"
#include "ColorParse.h"
#include "MBUtils.h"
#include "FileBuffer.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

SwimmerSet::SwimmerSet()
{
  m_total_reg   = 0;
  m_total_unreg = 0;

  m_max_size = 99;
  shuffleIDs();
}

//---------------------------------------------------------
// Procedure: handleSwimFile()

bool SwimmerSet::handleSwimFile(string str, double curr_time,
				string& warning)
{
  vector<string> lines = fileBuffer(str);
  if(lines.size() == 0) {
    warning = "File not found, or empty: " + str;
    return(false);
  }
  
  // Part 1: Parse all the lines
  for(unsigned int i=0; i<lines.size(); i++) {
    string orig = lines[i];
    string line = stripBlankEnds(stripComment(lines[i], "//"));
    if(line == "")
      continue;
    
    string param = biteStringX(line, '=');
    string value = line;
    
    if(param == "swimmer") {
      Swimmer swimmer = stringToSwimmer(value);
      string  sname   = swimmer.getName();
      if(m_map_swimmers.count(sname) != 0) {
	warning = "Bad SwimFile Line: " + orig;
	return(false);
      }
      if(swimmer.getType() == "reg")
	m_total_reg++;
      else
	m_total_unreg++;
      
      swimmer.setTimeEnter(curr_time);
      tagSwimmerID(swimmer);
      m_map_swimmers[sname] = swimmer;
    }
    else if((param == "region") || (param == "poly")) {
      m_rescue_region = string2Poly(value);
      m_rescue_region.set_color("edge", "gray90");
      m_rescue_region.set_color("vertex", "dodger_blue");
      m_rescue_region.set_vertex_size(5);
      if(!m_rescue_region.is_convex()) {
	warning = "Bad SwimFile Line: " + orig;
	return(false);
      }
    }
  }

  m_swim_file = str;
  
  return(true);
}

//---------------------------------------------------------
// Procedure: getSwimmers()

vector<Swimmer> SwimmerSet::getSwimmers() const
{
  vector<Swimmer> svector;

  map<string, Swimmer>::const_iterator p;
  for(p=m_map_swimmers.begin(); p!=m_map_swimmers.end(); p++) {
    Swimmer swimmer = p->second;
    svector.push_back(swimmer);
  }

  return(svector);
}

//---------------------------------------------------------
// Procedure: getSwimFileSpec()

vector<string> SwimmerSet::getSwimFileSpec() const
{
  vector<string> svector;

  svector.push_back("poly = " + m_rescue_region.get_spec_pts());

  map<string, Swimmer>::const_iterator p;
  for(p=m_map_swimmers.begin(); p!=m_map_swimmers.end(); p++) {
    string  name = p->first;
    Swimmer swimmer = p->second;
    double  start_x = swimmer.getStartX();
    double  start_y = swimmer.getStartX();
    string  x_str = doubleToStringX(start_x);
    string  y_str = doubleToStringX(start_y);

    string line = "swimmer = name=" + name;
    line += ", x=" + x_str + ", y=" + y_str;
    svector.push_back(line);
  }

  return(svector);
}

//---------------------------------------------------------
// Procedure: swimmerAlert()
//   Example: type=reg, x=2, y=3, name=joe
//            type=unreg, x=2, y=3
//      Note: XSWIMMER_ALERT 

bool SwimmerSet::swimmerAlert(string str, double curr_time,
			      string& warning)
{
  string sname = tokStringParse(str, "name");
  string stype = tokStringParse(str, "type");
  double x = tokDoubleParse(str, "x");
  double y = tokDoubleParse(str, "y");  

  return(addSwimmer(sname, stype, x, y, curr_time, warning));
}


//---------------------------------------------------------
// Procedure: addSwimmer()

bool SwimmerSet::addSwimmer(string sname, string stype,
			    double x, double y,
			    double curr_time,
			    string& warning)
{
  if((stype != "reg") && (stype != "unreg")) {
    warning = "Swimmer Alert with unknown type:[" + sname + "]";
    return(false);
  }
  if((sname != "") && (m_map_swimmers.count(sname))) {
    warning = "Swimmer Alert with duplicate name:" + sname;
    return(false);
  }
  
  if(sname == "") {
    if(stype == "reg") {
      m_total_reg++;
      sname = "p" + uintToString(m_total_reg, 2);
    }
    else {
      m_total_unreg++;
      sname = "x" + uintToString(m_total_unreg, 2);
    }
  }
  
  Swimmer new_swimmer(sname);
  new_swimmer.initXY(x, y);
  new_swimmer.setType(stype);
  new_swimmer.setTimeEnter(curr_time);
  tagSwimmerID(new_swimmer);
  
  m_map_swimmers[sname] = new_swimmer;
  return(true);
}



//---------------------------------------------------------
// Procedure: getNameClosestSwimmer()
//   Returns: The name of swimmer closest to x/y location

string SwimmerSet::getNameClosestSwimmer(double x, double y,
					 double min_range)
{
  // Sanity check
  if(m_map_swimmers.size() == 0)
    return("");
  
  string closest_sname;
  double closest_range = -1;
  
  map<string, Swimmer>::iterator p;
  for(p=m_map_swimmers.begin(); p!=m_map_swimmers.end(); p++) {
    string  sname   = p->first;
    Swimmer swimmer = p->second;
    if(swimmer.getState() == "rescued")
      continue;

    double  curr_x = swimmer.getCurrX();
    double  curr_y = swimmer.getCurrY();
    double  range = hypot(x-curr_x, y-curr_y);
    if(range > min_range)
      continue;
    
    if((closest_range < 0) || (range < closest_range)) {
      closest_sname = sname;
      closest_range = range;
    }
  }

  return(closest_sname);
}

//------------------------------------------------------------
// Procedure: getSwimmerNames()

set<string> SwimmerSet::getSwimmerNames() const
{
  set<string> swimmer_names;
  
  map<string, Swimmer>::const_iterator p;
  for(p=m_map_swimmers.begin(); p!=m_map_swimmers.end(); p++) {
    string  sname = p->first;
    swimmer_names.insert(sname);
  }
  return(swimmer_names);
}

//------------------------------------------------------------
// Procedure: modSwimmer()

bool SwimmerSet::modSwimmer(Swimmer swimmer)
{
  string sname = swimmer.getName();
  if(m_map_swimmers.count(sname) == 0)
    return(false);

  m_map_swimmers[sname] = swimmer;
  return(true);
}
  
//------------------------------------------------------------
// Procedure: hasSwimmer()

bool SwimmerSet::hasSwimmer(string sname) const
{
  if(m_map_swimmers.count(sname) == 0)
    return(false);
  return(true);
}
  
//------------------------------------------------------------
// Procedure: getSwimmer()

Swimmer SwimmerSet::getSwimmer(string sname) const
{
  Swimmer null_swimmer;
  if(m_map_swimmers.count(sname) == 0)
    return(null_swimmer);
  
  return(m_map_swimmers.at(sname));
}

//------------------------------------------------------------
// Procedure: hasSwimmerByID()

bool SwimmerSet::hasSwimmerByID(string id) const
{
  if(m_map_swimmer_ids.count(id) == 0)
    return(false);
  return(true);
}
  
//------------------------------------------------------------
// Procedure: getSwimmerByID()

Swimmer SwimmerSet::getSwimmerByID(string id) const
{
  Swimmer null_swimmer;
  if(m_map_swimmer_ids.count(id) == 0)
    return(null_swimmer);

  string sname = m_map_swimmer_ids.at(id);
  if(m_map_swimmers.count(sname) == 0)
    return(null_swimmer);

  return(m_map_swimmers.at(sname));
}

//------------------------------------------------------------
// Procedure: shuffleIDs()

void SwimmerSet::shuffleIDs()
{
  m_shuffled_ids.clear();
  for(unsigned int i=0; i<m_max_size; i++)
    m_shuffled_ids.push_back(i);

  random_shuffle(m_shuffled_ids.begin(), m_shuffled_ids.end());
}


//------------------------------------------------------------
// Procedure: tagSwimmerID()

void SwimmerSet::tagSwimmerID(Swimmer& swimmer)
{
  // If swimmers are maxed out, id is just incremental
  string new_id;
  if(m_map_swimmers.size() >= m_shuffled_ids.size())
    new_id = "id" + uintToString(m_map_swimmers.size(), 2);
  else {
    unsigned int next_ix = m_map_swimmers.size();
    new_id = "id" + uintToString(m_shuffled_ids[next_ix], 2);
  }
  
  swimmer.setID(new_id);
  m_map_swimmer_ids[new_id] = swimmer.getName();
}


//------------------------------------------------------------
// Procedure: getSavedSwimmerCnt()

unsigned int SwimmerSet::getSavedSwimmerCnt(string vname) const
{
  if(vname == "")
    return(0);

  vname = tolower(vname);
  
  unsigned int total = 0;
  map<string, Swimmer>::const_iterator p;
  for(p=m_map_swimmers.begin(); p!=m_map_swimmers.end(); p++) {
    Swimmer swimmer = p->second;
    if(vname == tolower(swimmer.getSavior()))
      total++;
  }
  
  return(total);
}


//------------------------------------------------------------
// Procedure: getKnownSwimmerCnt()
//      Note: Known swimmers are swimmers that were either of
//            type "reg" originally, or "unreg" swimmers that
//            were scouted by one or more vehicles.
//      Note: This criteria may be used to help determine the
//            end of the competition. 

unsigned int SwimmerSet::getKnownSwimmerCnt() const
{
  unsigned int total = 0;

  map<string, Swimmer>::const_iterator p;
  for(p=m_map_swimmers.begin(); p!=m_map_swimmers.end(); p++) {
    Swimmer swimmer = p->second;
    if(swimmer.getType() == "reg")
      total++;
    else if(swimmer.getScoutSet().size() != 0)
      total++;
  }
  
  return(total);
}


//------------------------------------------------------------
// Procedure: allPersonsRescued() 
//   Purpose: Determine if all persons, registered or not, have
//            been rescued. 

bool SwimmerSet::allPersonsRescued() const
{
  map<string, Swimmer>::const_iterator p;
  for(p=m_map_swimmers.begin(); p!=m_map_swimmers.end(); p++) {
    Swimmer swimmer = p->second;
    if(swimmer.getSavior() == "")
      return(false);
  }
  
  return(true);
}


//------------------------------------------------------------
// Procedure: allRegPersonsRescued()
//   Purpose: Determine if all known persons, e.g., all registered
//            persons, have been rescued. 

bool SwimmerSet::allRegPersonsRescued() const
{
  map<string, Swimmer>::const_iterator p;
  for(p=m_map_swimmers.begin(); p!=m_map_swimmers.end(); p++) {
    Swimmer swimmer = p->second;
    if(swimmer.getType() == "reg") {
      if(swimmer.getSavior() == "")
	return(false);
    }
  }
  
  return(true);
}


