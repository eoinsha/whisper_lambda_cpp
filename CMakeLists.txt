cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_STANDARD 11)
project(whisper_lambda_cpp LANGUAGES CXX)

find_package(aws-lambda-runtime)
find_library(WHISPER_LIBRARY NAMES whisper PATHS /usr/local/lib/static)
add_executable(${PROJECT_NAME} "bootstrap.cpp" "common.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE AWS::aws-lambda-runtime ${WHISPER_LIBRARY} pthread)
target_compile_features(${PROJECT_NAME} PRIVATE "cxx_std_11")
target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra")

add_executable(${PROJECT_NAME}_test "test_main.cpp" "common.cpp")
target_link_libraries(${PROJECT_NAME}_test PRIVATE AWS::aws-lambda-runtime ${WHISPER_LIBRARY} pthread)
target_compile_features(${PROJECT_NAME}_test PRIVATE "cxx_std_11")
target_compile_options(${PROJECT_NAME}_test PRIVATE "-Wall" "-Wextra")

aws_lambda_package_target(${PROJECT_NAME})