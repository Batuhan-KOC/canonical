cmake_minimum_required(VERSION 3.5.0)
project(UCII VERSION 0.1.0 LANGUAGES C CXX)

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(CURL REQUIRED) 
find_package(jsoncpp REQUIRED)

include_directories(${Boost_INCLUDE_DIR} ${CURL_INCLUDE_DIR} ${nlohmann_json_INCLUDE_DIRS})

add_executable(UCII main.cpp
    canonicalinterface.h canonicalinterface.cpp
    uciiparser.h uciiparser.cpp)

target_link_libraries(UCII PUBLIC ${Boost_LIBRARIES} ${CURL_LIBRARIES} jsoncpp_lib)

include(CTest)
enable_testing()

