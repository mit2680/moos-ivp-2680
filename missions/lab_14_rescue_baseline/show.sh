#!/bin/bash 

aloggrep NODE_REPORT_LOCAL LOG_ABE*/*.alog  --final -nr 
aloggrep NODE_REPORT_LOCAL LOG_BEN*/*.alog  --final -nr 
aloggrep NODE_REPORT_LOCAL LOG_CAL*/*.alog  --final -nr 
aloggrep NODE_REPORT_LOCAL LOG_DEB*/*.alog  --final -nr 

aloggrep RESCUE_APP LOG_ABE*/*.alog  --final -nr 
aloggrep RESCUE_APP LOG_BEN*/*.alog  --final -nr 
aloggrep RESCUE_APP LOG_CAL*/*.alog  --final -nr 
aloggrep RESCUE_APP LOG_DEB*/*.alog  --final -nr 

aloggrep BHV_DIR LOG_ABE*/*.alog  --final -nr 
aloggrep BHV_DIR LOG_BEN*/*.alog  --final -nr 
aloggrep BHV_DIR LOG_CAL*/*.alog  --final -nr 
aloggrep BHV_DIR LOG_DEB*/*.alog  --final -nr 
