/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SwimFldGenerator.h                                   */
/*    DATE: Apr 2nd, 2022                                       */
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

#ifndef SWIM_FIELD_GENERATOR_HEADER
#define SWIM_FIELD_GENERATOR_HEADER

#include <string>
#include "XYFieldGenerator.h"
#include "XYPolygon.h"

class SwimFldGenerator
{
 public:
  SwimFldGenerator();
  virtual ~SwimFldGenerator() {}

  bool   setSwimmerAmt(std::string);
  bool   setUnregAmt(std::string);
  bool   setBufferDist(std::string);

  bool   addPolygon(std::string s) {return(m_generator.addPolygon(s));}

  bool   generate();
  bool   generate_aux(double=1);

 protected: // Config variables
  unsigned int m_swimmer_amt;
  unsigned int m_unreg_amt;
  double       m_buffer_dist;

 protected: // State variables
  XYFieldGenerator  m_generator;
};

#endif 








