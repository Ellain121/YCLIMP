cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-13")
project(music_player)

add_compile_definitions(DEBUG)

add_subdirectory(SQLiteCpp)

# boost.asio doesnt work with ncurses without this macro
add_compile_definitions(NCURSES_NOMACROS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_compile_options(-Wall -Wextra)
#add_compile_options(-g)
add_compile_options("-lpthread")

set(PROJECT_LINK_LIBS libncursesw.so.6)

# link_directories(/lib/x86_64-gnu)
link_directories(/usr/local/lib)


include_directories(include)
include_directories(xml_lib)
include_directories(libs)

link_directories(libs)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.hpp" "include/*.tpp")

add_executable(music_player ${SOURCES} ${HEADERS})
target_link_libraries(music_player ${PROJECT_LINK_LIBS} SQLiteCpp bass)
#target_link_libraries(music_player PRIVATE SQLiteCpp)

target_compile_definitions(music_player PRIVATE PROJECT_DIR_CMAKE="${CMAKE_CURRENT_SOURCE_DIR}")
