
set(debugging_features_enabled FALSE)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
   set(debugging_features_enabled TRUE)

   set(__PPG_DEBUGGING_ENABLED 1)
else()
   set(__PPG_DEBUGGING_ENABLED 0)
endif()
   
option(PAPAGENO_LOGGING_ENABLED "Toggles logging output" ${debugging_features_enabled})
mark_as_advanced(PAPAGENO_LOGGING_ENABLED)

if(PAPAGENO_LOGGING_ENABLED)
   set(__PPG_LOGGING_ENABLED 1)
else()
   set(__PPG_LOGGING_ENABLED 0)
endif()

option(PAPAGENO_PRINT_SELF_ENABLED "Toggles vebose output of class objects" ${debugging_features_enabled})
mark_as_advanced(PAPAGENO_PRINT_SELF_ENABLED)

if(PAPAGENO_PRINT_SELF_ENABLED)
   set(__PPG_PRINT_SELF_ENABLED 1)
else()
   set(__PPG_PRINT_SELF_ENABLED 0)
endif()

option(PAPAGENO_ASSERTIONS_ENABLED "Toggles compile time assertions" ${debugging_features_enabled})
mark_as_advanced(PAPAGENO_ASSERTIONS_ENABLED)

if(PAPAGENO_ASSERTIONS_ENABLED)
   set(__PPG_ASSERTIONS_ENABLED 1)
else()
   set(__PPG_ASSERTIONS_ENABLED 0)
endif()

option(PAPAGENO_DISABLE_CONTEXT_SWITCHING "Disable the switching of context" FALSE)
mark_as_advanced(PAPAGENO_DISABLE_CONTEXT_SWITCHING)

if(PAPAGENO_DISABLE_CONTEXT_SWITCHING)
   set(__PPG_DISABLE_CONTEXT_SWITCHING 1)
else()
   set(__PPG_DISABLE_CONTEXT_SWITCHING 0)
endif()

option(PAPAGENO_STATISTICS_ENABLED "Enable the generation of statistics. Enable this feature for debugging." FALSE)
mark_as_advanced(PAPAGENO_STATISTICS_ENABLED)

if(PAPAGENO_STATISTICS_ENABLED)
   set(__PPG_STATISTICS_ENABLED 1)
else()
   set(__PPG_STATISTICS_ENABLED 0)
endif()

configure_file(
   "${CMAKE_SOURCE_DIR}/src/ppg_settings.h.in"
   "${CMAKE_BINARY_DIR}/src/ppg_settings.h"
   @ONLY
)

set(source_files        
	./detail/ppg_active_tokens_detail.c                                                                                                                 
	./detail/ppg_aggregate_detail.c   
	./detail/ppg_compression_detail.c                                                                                                           
	./detail/ppg_context_detail.c                                                                                                         
	./detail/ppg_event_buffer_detail.c
	./detail/ppg_furcation_detail.c
	./detail/ppg_global_detail.c     
	./detail/ppg_input_detail.c      
	./detail/ppg_malloc_detail.c                                                                                                            
	./detail/ppg_note_detail.c                                                                                                            
	./detail/ppg_pattern_detail.c                                                                                                         
	./detail/ppg_pattern_matching_detail.c
	./detail/ppg_signal_detail.c
	./detail/ppg_token_detail.c     
	./detail/ppg_token_vtable_detail.c                                                                                                           
	./ppg_bitfield.c
	./ppg_chord.c                                                                                                                         
	./ppg_cluster.c                                                                                                                       
	./ppg_compression.c                                                                                                                       
	./ppg_context.c                                                                                                                            
	./ppg_debug.c                                                                                                                       
	./ppg_event.c     
	./ppg_event_buffer.c                                                                                                                         
	./ppg_global.c   
	./ppg_input.c                                                                                                                    
	./ppg_leader_sequences.c                                                                                                                         
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

if(TARGET_SUPPORTS_SHARED_LIBS)
   set(initial_lib_type "SHARED")
else()
   set(initial_lib_type "STATIC")
endif()

if(PAPAGENO_QMK)
   if("${PAPAGENO_ADDITIONAL_INCLUDE_PATHS}" STREQUAL "")
      message(WARNING "The build option PAPAGENO_QMK has been enabled. This suggests that a dedicated version of Papageno is supposed to be build that can be linked with the Quantum Mechanical Keyboard firmware. However, the build variable PAPAGENO_ADDITIONAL_INCLUDE_PATHS is empty. For the build system to find appropriate QMK headers, please add a list of related paths to PAPAGENO_ADDITIONAL_INCLUDE_PATHS.")
   endif()

   add_definitions("-DPPG_BUILDING_FOR_QMK=1")
endif()

set(PAPAGENO_LIBRARY_TYPE "${initial_lib_type}" CACHE STRING "The type of papageno library to build SHARED/STATIC. Note that not all platforms support shared libraries.")

add_library(papageno ${PAPAGENO_LIBRARY_TYPE} ${source_files_full_path})
