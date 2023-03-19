cmake_minimum_required (VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project ("RavenageGameSpy")

find_package(CURL CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)

add_executable (RavenageGameSpy "RavenageGameSpy.cpp" )

target_compile_options(RavenageGameSpy PRIVATE /MT)

target_link_libraries(RavenageGameSpy PRIVATE CURL::libcurl rapidjson)

install(TARGETS RavenageGameSpy DESTINATION .)
