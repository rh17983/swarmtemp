#
# Check for correct GCC version
# On Linux it must be >= 4.3
#
macro(CHECK_GCC MIN_VERSION)
  execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  if(GCC_VERSION VERSION_GREATER ${MIN_VERSION} OR GCC_VERSION VERSION_EQUAL ${MIN_VERSION})
    message(STATUS "GCC/G++ version >= ${MIN_VERSION}")
  else(GCC_VERSION VERSION_GREATER ${MIN_VERSION} OR GCC_VERSION VERSION_EQUAL ${MIN_VERSION})
    message(FATAL_ERROR "You need to have at least GCC/G++ version ${MIN_VERSION}!")
  endif(GCC_VERSION VERSION_GREATER ${MIN_VERSION} OR GCC_VERSION VERSION_EQUAL ${MIN_VERSION})
endmacro(CHECK_GCC)

if(APPLE)
  # On Apple it must be >= 4.2.1
  check_gcc(4.2.1)
else(APPLE)
  # On Linux it must be >= 4.3
  check_gcc(4.3)
endif(APPLE)

#
# Check for dynamic library loading facility
#
if(ARGOS_DYNAMIC_LIBRARY_LOADING)
  find_package(DLFCN)
  if(NOT DLFCN_FOUND)  
    message(FATAL_ERROR "Required library dl not found.")
  endif(NOT DLFCN_FOUND)
  include_directories(${DLFCN_INCLUDE_DIR})
endif(ARGOS_DYNAMIC_LIBRARY_LOADING)

#
# Check for PThreads
# It is required only when compiling the simulator
#
if(ARGOS_BUILD_FOR_SIMULATOR)
  find_package(Pthreads)
  if(NOT PTHREADS_FOUND)  
    message(FATAL_ERROR "Required library pthreads not found.")
  endif(NOT PTHREADS_FOUND)
  add_definitions(${PTHREADS_DEFINITIONS})
  include_directories(${PTHREADS_INCLUDE_DIR})
endif(ARGOS_BUILD_FOR_SIMULATOR)

#
# Check for GSL
# It is required only when compiling the simulator
#
find_package(GSL)
if(GSL_FOUND)
  set(ARGOS_WITH_GSL ON)
  add_definitions(${CMAKE_GSL_CXX_FLAGS})
  include_directories(${GSL_INCLUDE_DIR})
  link_directories(${GSL_LINK_DIRECTORIES})
else(GSL_FOUND)
  if(ARGOS_BUILD_FOR_SIMULATOR)
    message(FATAL_ERROR "When compiling the ARGoS simulator, GSL is required")
  endif(ARGOS_BUILD_FOR_SIMULATOR)
endif(GSL_FOUND)

#
# Check for FreeImage
# It is required only when compiling the simulator
#
if(ARGOS_BUILD_FOR_SIMULATOR)
  find_package(FreeImage)
  if(NOT FREEIMAGE_FOUND)
    message(FATAL_ERROR "Required library FreeImage not found.")
  endif(NOT FREEIMAGE_FOUND)
  include_directories(${FREEIMAGE_INCLUDE_PATH})
endif(ARGOS_BUILD_FOR_SIMULATOR)

#
# Check for Google Perftools
#
find_package(GooglePerfTools)

#
# Check for Doxygen
#
set(ARGOS_BUILD_API OFF)
if(ARGOS_DOCUMENTATION)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    if(DOXYGEN_DOT_FOUND)
      set(ARGOS_BUILD_API ON)
    else(DOXYGEN_DOT_FOUND)
      message(WARNING "GraphViz/Dot not found, API documentation won't be generated")
    endif(DOXYGEN_DOT_FOUND)
  else(DOXYGEN_FOUND)
    message(WARNING "Doxygen not found, API documentation won't be generated")
  endif(DOXYGEN_FOUND)
endif(ARGOS_DOCUMENTATION)

#
# Check for ASCIIDoc
#
find_package(ASCIIDoc)

#
# Check for Lua 5.1
#
find_package(Lua51)
if(LUA51_FOUND)
  set(ARGOS_WITH_LUA ON)
  include_directories(${LUA_INCLUDE_DIR})
endif(LUA51_FOUND)
