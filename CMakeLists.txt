cmake_minimum_required(VERSION 3.14.0) # CMake install : https://cmake.org/download/
project(PawalWidget VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_PREFIX_PATH "d:/Qt/6.2.4/msvc2019_64")

include(FetchContent)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
aux_source_directory(./src srcs)

fetchcontent_declare(PawalNativeApi GIT_REPOSITORY "https://github.com/NaturalSelect/PawalNativeApi" GIT_TAG "master" SOURCE_DIR "${PROJECT_SOURCE_DIR}/lib/PawalNativeApi")
fetchcontent_makeavailable(PawalNativeApi)

# Specify MSVC UTF-8 encoding   
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${srcs})
else()
    if(ANDROID)
        add_library(${PROJECT_NAME} SHARED ${srcs})
    else()
        add_executable(${PROJECT_NAME} ${srcs})
    endif()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

target_link_libraries(${PROJECT_NAME} PRIVATE PawalApi)

set(PAWAL_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/lib/PawalNativeApi/include")

include_directories(${PAWAL_INCLUDE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
