# This file is part of Noggit3, licensed under GNU General Public License (version 3).

# warnings?
if( NOGGIT_ALL_WARNINGS )
  MESSAGE( STATUS "Spilling out mass warnings." )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wshadow")
endif( NOGGIT_ALL_WARNINGS )

# On Linux, default to console logging for easier debugging
if(NOT DEFINED NOGGIT_LOGTOCONSOLE)
  set(NOGGIT_LOGTOCONSOLE ON CACHE BOOL "Log to console instead of log.txt" FORCE)
  add_definitions(-DDEBUG__LOGGINGTOCONSOLE)
endif()
