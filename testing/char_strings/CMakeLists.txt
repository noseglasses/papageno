include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_library(

	papageno_char_strings
	papageno_char_strings.c
)

target_link_libraries(
	papageno_char_strings
	papageno
)

macro(ppg_add_test test_name)

	add_executable(
		"${test_name}"
		"${test_name}.c"
	)

	target_link_libraries(
		"${test_name}"
		papageno_char_strings
	)

#	add_test(NAME <name> COMMAND <command> [<arg>...]
#         [CONFIGURATIONS <config>...]
#         [WORKING_DIRECTORY <dir>])

	add_test(
		NAME "${test_name}" 
		COMMAND "${test_name}"
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	)
	
endmacro()
 
ppg_add_test(test_note_lines)
ppg_add_test(test_chords)
ppg_add_test(test_clusters)
ppg_add_test(test_context_switching)
ppg_add_test(test_fall_back)
ppg_add_test(test_fall_through)
ppg_add_test(test_abort_trigger)
ppg_add_test(test_enable_disable)
ppg_add_test(test_enable_disable_timeout)
ppg_add_test(test_layers)
ppg_add_test(test_token_precedence)
ppg_add_test(test_strict_notes)

# set_property(TARGET test_char_strings PROPERTY C_STANDARD 99)