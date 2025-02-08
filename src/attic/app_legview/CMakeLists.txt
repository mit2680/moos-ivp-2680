#--------------------------------------------------------
# The CMakeLists.txt for:                         legview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    m)
endif (${WIN32})

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  SET(SYSTEM_LIBS ${SYSTEM_LIBS} GL)
endif(CMAKE_SYSTEM_NAME STREQUAL Linux)

SET(SRC
  Leg_GUI.cpp
  Leg_GUI_Widgets.cpp
  LegViewer.cpp
  LegView_Info.cpp
  main.cpp
  )

IF (${APPLE})
  # If you want to create an Apple BUNDLE, do it here
  ADD_EXECUTABLE(legview ${SRC})
ELSE (${APPLE})
  ADD_EXECUTABLE(legview ${SRC})
ENDIF (${APPLE})

TARGET_LINK_LIBRARIES(legview
  swimmers
  marineview
  ${MOOSGeodesy_LIBRARIES}
  contacts
  geometry
  mbutil
  ${SYSTEM_LIBS}
  
  # Standard libraries used by this project's FLTK apps...
  fltk 
  fltk_gl 
  dl
  tiff)

# http://developer.apple.com/qa/qa2007/qa1567.html
IF (${APPLE})
  SET_TARGET_PROPERTIES(legview PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})



