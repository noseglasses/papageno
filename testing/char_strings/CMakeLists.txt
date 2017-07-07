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

endmacro()
 
ppg_add_test(test_note_lines)
ppg_add_test(test_chords)

# set_property(TARGET test_char_strings PROPERTY C_STANDARD 99)