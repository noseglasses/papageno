option(PAPAGENO_BUILD_PARSER FALSE)

if(PAPAGENO_BUILD_PARSER)

   option(PAPAGENO_DEVELOP_PARSER FALSE)
   mark_as_advanced(PAPAGENO_DEVELOP_PARSER)
   
   set(parser_lex "${CMAKE_CURRENT_SOURCE_DIR}/parser.lex.c")
   set(parser_lex_h "${CMAKE_CURRENT_SOURCE_DIR}/parser.lex.h")
   
   set(parser_yacc "${CMAKE_CURRENT_SOURCE_DIR}/parser.yacc.c")
   set(parser_yacc_h "${CMAKE_CURRENT_SOURCE_DIR}/parser.yacc.h")
   
   if(PAPAGENO_DEVELOP_PARSER)
      
      find_package(FLEX)
      
      if(NOT FLEX_FOUND)
         message(FATAL_ERROR "Cannot find the flex lexer package")
      endif()
      
      flex_target(
         parser_lex "${CMAKE_CURRENT_SOURCE_DIR}/parser.lex" "${parser_lex}" 
         COMPILE_FLAGS "--header-file=${parser_lex_h} --nounput")
      
      find_package(BISON)
      
      if(NOT BISON_FOUND)
         message(FATAL_ERROR "Unable to find bison")
      endif()
      set(yacc_verbose_output "${CMAKE_CURRENT_BINARY_DIR}/parser.yacc.log")
      
      bison_target(
         parser_yacc "${CMAKE_CURRENT_SOURCE_DIR}/parser.yacc" "${parser_yacc}" 
         VERBOSE "${yacc_verbose_output}"
         COMPILE_FLAGS --debug
      )
   
   endif()
   
   add_executable(
      parser
      "${parser_lex}" "${parser_yacc}"
   )
   set_property(TARGET parser PROPERTY C_STANDARD 99)
   
   target_compile_definitions(parser PUBLIC "-D_POSIX_C_SOURCE=1" "-DYY_NO_INPUT")
   
   if(TARGET parser_yacc)
      add_dependencies(parser parser_yacc)
   endif()
   
   if(TARGET parser_lex)
      add_dependencies(parser parser_lex)
   endif()
endif()