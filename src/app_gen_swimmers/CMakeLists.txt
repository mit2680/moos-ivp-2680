#--------------------------------------------------------
# The CMakeLists.txt for:                    gen_swimmers
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   GenSwimmers_Info.cpp
   main.cpp
)


ADD_EXECUTABLE(gen_swimmers ${SRC})

TARGET_LINK_LIBRARIES(gen_swimmers
  swimmers
  geometry
  mbutil
  ${SYSTEM_LIBS}
)
