cmake_minimum_required(VERSION 3.10)
project(bridge)

set(CMAKE_CXX_STANDARD 17)

# Find required includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dcv/include)

# Add library paths
if(WIN32)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/dcv/lib/win)
    set(DYNAMSOFT_LIBS 
        DynamsoftBarcodeReaderx64
        DynamsoftCaptureVisionRouterx64
        DynamsoftCorex64
        DynamsoftLicensex64
        DynamsoftUtilityx64
    )
elseif(APPLE)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/dcv/lib/mac)
    set(DYNAMSOFT_LIBS 
        DynamsoftBarcodeReader
        DynamsoftCaptureVisionRouter
        DynamsoftCore
        DynamsoftLicense
        DynamsoftUtility
    )
else()
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/dcv/lib/linux)
    set(DYNAMSOFT_LIBS 
        DynamsoftBarcodeReader
        DynamsoftCaptureVisionRouter
        DynamsoftCore
        DynamsoftLicense
        DynamsoftUtility
    )
endif()

# Create shared library only
add_library(bridge SHARED src/bridge.cpp)

# Link libraries
target_link_libraries(bridge ${DYNAMSOFT_LIBS})

# Platform-specific output naming and locations
if(WIN32)
    # Use DEF file for proper exports on Windows
    set_target_properties(bridge PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dcv/lib/win
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dcv/lib/win
        LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/bridge.def"
    )
    # Custom command to ensure bridge.dll is in the correct location
    add_custom_command(TARGET bridge POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:bridge>
        ${CMAKE_CURRENT_SOURCE_DIR}/dcv/lib/win/bridge.dll
    )
elseif(APPLE)
    set_target_properties(bridge PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dcv/lib/mac
    )
    # Custom command to ensure libbridge.dylib is in the correct location
    add_custom_command(TARGET bridge POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:bridge>
        ${CMAKE_CURRENT_SOURCE_DIR}/dcv/lib/mac/libbridge.dylib
    )
else()
    set_target_properties(bridge PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dcv/lib/linux
    )
    # Custom command to ensure libbridge.so is in the correct location
    add_custom_command(TARGET bridge POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:bridge>
        ${CMAKE_CURRENT_SOURCE_DIR}/dcv/lib/linux/libbridge.so
    )
endif()
