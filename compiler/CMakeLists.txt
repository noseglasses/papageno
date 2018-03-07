
if(PAPAGENO_BUILD_COMPILER)

   set (CMAKE_CXX_STANDARD 11)

   option(PAPAGENO_DEVELOP_COMPILER FALSE)
   mark_as_advanced(PAPAGENO_DEVELOP_COMPILER)
   
   set(compiler_lex "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/PPG_Parser.lex.cpp")
   set(compiler_lex_h "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/PPG_Parser.lex.hpp")
   
   set(compiler_yacc "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/PPG_Parser.yacc.cpp")
   set(compiler_yacc_h "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/PPG_Parser.yacc.hpp")
   
   if(PAPAGENO_DEVELOP_COMPILER)
   
      find_program(GENGETOPT_EXECUTABLE
         NAMES "gengetopt"
      )
      
      if("${GENGETOPT_EXECUTABLE}" STREQUAL "GENGETOPT_EXECUTABLE-NOTFOUND")
         message(FATAL_ERROR "Cannot find the gengetopt executable")
      endif()
      
      set(cmdline_txt_in "${CMAKE_CURRENT_SOURCE_DIR}/src/CommandLine/PPG_CommandLine.txt.in")
      set(cmdline_txt "${CMAKE_CURRENT_BINARY_DIR}/src/CommandLine/PPG_CommandLine.txt")
      set(cmdline_h "${CMAKE_CURRENT_SOURCE_DIR}/src/CommandLine/PPG_CommandLine.hpp")
      set(cmdline_c "${CMAKE_CURRENT_SOURCE_DIR}/src/CommandLine/PPG_CommandLine.cpp")
      set(compiler_cmdline_deps "${cmdline_h}" "${cmdline_c}")
      
      configure_file("${cmdline_txt_in}" "${cmdline_txt}")
      
      add_custom_command(
         OUTPUT ${compiler_cmdline_deps}
         DEPENDS "${cmdline_txt}"
         COMMAND "${GENGETOPT_EXECUTABLE}" "--input=${cmdline_txt}" --file-name=PPG_CommandLine --header-extension=hpp --c-extension=cpp --include-getopt
         WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/CommandLine"
         COMMENT "Generating compiler command line sepecification"
      )
      
      add_custom_target(compiler_cmdline DEPENDS ${compiler_cmdline_deps})
      
      find_package(FLEX)
      
      if(NOT FLEX_FOUND)
         message(FATAL_ERROR "Cannot find the flex lexer package")
      endif()
      
      flex_target(
         compiler_lex "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/PPG_Parser.lex" "${compiler_lex}" 
         COMPILE_FLAGS "--header-file=${compiler_lex_h} --nounput")
      
      find_package(BISON)
      
      if(NOT BISON_FOUND)
         message(FATAL_ERROR "Unable to find bison")
      endif()
      set(yacc_verbose_output "${CMAKE_CURRENT_BINARY_DIR}/src/Parser/PPG_Parser.yacc.log")
      
      bison_target(
         compiler_yacc "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/PPG_Parser.yacc" "${compiler_yacc}" 
         VERBOSE "${yacc_verbose_output}"
         COMPILE_FLAGS --debug
      )
   
   endif()
   
   set_source_files_properties("${cmdline_c}" PROPERTIES COMPILE_FLAGS -Wno-unused-but-set-variable)
   
   include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
   include_directories("${CMAKE_CURRENT_BINARY_DIR}")
   
   file(GLOB_RECURSE generator_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/Generator/*.cpp")
   file(GLOB_RECURSE parser_tree_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/ParserTree/*.cpp")
   
   set(compiler_hpp_in "${CMAKE_CURRENT_SOURCE_DIR}/PPG_Compiler.hpp.in")
   set(compiler_hpp "${CMAKE_CURRENT_BINARY_DIR}/PPG_Compiler.hpp")
   
   configure_file("${compiler_hpp_in}" "${compiler_hpp}")
   
   add_executable(
      compiler
      "${compiler_lex}" "${compiler_yacc}" "${cmdline_c}"
      ${generator_sources}
      ${parser_tree_sources}
      "${CMAKE_CURRENT_SOURCE_DIR}/PPG_Compiler.cpp"
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
