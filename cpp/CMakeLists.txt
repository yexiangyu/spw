cmake_minimum_required(VERSION 3.10)

project(spw)

IF(WIN32)
	include_directories(
		"C://Program Files//Teledyne DALSA//Sapera//Classes//Basic"
		"C://Program Files//Teledyne DALSA//Sapera//Include"
	)
	link_directories(
		"C://Program Files//Teledyne DALSA//Sapera//Lib//Win64"
	)
	link_libraries(
		"corapi"
		"SapClassBasic"
	)
ENDIF()

add_library(spw SHARED spw.cpp)

add_executable(demo demo.c)
target_link_libraries(demo PRIVATE spw)

add_executable(demo_cpp demo_cpp.cpp)