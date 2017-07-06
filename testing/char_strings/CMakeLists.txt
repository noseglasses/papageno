include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(
	test_char_strings
	
	papageno_char_strings.c
	test_char_strings.c
)

target_link_libraries(
	test_char_strings
	papageno
)

# set_property(TARGET test_char_strings PROPERTY C_STANDARD 99)