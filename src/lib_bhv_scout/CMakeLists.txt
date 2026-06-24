#--------------------------------------------------------
# The CMakeLists.txt for:                   lib_bhv_scout
# Author(s):                                   M.Benjamin
#--------------------------------------------------------
#                                               BHV_Scout
#--------------------------------------------------------

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Set System Specific Libraries
if (${WIN32})
  # Windows Libraries
  SET(SYSTEM_LIBS
      )
else (${WIN32})
  # Linux and Apple Libraries
  SET(SYSTEM_LIBS
      m )
endif (${WIN32})

ADD_LIBRARY(BHV_Scout SHARED 
   BHV_Scout.cpp)

if(APPLE)
  set(MBUTIL_LINK mbutil)
else()
  set(MBUTIL_LINK -Wl,--whole-archive mbutil -Wl,--no-whole-archive)
endif()

TARGET_LINK_LIBRARIES(BHV_Scout
   helmivp
   behaviors 
   ivpbuild 
   logic 
   ivpcore 
   bhvutil 
   geometry
  ${MBUTIL_LINK}
  ${MOOS_LIBRARIES}
  ${SYSTEM_LIBS}
  )


