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

Swimmer::Swimmer()
{
  m_start_x = 0;
  m_start_y = 0;
  m_curr_x  = 0;
  m_curr_y  = 0;

  m_time_enter = 0;
  m_time_found = 0;

  m_name  = "unknown";
  m_type  = "person";
  m_state = "swimming";
}

//---------------------------------------------------------
// Procedure: setState()

bool Swimmer::setState(string str)
{
  str = tolower(str);
  if((str != "swimming") && (str != "found"))
    return(false);

  m_state = str;
  return(true);
}

//---------------------------------------------------------
// Procedure: setType()

bool Swimmer::setType(string str)
{
  str = tolower(str);
  if((str != "person") && (str != "object"))
    return(false);

  m_type = str;
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
  if(m_time_found != 0)
    spec += ", time_found=" + doubleToStringX(m_time_found,2);
  if(m_state != "")
    spec += ", state=" + m_state;
  if(m_savior != "")
    spec += ", savior=" + m_savior;

  return(spec);
}


//---------------------------------------------------------
// Procedure: strinToSwimmer()

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
    else if(param == "time_found")
      swimmer.setTimeFound(dval);

    else if(param == "type")
      swimmer.setType(value);
    else if(param == "name")
      swimmer.setName(value);
    else if(param == "state")
      swimmer.setState(value);
    else if(param == "savior")
      swimmer.setSavior(value);
  }

  return(swimmer);
}

