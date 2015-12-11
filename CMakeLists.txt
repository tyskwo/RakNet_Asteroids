cmake_minimum_required(VERSION 2.6)

project(Asteroids)

include_directories(${CMAKE_SOURCE_DIR}/3rdParty/SFML/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/RakNet/source)

link_directories(${CMAKE_SOURCE_DIR}/3rdParty/SFML/lib)
link_directories(${CMAKE_SOURCE_DIR}/3rdParty/RakNet/lib)

add_executable(Client src/client/main.cpp)
add_executable(Server src/server/main.cpp)

set(DEP_LIBS
	sfml-system
	sfml-graphics
	sfml-window
	RakNetLibStatic
	Ws2_32.lib)

target_link_libraries(Client ${DEP_LIBS})
target_link_libraries(Server ${DEP_LIBS})