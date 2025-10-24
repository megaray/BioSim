cmake_minimum_required(VERSION 3.28)
project(BioSim LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main src/main.cpp
    src/neuralnetwork.h src/neuralnetwork.cpp
    src/terraintype.h src/terraintype.cpp
    src/survivallogic.h src/survivallogic.cpp
    src/entity.h src/entity.cpp
    src/gui.h src/gui.cpp
    src/simulation.h src/simulation.cpp
)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics)
