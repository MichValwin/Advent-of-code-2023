cmake_minimum_required(VERSION 3.12)
project(AdventOfCode)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the build type to Debug by default
set(CMAKE_BUILD_TYPE Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add, move and name every executable for each day
function(add_day_solution DAY_NAME)
    add_executable(${DAY_NAME} ${DAY_NAME}/main.cpp)

    set_target_properties(${DAY_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${DAY_NAME}
    )

    set_target_properties(${DAY_NAME} PROPERTIES
        OUTPUT_NAME advent_exec
    )
endfunction()

# Add an executable for each day
#add_day_solution(day1)
#add_day_solution(day2)
#add_day_solution(exampleParsingRegexDay2)
#add_day_solution(day3)
#add_day_solution(day4)
add_day_solution(day5)


# Set up the build types
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS "-Wall -pedantic -Wextra -std=c++17")

# Use address sanitizer on linux
# TODO