cmake_minimum_required(VERSION 3.6)
project(discordpp-websocket-websocketpp)

set(CMAKE_CXX_STANDARD 14)

###################



add_library(${PROJECT_NAME} INTERFACE)
# target_sources(${PROJECT_NAME} INTERFACE "${PROJECT_SOURCE_DIR}/discordpp")

INCLUDE_DIRECTORIES(${discordpp_SOURCE_DIR} ${json_SOURCE_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} INTERFACE  pthread ssl crypto) #discordpp json

#[[

set(SOURCE_FILES discordpp/websocket-websocketpp.hh)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# add_subdirectory(lib/curlpp)
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/curlpp/include/)

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# find_package(Boost REQUIRED system)

INCLUDE_DIRECTORIES(${discordpp_INCLUDE_DIRS} ${nlohmann_json_include_dir}) # ${Boost_INCLUDE_DIRS}
TARGET_LINK_LIBRARIES(${PROJECT_NAME} discordpp pthread ssl crypto) # ${Boost_LIBRARIES}
]]