/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: ManOverboard.cpp                                */
/*    DATE: Feb 19th, 2022                                  */
/************************************************************/

#include <vector>
#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "ManOverboard.h"
#include "XYFormatUtilsPoly.h"
#include "XYFieldGenerator.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------
// Constructor

ManOverboard::ManOverboard()
{
  m_show_region = false;
  m_show_region_tstamp = -1;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool ManOverboard::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval   = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    bool handled = false;
    if(key == "FOUND_SWIMMER") 
      handled = handleMailFoundSwimmer(sval);
    else if(key == "DB_CLIENTS") 
      handled = checkBlockApps(sval);
    
    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool ManOverboard::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool ManOverboard::Iterate()
{
  AppCastingMOOSApp::Iterate();

  postPendingMobAlerts();
  
  updateSwimmerPositions();
  postMobUpdates();
  postVisuals();
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool ManOverboard::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if(param == "swim_file") 
      handled = handleConfigSwimFile(value);
    else if(param == "show_region") 
      handled = setBooleanOnString(m_show_region, value);
    else if(param == "block_on") 
      handled = handleConfigBlockApps(value);

    if(!handled)
      reportUnhandledConfigWarning(orig);

  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables

void ManOverboard::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("FOUND_SWIMMER", 0);
  Register("DB_CLIENTS", 0);
} 
 
//---------------------------------------------------------
// Procedure: handleConfigSwimFile()

bool ManOverboard::handleConfigSwimFile(string str)
{
  vector<string> lines = fileBuffer(str);
  if(lines.size() == 0) {
    cout << "File not found, or empty file: " << str << endl;
    return(false);
  }
  
  set<string>     swimmer_names;
  vector<Swimmer> swimmers;
  XYPolygon       drop_region;
  
  // Part 1: Parse all the lines
  for(unsigned int i=0; i<lines.size(); i++) {
    string orig = lines[i];
    string line = lines[i];
    if(strBegins(line, "//"))
      continue;
    if(stripBlankEnds(line) == "")
      continue;

    string param = biteStringX(line, '=');
    string value = line;
    bool   malconfig = false;
    
    if(param == "swimmer") {
      Swimmer swimmer = stringToSwimmer(value);
      string  sname  = swimmer.getName();
      if(swimmer_names.count(sname) != 0)
	malconfig = true;
      swimmer.setTimeEnter(m_curr_time);
      swimmer_names.insert(sname);
      swimmers.push_back(swimmer);
    }
    else if((param == "region") || (param == "poly")) {
      drop_region = string2Poly(value);
      if(!drop_region.is_convex())
	malconfig = true;
    }
    else
      malconfig = true;


    if(malconfig) {
      string warning = "Unhandled SwimFile Line: " + orig;
      reportConfigWarning(warning);
      cout << warning << endl;
      return(false);
    }
  }

  // Part 2: Assign the contents to local member variables.
  for(unsigned int i=0; i<swimmers.size(); i++) {
    string sname = swimmers[i].getName();
    m_map_swimmer[sname] = swimmers[i];
    m_map_alerted[sname] = false;
  }

  m_set_swimmers = swimmer_names;
  m_swim_file    = str;
  m_drop_region  = drop_region;
  
  return(true);
}

//----------------------------------------------------------------
// Procedure: handleConfigBlockApps()

bool ManOverboard::handleConfigBlockApps(string block_apps)
{
  vector<string> svector = parseString(block_apps, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string block_app = stripBlankEnds(svector[i]);
    if(strContainsWhite(block_app))
      return(false);
    if(m_block_apps.count(block_app) > 0)
      return(false);
    m_block_apps.insert(block_app);
  }
  return(true);
}

//----------------------------------------------------------------
// Procedure: checkBlockApps()
//   Purpose: go through the current list of MOOSDB clients and if any
//            app is on the block list, remove it from the block list.
//   Returns: true if NO block apps remain

bool ManOverboard::checkBlockApps(string db_clients)
{
  if(m_block_apps.size() == 0)
    return(true);

  vector<string> svector = parseString(db_clients, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    if(m_block_apps.count(svector[i])) {
      m_block_apps.erase(svector[i]);
    }
  }
  return(m_block_apps.size() == 0);
}


//---------------------------------------------------------
// Procedure: handleMailFoundSwimmer()
//   Example: FOUND_SWIMMER = finder=abe, swimmer_name=theo

bool ManOverboard::handleMailFoundSwimmer(string str)
{
  string finder = "unknown";
  string sname;
  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    if(param == "swimmer_name")
      sname = value;
    else if(param == "finder")
      finder = value;
  }

  if(sname == "") {
    reportRunWarning("Un-identified found swimmer");
    return(false);
  }    
  if(m_set_swimmers.count(sname) == 0) {
    reportRunWarning("Swimmer name unknown");
    return(false);
  }
  if(m_map_swimmer[sname].getSavior() != "nobody") {
    reportRunWarning("Swimmer doubly found");
    return(false);
  }

  m_set_finders.insert(finder);
  
  double total_time_in_water = MOOSTime() - m_map_swimmer[sname].getTimeEnter();
  
  m_map_swimmer[sname].setState("found");
  m_map_swimmer[sname].setTimeFound(total_time_in_water);
  m_map_swimmer[sname].setSavior(finder);

  return(true);
}


//------------------------------------------------------------
// Procedure: postMetrics()
//   Purpose: Produce two metric reports. One system report for
//            all vehicles, and one individual report for all
//            vehicles that have found at least one swimmer
//
//   Summary: "total_found=2, total_unfound=1, ftime=239.2, ftime=34.2"
//    Report: "finder=abe, total_found=1, ftime=34.2"

void ManOverboard::postMetrics()
{
  //==========================================================
  // Part 1: Build the overall report for all finding vehicles
  //==========================================================
  unsigned int total_found = 0;
  unsigned int total_unfound = 0;

  string summary;
  
  set<string>::iterator p;
  for(p=m_set_swimmers.begin(); p!=m_set_swimmers.end(); p++) {
    string sname = *p;
    if(m_map_swimmer[sname].getState() == "found") {
      total_found++;
      double time_in_water = m_map_swimmer[sname].getTimeFound();
      summary += ", ftime=" + doubleToStringX(time_in_water,1);
    }
    else
      total_unfound++;
  }
  summary = ", total_unfound=" + uintToString(total_unfound) + summary;
  summary = "total_found=" + uintToString(total_found) + summary;
  Notify("MOB_SUMMARY", summary);
  
  
  //==========================================================
  // Part 2: Build report for each finding vehicle
  //==========================================================
  for(p=m_set_finders.begin(); p!=m_set_finders.end(); p++) {
    string report;
    unsigned int total_found = 0;
    string fname = *p;
    set<string>::iterator q;
    for(q=m_set_swimmers.begin(); q!=m_set_swimmers.end(); q++) {
      string sname = *q;
      if(m_map_swimmer[sname].getSavior() == fname) {
	total_found++;
	double time_in_water = m_map_swimmer[sname].getTimeFound();
	report += ", ftime=" + doubleToStringX(time_in_water,1);
      }
    }
    report = "total_found=" + uintToString(total_found) + report;
    Notify("MOB_REPORT", report);
  }
}

//------------------------------------------------------------
// Procedure: postPendingMobAlerts()

void ManOverboard::postPendingMobAlerts()
{
  return;
  if(m_block_apps.size() > 0)
    return;
  
  set<string>::const_iterator p;
  for(p=m_set_swimmers.begin(); p!=m_set_swimmers.end(); p++) {
    string sname = *p;
    if(m_map_alerted[sname])
      continue;

    string msg = "swimmer_name=" + sname;
    msg += ", utc=" + doubleToStringX(m_curr_time,2);
    msg += ", x=" + doubleToStringX(m_map_swimmer[sname].getStartX());
    msg += ", y=" + doubleToStringX(m_map_swimmer[sname].getStartY());
    Notify("MOB_ALERT", msg);
    m_map_alerted[sname] = true;
  }
}

//------------------------------------------------------------
// Procedure: postMobUpdates()

void ManOverboard::postMobUpdates()
{
  set<string>::const_iterator p;
  for(p=m_set_swimmers.begin(); p!=m_set_swimmers.end(); p++) {
    string sname = *p;
    string msg = "swimmer_name=" + sname;
    msg += ", utc=" + doubleToStringX(m_curr_time,2);
    msg += ", x=" + doubleToStringX(m_map_swimmer[sname].getCurrX());
    msg += ", y=" + doubleToStringX(m_map_swimmer[sname].getCurrY());
    Notify("MOB_UPDATE", msg);
  }
}

//------------------------------------------------------------
// Procedure: postVisuals()

void ManOverboard::postVisuals()
{
  if(m_show_region) {
    double delta = m_curr_time - m_show_region_tstamp;
    if(delta > 60) {
      m_drop_region.set_label("drop_region_mob");
      m_drop_region.set_vertex_color("off");
      m_drop_region.set_label_color("off");
      m_drop_region.set_edge_color("yellow");
      string spec = m_drop_region.get_spec();
      Notify("VIEW_POLYGON", spec);
    }
  }
}

//------------------------------------------------------------
// Procedure: updateSwimmerPositions()

void ManOverboard::updateSwimmerPositions()
{
  // For now the swimmers don't move from the start positions
  set<string>::iterator p;
  for(p=m_set_swimmers.begin(); p!=m_set_swimmers.end(); p++) {
    string sname = *p;
    m_map_swimmer[sname].setCurrX(m_map_swimmer[sname].getCurrX());
    m_map_swimmer[sname].setCurrY(m_map_swimmer[sname].getCurrY());
  }
}


//------------------------------------------------------------
// Procedure: buildReport()

bool ManOverboard::buildReport() 
{
  string poly_spec  = m_drop_region.get_spec();
  string swim_count = uintToString(m_map_swimmer.size());
  
  m_msgs << "Configuration: " << endl;
  m_msgs << "  Swimmer cnt: " << swim_count << endl;
  m_msgs << "  Drop Region: " << poly_spec  << endl;
  m_msgs << "    Swim File: " << m_swim_file << endl;
  
  ACTable actab(6);
  actab << "Swimmer | Type | Pos | State | Finder | Time";
  actab.addHeaderLines();

  set<string>::iterator p;
  for(p=m_set_swimmers.begin(); p!=m_set_swimmers.end(); p++) {
    string sname = *p;
    string swm_type  = m_map_swimmer[sname].getType();
    string swm_state = m_map_swimmer[sname].getState();

    double xpos = m_map_swimmer[sname].getCurrX();
    double ypos = m_map_swimmer[sname].getCurrY();
    string pos = doubleToStringX(xpos,1) + "," + doubleToStringX(ypos,1);
    
    string savior = "-";
    double duration = m_curr_time - m_map_swimmer[sname].getTimeEnter();
    if(swm_state == "found") {
      duration = m_map_swimmer[sname].getTimeFound();
      savior   = m_map_swimmer[sname].getSavior();
    }
    string duration_str = doubleToStringX(duration,1);

    actab << sname << swm_type << pos << swm_state << savior << duration_str;
  }
  
  m_msgs << actab.getFormattedString();

  return(true);
}




