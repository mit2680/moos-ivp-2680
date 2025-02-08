/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SwimFldGenerator.cpp                                 */
/*    DATE: Apr 2nd, 2022                                        */
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

#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "SwimFldGenerator.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

SwimFldGenerator::SwimFldGenerator()
{
  m_swimmer_amt = 1;
  m_unreg_amt   = 0;
  m_buffer_dist = 10;
}

//---------------------------------------------------------
// Procedure: setSwimmerAmt()

bool SwimFldGenerator::setSwimmerAmt(string amt)
{
  return(setUIntOnString(m_swimmer_amt, amt));
}

//---------------------------------------------------------
// Procedure: setUnregAmt()

bool SwimFldGenerator::setUnregAmt(string amt)
{
  return(setUIntOnString(m_unreg_amt, amt));
}

//---------------------------------------------------------
// Procedure: setBufferDist()

bool SwimFldGenerator::setBufferDist(string str)
{
  return(setNonNegDoubleOnString(m_buffer_dist, str));
}

//---------------------------------------------------------
// Procedure: generate()

bool SwimFldGenerator::generate()
{
  // =========================================================
  // Part 1: Sanity checks and Generate Points
  // =========================================================
  // Sanity check 1
  unsigned int total = m_swimmer_amt + m_unreg_amt;
  if(total == 0) {
    cout << "No objects requested. No objects generated." << endl;
    return(false);
  }
  // Sanity check 2
  if(m_generator.size() == 0) {
    cout << "No region specified. No objects generated" << endl;
    return(false);
  }
    
  // Seed the random number generator
  unsigned long tseed = time(NULL)+1;
  unsigned long pid = (long)getpid()+1;
  unsigned long seed = (tseed%999999);
  seed = ((rand())*seed)%999999;
  seed = (seed*pid)%999999;
  srand(seed);
    


  
  m_generator.setSnap(1);
  if(total > 50)
    m_generator.setSnap(0.1);  
  
  // Configure the generator and generate points
  m_generator.setBufferDist(m_buffer_dist);
  m_generator.setFlexBuffer(true);
  m_generator.generatePoints(total);
  vector<XYPoint> points = m_generator.getPoints();
  if(points.size() != total)
    return(false);


  // =========================================================
  // Part 2: Produce output from generated points
  // =========================================================
  double nearest = m_generator.getGlobalNearest();
  cout << "// Lowest dist between swimmers: ";
  cout << doubleToString(nearest,2) << endl;
  for(unsigned int i=0; i<m_generator.size(); i++) {
    string poly_spec = m_generator.getPolygon(i).get_spec(4);
    cout << "poly = " << poly_spec << endl;
  }
  
  // Output any and all swimmers (not unregistered swimmers)
  for(unsigned int i=0; i<m_swimmer_amt; i++) {
    string swimmer_name = "p";
    if(i+1<10)
      swimmer_name += "0";
    swimmer_name += uintToString(i+1);
    double xval = points[i].get_vx();
    double yval = points[i].get_vy();
    if(m_unreg_amt > 0)
      cout << "swimmer = type=reg, name=" << swimmer_name;
    else
      cout << "swimmer = name=" << swimmer_name;
    cout << ", x=" << doubleToStringX(xval,2);
    cout << ", y=" << doubleToStringX(yval,2) << endl;
  }
  
  // Output any and all unregistered swimmers
  for(unsigned int i=m_swimmer_amt; i<points.size(); i++) {
    string unreg_name = "x";
    unsigned int index = (i-m_swimmer_amt)+1;
    if(index < 10)
      unreg_name += "0";
    unreg_name += uintToString(index);
    double xval = points[i].get_vx();
    double yval = points[i].get_vy();
    cout << "swimmer = type=unreg, name=" << unreg_name;
    cout << ", x=" << doubleToStringX(xval,2);
    cout << ", y=" << doubleToStringX(yval,2) << endl;
  }
  
  return(true);
}


