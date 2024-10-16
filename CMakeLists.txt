cmake_minimum_required(VERSION 3.5.0)
project(UCII VERSION 0.1.0 LANGUAGES C CXX)

find_package( Boost 1.40 COMPONENTS program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

add_executable(UCII main.cpp
    canonicalinterface.h canonicalinterface.cpp
    uciiparser.h uciiparser.cpp)

target_link_libraries( UCII PUBLIC ${Boost_LIBRARIES} )

include(CTest)
enable_testing()

