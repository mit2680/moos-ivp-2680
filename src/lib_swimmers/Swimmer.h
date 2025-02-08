/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Swimmer.h                                       */
/*    DATE: Apr 2nd, 2022                                   */
/************************************************************/

#ifndef SWIMMER_HEADER
#define SWIMMER_HEADER

#include <string>
#include <list>

class Swimmer
{
 public:
  Swimmer(std::string sname="");
  ~Swimmer() {}; 

  void   initXY(double, double);

 public: // Setters
  void   setStartX(double v) {m_start_x=v;}
  void   setStartY(double v) {m_start_y=v;}
  void   setCurrX(double v)  {m_curr_x=v;}
  void   setCurrY(double v)  {m_curr_y=v;}
    
  void   setTimeEnter(double v)   {m_time_enter=v;}
  void   setTimeRescued(double v) {m_time_rescued=v;}

  bool   setState(std::string s);
  void   setSavior(std::string s)       {m_savior=s;}
  bool   setType(std::string s);
  void   setName(std::string s)         {m_name=s;}
  void   setID(std::string s)           {m_id=s;}
  void   setRescueTries(unsigned int v) {m_rescue_tries=v;}
  void   setScoutTries(unsigned int v)  {m_scout_tries=v;}
  void   addScouted(std::string s)      {m_set_scouted.insert(s);}
  void   incRescueTries()               {m_rescue_tries++;}
  void   incScoutTries()                {m_scout_tries++;}
  
 public: // Getters
  double getStartX() const  {return(m_start_x);}
  double getStartY() const  {return(m_start_y);}
  double getCurrX() const   {return(m_curr_x);}
  double getCurrY() const   {return(m_curr_y);}

  double getTimeEnter() const {return(m_time_enter);}
  double getTimeRescued() const {return(m_time_rescued);}

  std::string  getState() const  {return(m_state);}
  std::string  getSavior() const {return(m_savior);}
  std::string  getType() const   {return(m_type);}
  std::string  getName() const   {return(m_name);}
  std::string  getID() const     {return(m_id);}
  unsigned int getRescueTries() const  {return(m_rescue_tries);}
  unsigned int getScoutTries() const   {return(m_scout_tries);}

  std::set<std::string> getScoutSet() {return(m_set_scouted);}
  bool hasBeenScouted(std::string vname="") const;
  
  std::string getSpec() const;
  
 private:  
  double       m_start_x;
  double       m_start_y;
  double       m_curr_x;
  double       m_curr_y;
  double       m_time_enter;   // time overboard
  double       m_time_rescued; // time rescued
  std::string  m_state;        // swimming or rescued
  std::string  m_savior;       // who rescued
  std::string  m_type;         // reg or unreg
  std::string  m_name;         // key identifier
  std::string  m_id;       

  std::set<std::string> m_set_scouted;
  
  unsigned int m_rescue_tries;
  unsigned int m_scout_tries;
};

Swimmer stringToSwimmer(std::string);

#endif 
