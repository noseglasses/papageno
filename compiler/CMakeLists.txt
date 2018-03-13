
if(PAPAGENO_BUILD_COMPILER)

   set (CMAKE_CXX_STANDARD 11)

   option(PAPAGENO_DEVELOP_COMPILER FALSE)
   mark_as_advanced(PAPAGENO_DEVELOP_COMPILER)
   
   set(glockenspiel_lex "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/PPG_Parser.lex.cpp")
   set(glockenspiel_lex_h "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/PPG_Parser.lex.hpp")
   
   set(glockenspiel_yacc "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/PPG_Parser.yacc.cpp")
   set(glockenspiel_yacc_h "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/PPG_Parser.yacc.hpp")
   
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
      set(glockenspiel_cmdline_deps "${cmdline_h}" "${cmdline_c}")
      
      configure_file("${cmdline_txt_in}" "${cmdline_txt}")
      
      add_custom_command(
         OUTPUT ${glockenspiel_cmdline_deps}
         DEPENDS "${cmdline_txt}"
         COMMAND "${GENGETOPT_EXECUTABLE}" "--input=${cmdline_txt}" --file-name=PPG_CommandLine --header-extension=hpp --c-extension=cpp --include-getopt
         WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/CommandLine"
         COMMENT "Generating glockenspiel command line sepecification"
      )
      
      add_custom_target(glockenspiel_cmdline DEPENDS ${glockenspiel_cmdline_deps})
      
      find_package(FLEX)
      
      if(NOT FLEX_FOUND)
         message(FATAL_ERROR "Cannot find the flex lexer package")
      endif()
      
      flex_target(
         glockenspiel_lex "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/PPG_Parser.lex" "${glockenspiel_lex}" 
         COMPILE_FLAGS "--header-file=${glockenspiel_lex_h} --nounput")
      
      find_package(BISON)
      
      if(NOT BISON_FOUND)
         message(FATAL_ERROR "Unable to find bison")
      endif()
      set(yacc_verbose_output "${CMAKE_CURRENT_BINARY_DIR}/src/Parser/PPG_Parser.yacc.log")
      
      bison_target(
         glockenspiel_yacc "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/PPG_Parser.yacc" "${glockenspiel_yacc}" 
         VERBOSE REPORT_FILE "${yacc_verbose_output}"
         COMPILE_FLAGS --debug
      )
   
   endif()
   
   set_source_files_properties("${cmdline_c}" PROPERTIES COMPILE_FLAGS -Wno-unused-but-set-variable)
   
   include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
   include_directories("${CMAKE_CURRENT_BINARY_DIR}")
   
   file(GLOB_RECURSE generator_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/Generator/*.cpp")
   file(GLOB_RECURSE parser_tree_sources "${CMAKE_CURRENT_SOURCE_DIR}/src/ParserTree/*.cpp")
   
   set(glockenspiel_hpp_in "${CMAKE_CURRENT_SOURCE_DIR}/PPG_Compiler.hpp.in")
   set(glockenspiel_hpp "${CMAKE_CURRENT_BINARY_DIR}/PPG_Compiler.hpp")
   
   configure_file("${glockenspiel_hpp_in}" "${glockenspiel_hpp}")
   
   add_executable(
      glockenspiel
      "${glockenspiel_lex}" "${glockenspiel_yacc}" "${cmdline_c}"
      ${generator_sources}
      ${parser_tree_sources}
      "${CMAKE_CURRENT_SOURCE_DIR}/PPG_Compiler.cpp"
   )
   set_property(TARGET glockenspiel PROPERTY C_STANDARD 99)
   
   target_compile_definitions(glockenspiel PUBLIC "-D_POSIX_C_SOURCE=1" "-DYY_NO_INPUT")
   
   if(TARGET glockenspiel_cmdline)
      add_dependencies(glockenspiel glockenspiel_cmdline)
   endif()
   
   if(TARGET glockenspiel_yacc)
      add_dependencies(glockenspiel glockenspiel_yacc)
   endif()
   
   if(TARGET glockenspiel_lex)
      add_dependencies(glockenspiel glockenspiel_lex)
   endif()
endif()
