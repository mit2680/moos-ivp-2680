/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Swimmer.h                                       */
/*    DATE: Apr 2nd, 2022                                   */
/************************************************************/

#ifndef SWIMMER_HEADER
#define SWIMMER_HEADER

#include <string>

class Swimmer
{
 public:
  Swimmer();
  ~Swimmer() {}; 

  void   setStartX(double v) {m_start_x=v;}
  void   setStartY(double v) {m_start_y=v;}
  void   setCurrX(double v)  {m_curr_x=v;}
  void   setCurrY(double v)  {m_curr_y=v;}

  void   setTimeEnter(double v)  {m_time_enter=v;}
  void   setTimeFound(double v)  {m_time_found=v;}

  bool   setState(std::string s);
  void   setSavior(std::string s) {m_savior=s;}
  void   setName(std::string s)  {m_name=s;}
  bool   setType(std::string s);

  double getStartX() const  {return(m_start_x);}
  double getStartY() const  {return(m_start_y);}
  double getCurrX() const   {return(m_curr_x);}
  double getCurrY() const   {return(m_curr_y);}

  double getTimeEnter() const {return(m_time_enter);}
  double getTimeFound() const {return(m_time_found);}

  std::string getState() const  {return(m_state);}
  std::string getSavior() const {return(m_savior);}
  std::string getType() const   {return(m_type);}
  std::string getName() const   {return(m_name);}

  std::string getSpec() const;
  
 private:  
  double       m_start_x;
  double       m_start_y;
  double       m_curr_x;
  double       m_curr_y;
  double       m_time_enter;   // time overboard
  double       m_time_found;   // time recovered
  std::string  m_state;        // swimming or found
  std::string  m_savior;       // who recoverd
  std::string  m_type;         // person or object
  std::string  m_name;         // key identifier
};

Swimmer stringToSwimmer(std::string);

#endif 
