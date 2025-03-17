/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: PointAssignEval.h                               */
/*    DATE: March 17th, 2025                                */
/************************************************************/

#ifndef POINT_ASSIGN_EVAL_HEADER
#define POINT_ASSIGN_EVAL_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class PointAssignEval : public AppCastingMOOSApp
{
public:
  PointAssignEval();
  ~PointAssignEval() {};
  
 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();
  
protected:
  void registerVariables();
  
private: // Configuration variables
  unsigned int m_pt_postings_expected;
  
private: // State variables
  std::set<std::string> m_postings;
  
};

#endif 
