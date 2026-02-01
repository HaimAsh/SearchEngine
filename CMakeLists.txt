CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

PROJECT(SearchEngine)

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ABSL_PROPAGATE_CXX_STD ON) # Ensures Abseil uses your C++ standard
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# Using 'SYSTEM' prevents your compiler from showing warnings from Abseil code
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/abseil-cpp SYSTEM)

# GTest Setup
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE) # GMock is very useful for callbacks

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/googletest SYSTEM)

find_package(LibXml2 REQUIRED)

file(GLOB_RECURSE SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

list(FILTER SRC_FILES EXCLUDE REGEX "/cmake[^/]*/")
list(FILTER SRC_FILES EXCLUDE REGEX "3rd_party/")
list(FILTER SRC_FILES EXCLUDE REGEX "test/")

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${LIBXML2_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        absl::strings
        absl::flat_hash_map
        absl::log
        ${LIBXML2_LIBRARIES}
)

# 2. The Test Executable
# Create a separate list for test files so they don't mix with main()
file(GLOB_RECURSE TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp)

if(TEST_FILES)
    # Important: Create a copy of SRC_FILES without main.cpp
    set(TEST_LOGIC_FILES ${SRC_FILES})
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fno-pie -no-pie")
    list(FILTER TEST_LOGIC_FILES EXCLUDE REGEX "main.cpp")

    message(STATUS "Test files being built: ${TEST_FILES}")
    add_executable(run_tests ${TEST_FILES} ${TEST_LOGIC_FILES})

    target_compile_options(run_tests PRIVATE -fsanitize=thread)

    target_include_directories(run_tests PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${LIBXML2_INCLUDE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/googletest/googletest/include
            ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/googletest/googlemock/include)

    target_link_libraries(run_tests
            PRIVATE
            gtest_main
            gmock
            absl::strings
            absl::flat_hash_map
            ${LIBXML2_LIBRARIES}
            -fsanitize=thread
    )

    message(STATUS "Registered test target 'run_tests' with files: ${TEST_FILES}")
endif()