#--------------------------------------------------------
# The CMakeLists.txt for:                pPointAssignEval
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  PointAssignEval.cpp
  PointAssignEval_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pPointAssignEval ${SRC})

TARGET_LINK_LIBRARIES(pPointAssignEval
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

