cmake_minimum_required (VERSION 3.8)

set (INC_DIR ${CMAKE_SOURCE_DIR}/inc)

project ("search_engine_v3")

include(FetchContent)

set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)

FetchContent_MakeAvailable(googletest)


add_executable (search_engine_v3 
"search_engine_v3.cpp"		
${INC_DIR}/Converter.h		"Converter.cpp" 
${INC_DIR}/SearchServer.h	"SearchServer.cpp"
${INC_DIR}/InvertedIndex.h	"InvertedIndex.cpp"
${INC_DIR}/Structures.h)

set(gtest_disable_pthreads on)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_include_directories (search_engine_v3 PUBLIC ${INC_DIR})

target_link_libraries(search_engine_v3 PRIVATE nlohmann_json::nlohmann_json )
#target_link_libraries(search_engine_v3 PRIVATE gtest_main)

#include(GoogleTest)
#gtest_discover_tests(search_engine_v3)

enable_testing()
