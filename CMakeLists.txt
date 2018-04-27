cmake_minimum_required(VERSION 2.8)

project(Papageno C CXX)

macro(use_c99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
    endif ()
  else ()
    set (CMAKE_C_STANDARD 99)
  endif ()
endmacro(use_c99)

use_c99()

option(PAPAGENO_BUILD_LIBRARY "Enable/Disable build of the Papageno library" TRUE)
if(PAPAGENO_BUILD_LIBRARY)

   # Determine the current project version
   #
   find_program(GIT_EXECUTABLE
      NAMES git
   )

   if("${GIT_EXECUTABLE}" STREQUAL "GIT_EXECUTABLE-NOTFOUND")
      message(FATAL_ERROR "Papageno requires git to retrieve versioning info. Please set the variable GIT_EXECUTABLE accordingly.")
   endif()

   execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      OUTPUT_VARIABLE version
      OUTPUT_STRIP_TRAILING_WHITESPACE
   )

   set(PAPAGENO_VERSION "${version}" CACHE INTERNAL "The program version" FORCE)

   set(default_platform "generic")

   set(papageno_platform_description "Set the platform to build for. Currently supported: generic, atmega32u4, atmega1280")
   set(PAPAGENO_PLATFORM "${default_platform}" CACHE STRING "${papageno_platform_description}")
   mark_as_advanced(PAPAGENO_PLATFORM)

   set(default_plaform_file "${CMAKE_SOURCE_DIR}/cmake/platforms/platform_${PAPAGENO_PLATFORM}.cmake")

   set(PAPAGENO_PLATFORM_CMAKE_FILE "${default_plaform_file}" CACHE FILEPATH "A path to a cmake file that contains platform specific settings")
   mark_as_advanced(PAPAGENO_PLATFORM_CMAKE_FILE)
      
   if(EXISTS "${PAPAGENO_PLATFORM_CMAKE_FILE}")
      include("${PAPAGENO_PLATFORM_CMAKE_FILE}")
   else ()

      if(NOT "${PAPAGENO_PLATFORM_CMAKE_FILE}" STREQUAL "")
         message(WARNING "The specified PAPAGENO_PLATFORM_CMAKE_FILE=${PAPAGENO_PLATFORM_CMAKE_FILE} cannot be found. Falling back to generic platform.")
      endif()
            
      include("${CMAKE_SOURCE_DIR}/cmake/platforms/platform_generic.cmake")
   endif()

   if("${PAPAGENO_PLATFORM}" STREQUAL "")
      if(NOT "${__PPG_PLATFORM}" STREQUAL "")
         set(PAPAGENO_PLATFORM "${__PPG_PLATFORM}" CACHE STRING "${papageno_platform_description}" FORCE)
      endif()
   endif()

   get_filename_component(c_cmp_basename "${CMAKE_C_COMPILER}" NAME)

   if("${c_cmp_basename}" STREQUAL "avr-gcc")
      set(PAPAGENO_PLATFORM_ACTUAL "avr-gcc" CACHE INTERNAL "" FORCE)
   else()
      set(PAPAGENO_PLATFORM_ACTUAL "generic" CACHE INTERNAL "" FORCE)
   endif()

   # message("PAPAGENO_PLATFORM=${PAPAGENO_PLATFORM}")
   # message("PAPAGENO_PLATFORM_CMAKE_FILE=${PAPAGENO_PLATFORM_CMAKE_FILE}")
   # message("PAPAGENO_PLATFORM_ACTUAL=${PAPAGENO_PLATFORM_ACTUAL}")

   if("${PAPAGENO_PLATFORM_ACTUAL}" STREQUAL "avr-gcc")

      find_program(
         PAPAGENO_SIMAVR_EXECUTABLE
         NAMES simavr run_avr
      )

      if(NOT EXISTS "${PAPAGENO_SIMAVR_EXECUTABLE}")
         message(WARNING "Unable to find the simavr or run_avr executable. Please specify its absolute path through the variable PAPAGENO_SIMAVR_EXECUTABLE")
      endif()

      set(PAPAGENO_RUN_WRAPPER "${PAPAGENO_SIMAVR_EXECUTABLE}" -m ${PAPAGENO_PLATFORM})
   endif()

   option(PAPAGENO_DOXYGEN "Build the doxygen documentation of the Papageno API" FALSE)
   mark_as_advanced(PAPAGENO_DOXYGEN)

   if("${PAPAGENO_PLATFORM_ACTUAL}" STREQUAL "avr-gcc")
      if(NOT DEFINED PAPAGENO_TESTING_ENABLED)
         message("Build of testing system disabled as not available in cross builds")
         set(testing_state_initial FALSE) 
      endif()
   else()
      set(testing_state_initial TRUE) 
   endif()

   option(PAPAGENO_TESTING_ENABLED "Build and run testing environment" ${testing_state_initial})
   mark_as_advanced(PAPAGENO_TESTING_ENABLED)

   option(PAPAGENO_TEST_COVERAGE "This flags enables testing code coverage" FALSE)
   mark_as_advanced(PAPAGENO_TEST_COVERAGE)

   option(PAPAGENO_VALGRIND "Run tests using valgrind to detect memory related errors" FALSE)
   mark_as_advanced(PAPAGENO_VALGRIND)

   option(PAPAGENO_QMK "Builds a dedicated version of Papageno to be used with the Quantum Mechanical Keyboard firmware" FALSE)
   mark_as_advanced(PAPAGENO_QMK)

   set(PAPAGENO_ADDITIONAL_INCLUDE_PATHS "" CACHE STRING "Add additional include paths that are necessary to build")
   mark_as_advanced(PAPAGENO_ADDITIONAL_INCLUDE_PATHS)

   if(NOT "${PAPAGENO_ADDITIONAL_INCLUDE_PATHS}" STREQUAL "")
      include_directories(${PAPAGENO_ADDITIONAL_INCLUDE_PATHS})
   endif()

   #-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
   # General settings
   #-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

   include_directories("${CMAKE_SOURCE_DIR}/src")
   include_directories("${CMAKE_BINARY_DIR}/src")

   #-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
   # Test Coverage Settings
   #-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

   if(PAPAGENO_TEST_COVERAGE)
      
      set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/CMakeModules)
      
      set(coverage_output_path "${CMAKE_BINARY_DIR}/html/coverage")
      
      file(MAKE_DIRECTORY "${coverage_output_path}")

      # See: https://raw.githubusercontent.com/bilke/cmake-modules/master/CodeCoverage.cmake
         
      include(CodeCoverage)

      append_coverage_compiler_flags()
         
      init_coverage()
   endif()

   # Enforce pedantic compiler behavior
   #
   if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
      
   endif()

   if(PAPAGENO_VALGRIND)

      find_program( MEMORYCHECK_COMMAND valgrind )
      set( MEMORYCHECK_COMMAND_OPTIONS "--error-exitcode=1 --trace-children=yes --leak-check=full" )
   endif()

   #-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
   # Enable using Boost Preprocessor library
   #-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
   #
   include_directories("${CMAKE_SOURCE_DIR}/3rd_party")

   #-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
   # Doxygen Settings
   #-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

   find_package(Doxygen)

   if(PAPAGENO_DOXYGEN)

      if(NOT DOXYGEN_FOUND)
            message(FATAL_ERROR "Doxygen is needed to build the documentation.")
      endif()
      
      set(doc_dir "${CMAKE_BINARY_DIR}/html/doc")
      
      file(MAKE_DIRECTORY "${doc_dir}")
      
      set(doxyfile_in "${CMAKE_SOURCE_DIR}/doc/papageno.doxygen.in")
      
      set(doxyfile "${doc_dir}/papageno.doxygen")
      
      file(MAKE_DIRECTORY "${doc_dir}")
      
      set(PAPAGENO_BASE_DIR "${CMAKE_SOURCE_DIR}")
      
      configure_file("${doxyfile_in}" "${doxyfile}")
      
      add_custom_target(
         doxygen 
         ALL
         COMMAND
            ${DOXYGEN_EXECUTABLE} ${doxyfile}
         WORKING_DIRECTORY "${doc_dir}"
         COMMENT "Generating API documentation with Doxygen"
         VERBATIM
      )
   endif()

   add_subdirectory(src)
endif()

option(PAPAGENO_BUILD_GLOCKENSPIEL "Enable/Disable build of Papageno's Glockenpiel compiler" TRUE)
if(PAPAGENO_BUILD_GLOCKENSPIEL)
   add_subdirectory(glockenspiel)
endif()

if(PAPAGENO_TESTING_ENABLED)
	include(CTest)
	enable_testing()
	add_subdirectory(testing)
endif()

if(PAPAGENO_TEST_COVERAGE)
   finalize_coverage()
endif()
