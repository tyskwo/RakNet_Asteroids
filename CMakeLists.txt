cmake_minimum_required(VERSION 2.6)

project(Asteroids)

include_directories(${CMAKE_SOURCE_DIR}/3rdParty/SFML/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/RakNet/source)
include_directories(${CMAKE_SOURCE_DIR}/3rdParty/Box2D/source)

link_directories(${CMAKE_SOURCE_DIR}/3rdParty/SFML/lib)
link_directories(${CMAKE_SOURCE_DIR}/3rdParty/RakNet/lib)
link_directories(${CMAKE_SOURCE_DIR}/3rdParty/Box2D/lib)

add_executable(Client src/client/main.cpp 
src/common/NetworkStructs.h src/common/PhysicsStructs.h src/common/PhysicsStructs.cpp
src/client/Client.h src/client/Client.cpp
src/common/Object.h src/common/Object.cpp
src/common/Asteroid.h src/common/Asteroid.cpp
src/common/Bullet.h src/common/Bullet.cpp
src/common/Ship.h src/common/Ship.cpp
src/common/Game.h src/common/Game.cpp
src/common/Timer.h src/common/Timer.cpp
src/client/ObjectInfo.h src/client/ObjectInfo.cpp
)

add_executable(Server src/server/main.cpp 
src/common/NetworkStructs.h src/common/PhysicsStructs.h src/common/PhysicsStructs.cpp
src/server/Server.h src/server/Server.cpp
src/common/Object.h src/common/Object.cpp
src/common/Asteroid.h src/common/Asteroid.cpp
src/common/Bullet.h src/common/Bullet.cpp
src/common/Ship.h src/common/Ship.cpp
src/common/Game.h src/common/Game.cpp
src/common/Timer.h src/common/Timer.cpp
)

add_custom_command(TARGET Client PRE_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy_directory
 ${CMAKE_SOURCE_DIR}/3rdParty/bin $<TARGET_FILE_DIR:Client>)

add_custom_command(TARGET Client PRE_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy_directory
 ${CMAKE_SOURCE_DIR}/src/assets $<TARGET_FILE_DIR:Client>)

add_custom_command(TARGET Server PRE_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy_directory
 ${CMAKE_SOURCE_DIR}/3rdParty/bin $<TARGET_FILE_DIR:Server>)

add_custom_command(TARGET Server PRE_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy_directory
 ${CMAKE_SOURCE_DIR}/src/assets $<TARGET_FILE_DIR:Server>)

set(DEP_LIBS
	sfml-system
	sfml-graphics
	sfml-window
	RakNetLibStatic
	Box2D
	Ws2_32.lib)

target_link_libraries(Client ${DEP_LIBS})
target_link_libraries(Server ${DEP_LIBS})