cmake_minimum_required(VERSION 3.14.0 FATAL_ERROR)

project(dqcsim-qx
    VERSION 0.0.1
    DESCRIPTION "DQCsim backend for the QX simulator"
    LANGUAGES CXX
)

# Include DQCsim.
include(FetchContent)
FetchContent_Declare(dqcsim
    GIT_REPOSITORY  https://github.com/mbrobbel/dqcsim.git
    GIT_TAG         master
)
FetchContent_MakeAvailable(dqcsim)

# Include QX.
include(cmake/qx.cmake)

# Main operator executable.
add_executable(
    dqcsbeqx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)
target_include_directories(
    dqcsbeqx PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(dqcsbeqx dqcsim qx)