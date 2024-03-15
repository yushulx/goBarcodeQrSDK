cmake_minimum_required(VERSION 3.0.0)

project(bridge VERSION 0.1.0)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(TARGET_LIB_DIR "${PROJECT_SOURCE_DIR}/../linux/")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(TARGET_LIB_DIR "${PROJECT_SOURCE_DIR}/../windows/")
endif()

link_directories(${TARGET_LIB_DIR})
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/../")
add_library(bridge SHARED bridge.cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries (${PROJECT_NAME} "DynamsoftLicense" "DynamsoftBarcodeReader" "DynamsoftCaptureVisionRouter" "DynamsoftCore")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(CMAKE_CL_64)
        target_link_libraries (${PROJECT_NAME} "DynamsoftLicensex64" "DynamsoftBarcodeReaderx64" "DynamsoftCaptureVisionRouterx64" "DynamsoftCorex64")
    endif()
endif()

# Copy the built library to the target directory
add_custom_command(TARGET bridge POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:bridge>"
    "${TARGET_LIB_DIR}")
