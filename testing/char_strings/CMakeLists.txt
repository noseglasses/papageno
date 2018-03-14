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

function(ppg_generate_test)

   set(multi_value_args "")
   set(one_value_args "NAME;EXECUTABLE")
   set(options "")

   cmake_parse_arguments(args "${options}" "${one_value_args}"
                     "${multi_value_args}" ${ARGN} )
       
   if("${PAPAGENO_PLATFORM_ACTUAL}" STREQUAL "avr-gcc")
      
      add_test(
         NAME "${args_NAME}" 
         COMMAND ${CMAKE_COMMAND} "-DCMD=${PAPAGENO_RUN_WRAPPER};${args_EXECUTABLE}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/TestAndCheck.script.cmake"
         ${args_UNPARSED_ARGUMENTS}
      )
   else()
      add_test(
         NAME "${args_NAME}" 
         COMMAND ${PAPAGENO_RUN_WRAPPER} "${args_EXECUTABLE}"
         ${args_UNPARSED_ARGUMENTS}
      )
   endif()

endfunction()
	
function(ppg_add_test name)

   set(multi_value_args "")
   set(one_value_args "COMPILE_DEFINITIONS;LINK_LIBRARIES")
   set(options "")

   cmake_parse_arguments(args "${options}" "${one_value_args}"
                     "${multi_value_args}" ${ARGN} )

   set(code_file "${CMAKE_CURRENT_SOURCE_DIR}/${name}/${name}.c")

   # Add the normal test executable
   #
   set(run_target_name "${name}_run")

	add_executable(
		"${run_target_name}"
		"${code_file}"
	)

   set_target_properties(
      "${run_target_name}"
      PROPERTIES COMPILE_FLAGS 
         "${args_COMPILE_DEFINITIONS}"
   )

	target_link_libraries(
		"${run_target_name}"
		papageno_char_strings
	)

   if(DEFINED args_LINK_LIBRARIES)
      target_link_libraries(
         "${run_target_name}"
         "${args_LINK_LIBRARIES}"
      )
	endif()
	
	ppg_generate_test(
		NAME "${run_target_name}" 
		EXECUTABLE "${run_target_name}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	)
	
	if(PAPAGENO_TEST_COVERAGE)
	      
      SETUP_TARGET_FOR_JOINT_COVERAGE(
         NAME "${run_target_name}_coverage"
         EXECUTABLE "${run_target_name}"
         DEPENDENCIES "${run_target_name}"
      )
   endif()
endfunction()

function(ppg_add_compiler_test name)

   if(NOT PAPAGENO_BUILD_GLOCKENSPIEL)
      return()
   endif()

   set(multi_value_args "")
   set(one_value_args "COMPILE_DEFINITIONS;LINK_LIBRARIES")
   set(options "")

   cmake_parse_arguments(args "${options}" "${one_value_args}"
                     "${multi_value_args}" ${ARGN})

   # Add the normal test executable
   #
   set(run_target_name "${name}_compiled")
   
   set(ppg_file "${CMAKE_CURRENT_SOURCE_DIR}/${name}/${name}_ppg.h")                     
   set(code_file "${CMAKE_CURRENT_SOURCE_DIR}/${name}/${name}_compiler_run.c")
	set(compile_output_file "${CMAKE_CURRENT_BINARY_DIR}/${name}/${name}_ppg_compiled.c")
   
   set(PPG_CS_COMPILED_CODE_INCLUDE_FILE "\"${compile_output_file}\"")
      
   ppg_get_test_file_defines(
      "${name}"
      "actions;context_initialization;tests;token_tree;includes;layers;ppg"
   )
   configure_file("${CMAKE_CURRENT_SOURCE_DIR}/test_with_compile.c.in" "${code_file}")
   
   add_custom_command(
      OUTPUT "${compile_output_file}"
      DEPENDS "${ppg_file}" glockenspiel
      COMMAND $<TARGET_FILE:glockenspiel> -i "${ppg_file}" -o "${compile_output_file}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${name}"
   )
   add_custom_target("${run_target_name}_compile" DEPENDS "${compile_output_file}")
   
#    set_source_files_properties("${compile_output_file}" PROPERTIES GENERATED TRUE)

	add_dependencies(
      "${run_target_name}_compile"
      glockenspiel
   )
   
	add_executable(
		"${run_target_name}"
		"${code_file}"
	)
	
	add_dependencies(
      "${run_target_name}"
      "${run_target_name}_compile"
   )
   
   set_target_properties(
      "${run_target_name}"
      PROPERTIES COMPILE_FLAGS 
         "${args_COMPILE_DEFINITIONS}"
   )

	target_link_libraries(
		"${run_target_name}"
		papageno_char_strings
	)

   if(DEFINED args_LINK_LIBRARIES)
      target_link_libraries(
         "${run_target_name}"
         "${args_LINK_LIBRARIES}"
      )
	endif()
   
	ppg_generate_test(
		NAME "${run_target_name}" 
		EXECUTABLE "${run_target_name}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	)
	
   set_tests_properties("${run_target_name}" PROPERTIES DEPENDS "${run_target_name}_compile")
   
	if(PAPAGENO_TEST_COVERAGE)
	      
      SETUP_TARGET_FOR_JOINT_COVERAGE(
         NAME "${run_target_name}_coverage"
         EXECUTABLE "${run_target_name}"
         DEPENDENCIES "${run_target_name}"
      )
   endif()
endfunction()

macro(ppg_get_test_file_defines
   name_
   type_list_
)  
   foreach(type ${type_list_})

      set(include_file "${CMAKE_CURRENT_SOURCE_DIR}/${name}/${name_}_${type}.h")

      if(NOT EXISTS "${include_file}")
         set(include_file "${CMAKE_CURRENT_SOURCE_DIR}/default_${type}.h")
      endif()

      string(TOUPPER "${type}" type_upper)

      set(define_name "PPG_CS_${type_upper}_INCLUDE_FILE")
      
      file(RELATIVE_PATH include_file_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${include_file}")

      set(${define_name} "\"${include_file_rel}\"")

   endforeach()
endmacro()

function(ppg_add_test_full name)
   ppg_add_compiler_test("${name}")
   ppg_add_test_with_compression("${name}")
endfunction()

function(ppg_add_test_with_compression name)

   set(multi_value_args "")
   set(one_value_args "COMPILE_DEFINITIONS;LINK_LIBRARIES")
   set(options "")

   cmake_parse_arguments(args "${options}" "${one_value_args}"
                     "${multi_value_args}" ${ARGN} )

   set(code_file "${CMAKE_CURRENT_SOURCE_DIR}/test_with_compression.c.in")

	include_directories("${CMAKE_CURRENT_BINARY_DIR}")
	
   ppg_get_test_file_defines(
      "${name}"
      "actions;context_initialization;tests;token_tree;includes;layers"
   )

#    message("file_defines: ${file_defines}")

   # Add the normal test executable
   #
   set(run_target_name "${name}_run")

   set(run_target_name_c "${CMAKE_CURRENT_BINARY_DIR}/${name}/${name}_compiler_run.c")
   configure_file("${code_file}" "${run_target_name_c}")

	add_executable(
		"${run_target_name}"
		"${run_target_name_c}"
	)

   set_target_properties(
      "${run_target_name}"
      PROPERTIES COMPILE_FLAGS 
         "${args_COMPILE_DEFINITIONS}"
   )

	target_link_libraries(
		"${run_target_name}"
		papageno_char_strings
	)

   if(DEFINED args_LINK_LIBRARIES)
      target_link_libraries(
         "${run_target_name}"
         "${args_LINK_LIBRARIES}"
      )
	endif()

	ppg_generate_test(
		NAME "${run_target_name}" 
		EXECUTABLE "${run_target_name}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	)
	
	if(PAPAGENO_TEST_COVERAGE)
	      
      SETUP_TARGET_FOR_JOINT_COVERAGE(
         NAME "${run_target_name}_coverage"
         EXECUTABLE "${run_target_name}"
         DEPENDENCIES "${run_target_name}"
      )
   endif()
	   
   # Add an executable that compresses the context
   #
   set(build_compress_executable_target_name "${name}_compress_build_executable")
	set(compressed_code_file "${CMAKE_CURRENT_BINARY_DIR}/${name}/${build_compress_executable_target_name}_compressed_code.h")
	
   set(build_compress_executable_target_name_c "${CMAKE_CURRENT_BINARY_DIR}/${name}/${build_compress_executable_target_name}.c")
   configure_file("${code_file}" "${build_compress_executable_target_name_c}")

   add_executable(
		"${build_compress_executable_target_name}"
		"${build_compress_executable_target_name_c}"
	)
		
   set_target_properties(
		"${build_compress_executable_target_name}"
      PROPERTIES COMPILE_FLAGS 
         "${file_defines} -DPPG_CS_COMPRESS_CONTEXT=1 -DPPG_CS_SUPPRESS_TESTS=1 ${args_COMPILE_DEFINITIONS}"
   )

	target_link_libraries(
		"${build_compress_executable_target_name}"
		papageno_char_strings
	)	

   if(DEFINED args_LINK_LIBRARIES)
      target_link_libraries(
         "${build_compress_executable_target_name}"
         "${args_LINK_LIBRARIES}"
      )
   endif()
   
   if(PAPAGENO_TEST_COVERAGE)
	      
      SETUP_TARGET_FOR_JOINT_COVERAGE(
         NAME "${build_compress_executable_target_name}_coverage"
         EXECUTABLE "${build_compress_executable_target_name}"
         DEPENDENCIES "${build_compress_executable_target_name}"
      )
   endif()

   set(compress_target_name "${name}_compress_processing")
   
   # Add a target that issues context compression during build
   #
	add_custom_command(
      OUTPUT "${compressed_code_file}"
      COMMAND ${CMAKE_COMMAND} 
         "-DPAPAGENO_TEST_EXECUTABLE=${CMAKE_CURRENT_BINARY_DIR}/${build_compress_executable_target_name}"
         "-DPAPAGENO_COMPRESSED_CODE_FILE=${compressed_code_file}"
         "-DPAPAGENO_RUN_WRAPPER=\"${PAPAGENO_RUN_WRAPPER}\""
         -P "${CMAKE_CURRENT_SOURCE_DIR}/TestingWithCompression.script.cmake"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
   )
	
	add_custom_target("${compress_target_name}" DEPENDS "${compressed_code_file}")
	
	add_dependencies("${compress_target_name}" "${build_compress_executable_target_name}")
	
   # To allow for memory debugger based testing of the compression process
   # we also add it as a test, although it has already been run during the build
   # stage.
   #
   set(compress_test_target_name "${name}_compress")
   ppg_generate_test(
      NAME "${compress_test_target_name}" 
      EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/${build_compress_executable_target_name}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
   )

	# Add an executable that extracts the copressed context
	# and runs tests
	#
   set(extract_and_test_target_name "${name}_extract_and_test")

   set(PPG_CS_COMPRESSED_CODE_FILE "#include \"${compressed_code_file}\"")
   set(extract_and_test_target_name_c "${CMAKE_CURRENT_BINARY_DIR}/${name}/${extract_and_test_target_name}.c")
   configure_file("${code_file}" "${extract_and_test_target_name_c}")
   set(PPG_CS_COMPRESSED_CODE_FILE "")

   add_executable(
		"${extract_and_test_target_name}"
		"${extract_and_test_target_name_c}"
	)
	
	add_dependencies("${extract_and_test_target_name}" "${compress_target_name}")
	
   set_target_properties(
		"${extract_and_test_target_name}"
      PROPERTIES COMPILE_FLAGS
         "${file_defines} -DPPG_CS_READ_COMPRESSED_CONTEXT=1 ${args_COMPILE_DEFINITIONS}"
   )

	target_link_libraries(
		"${extract_and_test_target_name}"
		papageno_char_strings
	)
	   
   if(DEFINED args_LINK_LIBRARIES)
      target_link_libraries(
         "${extract_and_test_target_name}"
         "${args_LINK_LIBRARIES}"
      )
   endif()

	ppg_generate_test(
		NAME "${extract_and_test_target_name}" 
		EXECUTABLE "${extract_and_test_target_name}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	)
	
   if(PAPAGENO_TEST_COVERAGE)
	      
      SETUP_TARGET_FOR_JOINT_COVERAGE(
         NAME "${extract_and_test_target_name}_coverage"
         EXECUTABLE "${extract_and_test_target_name}"
         DEPENDENCIES "${extract_and_test_target_name}"
      )
   endif()

endfunction()

ppg_add_test(context_switching)
ppg_add_test(enable_disable)
ppg_add_test(enable_disable_timeout)
ppg_add_test(fallback)

ppg_add_test_full(abort_trigger)
ppg_add_test_full(chords)
ppg_add_test_full(clusters)
ppg_add_test_full(layers)
ppg_add_test_full(leader_sequences)
ppg_add_test_full(note_lines)
ppg_add_test_full(strict_notes)
ppg_add_test_full(token_precedence)

if(NOT "${PAPAGENO_PLATFORM_ACTUAL}" STREQUAL "avr-gcc")

   # The following test is too large for the atmel platform
   #
   ppg_add_test_with_compression(large_systems COMPILE_DEFINITIONS 
      "-DPAPAGENO_TEST__TREE_DEPTH=4 -DPAPAGENO_TEST__N_CHARS=4 -DPAPAGENO_TEST__FAIL=1"
      LINK_LIBRARIES "m"
   )
endif()

# ppg_add_test_with_compression(large_systems_2)
