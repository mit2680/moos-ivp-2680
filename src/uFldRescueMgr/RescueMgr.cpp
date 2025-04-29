/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng, MIT Cambridge MA             */
/*    FILE: RescueMgr.cpp                                        */
/*    DATE: Feb 18th 2022                                        */
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
#include <cmath>
#include "RescueMgr.h"
#include "VarDataPairUtils.h"
#include "MacroUtils.h"
#include "NodeRecordUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "ColorParse.h"
#include "XYCircle.h"
#include "XYMarker.h"
#include "MBUtils.h"
#include "ACTable.h"
#include "FileBuffer.h"
#include "XYFormatUtilsPoly.h"
#include "XYPolyExpander.h"

using namespace std;

//---------------------------------------------------------
// Constructor

RescueMgr::RescueMgr()
{
  // Config vars
  m_rescue_rng_min = 25;
  m_rescue_rng_max = 40;
  m_rescue_rng_pd  = 0.5;

  m_rescue_rng_transparency = 0.1;
  m_rescue_rng_show = true;
  m_finish_upon_win = false;
  m_swimmer_color   = "dodger_blue";

  // State vars
  m_finished       = false;
  m_scouts_inplay  = false;
  m_total_rescuers = 0;
  m_last_broadcast = 0;
  m_vname_leader   = "tie";

  m_known_unrescued = 0;
  
  m_ac.setMaxEvents(20);
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool RescueMgr::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    //p->Trace();
    CMOOSMsg msg = *p;
	
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
    string comm  = msg.GetCommunity();

    bool handled = false;
    string warning;
    if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL"))
      handled = handleMailNodeReport(sval);
    else if(key == "RESCUE_REQUEST")
      handled = handleMailRescueRequest(sval);
    else if(key == "SCOUT_REQUEST")
      handled = handleMailScoutRequest(sval);
    else if((key == "XSWIMMER_ALERT") && (comm == "shoreside")) {
      handled = m_swimset.swimmerAlert(sval, m_curr_time, warning);
      updateFinishStatus();
      postSwimMarkers();
    }
    else if((key == "XFOUND_SWIMMER") && (comm == "shoreside")) {
      string xstr = tokStringParse(sval, "x");
      string ystr = tokStringParse(sval, "y");
      if((xstr == "") || (ystr == ""))
	handled = false;
      else {
	double xval = atof(xstr.c_str());
	double yval = atof(ystr.c_str());
	string sname = m_swimset.getNameClosestSwimmer(xval, yval);
	declareRescuedSwimmer("nature", sname);
	postSwimMarkers();
	handled = true;
      }
    }
  
    if(!handled) {
      reportRunWarning("Unhandled mail: " + key);
      reportRunWarning(warning);
    }
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool RescueMgr::OnConnectToServer()
{
  registerVariables();  
  return(true);
}


//------------------------------------------------------------
// Procedure: registerVariables()

void RescueMgr::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  Register("XSWIMMER_ALERT", 0);
  Register("XFOUND_SWIMMER", 0);
  Register("NODE_REPORT", 0);
  Register("NODE_REPORT_LOCAL", 0);
  Register("RESCUE_REQUEST", 0);
  Register("SCOUT_REQUEST", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool RescueMgr::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(!m_finished) {
    tryRescues();
    tryScouts();
  }
  
  postRescueRngPolys();
  postScoutRngPolys();

  // periodically broadcast swimmer info to all vehicles
  if((m_curr_time - m_last_broadcast) > 15) {
    broadcastSwimmers();
    applyTMateColors();
    m_last_broadcast = m_curr_time;
  }
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool RescueMgr::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();
  
  STRING_LIST sParams;
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams)) 
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig   = *p;
    string line   = *p;
    string param  = biteStringX(line, '=');
    string value  = line;

    bool handled = false;
    string warning;
    if(param == "swim_file") {
      handled = m_swimset.handleSwimFile(value, m_curr_time, warning);
      Notify("PLOGGER_CMD", "COPY_FILE_REQUEST=" + value);
      updateFinishStatus();
    }
    else if(param == "show_rescue_rng")
      handled = setBooleanOnString(m_rescue_rng_show, value);
    else if(param == "rescue_rng_min")
      handled = handleConfigRescueRangeMin(value);
    else if(param == "rescue_rng_max")
      handled = handleConfigRescueRangeMax(value);
    else if(param == "rescue_rng_pd")
      handled = handleConfigRescueRangePd(value);
    else if((param == "rescue_rng_transparency") && isNumber(value))
      handled = setNonNegDoubleOnString(m_rescue_rng_transparency, value);
    else if(param == "winner_flag") 
      handled = addVarDataPairOnString(m_winner_flags, value);
    else if(param == "leader_flag") 
      handled = addVarDataPairOnString(m_leader_flags, value);
    else if(param == "finish_flag") 
      handled = addVarDataPairOnString(m_finish_flags, value);
    else if(param == "swimmer_color") 
      handled = setColorOnString(m_swimmer_color, value);
    else if(param == "finish_upon_win") 
      handled = setBooleanOnString(m_finish_upon_win, value);

    if(!handled) {
      reportUnhandledConfigWarning(orig);
      if(warning != "")
	reportUnhandledConfigWarning(warning);
    }
  }
  postSwimMarkers();

  srand(time(NULL));
  
  registerVariables();

  return(true);
}

//------------------------------------------------------------
// Procedure: handleConfigRescueRangeMin()

bool RescueMgr::handleConfigRescueRangeMin(string str)
{
  if(!isNumber(str))
    return(false);

  m_rescue_rng_min = atof(str.c_str());

  if(m_rescue_rng_min < 0)
    m_rescue_rng_min = 0;
  if(m_rescue_rng_max <= m_rescue_rng_min)
    m_rescue_rng_max = m_rescue_rng_min + 1;

  return(true);
}
    
//------------------------------------------------------------
// Procedure: handleConfigSensorRangeMax()

bool RescueMgr::handleConfigRescueRangeMax(string str)
{
  if(!isNumber(str))
    return(false);

  m_rescue_rng_max = atof(str.c_str());

  if(m_rescue_rng_max < 0)
    m_rescue_rng_max = 0;
  if(m_rescue_rng_min >= m_rescue_rng_max)
    m_rescue_rng_min = m_rescue_rng_max * 0.9;
  
  return(true);
}
    
//------------------------------------------------------------
// Procedure: handleConfigRescueRangePd()

bool RescueMgr::handleConfigRescueRangePd(string str)
{
  if(!isNumber(str))
    return(false);

  m_rescue_rng_pd = atof(str.c_str());

  if(m_rescue_rng_pd < 0)
    m_rescue_rng_pd = 0;
  if(m_rescue_rng_pd > 1)
    m_rescue_rng_pd = 1;
  
  return(true);
}


//---------------------------------------------------------
// Procedure: handleMailNodeReport()
//   Example: NAME=alpha,TYPE=KAYAK,UTC_TIME=1267294386.51,
//            X=29.66,Y=-23.49,LAT=43.825089, LON=-70.330030, 
//            SPD=2.00, HDG=119.06,YAW=119.05677,DEPTH=0.00,     
//            LENGTH=4.0,MODE=ENGAGED

bool RescueMgr::handleMailNodeReport(const string& node_report_str)
{
  NodeRecord new_record = string2NodeRecord(node_report_str);

  if(!new_record.valid()) {
    Notify("SWM_DEBUG", "Invalid incoming node report");
    reportRunWarning("ERROR: Unhandled node record");
    return(false);
  }

  // In case there is an outstanding RunWarning indicating the lack
  // of a node report for a given vehicle, retract it here. This is 
  // mostly a startup timing issue. Sometimes a sensor request is 
  // received before a node report. Only a problem if the node report
  // never comes. Once we get one, it's no longer a problem.
  string vname = new_record.getName();
  retractRunWarning("No NODE_REPORT received for " + vname);

  if(m_map_node_records.count(vname) == 0)
    broadcastSwimmers();
  
  m_map_node_records[vname] = new_record;  

  return(true);
}

//---------------------------------------------------------
// Procedure: tryScouts()

void RescueMgr::tryScouts()
{
  // For each vehicle, check if pending scout actions are to be applied
  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string vname = p->first;
    tryScoutsVName(vname);
  }
}

//---------------------------------------------------------
// Procedure: tryScoutsVName()

void RescueMgr::tryScoutsVName(string vname)
{
  // If vehicle has not posted a scout request recently, then the
  // scout ability is off for this vehicle.
  double elapsed_req = m_curr_time - m_map_node_last_scout_req[vname];
  if(elapsed_req > 5)
    return;
  double elapsed_try = m_curr_time - m_map_node_last_scout_try[vname];
  if(elapsed_try < 1.5)
    return;
  if(m_map_node_vroles[vname] != "scout")
    return;
  
  m_map_node_last_scout_try[vname] = m_curr_time;
  m_map_node_scout_tries[vname]++;
  
  set<string> swimmer_names = m_swimset.getSwimmerNames();
  set<string>::iterator p;
  for(p=swimmer_names.begin(); p!=swimmer_names.end(); p++) {
    string sname = *p;
    tryScoutsVNameSwimmer(vname, sname);
  }
}

//---------------------------------------------------------
// Procedure: tryScoutsVNameSwimmer()

void RescueMgr::tryScoutsVNameSwimmer(string vname, string sname)
{
  Swimmer swimmer = m_swimset.getSwimmer(sname);
  if(swimmer.getState() == "rescued")
    return;
  
  bool result = rollDice(vname, sname, "scout");
  if(result)
    declareScoutedSwimmer(vname, sname);  
}

//---------------------------------------------------------
// Procedure: tryRescues()

void RescueMgr::tryRescues()
{
  map<string, NodeRecord>::iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string vname = p->first;
    tryRescuesVName(vname);
  }
}


//---------------------------------------------------------
// Procedure: tryRescuesVName()

void RescueMgr::tryRescuesVName(string vname)
{
  // If vehicle has has not posted a rescue request recently, then the
  // rescue ability is off for this vehicle.
  double elapsed_req = m_curr_time - m_map_node_last_rescue_req[vname];
  if(elapsed_req > 5)
    return;
  double elapsed_try = m_curr_time - m_map_node_last_rescue_try[vname];
  if(elapsed_try < 1)
    return;
  if(m_map_node_vroles[vname] != "rescue")
    return;

  m_map_node_last_rescue_try[vname] = m_curr_time;
  m_map_node_rescue_tries[vname]++;
  
  set<string> swimmer_names = m_swimset.getSwimmerNames();
  set<string>::iterator p;
  for(p=swimmer_names.begin(); p!=swimmer_names.end(); p++) {
    string sname = *p;
    tryRescuesVNameSwimmer(vname, sname);
  }
}


//---------------------------------------------------------
// Procedure: tryRescuesVNameSwimmer()

void RescueMgr::tryRescuesVNameSwimmer(string vname, string sname)
{
  Swimmer swimmer = m_swimset.getSwimmer(sname);
  if(swimmer.getState() == "rescued")
    return;
  
  bool detect_result = rollDice(vname, sname, "rescue");
  if(detect_result) 
    declareRescuedSwimmer(vname, sname);  
}

//------------------------------------------------------------
// Procedure: updateLeaderStatus()

void RescueMgr::updateLeaderStatus()
{
  // Part 1: Note prev leader to detect a lead change
  string prev_leader = m_vname_leader;
  
  // Part 2: Calc highest number of rescues over any vehicle
  unsigned int highest_rescue_count = 0;
  map<string, unsigned int>::iterator p;
  for(p=m_map_node_rescues.begin(); p!=m_map_node_rescues.end(); p++) {
    unsigned int rescues = p->second;
    if(rescues > highest_rescue_count)
      highest_rescue_count = rescues;
  }

  // Part 3: Calc vector of vnames having highest rescue count
  vector<string> leader_vnames;
  for(p=m_map_node_rescues.begin(); p!=m_map_node_rescues.end(); p++) {
    string vname = p->first;
    unsigned int rescues = p->second;
    if(rescues == highest_rescue_count)
      leader_vnames.push_back(vname);
  }

  // Part 4: Set the new leader or update leader to tie status
  if(leader_vnames.size() == 1)
    m_vname_leader = leader_vnames[0];
  else
    m_vname_leader = "tie";

  // Part 5: If no change, we're done. Otherwise make postings
  if(m_vname_leader == prev_leader)
    return;

  Notify("UFRM_LEADER", m_vname_leader);
  postFlags(m_leader_flags);
}

//------------------------------------------------------------
// Procedure: updateWinnerStatus()

void RescueMgr::updateWinnerStatus(bool finished)
{
  // Once a winner always a winner
  if(m_vname_winner != "")
    return;

  // Part 2: Determine the threshold for winning
  unsigned int known_swimmer_cnt = m_swimset.getKnownSwimmerCnt();
  double       win_thresh = (double)(known_swimmer_cnt) / 2;

  // Part 3: Calc vector of vnames having reached the win threshold
  // Possibly >1 winner for now. Will handle tie-breaker afterwards.
  vector<string> winner_vnames;
  map<string, unsigned int>::iterator p;
  for(p=m_map_node_rescues.begin(); p!=m_map_node_rescues.end(); p++) {
    string vname = p->first;
    unsigned int rescues = p->second;
    if(rescues >=  win_thresh)
      winner_vnames.push_back(vname);
  }

  // Part 4: If no winners then done for now
  if(winner_vnames.size() == 0) {
    Notify("UFRM_WINNER", "pending");
    return;
  }

  string would_be_winner;
  
  // Part 5: If one winner, set the winner
  if(winner_vnames.size() == 1)
    would_be_winner = winner_vnames[0];

  // Part 6: If multiple vnames meeting win threshold, do tiebreaker
  if(winner_vnames.size() > 1) {
    string first_winner;
    double first_winner_utc = 0;
    map<string, double>::iterator q;
    for(q=m_map_node_last_rescue_utc.begin();
	q!=m_map_node_last_rescue_utc.end(); q++) {
      string vname = q->first;
      double utc = q->second;
      if((first_winner == "") || (utc < first_winner_utc)) {
	first_winner = vname;
	first_winner_utc = utc;
      }
    }
    would_be_winner = first_winner;
  }

  // If this competition has scout vehicles, and we're not yet
  // finished, then hold off on declaring a winner.
  if(m_scouts_inplay && !finished && !m_finish_upon_win)
    return;

  m_vname_winner = would_be_winner;
  Notify("UFRM_WINNER", m_vname_winner);
  postFlags(m_winner_flags);
}

//------------------------------------------------------------
// Procedure: updateFinishStatus()
//   Purpose: Completion is when all KNOWN swimmers have been
//            rescued. A swimmer is known if either (a) it is
//            a registered swimmer known at the outset, or (b)
//            it has been scouted.
//      Note: It is possible that after completion, further
//            vehicles could become scouted, but scouting will
//            be disabled, once the complete state has been
//            reached.

void RescueMgr::updateFinishStatus()
{
  // Once we are finished, we are always finished
  if(m_finished)
    return;
  
  set<string> snames = m_swimset.getSwimmerNames();
  if(snames.size() == 0)
    return;

  // Assume finished is true until we find unrescued swimmer
  m_known_unrescued = 0;
  set<string>::iterator p;
  for(p=snames.begin(); p!=snames.end(); p++) {
    string sname = *p;
    Swimmer swimmer = m_swimset.getSwimmer(sname);
    bool swimmer_is_known = false;

    if(swimmer.getType() == "reg")
      swimmer_is_known = true;
    else {
      if(swimmer.hasBeenScouted())
	swimmer_is_known = true;
    }
    
    if(swimmer_is_known && (swimmer.getState() != "rescued"))
      m_known_unrescued++;
  }

  bool finished = false;  
  // First and most general criteria for finishing is when all
  // known swimmers have been rescued
  if(m_known_unrescued == 0)
    finished = true;

  // Second criteria if no scouts in play, and a majority has been
  // rescued by one team (winner declared), AND finish_upon_win is
  // set to true, then we can finish.
  if(m_finish_upon_win && (m_vname_winner != "") && !m_scouts_inplay)
    finished = true;
  if(!finished)
    return;
      
    
  m_finished = true;  
  Notify("UFRM_FINISHED", boolToString(m_finished));
  postFlags(m_finish_flags);

  updateWinnerStatus(true);
}

//------------------------------------------------------------
// Procedure: rollDice()
//   
//   
// 1.0 ^       sensor_rng_min       sensor_rng_max
//     |
//     |            |                 |
// Pd  |------------o                 |
//     |            |  \              |
//     |            |     \           |
//     |            |        \        |
//     |            |           \     |
//     |            |              \  |
//     o------------------------------o----------------------------->
//         range from swimmer to ownship
//   

bool RescueMgr::rollDice(string vname, string sname, string dtype)
{
  // Part 1: Sanity checking
  if(!m_swimset.hasSwimmer(sname))
    return(false);
  if(m_map_node_records.count(vname)==0)
    return(false);

  Swimmer swimmer = m_swimset.getSwimmer(sname);

  // Part 2: Calculated the range to swimmer
  double vx = m_map_node_records[vname].getX();
  double vy = m_map_node_records[vname].getY();
  double sx = swimmer.getCurrX();
  double sy = swimmer.getCurrY();
  double range_to_swimmer = hypot((vx-sx), (vy-sy));
  
  // Part 3: Calculate Pd threshold modified by range to swimmer
  int    rand_int  = rand() % 10000;
  double dice_roll = (double)(rand_int) / 10000;

  double pd = m_rescue_rng_pd;
  if(range_to_swimmer >= m_rescue_rng_max)
    pd = 0;
  else if(range_to_swimmer >= m_rescue_rng_min) {
    double pct = range_to_swimmer - m_rescue_rng_min;
    pct = pct / (m_rescue_rng_max - m_rescue_rng_min);
    pd = pct * pd;
  }
  
  if(range_to_swimmer <= m_rescue_rng_max) {
    if(dtype == "rescue")
      swimmer.incRescueTries();
    else if(dtype == "scout")
      swimmer.incScoutTries();
  }
  m_swimset.modSwimmer(swimmer);
  
  // Apply the dice role to the Pd
  if(dice_roll >= pd)
    return(false);

  return(true);
}

//---------------------------------------------------------
// Procedure: handleMailRescueRequest()
//   Example: vname=alpha

bool RescueMgr::handleMailRescueRequest(string request)
{
  string vname = tokStringParse(request, "vname");

  if(vname == "")
    return(reportRunWarning("Rescue request with no vname"));

  // Ensure this vname has not previously been a scout vehicle
  if(m_map_node_vroles.count(vname)) {
    if(m_map_node_vroles[vname] != "rescue") 
      return(reportRunWarning(vname + " is rescue double-agent"));
  }
  else {
    m_map_node_vroles[vname] = "rescue";
    m_total_rescuers++;
  }
    
  m_map_node_rescue_reqs[vname]++;
  m_map_node_last_rescue_req[vname] = MOOSTime();
  return(true);
}

//---------------------------------------------------------
// Procedure: handleMailScoutRequest()
//   Example: vname=cal, tmate=abe

bool RescueMgr::handleMailScoutRequest(string request)
{
  string vname = tokStringParse(request, "vname");
  string tmate = tokStringParse(request, "tmate");

  // Sanity Check 1: check for empty vname or tname
  if(vname == "") 
    return(reportRunWarning("Scout request with no vname"));
  if(tmate == "") 
    return(reportRunWarning("Scout request with no teammate name"));

  m_scouts_inplay = true;
  m_finish_upon_win = true;
  
  // Sanity Check 2: check vname has not before been a rescue vehicle
  if(m_map_node_vroles.count(vname)) {
    if(m_map_node_vroles[vname] != "scout") 
      return(reportRunWarning(vname + " is scout double-agent"));
  }
  else
    m_map_node_vroles[vname] = "scout";

  // Sanity Check 3: check vname has had different teammate before
  if(m_map_node_tmate.count(vname)) {
    if(m_map_node_tmate[vname] != tmate) 
      return(reportRunWarning(vname + " is disloyal scout"));
  }
  else
    m_map_node_tmate[vname] = tmate;
  

  m_map_node_scout_reqs[vname]++;
  m_map_node_last_scout_req[vname] = MOOSTime();
  return(true);
}

//------------------------------------------------------------
// Procedure: declareRescuedSwimmer()
//     Notes: Example postings:
//            RESCUED_SWIMMER = id=s1, finder=abe
//            FOUND_SWIMMER = id=s1, finder=abe   (deprecated)

void RescueMgr::declareRescuedSwimmer(string vname, string sname) 
{
  // Part 1: Sanity check
  if(!m_swimset.hasSwimmer(sname))
    return;

  // Part 2: Update the notables data structures to support calc
  // of leader differentials
  addNotable(vname, sname);

  // Part 3: Update the simmer status, mark the savior. Note the
  // check for swimmer being not yet rescued was done earlier
  Swimmer swimmer = m_swimset.getSwimmer(sname);  
  swimmer.setState("rescued");
  swimmer.setSavior(vname);
  m_swimset.modSwimmer(swimmer);

  // Part 4: Update the rescue stats for this vehicle
  m_map_node_rescues[vname]++;
  m_map_node_last_rescue_utc[vname] = m_curr_time;

  // Part 5: Update the leader, winner and finish status 
  updateLeaderStatus();
  updateWinnerStatus();
  updateFinishStatus();

  // Part 6: Generate postings, visuals and events
  reportEvent("Swimmer " + sname + " has been rescued by " + vname + "!");
  
  postSwimMarkers();
  
  string idstr = m_swimset.getSwimmer(sname).getID();
  idstr = findReplace(idstr, "id", "");  
  string msg = "id=" + idstr + ", finder=" + vname;
  Notify("RESCUED_SWIMMER", msg);  
  Notify("FOUND_SWIMMER", msg);  // (deprecated)
}

//------------------------------------------------------------
// Procedure: declareScoutedSwimmer()
//     Notes: Example postings:
//            SCOUTED_SWIMMER_AB = id=s1, x=23, y=34

void RescueMgr::declareScoutedSwimmer(string vname, string sname) 
{
  if(!m_swimset.hasSwimmer(sname))
    return;

  Swimmer swimmer = m_swimset.getSwimmer(sname);
  if(swimmer.getType() != "unreg")
    return;
  if(swimmer.hasBeenScouted(vname))
    return;
  
  swimmer.addScouted(vname);
  m_swimset.modSwimmer(swimmer);
  
  m_map_node_scouts[vname]++;
  reportEvent("Swimmer " + sname + " has been scouted by " + vname + "!");

  postSwimMarkers();
  
  string idstr = m_swimset.getSwimmer(sname).getID();
  idstr = findReplace(idstr, "id", "");
  
  string msg = "id=" + idstr;
  msg += ", x=" + doubleToString(swimmer.getCurrX(),2);
  msg += ", y=" + doubleToString(swimmer.getCurrY(),2);
  Notify("SCOUTED_SWIMMER_" + toupper(vname), msg);  
}

//------------------------------------------------------------
// Procedure: postRescueRngPolys()

void RescueMgr::postRescueRngPolys()
{
  if(!m_rescue_rng_show)
    return;
  
  map<string, double>::iterator p;
  for(p=m_map_node_last_rescue_req.begin();
      p!=m_map_node_last_rescue_req.end(); p++) {
  
    string vname = p->first;
    double last_req = p->second;
    double elapsed = m_curr_time - last_req;
    if(elapsed < 3)
      postRangePolys(vname, "rescue", true);
    else
      postRangePolys(vname, "rescue", false);
  }
}

//------------------------------------------------------------
// Procedure: postScoutRngPolys()

void RescueMgr::postScoutRngPolys()
{
  if(!m_rescue_rng_show)
    return;
  
  map<string, double>::iterator p;
  for(p=m_map_node_last_scout_req.begin();
      p!=m_map_node_last_scout_req.end(); p++) {
  
    string vname = p->first;
    double last_req = p->second;
    double elapsed = m_curr_time - last_req;
    if(elapsed < 3)
      postRangePolys(vname, "scout", true);
    else
      postRangePolys(vname, "scout", false);
  }
}

//------------------------------------------------------------
// Procedure: postRangePolys()

void RescueMgr::postRangePolys(string vname, string tag, bool active)
{
  if(m_map_node_records.count(vname) == 0)
    return;
  
  double x = m_map_node_records[vname].getX();
  double y = m_map_node_records[vname].getY();

  XYCircle circ(x, y, m_rescue_rng_max);
  circ.set_label("sensor_max_" + tag + "_" + vname);
  circ.set_active(active);
  circ.set_vertex_color("off");
  circ.set_label_color("off");
  circ.set_edge_color("off");
  circ.set_color("fill", "white");
  circ.set_transparency(m_rescue_rng_transparency);
  
  string spec1 = circ.get_spec();
  Notify("VIEW_CIRCLE", spec1);

  circ.set_label("sensor_min_" + tag + "_" + vname);
  circ.setRad(m_rescue_rng_min);
  string spec2 = circ.get_spec();
  Notify("VIEW_CIRCLE", spec2);
}

//------------------------------------------------------------
// Procedure: broadcastSwimmers()
//   Example: SWIMMER_ALERT = x=34, y=85, id=21

void RescueMgr::broadcastSwimmers()
{
  map<string, NodeRecord>::const_iterator p;
  for(p=m_map_node_records.begin(); p!=m_map_node_records.end(); p++) {
    string vname = p->first;
    string var = "SWIMMER_ALERT_" + toupper(vname);
    
    set<string> swimmers = m_swimset.getSwimmerNames();
    set<string>::const_iterator q;
    for(q=swimmers.begin(); q!=swimmers.end(); q++) {
      string  sname = *q;
      Swimmer swimmer = m_swimset.getSwimmer(sname);
      string  stype = swimmer.getType();
      if(stype == "reg") {
	string  id_str = swimmer.getID();
	id_str = findReplace(id_str, "id", "");  // convert "id23" ot "23"
	string msg = "x=" + doubleToStringX(swimmer.getCurrX(),1);
	msg += ", y=" + doubleToStringX(swimmer.getCurrY(),1);
	msg += ", id=" + id_str;
	Notify(var, msg);
      }
    }
  }
}


//------------------------------------------------------------
// Procedure: applyTMateColors()

void RescueMgr::applyTMateColors()
{
  map<string, string>::iterator p;  
  for(p=m_map_node_tmate.begin(); p!=m_map_node_tmate.end(); p++) {
    string scout_vname = p->first;
    string rescue_vname = p->second;

    if(m_map_node_records.count(rescue_vname)) {
      string rescue_color = m_map_node_records[rescue_vname].getColor();
      Notify("NODE_COLOR_CHANGE_" + toupper(scout_vname), rescue_color);
    }
  }
}


//------------------------------------------------------------
// Procedure: postSwimMarkers()

void RescueMgr::postSwimMarkers()
{
  set<string> swimmer_names = m_swimset.getSwimmerNames();
  set<string>::iterator p;
  for(p=swimmer_names.begin(); p!=swimmer_names.end(); p++) {
    string sname = *p;
    postSwimMarker(sname);
  }
  
  XYPolygon poly = m_swimset.getRescueRegion();
  if(poly.is_convex()) {
    Notify("VIEW_POLYGON", poly.get_spec());
    Notify("RESCUE_REGION", poly.get_spec());
  }
}

//------------------------------------------------------------
// Procedure: postSwimMarker()

void RescueMgr::postSwimMarker(string sname)
{
  if(!m_swimset.hasSwimmer(sname))
    return;

  Swimmer swimmer = m_swimset.getSwimmer(sname);
  string  stype   = swimmer.getType();
  string  savior  = swimmer.getSavior();
  
  bool notable = isNotable(sname);
  
  XYMarker marker;
  marker.set_label(sname);
  marker.set_type("triangle");
  marker.set_vx(swimmer.getCurrX());
  marker.set_vy(swimmer.getCurrY());
  marker.set_width(3);
  marker.set_edge_color("green");
  marker.set_transparency(0.3);
  
  if((stype == "reg") || (savior != "")) {
    string marker_color = m_swimmer_color;
    string savior = swimmer.getSavior();
    if((savior != "") && (savior != "nature")) {
      marker_color = m_map_node_records[savior].getColor();
      if(!notable)
	marker.set_transparency(0.7);
      else
	marker.set_transparency(0.1);
    }
    if(savior == "nature")
      marker.set_color("primary_color", "gray50");
    else
      marker.set_color("primary_color", marker_color);
  }
  else {
    string color1 = "gray50";
    string color2 = "gray50";
    set<string> scouts = swimmer.getScoutSet();
    if((scouts.size() == 1) || (scouts.size() == 2)) {
      set<string>::iterator p;
      for(p=scouts.begin(); p!=scouts.end(); p++) {
	string scout = *p;
	string scout_tmate = m_map_node_tmate[scout];
	string color = m_map_node_records[scout_tmate].getColor();
	if(isColor(color) && (color1 == "gray50"))
	  color1 = color;
	else if(isColor(color) && (color2 == "gray50"))
	  color2 = color;
      }
    }
    marker.set_type("efield");
    marker.set_color("primary_color", color1);
    marker.set_color("secondary_color", color2);
      
    
#if 0
    marker.set_type("circle");
    marker.set_color("primary_color", "gray50");
#endif
  }

  Notify("VIEW_MARKER", marker.get_spec());
}

//------------------------------------------------------------
// Procedure: postFlags()

void RescueMgr::postFlags(const vector<VarDataPair>& flags)
{
  for(unsigned int i=0; i<flags.size(); i++) {
    VarDataPair pair = flags[i];
    string moosvar = pair.get_var();

    // If posting is a double, just post. No macro expansion
    if(!pair.is_string()) {
      double dval = pair.get_ddata();
      Notify(moosvar, dval);
    }
    // Otherwise if string posting, handle macro expansion
    else {
      string sval = pair.get_sdata();
      sval = macroExpand(sval, "LEADER", m_vname_leader);
      sval = macroExpand(sval, "WINNER", m_vname_winner);
      
      Notify(moosvar, sval);
    }
  }
}

//------------------------------------------------------------
// Procedure: addNotable()
//   Purpose: The notables map is where we keep track of (a)
//            the most recent swimmers rescued for each vehicle
//            When we have data for all vehicles, we use this
//            map to pop-off equal amounts of swimmers for each
//            vehicle until some vehicle is an empty list. This
//            way the remaining swimmers are the "notable" once
//            since they represent the most recent swimmers that
//            provide the leading vehicle with the lead.

void RescueMgr::addNotable(string vname, string sname)
{
  if(vname == "nature")
    return;

#if 0
  string stype = m_swimset.getSwimmer(sname).getType();
  if(m_swimset.getSwimmer(sname).getType() == "person") 
    m_map_notables[vname].push_front(sname);
  else
    return;
#endif
#if 1
  m_map_notables[vname].push_front(sname);
#endif

  
  bool some_empty = false;
  map<string, list<string> >::iterator p;
  for(p=m_map_notables.begin(); p!=m_map_notables.end(); p++) {
    if(p->second.size() == 0)
      some_empty = true;
  }

  if(some_empty)
    return;
  if(m_map_notables.size() < m_total_rescuers)
    return;
  if(m_map_notables.size() == 1)
    return;
  
  for(p=m_map_notables.begin(); p!=m_map_notables.end(); p++)
    p->second.pop_back();
}

//------------------------------------------------------------
// Procedure: isNotable()

bool RescueMgr::isNotable(string sname)
{
  map<string, list<string> >::iterator p;
  for(p=m_map_notables.begin(); p!=m_map_notables.end(); p++) {
    if(listContains(p->second, sname))
      return(true);
  }

  return(false);
}

//------------------------------------------------------------
// Procedure: buildReport()

bool RescueMgr::buildReport()
{
  string str_rng_min = doubleToStringX(m_rescue_rng_min,1);
  string str_rng_max = doubleToStringX(m_rescue_rng_max,1);
  string str_rng_pd  = doubleToStringX(m_rescue_rng_pd,2);
  string str_trans   = doubleToString(m_rescue_rng_transparency,2);
  
  m_msgs << "======================================" << endl;
  m_msgs << "RescueMgr Configuration "                 << endl;
  m_msgs << "======================================" << endl;
  m_msgs << "rescue_rng_min: " << str_rng_min << endl;
  m_msgs << "rescue_rng_max: " << str_rng_max << endl;
  m_msgs << "rescue_rng_pd:  " << str_rng_pd  << endl;
  m_msgs << "rescue_rng_show: " << boolToString(m_rescue_rng_show) << endl;
  m_msgs << "transparency:   " << str_trans            << endl;
  m_msgs << "swim_file:      " << m_swimset.getSwimFile()  << endl;
  m_msgs << endl;
  

  m_msgs << "======================================" << endl;
  m_msgs << "Vehicle Rescue Summary "                << endl;
  m_msgs << "======================================" << endl;

  ACTable actab = ACTable(7);
  actab << "Vehi | Rescue | Rescue | Swimmers | Scout | Scout | Swimmers";
  actab << "Name | Reqs   | Tries  | Rescued  | Reqs  | Tries | Scouted ";
  actab.addHeaderLines();

  string finished_str = boolToString(m_finished);
  finished_str += " (" + uintToString(m_known_unrescued) + " remaining)";
  
  m_msgs << "Total vehicles: " << m_map_node_records.size() << endl;
  m_msgs << "Leader vehicle: " << m_vname_leader << endl;     
  m_msgs << "Winner vehicle: " << m_vname_winner << endl;     
  m_msgs << "Mission Finished: " << finished_str << endl;     
  m_msgs << endl;
  
  map<string, NodeRecord>::iterator q;
  for(q=m_map_node_records.begin(); q!=m_map_node_records.end(); q++) {
    string vname    = q->first;
    string rs_reqs  = uintToString(m_map_node_rescue_reqs[vname]);
    string rs_tries = uintToString(m_map_node_rescue_tries[vname]);
    string rescues  = uintToString(m_map_node_rescues[vname]);
    string sc_reqs  = uintToString(m_map_node_scout_reqs[vname]);
    string sc_tries = uintToString(m_map_node_scout_tries[vname]);
    string scouts   = uintToString(m_map_node_scouts[vname]);
    actab << vname << rs_reqs << rs_tries << rescues;
    actab << sc_reqs << sc_tries << scouts;
  }
  m_msgs << actab.getFormattedString();

  m_msgs << endl << endl;
  m_msgs << "======================================" << endl;
  m_msgs << "Swimmer Summary "                       << endl;
  m_msgs << "======================================" << endl;
  actab = ACTable(9);  
  actab << "Name | ID | Type | Pos| State | Savior| Tries | Scouts | Time ";
  actab.addHeaderLines();

  set<string> swimmer_names = m_swimset.getSwimmerNames();
  
  set<string>::iterator p;
  for(p=swimmer_names.begin(); p!=swimmer_names.end(); p++) {
    string  sname   = *p;
    Swimmer swimmer = m_swimset.getSwimmer(sname);
    string  id    = swimmer.getID();
    string  stype = swimmer.getType();
    string  state = swimmer.getState();
    string  tries = uintToString(swimmer.getRescueTries());

    double xpos = swimmer.getCurrX();
    double ypos = swimmer.getCurrY();
    string pos = doubleToStringX(xpos,0) + "," + doubleToStringX(ypos,0);

    string savior = "-";
    double duration = m_curr_time - swimmer.getTimeEnter();
    if(state == "rescued") {
      duration = swimmer.getTimeRescued();
      savior   = swimmer.getSavior();
    }

    set<string> scout_set = swimmer.getScoutSet();
    string scouts = stringSetToString(scout_set);
    
    string dur_str = doubleToStringX(duration,1);
    
    actab << sname << id << stype << pos << state;
    actab << savior << tries << scouts << dur_str;
  }
  m_msgs << actab.getFormattedString();
  m_msgs << endl << endl;


  return(true);
}
