#--------------------------------------------------------
# The CMakeLists.txt for:                        swimview
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
  SWIM_GUI.cpp
  SWIM_GUI_Widgets.cpp
  SwimViewer.cpp
  SwimView_Info.cpp
  main.cpp
  )

IF (${APPLE})
  # If you want to create an Apple BUNDLE, do it here
  ADD_EXECUTABLE(swimview ${SRC})
ELSE (${APPLE})
  ADD_EXECUTABLE(swimview ${SRC})
ENDIF (${APPLE})

TARGET_LINK_LIBRARIES(swimview
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
  SET_TARGET_PROPERTIES(swimview PROPERTIES	  
    LINK_FLAGS "-framework OpenGL -framework AGL -framework Cocoa"
    )
ENDIF (${APPLE})



