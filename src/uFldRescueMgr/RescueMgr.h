/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RescueMgr.h                                          */
/*    DATE: Feb 18th, 2022                                       */
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

#ifndef UFLD_SWIM_SENSOR_MOOSAPP_HEADER
#define UFLD_SWIM_SENSOR_MOOSAPP_HEADER

#include <map>
#include <string>
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "NodeRecord.h"
#include "SwimmerSet.h"
#include "VarDataPair.h"

class RescueMgr : public AppCastingMOOSApp
{
 public:
  RescueMgr();
  virtual ~RescueMgr() {}

 public: // Standard MOOSApp functions to overload
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload
  bool buildReport();
  void registerVariables();

 protected: // Configuration utility
  bool handleConfigRescueRangeMin(std::string);
  bool handleConfigRescueRangeMax(std::string);
  bool handleConfigRescueRangePd(std::string);

 protected: // Incoming mail utility
  bool handleMailNodeReport(const std::string&);
  bool handleMailRescueRequest(std::string);
  bool handleMailScoutRequest(std::string);

 protected: // Outgoing mail utility
  void declareRescuedSwimmer(std::string vname, std::string sname);
  void declareScoutedSwimmer(std::string vname, std::string sname);
  
 protected: // Utilities
  void  tryRescues();
  void  tryRescuesVName(std::string vname);
  void  tryRescuesVNameSwimmer(std::string vname, std::string sname);

  void  tryScouts();
  void  tryScoutsVName(std::string vname);
  void  tryScoutsVNameSwimmer(std::string vname, std::string sname);

  void  updateLeaderStatus();
  void  updateWinnerStatus(bool finished=false);
  void  updateFinishStatus();
  
  bool  rollDice(std::string vname, std::string, std::string dtype);

  void  postRescueRngPolys();
  void  postScoutRngPolys();
  void  postRangePolys(std::string vname, std::string tag, bool active);
  void  postSwimMarkers();
  void  postSwimMarker(std::string sname);

  void  postFlags(const std::vector<VarDataPair>& flags);
  void  broadcastSwimmers();

  void  applyTMateColors();
  
  void  addNotable(std::string vname, std::string sname);
  bool  isNotable(std::string sname);
  
 protected: // State variables

  SwimmerSet m_swimset;

  double     m_last_broadcast;
  
  // Key for each map below is the vehicle name. 
  std::map<std::string, NodeRecord>   m_map_node_records;
  std::map<std::string, std::string>  m_map_node_vroles;
  
  std::map<std::string, double>       m_map_node_last_rescue_req;
  std::map<std::string, double>       m_map_node_last_rescue_try;
  std::map<std::string, double>       m_map_node_last_rescue_utc;
  std::map<std::string, unsigned int> m_map_node_rescue_reqs;  
  std::map<std::string, unsigned int> m_map_node_rescue_tries;  
  std::map<std::string, unsigned int> m_map_node_rescues;

  std::map<std::string, double>       m_map_node_last_scout_req;
  std::map<std::string, double>       m_map_node_last_scout_try;
  std::map<std::string, unsigned int> m_map_node_scout_reqs;  
  std::map<std::string, unsigned int> m_map_node_scout_tries;  
  std::map<std::string, unsigned int> m_map_node_scouts;

  // Key for this map is the scout vname
  std::map<std::string, std::string>  m_map_node_tmate;
  
  
  // Notables map key is vname to list of recent swimmers rescued
  std::map<std::string, std::list<std::string> > m_map_notables;
  
  unsigned int m_total_rescuers;
  std::string  m_vname_leader;
  std::string  m_vname_winner;
  bool         m_finished;
  bool         m_scouts_inplay;
  
  unsigned int m_known_unrescued;
  
 protected: // Configuration variables

  std::vector<VarDataPair> m_winner_flags;
  std::vector<VarDataPair> m_leader_flags;
  std::vector<VarDataPair> m_finish_flags;
  
  double m_rescue_rng_min;
  double m_rescue_rng_max;
  double m_rescue_rng_pd;

  std::string m_swimmer_color;

  bool   m_finish_upon_win;
  
  bool   m_rescue_rng_show;
  double m_rescue_rng_transparency;
};

#endif 
