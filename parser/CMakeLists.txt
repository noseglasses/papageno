
if(PAPAGENO_BUILD_COMPILER)

   option(PAPAGENO_DEVELOP_PARSER FALSE)
   mark_as_advanced(PAPAGENO_DEVELOP_PARSER)
   
   set(compiler_lex "${CMAKE_CURRENT_SOURCE_DIR}/compiler.lex.c")
   set(compiler_lex_h "${CMAKE_CURRENT_SOURCE_DIR}/compiler.lex.h")
   
   set(compiler_yacc "${CMAKE_CURRENT_SOURCE_DIR}/compiler.yacc.c")
   set(compiler_yacc_h "${CMAKE_CURRENT_SOURCE_DIR}/compiler.yacc.h")
   
   if(PAPAGENO_DEVELOP_PARSER)
   
      find_program(GENGETOPT_EXECUTABLE
         NAMES "gengetopt"
      )
      
      if("${GENGETOPT_EXECUTABLE}" STREQUAL "GENGETOPT_EXECUTABLE-NOTFOUND")
         message(FATAL_ERROR "Cannot find the gengetopt executable")
      endif()
      
      set(cmdline_in "${CMAKE_CURRENT_SOURCE_DIR}/compiler.cmdline")
      set(cmdline_h "${CMAKE_CURRENT_SOURCE_DIR}/cmdline.h")
      set(cmdline_c "${CMAKE_CURRENT_SOURCE_DIR}/cmdline.c")
      set(compiler_cmdline_deps "${cmdline_h}" "${cmdline_c}")
      add_custom_command(
         OUTPUT "${cmdline_h}" "${cmdline_c}"
         DEPENDS "${cmdline_in}"
         COMMAND "${GENGETOPT_EXECUTABLE}" "--input=${cmdline_in}" --include-getopt
         WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
         COMMENT "Generating compiler command line sepecification"
      )
      
      add_custom_target(compiler_cmdline DEPENDS ${compiler_cmdline_deps})
      
      find_package(FLEX)
      
      if(NOT FLEX_FOUND)
         message(FATAL_ERROR "Cannot find the flex lexer package")
      endif()
      
      flex_target(
         compiler_lex "${CMAKE_CURRENT_SOURCE_DIR}/compiler.lex" "${compiler_lex}" 
         COMPILE_FLAGS "--header-file=${compiler_lex_h} --nounput")
      
      find_package(BISON)
      
      if(NOT BISON_FOUND)
         message(FATAL_ERROR "Unable to find bison")
      endif()
      set(yacc_verbose_output "${CMAKE_CURRENT_BINARY_DIR}/compiler.yacc.log")
      
      bison_target(
         compiler_yacc "${CMAKE_CURRENT_SOURCE_DIR}/compiler.yacc" "${compiler_yacc}" 
         VERBOSE "${yacc_verbose_output}"
         COMPILE_FLAGS --debug
      )
   
   endif()
   
   set_source_files_properties("${cmdline_c}" PROPERTIES COMPILE_FLAGS -Wno-unused-but-set-variable)
   
   add_executable(
      compiler
      "${compiler_lex}" "${compiler_yacc}" "${cmdline_c}"
   )
   set_property(TARGET compiler PROPERTY C_STANDARD 99)
   
   target_compile_definitions(compiler PUBLIC "-D_POSIX_C_SOURCE=1" "-DYY_NO_INPUT")
   
   if(TARGET compiler_cmdline)
      add_dependencies(compiler compiler_cmdline)
   endif()
   
   if(TARGET compiler_yacc)
      add_dependencies(compiler compiler_yacc)
   endif()
   
   if(TARGET compiler_lex)
      add_dependencies(compiler compiler_lex)
   endif()
endif()
