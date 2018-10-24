cmake_minimum_required(VERSION 3.9)

project(hashtable
	LANGUAGES CXX
	VERSION 0.1
)

add_subdirectory(third_party/linenoise-ng)

add_executable(hashmap
    AST.cpp
    Hashtable.cpp
    main.cpp
    RefArray.cpp
)

set_property(TARGET hashmap PROPERTY CXX_STANDARD 17)

target_link_libraries(hashmap PRIVATE linenoise)
target_include_directories(hashmap PRIVATE third_party/linenoise-ng/include)
