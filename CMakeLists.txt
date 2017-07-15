cmake_minimum_required(VERSION 3.6)
project(discordpp-websocket-websocketpp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES discordpp/websocket-websocketpp.hh)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# add_subdirectory(lib/curlpp)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/curlpp/include/)

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# find_package(Boost REQUIRED system)

INCLUDE_DIRECTORIES(${discordpp_INCLUDE_DIRS}) # ${Boost_INCLUDE_DIRS}
TARGET_LINK_LIBRARIES(${PROJECT_NAME} discordpp pthread ssl crypto) # ${Boost_LIBRARIES}
