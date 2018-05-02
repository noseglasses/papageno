cmake_minimum_required(VERSION 2.8)

project(Glockenspiel CXX)

set (CMAKE_CXX_STANDARD 11)

option(PAPAGENO_DEVELOP_GLOCKENSPIEL FALSE)
mark_as_advanced(PAPAGENO_DEVELOP_GLOCKENSPIEL)

set(glockenspiel_lex "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/GLS_Parser.lex.cpp")
set(glockenspiel_lex_h "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/GLS_Parser.lex.hpp")

set(glockenspiel_yacc "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/GLS_Parser.yacc.cpp")
set(glockenspiel_yacc_h "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/GLS_Parser.yacc.hpp")

if(PAPAGENO_DEVELOP_GLOCKENSPIEL)

   find_program(GENGETOPT_EXECUTABLE
      NAMES "gengetopt"
   )
   
   if("${GENGETOPT_EXECUTABLE}" STREQUAL "GENGETOPT_EXECUTABLE-NOTFOUND")
      message(FATAL_ERROR "Cannot find the gengetopt executable")
   endif()
   
   set(cmdline_txt_in "${CMAKE_CURRENT_SOURCE_DIR}/src/CommandLine/GLS_CommandLineGengetopt.txt.in")
   set(cmdline_txt "${CMAKE_CURRENT_BINARY_DIR}/src/CommandLine/GLS_CommandLineGengetopt.txt")
   set(cmdline_h "${CMAKE_CURRENT_SOURCE_DIR}/src/CommandLine/GLS_CommandLineGengetopt.hpp")
   set(cmdline_c "${CMAKE_CURRENT_SOURCE_DIR}/src/CommandLine/GLS_CommandLineGengetopt.cpp")
   set(glockenspiel_cmdline_deps "${cmdline_h}" "${cmdline_c}")
   
   configure_file("${cmdline_txt_in}" "${cmdline_txt}")
   
   add_custom_command(
      OUTPUT ${glockenspiel_cmdline_deps}
      DEPENDS "${cmdline_txt}"
      COMMAND "${GENGETOPT_EXECUTABLE}" "--input=${cmdline_txt}" --file-name=GLS_CommandLineGengetopt --header-extension=hpp --c-extension=cpp --include-getopt
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/CommandLine"
      COMMENT "Generating glockenspiel command line sepecification"
   )
   
   add_custom_target(glockenspiel_cmdline DEPENDS ${glockenspiel_cmdline_deps})
   
   find_package(FLEX)
   
   if(NOT FLEX_FOUND)
      message(FATAL_ERROR "Cannot find the flex lexer package")
   endif()
   
   flex_target(
      glockenspiel_lex "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/GLS_Parser.lex" "${glockenspiel_lex}" 
      COMPILE_FLAGS "--header-file=${glockenspiel_lex_h} --nounput")
   
   find_package(BISON)
   
   if(NOT BISON_FOUND)
      message(FATAL_ERROR "Unable to find bison")
   endif()
   set(yacc_verbose_output "${CMAKE_CURRENT_BINARY_DIR}/src/Parser/GLS_Parser.yacc.log")
   
#       if(${CMAKE_VERSION} GREATER 3.6.0) 
#          set(bison_report_file_spec VERBOSE REPORT_FILE "${yacc_verbose_output}")
#       else()
#          set(bison_report_file_spec VERBOSE "${yacc_verbose_output}")
#       endif()
   
   bison_target(
      glockenspiel_yacc "${CMAKE_CURRENT_SOURCE_DIR}/src/Parser/GLS_Parser.yacc" "${glockenspiel_yacc}"
#          ${bison_report_file_spec}
      COMPILE_FLAGS "--debug -v -l"
   )
   
   if(NOT BISON_glockenspiel_yacc_DEFINED)
      message(FATAL_ERROR "Failed defining bison target")
   endif()

endif()

set_source_files_properties("${cmdline_c}" PROPERTIES COMPILE_FLAGS -Wno-unused-but-set-variable)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

set(sources "")
function(collect_sources source_list subdir)
   file(GLOB_RECURSE sources__ "${CMAKE_CURRENT_SOURCE_DIR}/src/${subdir}/*.cpp")
   set("${source_list}" "${${source_list}};${sources__}" PARENT_SCOPE)
endfunction()

collect_sources(sources "Generator")
collect_sources(sources "ParserTree")
collect_sources(sources "Settings")
collect_sources(sources "CommandLine")

#    foreach(source ${sources})
#       message("${source}")
#    endforeach()

set(glockenspiel_hpp_in "${CMAKE_CURRENT_SOURCE_DIR}/GLS_Compiler.hpp.in")
set(glockenspiel_hpp "${CMAKE_CURRENT_BINARY_DIR}/GLS_Compiler.hpp")

configure_file("${glockenspiel_hpp_in}" "${glockenspiel_hpp}")

add_executable(
   glockenspiel
   "${glockenspiel_lex}" "${glockenspiel_yacc}" "${cmdline_c}"
   ${sources}
   "${CMAKE_CURRENT_SOURCE_DIR}/GLS_Compiler.cpp"
)
set_property(TARGET glockenspiel PROPERTY C_STANDARD 99)

# target_compile_definitions(glockenspiel PUBLIC "-D_POSIX_C_SOURCE=1" "-DYY_NO_INPUT")
target_compile_definitions(glockenspiel PUBLIC "-DYY_NO_INPUT")

if(TARGET glockenspiel_cmdline)
   add_dependencies(glockenspiel glockenspiel_cmdline)
endif()

if(TARGET glockenspiel_yacc)
   add_dependencies(glockenspiel glockenspiel_yacc)
endif()

if(TARGET glockenspiel_lex)
   add_dependencies(glockenspiel glockenspiel_lex)
endif()
