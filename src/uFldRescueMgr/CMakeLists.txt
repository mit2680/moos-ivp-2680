#--------------------------------------------------------
# The CMakeLists.txt for:                   uFldRescueMgr
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    m
    pthread)
endif (${WIN32})

SET(SRC
   RescueMgr.cpp
   RescueMgr_Info.cpp
   main.cpp
)

ADD_EXECUTABLE(uFldRescueMgr ${SRC})

TARGET_LINK_LIBRARIES(uFldRescueMgr
  swimmers
  ${MOOS_LIBRARIES}
  contacts
  geometry 
  mbutil
  apputil
  ${SYSTEM_LIBS}
)


