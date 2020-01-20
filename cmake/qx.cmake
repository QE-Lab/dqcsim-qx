
# Add the QX subdir, then disable as many targets as we can...
add_subdirectory(${PROJECT_SOURCE_DIR}/qx-simulator)
set_target_properties(qx-server PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(qx-simulator-old PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(qx-simulator PROPERTIES EXCLUDE_FROM_ALL TRUE)

# Unfortunately QX's CMakeLists don't expose a library to link against. So we
# create one here.
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
        lexgram
    INTERFACE_INCLUDE_DIRECTORIES
        "${QX_SRC};${QX_SRC}/xpu-0.1.5;${QX_SRC}/parser/libqasm/qasm_flex_bison/library/;${QX_SRC}/qcode"
)
separate_arguments(QX_CFLAGS UNIX_COMMAND ${QX_CFLAGS})
target_compile_options(qx INTERFACE ${QX_CFLAGS})
target_link_options(qx INTERFACE ${QX_CFLAGS})
