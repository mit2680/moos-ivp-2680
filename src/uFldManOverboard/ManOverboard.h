/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: ManOverboard.h                                  */
/*    DATE: Feb 19th, 2022                                  */
/************************************************************/

#ifndef MAN_OVERBOARD_HEADER
#define MAN_OVERBOARD_HEADER

#include <map>
#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "NodeRecord.h"
#include "XYPolygon.h"
#include "Swimmer.h"

class ManOverboard : public AppCastingMOOSApp
{
 public:
  ManOverboard();
  ~ManOverboard() {};
  
 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  
 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();
  void registerVariables();
  
 protected:
  bool handleConfigSwimFile(std::string);
  bool handleConfigBlockApps(std::string);
  
  bool handleMailFoundSwimmer(std::string);
  void updateSwimmerPositions();
  bool checkBlockApps(std::string);
  void postMetrics();
  void postVisuals();
 
  void postPendingMobAlerts();
  void postMobUpdates();
  
 private: // Configuration variables

  XYPolygon    m_drop_region;
  bool         m_show_region;

  std::string  m_swim_file;

  std::set<std::string>  m_block_apps;
  
 private: // State variables  
  
  // Map keyed on swimmer name
  std::map<std::string, Swimmer> m_map_swimmer;
  std::map<std::string, bool>    m_map_alerted;

  std::set<std::string> m_set_swimmers;
  std::set<std::string> m_set_finders;

  double m_show_region_tstamp;
};

#endif 
