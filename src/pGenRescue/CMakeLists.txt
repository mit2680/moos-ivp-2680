#--------------------------------------------------------
# The CMakeLists.txt for:                      pGenRescue
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pGenRescue ${SRC})

TARGET_LINK_LIBRARIES(pGenRescue
   ${MOOS_LIBRARIES}
   geometry
   mbutil
   m
   pthread)

