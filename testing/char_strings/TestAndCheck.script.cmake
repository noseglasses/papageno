execute_process(
   COMMAND ${CMD}
   OUTPUT_VARIABLE output
   ERROR_VARIABLE error
   RESULT_VARIABLE result
)

if(   ("${output}" MATCHES "__PAPAGENO_TEST_FAILED__")
   OR ("${error}" MATCHES "__PAPAGENO_TEST_FAILED__")
   OR (NOT "${result}" EQUAL 0)
)
   message("Error running ${CMD}")
   message("output: ${output}")
   message("error: ${error}")
   message(FATAL_ERROR "Exit with error")
endif()