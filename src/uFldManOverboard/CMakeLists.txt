#--------------------------------------------------------
# The CMakeLists.txt for:                uFldManOverboard
# Author(s):                             Michael Benjamin
#--------------------------------------------------------

SET(SRC
  ManOverboard.cpp
  ManOverboard_Info.cpp
  Swimmer.cpp
  main.cpp
)

ADD_EXECUTABLE(uFldManOverboard ${SRC})

TARGET_LINK_LIBRARIES(uFldManOverboard
   ${MOOS_LIBRARIES}
   apputil
   geometry
   contacts
   mbutil
   m
   pthread)

