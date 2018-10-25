cmake_minimum_required(VERSION 3.9)

project(hashtable
	LANGUAGES CXX
	VERSION 0.1
)

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

target_link_libraries(hashmap PRIVATE linenoise)
