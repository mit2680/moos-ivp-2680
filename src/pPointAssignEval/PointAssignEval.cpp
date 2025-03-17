/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: PointAssignEval.cpp                             */
/*    DATE: March 17th, 2025                                */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "ACTable.h"
#include "PointAssignEval.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

PointAssignEval::PointAssignEval()
{
  m_pts_expected = 100;
  m_first_received = false;
  m_last_received  = false;

  m_result_posted  = false;
}

//---------------------------------------------------------
// Procedure: OnNewMail()

bool PointAssignEval::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key    = msg.GetKey();
    string sval  = msg.GetString(); 

#if 0 // Keep these around just for template
    string comm  = msg.GetCommunity();
    double dval  = msg.GetDouble();
    string msrc  = msg.GetSource();
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
#endif

    if(key == "VISIT_POINT") {
      if(sval == "firstpoint")
	m_first_received = true;
      else if(sval == "lastpoint")
	m_last_received = true;
      else 
	m_postings.insert(sval);
    }
    
    else if(key != "APPCAST_REQ") // handled by AppCastingMOOSApp
      reportRunWarning("Unhandled Mail: " + key);
  }
  
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer()

bool PointAssignEval::OnConnectToServer()
{
   registerVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool PointAssignEval::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(!m_result_posted) {
    if(m_postings.size() >= m_pts_expected) {
      Notify("MISSION_EVALUATED", "true");
      Notify("MISSION_SUCCESS", "true");
      m_result_posted = true;
    }
  }
  
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool PointAssignEval::OnStartUp()
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
    if(param == "pts_expected")
      handled = setUIntOnString(m_pts_expected, value);

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  registerVariables();	
  return(true);
}

//---------------------------------------------------------
// Procedure: registerVariables()

void PointAssignEval::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("VISIT_POINT", 0);
}


//------------------------------------------------------------
// Procedure: buildReport()

bool PointAssignEval::buildReport() 
{
  m_msgs << "Config:                                     " << endl;
  m_msgs << "  pts_expected: " << m_pts_expected << endl;
  m_msgs << endl;
  m_msgs << "Status:                                     " << endl;
  m_msgs << "  first_received: " << boolToString(m_first_received) << endl;
  m_msgs << "   last_received: " << boolToString(m_last_received) << endl;
  m_msgs << "    pts_received: " << m_postings.size() << endl;

  return(true);
}




