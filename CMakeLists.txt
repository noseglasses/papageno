cmake_minimum_required(VERSION 2.8)

project(Papageno)

# Enforce pedantic compiler behavior
#
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
endif()

set(source_directory "${CMAKE_SOURCE_DIR}/src")

set(source_files
	./ppg_global.c   
	./ppg_note.c                                                                                                                          
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
	./ppg_input.c                                                                                                                         
	./ppg_chord.c                                                                                                                         
	./ppg_time.c
)

set(source_files_full_path "")

foreach(file ${source_files})
	list(APPEND source_files_full_path "${source_directory}/${file}")
endforeach()

include_directories(
	"${source_directory}"
)

add_library(papageno ${source_files_full_path})

set_property(TARGET papageno PROPERTY C_STANDARD 99)