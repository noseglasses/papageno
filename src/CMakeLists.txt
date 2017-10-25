
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

set(settings_file "ppg_settings.h")

configure_file(
   "${CMAKE_SOURCE_DIR}/src/${settings_file}.in"
   "${CMAKE_BINARY_DIR}/src/${settings_file}"
   @ONLY
)

set(source_files_                                                                                                          
	ppg_bitfield.c
	ppg_chord.c                                                                                                                         
	ppg_cluster.c                                                                                                                       
	ppg_compression.c                                                                                                                       
	ppg_context.c                                                                                                                            
	ppg_debug.c                                                                                                                       
	ppg_event.c     
	ppg_event_buffer.c                                                                                                                         
	ppg_global.c   
	ppg_input.c                                                                                                                    
	ppg_leader_sequences.c                                                                                                                         
	ppg_note.c      
	ppg_pattern.c    
   ppg_statistics.c             
	ppg_tap_dance.c                                                                                                                     
	ppg_time.c                                                                                                                       
	ppg_timeout.c                       
)

set(source_files_detail
	ppg_active_tokens_detail.c                                                                                                                 
	ppg_aggregate_detail.c   
	ppg_compression_detail.c                                                                                                           
	ppg_context_detail.c                                                                                                         
	ppg_event_buffer_detail.c
	ppg_furcation_detail.c
	ppg_global_detail.c     
	ppg_input_detail.c      
	ppg_malloc_detail.c                                                                                                            
	ppg_note_detail.c                                                                                                            
	ppg_pattern_detail.c                                                                                                         
	ppg_pattern_matching_detail.c
	ppg_signal_detail.c
	ppg_token_detail.c     
	ppg_token_vtable_detail.c    
)

set(source_files ${source_files_})
foreach(file ${source_files_detail})
   list(APPEND source_files "detail/${file}")
endforeach()

set(header_files_
   papageno.h
   ppg_cluster.h
   ppg_statistics.h
   ppg_signal_callback.h
   ppg_event.h
   ppg_event_buffer.h
   ppg_input.h
   ppg_tap_dance.h
   ppg_action_flags.h
   ppg_action.h
   ppg_pattern.h
   ppg_debug.h
   ppg_compression.h
   ppg_global.h
   ppg_chord.h
   ppg_timeout.h
   ppg_token.h
   ppg_time.h
   ppg_signals.h
   ppg_note.h
   ppg_context.h
   ppg_layer.h
   ppg_bitfield.h
   ppg_leader_sequences.h
)

set(header_files_detail
   ppg_signal_detail.h
   ppg_compression_detail.h
   ppg_pattern_detail.h
   ppg_event_buffer_detail.h
   ppg_input_detail.h
   ppg_aggregate_detail.h
   ppg_pattern_matching_detail.h
   ppg_context_detail.h
   ppg_malloc_detail.h
   ppg_cluster_detail.h
   ppg_global_detail.h
   ppg_note_detail.h
   ppg_token_detail.h
   ppg_token_vtable_detail.h
   ppg_token_precedence_detail.h
   ppg_furcation_detail.h
   ppg_chord_detail.h
   ppg_active_tokens_detail.h
)

set(header_files ${header_files_})
foreach(file ${header_files_detail})
   list(APPEND header_files "detail/${file}")
endforeach()

set(source_files_full_path "")
foreach(file ${source_files})
	list(APPEND source_files_full_path "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
endforeach()

set(source_files_src_path "")
foreach(file ${source_files})
	list(APPEND source_files_src_path "src/${file}")
endforeach()

set(header_files_full_path "")
foreach(file ${header_files})
	list(APPEND header_files_full_path "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
endforeach()

set(header_files_src_path "")
foreach(file ${header_files})
	list(APPEND header_files_src_path "src/${file}")
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

set(arduino_build_dir "${CMAKE_BINARY_DIR}/arduino")
file(MAKE_DIRECTORY "${arduino_build_dir}")

# Add a target to generate an arduino library
#
set(arduino_library "${arduino_build_dir}/Papageno.zip")

set(generate_arduino_script "${arduino_build_dir}/generate_arduino.script.cmake")

set(arduino_library_description_file "${arduino_build_dir}/library.properties")

file(WRITE "${generate_arduino_script}"
"
file(WRITE \"${arduino_library_description_file}\"
\"name=Papageno
version=0.0.0
author=Florian Fleissner
maintainer=Florian Fleissner <shinynoseglasses@gmail.com>
sentence=Papageno pattern matching library.
paragraph=Papageno pattern matching library.
category=Communication
url=https://github.com/noseglasses/papageno
architectures=avr
dot_a_linkage=true
includes=papageno.h
\")

execute_process(
   COMMAND zip \"${arduino_library}\" 
      ${source_files_src_path} 
      ${header_files_src_path}
   WORKING_DIRECTORY \"${CMAKE_SOURCE_DIR}\"
)

execute_process(
   COMMAND zip -u \"${arduino_library}\"
      \"src/${settings_file}\"
   WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}\"
)

execute_process(
   COMMAND zip -u \"${arduino_library}\"
      \"library.properties\"
   WORKING_DIRECTORY \"${arduino_build_dir}\"
)
")

add_custom_command(
   OUTPUT "${arduino_library}"
   DEPENDS 
      ${source_files_full_path} 
      ${header_files_full_path} 
      "${CMAKE_BINARY_DIR}/src/${settings_file}"
   COMMAND "${CMAKE_COMMAND}" -P "${generate_arduino_script}" 
)

add_custom_target(arduino_zip DEPENDS "${arduino_library}")

# Generate a dummy arduino library by using symbolic links to source files
#
set(sym_links_creation_script "${arduino_build_dir}/generate_symlinks.script.cmake")

file(WRITE "${sym_links_creation_script}"
"
file(MAKE_DIRECTORY \"${arduino_build_dir}/src\")
file(MAKE_DIRECTORY \"${arduino_build_dir}/src/detail\")

function(update_sym_link
   work_dir_
   target_
   link_
)
   file(REMOVE \"\${work_dir_}/\${link_}\")
   
   execute_process(
      COMMAND
         \"${CMAKE_COMMAND}\" -E create_symlink \"\${target_}\" \"\${link_}\"
      WORKING_DIRECTORY \"\${work_dir_}\"
   )
endfunction()

foreach(file ${source_files_} ${header_files_})
   update_sym_link(
      \"${arduino_build_dir}/src\"
      \"${CMAKE_SOURCE_DIR}/src/\${file}\" 
      \"\${file}\"
   )
endforeach()

foreach(file ${source_files_detail} ${header_files_detail})
   update_sym_link(
      \"${arduino_build_dir}/src/detail\"
      \"${CMAKE_SOURCE_DIR}/src/detail/\${file}\"
      \"\${file}\"
   )
endforeach()

update_sym_link(
   \"${CMAKE_SOURCE_DIR}/src/\${file}\"
   \"${CMAKE_BINARY_DIR}/src/${settings_file}\"
   \"${settings_file}\"
)
")

add_custom_command(
   OUTPUT "${arduino_build_dir}/src/ppg_context.h"
   DEPENDS 
      ${source_files_full_path} 
      ${header_files_full_path} 
      "${CMAKE_BINARY_DIR}/src/${settings_file}"
   COMMAND "${CMAKE_COMMAND}" -P "${sym_links_creation_script}" 
)

add_custom_target(arduino_symlinks DEPENDS "${arduino_build_dir}/src/ppg_context.h")

add_custom_target(arduino DEPENDS arduino_symlinks arduino_zip)