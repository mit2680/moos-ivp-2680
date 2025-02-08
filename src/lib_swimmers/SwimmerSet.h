/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SwimmerSet.h                                         */
/*    DATE: March 3rd, 2022                                      */
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

#ifndef SWIMMER_SET_HEADER
#define SWIMMER_SET_HEADER

#include <map>
#include <string>
#include "XYMarker.h"
#include "XYPolygon.h"
#include "Swimmer.h"

class SwimmerSet
{
 public:
  SwimmerSet();
  virtual ~SwimmerSet() {}

  bool handleSwimFile(std::string, double, std::string& warning);
  bool swimmerAlert(std::string, double, std::string& warning);
  bool modSwimmer(Swimmer);

  std::string getNameClosestSwimmer(double x, double y, double rng=10);
  
  bool    hasSwimmer(std::string sname) const;
  Swimmer getSwimmer(std::string sname) const;

  bool    hasSwimmerByID(std::string id) const;
  Swimmer getSwimmerByID(std::string id) const;

  bool    addSwimmer(std::string name, std::string stype,
		     double xpos, double ypos,
		     double curr_time,
		     std::string& warning);
  
  std::set<std::string> getSwimmerNames() const;

  XYPolygon   getRescueRegion() const {return(m_rescue_region);}
  std::string getSwimFile() const   {return(m_swim_file);}

  void tagSwimmerID(Swimmer& swimmer);

  unsigned int size() const {return(m_map_swimmers.size());}
  unsigned int getSavedSwimmerCnt(std::string vname) const;
  unsigned int getKnownSwimmerCnt() const;
  unsigned int getTotalReg() const {return(m_total_reg);}
  unsigned int getTotalUnreg() const {return(m_total_unreg);}
  unsigned int getMinSep() const {return(0);}
  
  bool allPersonsRescued() const;
  bool allRegPersonsRescued() const;
  
  std::vector<Swimmer> getSwimmers() const;
  
  std::vector<std::string> getSwimFileSpec() const;
  
 protected:
  void shuffleIDs();
  
 protected: // State variables

  // Map Swimmer Name to Swimmers
  std::map<std::string, Swimmer>     m_map_swimmers;

  // Map Swimmer ID to Swimmer Name
  std::map<std::string, std::string> m_map_swimmer_ids;
  
  unsigned int m_total_reg;
  unsigned int m_total_unreg;

  std::vector<int> m_shuffled_ids;
  
 protected: // Configuration variables

  std::string m_swim_file;
  XYPolygon   m_rescue_region;

  unsigned int m_max_size;
};

#endif 
