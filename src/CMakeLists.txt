
option(PAPAGENO_PEDANTIC_TOKENS "Enabling this option compiles Papageno in pedantic-tokens-mode"
   FALSE)
mark_as_advanced(PAPAGENO_PEDANTIC_TOKENS)

if(PAPAGENO_PEDANTIC_TOKENS)
   set(__PPG_PEDANTIC_TOKENS 1)
else()
   set(__PPG_PEDANTIC_TOKENS 0)
endif()

set(debugging_features_enabled 0)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
   set(debugging_features_enabled 1)
endif()

set(__PPG_HAVE_DEBUG ${debugging_features_enabled})
set(__PPG_PRINT_SELF_ENABLED ${debugging_features_enabled})
set(__PPG_HAVE_ASSERTIONS ${debugging_features_enabled})

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

configure_file(
   "${CMAKE_SOURCE_DIR}/src/ppg_settings.h.in"
   "${CMAKE_BINARY_DIR}/src/ppg_settings.h"
   @ONLY
)

include_directories("${CMAKE_BINARY_DIR}/src")

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
	./ppg_tap_dance.c                                                                                                                     
	./ppg_time.c                                                                                                                       
	./ppg_timeout.c                                                                                                              
)

set(source_files_full_path "")

foreach(file ${source_files})
	list(APPEND source_files_full_path "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
endforeach()

include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

add_library(papageno SHARED ${source_files_full_path})