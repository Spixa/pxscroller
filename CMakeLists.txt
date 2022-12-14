cmake_minimum_required(VERSION 3.21)

project(PixelScroller VERSION 0.1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
 set(CMAKE_BUILD_TYPE "Release")
endif()

# BUILD EXTERNAL LIBRARIES
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
ExternalProject_Add(SFML 
		#GIT_REPOSITORY https://github.com/SFML/SFML.git GIT_TAG 2.5.1 
		SOURCE_DIR  ${CMAKE_SOURCE_DIR}/ext/SFML 
    		CMAKE_ARGS 
			-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
			-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
ExternalProject_Get_Property(SFML INSTALL_DIR)
set(SFML_LIB ${INSTALL_DIR})

include_directories(${SFML_LIB}/include ${CMAKE_SOURCE_DIR}/src)
link_directories(${SFML_LIB}/lib)

set(PX_SRCS
	${CMAKE_SOURCE_DIR}/src/anim_comp.cpp
	${CMAKE_SOURCE_DIR}/src/entity.cpp
	${CMAKE_SOURCE_DIR}/src/game_engine.cpp
	${CMAKE_SOURCE_DIR}/src/main.cpp
)

add_executable(pxs ${PX_SRCS})
target_link_libraries(pxs
  optimized sfml-system		debug sfml-system-d 
  optimized sfml-window		debug sfml-window-d 
  optimized sfml-graphics		debug sfml-graphics-d 
)
