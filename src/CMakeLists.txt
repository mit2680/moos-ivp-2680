##############################################################################
# FILE:        moos-ivp-2680/src/CMakeLists.txt
# DATE:        2021/05/23
# DESCRIPTION: CMakeLists.txt file for the moos-ivp-2.680 source directory
##############################################################################


#============================================================================
# Add the libraries in the current directory to the include path
#============================================================================
FILE(GLOB LOCAL_LIBRARY_DIRS ./lib_*)
INCLUDE_DIRECTORIES(${LOCAL_LIBRARY_DIRS})

IF( EXISTS ${CMAKE_SOURCE_DIR}/src/lib_behaviors-test )
  ADD_SUBDIRECTORY(lib_behaviors-test)
ENDIF()

#========================================================================
# Let users control whether or not GUI-related libraries/apps are built.
#========================================================================
SET(BUILD_BOT_CODE_ONLY OFF CACHE BOOL "Build minimal set of bot apps.")

message("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
message(" SRC: BUILD_BOT_CODE_ONLY: ${BUILD_BOT_CODE_ONLY}")
message("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")


#============================================================================
# Add the LAB Front Estimation Modules (if present)
#============================================================================
#IF( EXISTS ${CMAKE_SOURCE_DIR}/src/lib_henrik_anneal )
#  ADD_SUBDIRECTORY(lib_henrik_anneal)
#ENDIF()

IF( EXISTS ${CMAKE_SOURCE_DIR}/src/lib_bhv_scout )
  ADD_SUBDIRECTORY(lib_bhv_scout)
ENDIF()

#IF( EXISTS ${CMAKE_SOURCE_DIR}/src/pFrontEstimate )
#  ADD_SUBDIRECTORY(pFrontEstimate)
#ENDIF()

#IF( EXISTS ${CMAKE_SOURCE_DIR}/src/pFrontGridRender )
#  ADD_SUBDIRECTORY(pFrontGridRender)
#ENDIF()

#IF( EXISTS ${CMAKE_SOURCE_DIR}/src/pGradeFrontEstimate )
#  ADD_SUBDIRECTORY(pGradeFrontEstimate)
#ENDIF()

#IF( EXISTS ${CMAKE_SOURCE_DIR}/src/uFldCTDSensor )
#  ADD_SUBDIRECTORY(uFldCTDSensor)
#ENDIF()

#============================================================================
# Add the LAB Hazard Search Modules (if present)
#============================================================================
IF( EXISTS ${CMAKE_SOURCE_DIR}/src/lib_ufld_hazards )
  ADD_SUBDIRECTORY(lib_ufld_hazards)
ENDIF()

IF( EXISTS ${CMAKE_SOURCE_DIR}/src/app_gen_hazards )
  ADD_SUBDIRECTORY(app_gen_hazards)
ENDIF()

IF( EXISTS ${CMAKE_SOURCE_DIR}/src/uFldHazardMetric )
  ADD_SUBDIRECTORY(uFldHazardMetric)
ENDIF()

IF( EXISTS ${CMAKE_SOURCE_DIR}/src/uFldHazardMgr )
  ADD_SUBDIRECTORY(uFldHazardMgr)
ENDIF()

IF( EXISTS ${CMAKE_SOURCE_DIR}/src/uFldHazardSensor )
  ADD_SUBDIRECTORY(uFldHazardSensor)
ENDIF()

#============================================================================
# Add the LAB Collaborative Rescue Modules (if present)
#============================================================================
IF( EXISTS ${CMAKE_SOURCE_DIR}/src/lib_swimmers )
  ADD_SUBDIRECTORY(lib_swimmers)
ENDIF()


IF("${BUILD_BOT_CODE_ONLY}" STREQUAL "OFF")
  IF( EXISTS ${CMAKE_SOURCE_DIR}/src/app_swimview )
     ADD_SUBDIRECTORY(app_swimview)
  ENDIF()
  IF( EXISTS ${CMAKE_SOURCE_DIR}/src/app_legview )
     ADD_SUBDIRECTORY(app_legview)
  ENDIF()
ENDIF()


IF( EXISTS ${CMAKE_SOURCE_DIR}/src/app_gen_swimmers )
  ADD_SUBDIRECTORY(app_gen_swimmers)
ENDIF()

#IF( EXISTS ${CMAKE_SOURCE_DIR}/src/uFldManOverboard )
#  ADD_SUBDIRECTORY(uFldManOverboard)
#ENDIF()

IF( EXISTS ${CMAKE_SOURCE_DIR}/src/uFldRescueMgr )
  ADD_SUBDIRECTORY(uFldRescueMgr)
ENDIF()


##############################################################################
#                           END of CMakeLists.txt
##############################################################################

