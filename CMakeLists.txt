cmake_minimum_required(VERSION 3.9)

project(hashtable
	LANGUAGES CXX C
	VERSION 0.1
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fdiagnostics-color=always")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always")
add_definitions(-DUT_DIRECT_TRACE_REGISTRATION )
include(omr_config.cmake)
include_directories(.)
include_directories(include)
add_subdirectory(glue)
add_subdirectory(third_party/omr)
#add_subdirectory(third_party/linenoise-ng)

add_library(linenoise STATIC
	third_party/linenoise-ng/src/ConvertUTF.cpp
	third_party/linenoise-ng/src/linenoise.cpp
	third_party/linenoise-ng/src/wcwidth.cpp
)
target_include_directories(linenoise PUBLIC third_party/linenoise-ng/include)

if(MSVC)
	target_compile_definitions(linenoise PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

add_executable(hashmap
    AST.cpp
    Hashtable.cpp
    main.cpp
    RefArray.cpp
)

if(MSVC_IDE)
	target_sources(hashmap
		PRIVATE
		AST.hpp
		Environ.hpp
		Hash.hpp
		HashTable.hpp
		Object.hpp
		Value.hpp
	)
endif()
set_target_properties(
	hashmap
	PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED TRUE)

target_link_libraries(hashmap
	PRIVATE
	linenoise
	omrgc
)

target_include_directories(hashmap
	PRIVATE
	${omr_BINARY_DIR}
	${omr_SOURCE_DIR}/include_core
    ${omr_BINARY_DIR}/include_core
)
