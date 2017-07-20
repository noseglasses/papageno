
option(PAPAGENO_PEDANTIC_TOKENS "Enabling this option compiles Papageno in pedantic-tokens-mode"
   FALSE)
mark_as_advanced(PAPAGENO_PEDANTIC_TOKENS)

if(PAPAGENO_PEDANTIC_TOKENS)
   set(__PPG_PEDANTIC_TOKENS 1)
else()
   set(__PPG_PEDANTIC_TOKENS 0)
endif()

set(debugging_features_enabled FALSE)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
   set(debugging_features_enabled TRUE)
endif()
   
option(PAPAGENO_HAVE_LOGGING "Toggles logging output" ${debugging_features_enabled})
mark_as_advanced(PAPAGENO_HAVE_LOGGING)

if(PAPAGENO_HAVE_LOGGING)
   set(__PPG_HAVE_LOGGING 1)
else()
   set(__PPG_HAVE_LOGGING 0)
endif()

option(PAPAGENO_PRINT_SELF_ENABLED "Toggles vebose output of class objects" ${debugging_features_enabled})
mark_as_advanced(PAPAGENO_PRINT_SELF_ENABLED)

if(PAPAGENO_PRINT_SELF_ENABLED)
   set(__PPG_PRINT_SELF_ENABLED 1)
else()
   set(__PPG_PRINT_SELF_ENABLED 0)
endif()

option(PAPAGENO_HAVE_ASSERTIONS "Toggles compile time assertions" ${debugging_features_enabled})
mark_as_advanced(PAPAGENO_HAVE_ASSERTIONS)

if(PAPAGENO_HAVE_ASSERTIONS)
   set(__PPG_HAVE_ASSERTIONS 1)
else()
   set(__PPG_HAVE_ASSERTIONS 0)
endif()

option(PAPAGENO_DISABLE_CONTEXT_SWITCHING "Disable the switching of context" FALSE)
mark_as_advanced(PAPAGENO_DISABLE_CONTEXT_SWITCHING)

if(PAPAGENO_DISABLE_CONTEXT_SWITCHING)
   set(__PPG_DISABLE_CONTEXT_SWITCHING 1)
else()
   set(__PPG_DISABLE_CONTEXT_SWITCHING 0)
endif()

option(PAPAGENO_HAVE_STATISTICS "Enable the generation of statistics. Enable this feature for debugging." FALSE)
mark_as_advanced(PAPAGENO_HAVE_STATISTICS)

if(PAPAGENO_HAVE_STATISTICS)
   set(__PPG_HAVE_STATISTICS 1)
else()
   set(__PPG_HAVE_STATISTICS 0)
endif()

set(PAPAGENO_PLATFORM "" CACHE STRING "Set the platform to build for. Currently supported: generic, avr-gcc")
mark_as_advanced(PAPAGENO_PLATFORM)

set(PAPAGENO_PLATFORM_CMAKE_FILE "" CACHE FILEPATH "A path to a cmake file that contains platform specific settings")
mark_as_advanced(PAPAGENO_PLATFORM_CMAKE_FILE)

if(EXISTS "${PAPAGENO_PLATFORM_CMAKE_FILE}")
   include("${PAPAGENO_PLATFORM_CMAKE_FILE}")
else ()

   if(NOT "${PAPAGENO_PLATFORM_CMAKE_FILE}" STREQUAL "")
      message(WARNING "The specified PAPAGENO_PLATFORM_CMAKE_FILE=${PAPAGENO_PLATFORM_CMAKE_FILE} cannot be found")
   endif()

   get_filename_component(c_cmp_basename "${CMAKE_C_COMPILER}" NAME)

   if("${PAPAGENO_PLATFORM}" STREQUAL "") 
      if("${c_cmp_basename}" STREQUAL "avr-gcc")
         set(platform "avr-gcc")
      else()
         set(platform "generic")
      endif()
   else()
      set(platform "${PAPAGENO_PLATFORM}")
   endif()
         
   include("${CMAKE_SOURCE_DIR}/cmake/platforms/platform_${platform}.cmake")
endif()

configure_file(
   "${CMAKE_SOURCE_DIR}/src/ppg_settings.h.in"
   "${CMAKE_BINARY_DIR}/src/ppg_settings.h"
   @ONLY
)

set(source_files                                                                                                         
	./detail/ppg_aggregate_detail.c                                                                                                       
	./detail/ppg_context_detail.c                                                                                                         
	./detail/ppg_event_buffer_detail.c
	./detail/ppg_furcation_detail.c
	./detail/ppg_global_detail.c     
	./detail/ppg_input_detail.c                                                                                                           
	./detail/ppg_note_detail.c                                                                                                            
	./detail/ppg_pattern_detail.c                                                                                                         
	./detail/ppg_pattern_matching_detail.c
	./detail/ppg_signal_detail.c
	./detail/ppg_token_detail.c                                                                                                           
	./ppg_bitfield.c
	./ppg_chord.c                                                                                                                         
	./ppg_cluster.c                                                                                                                       
	./ppg_context.c                                                                                                                        
	./ppg_event.c     
	./ppg_event_buffer.c                                                                                                                         
	./ppg_global.c   
	./ppg_input.c                                                                                                                         
	./ppg_note.c      
	./ppg_pattern.c    
   ./ppg_statistics.c             
	./ppg_tap_dance.c                                                                                                                     
	./ppg_time.c                                                                                                                       
	./ppg_timeout.c                                                                                                              
)

set(source_files_full_path "")

foreach(file ${source_files})
	list(APPEND source_files_full_path "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
endforeach()

add_library(papageno SHARED ${source_files_full_path})