if(NOT PAPAGENO_TESTING_ENABLED)
   return()
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_library(

	papageno_char_strings
	papageno_char_strings.c
)

target_link_libraries(
	papageno_char_strings
	papageno
)

# macro(ppg_add_test test_name)
# 
#    # Add the normal test executable
#    #
#    set(run_target_name "${test_name}_run")
# 	add_executable(
# 		"${test_name}_run"
# 		"${test_name}.c"
# 	)
# 	target_link_libraries(
# 		"${run_target_name}"
# 		papageno_char_strings
# 	)
# 	
# 	add_test(
# 		NAME "${run_target_name}" 
# 		COMMAND "${run_target_name}"
# 		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
# 	)
# 	   
#    # Add an executable that compresses the context
#    #
#    set(build_compress_executable_target_name "${test_name}_compress_build_executable")
# 	set(compressed_code_file "${CMAKE_CURRENT_BINARY_DIR}/${build_compress_executable_target_name}_compressed_code.h")
# 	
#    add_executable(
# 		"${build_compress_executable_target_name}"
# 		"${test_name}.c"
# 	)
# 		
#    set_target_properties(
# 		"${build_compress_executable_target_name}"
#       PROPERTIES COMPILE_FLAGS 
#          "-DPPG_CS_COMPRESS_CONTEXT=1 -DPPG_CS_SUPPRESS_TESTS=1"
#    )
# 
# 	target_link_libraries(
# 		"${build_compress_executable_target_name}"
# 		papageno_char_strings
# 	)	
# 	
#    set(compress_target_name "${test_name}_compress_processing")
#    
#    # Add a target that issues context compression during build
#    #
# 	add_custom_command(
#       OUTPUT "${compressed_code_file}"
#       COMMAND ${CMAKE_COMMAND} 
#          "-DPAPAGENO_TEST_EXECUTABLE=${CMAKE_CURRENT_BINARY_DIR}/${build_compress_executable_target_name}"
#          "-DPAPAGENO_COMPRESSED_CODE_FILE=${compressed_code_file}"
#          -P "${CMAKE_CURRENT_SOURCE_DIR}/TestingWithCompression.script.cmake"
# 		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
#    )
# 	
# 	add_custom_target("${compress_target_name}" DEPENDS "${compressed_code_file}")
# 	
# 	add_dependencies("${compress_target_name}" "${build_compress_executable_target_name}")
# 	
#    # To allow for memory debugger based testing of the compression process
#    # we also add it as a test, although it has already been run during the build
#    # stage.
#    #
#    set(compress_test_target_name "${test_name}_compress")
#    add_test(
#       NAME "${compress_test_target_name}" 
#       COMMAND "${CMAKE_CURRENT_BINARY_DIR}/${build_compress_executable_target_name}"
#       WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
#    )
# 
# 	# Add an executable that extracts the copressed context
# 	# and runs tests
# 	#
#    set(extract_and_test_target_name "${test_name}_extract_and_test")
#    add_executable(
# 		"${extract_and_test_target_name}"
# 		"${test_name}.c"
# 	)
# 	
# 	add_dependencies("${extract_and_test_target_name}" "${compress_target_name}")
# 	
#    set_target_properties(
# 		"${extract_and_test_target_name}"
#       PROPERTIES COMPILE_FLAGS
#          "-DPPG_CS_READ_COMPRESSED_CONTEXT=1 -DPPG_CS_COMPRESSED_CODE_FILE=\\\"${compressed_code_file}\\\""
#    )
# 
# 	target_link_libraries(
# 		"${extract_and_test_target_name}"
# 		papageno_char_strings
# 	)
# 	
# 	add_test(
# 		NAME "${extract_and_test_target_name}" 
# 		COMMAND "${extract_and_test_target_name}"
# 		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
# 	)
# 	
# 	include_directories("${CMAKE_CURRENT_BINARY_DIR}")
# 
# #	add_test(NAME <name> COMMAND <command> [<arg>...]
# #         [CONFIGURATIONS <config>...]
# #         [WORKING_DIRECTORY <dir>])
# 
# 	
# 	if(PAPAGENO_TEST_COVERAGE)
# 	      
#       SETUP_TARGET_FOR_JOINT_COVERAGE(
#          NAME "${test_name}_coverage"
#          EXECUTABLE "${test_name}"
#          DEPENDENCIES "${test_name}"
#       )
#    endif()
# 	
# endmacro()

macro(ppg_create_if_not_existent filename)
   if(NOT EXISTS "${filename}")
      file(WRITE "${filename}" "")
   endif()
endmacro()

macro(ppg_add_test test_name)
   ppg_create_if_not_existent("${test_name}_actions.h")
   ppg_create_if_not_existent("${test_name}_tests.h")
   ppg_create_if_not_existent("${test_name}_token_tree.h")
endmacro()
 
ppg_add_test(test_abort_trigger)
ppg_add_test(test_chords)
ppg_add_test(test_clusters)
ppg_add_test(test_context_switching)
ppg_add_test(test_enable_disable)
ppg_add_test(test_enable_disable_timeout)
ppg_add_test(test_fallback)
ppg_add_test(test_large_systems)
ppg_add_test(test_layers)
ppg_add_test(test_leader_sequences)
ppg_add_test(test_note_lines)
ppg_add_test(test_strict_notes)
ppg_add_test(test_token_precedence)
# ppg_add_test(test_compression)


# ppg_add_test(test_large_systems_2)

# # # # target_compile_options(
# # # #    test_large_systems 
# # # #    PUBLIC
# # # #    -DPAPAGENO_TEST__TREE_DEPTH=4
# # # # 	-DPAPAGENO_TEST__N_CHARS=4
# # # #    -DPAPAGENO_TEST__FAIL=1
# # # # )

# target_compile_options(
#    test_large_systems_2
#    PUBLIC
#    -DPAPAGENO_TEST__FAIL=0
# )


# # # # target_link_libraries(test_large_systems m)

# set_property(TARGET test_char_strings PROPERTY C_STANDARD 99)