/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT                                             */
/*    FILE: FrontEstimate.h                                 */
/*    DATE: May 2021                                        */
/************************************************************/

#ifndef FRONT_ESTIMATE_HEADER
#define FRONT_ESTIMATE_HEADER

#include <string> 

class FrontEstimate 
{
 public:
  FrontEstimate() {
    r_offset = 0;
    r_angle  = 0;
    r_amplitude = 0;
    r_period = 0;
    r_wavelength = 0;
    r_alpha = 0;
    r_beta  = 0;
    r_T_N = 0;
    r_T_S = 0;
  }
  
  ~FrontEstimate(){};

  std::string r_vname;
  double r_offset;
  double r_angle;
  double r_amplitude;
  double r_period;
  double r_wavelength;
  double r_alpha;
  double r_beta;
  double r_T_N;
  double r_T_S;

  double r_score;
  double r_error;
  double r_elapsed;
};

#endif 
