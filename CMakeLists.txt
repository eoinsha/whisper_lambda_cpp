cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_STANDARD 17)
project(whisper_lambda_cpp LANGUAGES CXX)

find_package(aws-lambda-runtime)
set(AWS_SDK_BUILD_SHARED_LIBS OFF)
find_package(AWSSDK COMPONENTS s3)
find_library(WHISPER_LIBRARY NAMES whisper PATHS /usr/local/lib/static)
find_package(ZLIB REQUIRED)

add_executable(${PROJECT_NAME} "bootstrap.cpp" "s3_audio_processing.cpp" "transcription.cpp" "common.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE AWS::aws-lambda-runtime ${WHISPER_LIBRARY} pthread ${AWSSDK_LINK_LIBRARIES} ZLIB::ZLIB)
target_include_directories(${PROJECT_NAME} PRIVATE /opt/whisper/include)
target_compile_features(${PROJECT_NAME} PRIVATE "cxx_std_11")
target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra" "-Wconversion" "-Wshadow" "-Wno-sign-conversion")

add_executable(${PROJECT_NAME}_test "test_main.cpp" "s3_audio_processing.cpp" "transcription.cpp" "common.cpp")
target_link_libraries(${PROJECT_NAME}_test PRIVATE AWS::aws-lambda-runtime ${WHISPER_LIBRARY} pthread ${AWSSDK_LINK_LIBRARIES} ZLIB::ZLIB)
target_include_directories(${PROJECT_NAME}_test PRIVATE /opt/whisper/include)
target_compile_features(${PROJECT_NAME}_test PRIVATE "cxx_std_11")
target_compile_options(${PROJECT_NAME}_test PRIVATE "-Wall" "-Wextra")

aws_lambda_package_target(${PROJECT_NAME})