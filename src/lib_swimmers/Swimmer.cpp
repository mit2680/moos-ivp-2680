/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Swimmer.cpp                                     */
/*    DATE: Apr 2nd, 2022                                   */
/************************************************************/

#include <vector>
#include <iterator>
#include "MBUtils.h"
#include "Swimmer.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

Swimmer::Swimmer(string sname)
{
  m_start_x = 0;
  m_start_y = 0;
  m_curr_x  = 0;
  m_curr_y  = 0;

  m_time_enter   = 0;
  m_time_rescued = 0;

  m_state = "swimming";
  m_type  = "reg";
  m_name  = sname;

  m_rescue_tries = 0;
  m_scout_tries  = 0;
}

//---------------------------------------------------------
// Procedure: initXY()

void Swimmer::initXY(double x, double y)
{
  m_start_x = x;
  m_start_y = y;
  m_curr_x  = x;
  m_curr_y  = y;
}

//---------------------------------------------------------
// Procedure: setState()

bool Swimmer::setState(string str)
{
  str = tolower(str);
  if((str != "swimming") && (str != "rescued"))
    return(false);

  m_state = str;
  return(true);
}

//---------------------------------------------------------
// Procedure: setType()

bool Swimmer::setType(string str)
{
  str = tolower(str);
  if((str == "person") || (str == "reg"))
    m_type = "reg";
  else if((str == "floater") || (str == "unreg"))
    m_type = "unreg";
  else
    return(false);

  return(true);
}

//---------------------------------------------------------
// Procedure: hasBeenScouted()
//      Note: If no vehicle name provided, then the question
//            is whether *anyone* has scouted this swimmer.
//            Otherwise the question is whether this swimmer
//            has been scouted by the given vname.

bool Swimmer::hasBeenScouted(string vname) const
{
  if(vname == "")
    return(m_set_scouted.size() != 0);
    
  if(m_set_scouted.count(vname) == 0)
    return(false);
  
  return(true);
}

//---------------------------------------------------------
// Procedure: getSpec()

string Swimmer::getSpec() const
{
  string spec = "type=" + m_type + ", name=" + m_name;
  if(m_start_x != 0)
    spec += ", start_x=" + doubleToStringX(m_start_x,2);
  if(m_start_y != 0)
    spec += ", start_y=" + doubleToStringX(m_start_y,2);
  if(m_curr_x != 0)
    spec += ", curr_x=" + doubleToStringX(m_curr_x,2);
  if(m_curr_y != 0)
    spec += ", curr_y=" + doubleToStringX(m_curr_y,2);
  if(m_time_enter != 0)
    spec += ", time_enter=" + doubleToStringX(m_time_enter,2);
  if(m_time_rescued != 0)
    spec += ", time_rescued=" + doubleToStringX(m_time_rescued,2);
  if(m_state != "")
    spec += ", state=" + m_state;
  if(m_savior != "")
    spec += ", savior=" + m_savior;
  if(m_id != "")
    spec += ", id=" + m_id;
  if(m_rescue_tries != 0)
    spec += ", rescue_tries=" + uintToString(m_rescue_tries);
  if(m_scout_tries != 0)
    spec += ", scout_tries=" + uintToString(m_scout_tries);

  return(spec);
}


//---------------------------------------------------------
// Procedure: stringToSwimmer()

Swimmer stringToSwimmer(std::string str)
{
  Swimmer null_swimmer;
  Swimmer swimmer;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    double dval  = atof(value.c_str());

    if(param == "start_x")
      swimmer.setStartX(dval);
    else if(param == "start_y")
      swimmer.setStartY(dval);

    else if(param == "x") {
      swimmer.setStartX(dval);
      swimmer.setCurrX(dval);
    }
    else if(param == "y") {
      swimmer.setStartY(dval);
      swimmer.setCurrY(dval);
    }
    
    else if(param == "curr_x")
      swimmer.setCurrX(dval);
    else if(param == "curr_y")
      swimmer.setCurrY(dval);
    else if(param == "time_enter")
      swimmer.setTimeEnter(dval);
    else if(param == "time_rescued")
      swimmer.setTimeRescued(dval);

    else if(param == "type")
      swimmer.setType(value);
    else if(param == "name")
      swimmer.setName(value);
    else if(param == "id")
      swimmer.setID(value);
    else if(param == "state")
      swimmer.setState(value);
    else if(param == "savior")
      swimmer.setSavior(value);
    else if(param == "rescue_tries")
      swimmer.setRescueTries((unsigned int)(dval));
    else if(param == "scout_tries")
      swimmer.setScoutTries((unsigned int)(dval));
  }

  return(swimmer);
}

