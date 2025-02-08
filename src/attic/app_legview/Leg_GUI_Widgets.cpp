/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: DPATH_GUI_Widgets.cpp                                */
/*    DATE: June 11th, 2023                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "Leg_GUI.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//--------------------------------------------------------------- 
// Procedure: initWidgets()     

void Leg_GUI::initWidgets()
{
  Fl_Color fcolor_blue  = fl_rgb_color(140, 140, 220);
  //Fl_Color fcolor_beige = fl_rgb_color(223, 219, 191);

  //=================================================
  // Main Ship Viewer
  //=================================================
  m_leg_viewer = new LegViewer(5, 30, w()-10, h()-60);
  m_mviewer    = m_leg_viewer;

  //=================================================
  // MID Panel
  //=================================================
  m_fld_sfile = new Fl_Output(0,0,1,1, "swim_file:"); 
  m_fld_sfile->color(fcolor_blue);
  m_fld_sfile->clear_visible_focus();

  m_fld_reg = new Fl_Output(0,0,1,1, "reg:"); 
  m_fld_reg->clear_visible_focus();

  m_fld_unreg = new Fl_Output(0,0,1,1, "unreg:"); 
  m_fld_unreg->clear_visible_focus();

  m_fld_minsep = new Fl_Output(0,0,1,1, "minsep:"); 
  m_fld_minsep->clear_visible_focus();
}

//---------------------------------------------------------------------- 
// Procedure: resizeWidgetsShape()     

void Leg_GUI::resizeWidgetsShape()
{
  int extra_wid = w() - m_start_wid;
  if(extra_wid < 0)
    extra_wid = 0;
  int field_hgt = 20;

  int row0 = h() - 25;
  int row1 = row0 + 25;

  int col1 = 70;
  int col2 = 250;
  int col3 = 400;
  int col4 = 550 + (0.25 * extra_wid);
  //int col5 = 460 + (0.50 * extra_wid);
  //int col6 = 660 + (0.75 * extra_wid);
  
  //===================================================
  // Main Ship Viewer
  //===================================================
  m_leg_viewer->resize(5, 30, w()-10, h()-60);

  //===================================================
  // Fields
  //===================================================
  int sfi_x = col1;
  int sfi_y = row0;
  int sfi_wid = 120;
  m_fld_sfile->resize(sfi_x, sfi_y, sfi_wid, field_hgt);

  int reg_x = col2;
  int reg_y = row0;
  int reg_wid = 70;
  m_fld_reg->resize(reg_x, reg_y, reg_wid, field_hgt);

  int unr_x = col3;
  int unr_y = row0;
  int unr_wid = 70;
  m_fld_unreg->resize(unr_x, unr_y, unr_wid, field_hgt);

  int msp_x = col4;
  int msp_y = row0;
  int msp_wid = 70;
  m_fld_minsep->resize(msp_x, msp_y, msp_wid, field_hgt);
}
  
//---------------------------------------------------------------------- 
// Procedure: resizeWidgetsText()

void Leg_GUI::resizeWidgetsText()
{
  int text_size  = 12;
  int label_size = 12;
  
  m_fld_sfile->textsize(text_size);
  m_fld_sfile->labelsize(label_size);

  m_fld_reg->textsize(text_size);
  m_fld_reg->labelsize(label_size);

  m_fld_unreg->textsize(text_size);
  m_fld_unreg->labelsize(label_size);

  m_fld_minsep->textsize(text_size);
  m_fld_minsep->labelsize(label_size);
}
