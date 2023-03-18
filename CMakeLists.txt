
cmake_minimum_required (VERSION 3.8)

include("D:/vcpkg/scripts/buildsystems/vcpkg.cmake")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")




set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project ("RavenageGameSpy")

set(CURL_DIR D:/vcpkg/installed/x64-windows-static/share/curl/)
set(RapidJSON_DIR D:/vcpkg/installed/x64-windows-static/share/rapidjson/)

find_package(CURL CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)


# Add source to this project's executable.
add_executable (RavenageGameSpy "RavenageGameSpy.cpp" )


  target_compile_options(RavenageGameSpy PRIVATE /MT)


target_link_libraries(RavenageGameSpy PRIVATE CURL::libcurl
									rapidjson)


install(TARGETS RavenageGameSpy DESTINATION bin)
