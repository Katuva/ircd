cmake_minimum_required(VERSION 3.31)
project(ircd)

include(FetchContent)

FetchContent_Declare(
        fkYAML
        GIT_REPOSITORY https://github.com/fktn-k/fkYAML.git
        GIT_TAG v0.4.2
)
FetchContent_MakeAvailable(fkYAML)

set(CMAKE_CXX_STANDARD 26)

add_executable(ircd main.cpp)
target_link_libraries(ircd PRIVATE fkYAML::fkYAML)
