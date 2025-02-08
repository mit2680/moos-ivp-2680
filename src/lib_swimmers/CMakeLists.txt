#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_swimmers
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  SwimFldGenerator.cpp
  Swimmer.cpp
  SwimmerSet.cpp
)

SET(HEADERS
  SwimFldGenerator.h
  Swimmer.h
  SwimmerSet.h
)

# Build Library
ADD_LIBRARY(swimmers ${SRC})

