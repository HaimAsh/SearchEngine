CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

PROJECT(SearchEngine)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ABSL_PROPAGATE_CXX_STD ON) # Ensures Abseil uses your C++ standard
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Using 'SYSTEM' prevents your compiler from showing warnings from Abseil code
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/abseil-cpp SYSTEM)

file(GLOB_RECURSE SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

list(FILTER SRC_FILES EXCLUDE REGEX "/cmake[^/]*/")
list(FILTER SRC_FILES EXCLUDE REGEX "3rd_party/")

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        absl::strings
        absl::flat_hash_map
        absl::log
)