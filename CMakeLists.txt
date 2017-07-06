cmake_minimum_required(VERSION 2.8)

project(Papageno)

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

# Enforce pedantic compiler behavior
#
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
endif()

add_definitions(-DDEBUG_PAPAGENO)

set(source_directory "${CMAKE_SOURCE_DIR}/src")

set(source_files
	./ppg_global.c   
	./ppg_note.c      
	./ppg_event.c                                                                                                                         
	./ppg_context.c                                                                                                                        
	./ppg_tap_dance.c                                                                                                                     
	./ppg_cluster.c                                                                                                                       
	./ppg_pattern.c                                                                                                                       
	./detail/ppg_aggregate_detail.c                                                                                                       
	./detail/ppg_context_detail.c                                                                                                         
	./detail/ppg_pattern_detail.c                                                                                                         
	./detail/ppg_token_detail.c                                                                                                           
	./detail/ppg_input_detail.c                                                                                                           
	./detail/ppg_note_detail.c                                                                                                            
	./detail/ppg_global_detail.c     
	./detail/ppg_event_buffer_detail.c
	./detail/ppg_furcation_detail.c
	./ppg_input.c                                                                                                                         
	./ppg_chord.c                                                                                                                         
	./ppg_time.c                                                                                                                       
	./ppg_timeout.c
)

set(source_files_full_path "")

foreach(file ${source_files})
	list(APPEND source_files_full_path "${source_directory}/${file}")
endforeach()

include_directories(
	"${source_directory}"
)

add_library(papageno ${source_files_full_path})

find_package(Doxygen)

option(PAPAGENO_DOXYGEN "Build the doxygen documentation of the Papageno API" ${DOXYGEN_FOUND})

if(PAPAGENO_DOXYGEN)

	if(NOT DOXYGEN_FOUND)
			message(FATAL_ERROR "Doxygen is needed to build the documentation.")
	endif()
	
	set(doc_dir "${CMAKE_BINARY_DIR}/doc")
	
	file(MAKE_DIRECTORY "${doc_dir}")
	
	set(doxyfile_in "${CMAKE_SOURCE_DIR}/doc/papageno.doxygen.in")
	
	set(doxyfile "${doc_dir}/pasimodo.doxygen")
	
	file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/doc")
	
	set(PAPAGENO_BASE_DIR "${CMAKE_SOURCE_DIR}")
	
	configure_file("${doxyfile_in}" "${doxyfile}")
	
	add_custom_target(
		doxygen 
		ALL
		COMMAND
			${DOXYGEN_EXECUTABLE} ${doxyfile}
		WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/doc"
		COMMENT "Generating API documentation with Doxygen"
		VERBATIM
	)
endif()

subdirs(testing)
