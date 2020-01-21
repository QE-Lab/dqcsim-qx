
# Unfortunately QX's CMakeLists don't expose a library to link against. So we
# have to create one here. Also, libqasm very annoyingly produces only a shared
# object file called liblexgram.so, i.e. completely not namespaced for cQASM so
# not something you want in your /usr/lib... so we define THAT target here as
# well and instead link statically.

# Use C++11.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)

#==============================================================================
# libQASM
#==============================================================================
set(LIBQASM_SRC ${PROJECT_SOURCE_DIR}/qx-simulator/src/parser/libqasm/qasm_flex_bison/library)

find_package(BISON 3.0)
find_package(FLEX 2.6)

bison_target(
    libqasm_parser
    ${LIBQASM_SRC}/grammar.y
    ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.c
    COMPILE_FLAGS "-Wall -d -t -g -r all"
)
flex_target(
    libqasm_scanner
    ${LIBQASM_SRC}/lex.l
    ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c
)
add_flex_bison_dependency(libqasm_scanner libqasm_parser)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.c PROPERTIES LANGUAGE CXX)
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c PROPERTIES LANGUAGE CXX)
add_library(libqasm STATIC ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.c ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)
target_include_directories(libqasm PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${LIBQASM_SRC})
set_property(TARGET libqasm PROPERTY CXX_STANDARD 11)

#==============================================================================
# QX simulator
#==============================================================================
set(QX_SRC ${PROJECT_SOURCE_DIR}/qx-simulator/src)

find_package(OpenMP)
if(OPENMP_FOUND)
  set(QX_CFLAGS "${QX_CFLAGS} ${OpenMP_CXX_FLAGS}")
else(OPENMP_FOUND)
  message("ERROR: OpenMP could not be found.")
endif(OPENMP_FOUND)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(QX_CFLAGS "${QX_CFLAGS} -std=c++11 -Wall -Wfatal-errors -DCG_BC -DQX_SPARSE_MV_MUL -D__BUILTIN_LINALG__ -Wno-sign-compare -msse4.2 -DUSE_OPENMP -Wno-unused-variable -Wno-reorder")
endif(CMAKE_COMPILER_IS_GNUCXX)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CXX "Clang-omp++" PARENT_SCOPE)
  set(QX_CFLAGS "${QX_CFLAGS} -std=c++11 -Wall -Wfatal-errors -Wno-unused-local-typedef -DCG_BC -DQX_SPARSE_MV_MUL -D__BUILTIN_LINALG__ -msse4.2 -DUSE_OPENMP -Wno-reorder -Wno-unneeded-internal-declaration -Wno-unused-variable -Wno-unused-private-field -Wno-deprecated-register -Wno-unused-function")
endif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")

set(QX_CFLAGS "${QX_CFLAGS} -DXPU_TIMER")

add_library(qx INTERFACE)
set_target_properties(qx PROPERTIES
    INTERFACE_LINK_LIBRARIES
        libqasm
    INTERFACE_INCLUDE_DIRECTORIES
        "${QX_SRC};${QX_SRC}/xpu-0.1.5;${QX_SRC}/qcode"
)
separate_arguments(QX_CFLAGS UNIX_COMMAND ${QX_CFLAGS})
target_compile_options(qx INTERFACE ${QX_CFLAGS})
target_link_options(qx INTERFACE ${QX_CFLAGS})
