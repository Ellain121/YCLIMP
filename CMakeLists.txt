cmake_minimum_required(VERSION 3.30)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
project(music_player)

add_compile_definitions(DEBUG)

add_subdirectory(SQLiteCpp)


###
set (BOOST_LIBS Boost::asio Boost::system)

if(DOWNLOAD_LOCAL_BOOST)
    message(STATUS "Boost will be downloaded in build directory")
    # set(BOOST_INCLUDE_LIBRARIES asio filesystem system)

    if (NOT CMAKE_POLICY_VERSION_MINIMUM EQUAL "3.5")
        message("Please set flags as in instruction.")
        message(FATAL_ERROR "Exiting: CMAKE_POLICY_VERSION_MINIMUM is not '3.5'")
    endif()

    include(FetchContent)
    FetchContent_Declare(
        Boost
        GIT_REPOSITORY https://github.com/boostorg/boost.git
        GIT_TAG boost-1.80.0
    )
    FetchContent_MakeAvailable(Boost)

else()
    find_package(Boost COMPONENTS filesystem system)
    if(NOT Boost_FOUND)
        message(FATAL_ERROR "Required Boost components (filesystem, system) not found!")
    endif()
endif()

# boost.asio doesnt work with ncurses without this macro
add_compile_definitions(NCURSES_NOMACROS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_compile_options(-Wall -Wextra)
#add_compile_options(-g)
add_compile_options("-lpthread")

set(PROJECT_LINK_LIBS libncursesw.so.6)

# link_directories(/lib/x86_64-gnu)
# link_directories(/usr/local/lib)


include_directories(include)
include_directories(xml_lib)
include_directories(libs)

link_directories(libs)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.hpp" "include/*.tpp")

add_executable(music_player ${SOURCES} ${HEADERS})
target_link_libraries(music_player ${PROJECT_LINK_LIBS} SQLiteCpp bass ${BOOST_LIBS})

target_compile_definitions(music_player PRIVATE PROJECT_DIR_CMAKE="${CMAKE_CURRENT_SOURCE_DIR}")
