cmake_minimum_required(VERSION 3.30.5)
project(AstralReign)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#SFML setup
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

### Todo: Fix this to not generate 1.2 GB of files, for some reason.
#Clay setup
#include(FetchContent)
#FetchContent_Declare(Clay
#    GIT_REPOSITORY https://github.com/nicbarker/clay.git
#    GIT_TAG v0.13
#    GIT_SHALLOW ON
#    EXCLUDE_FROM_ALL
#    SYSTEM)
#FetchContent_MakeAvailable(clay)



add_executable(${PROJECT_NAME} src/main.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::System SFML::Graphics SFML::Audio SFML::Window)