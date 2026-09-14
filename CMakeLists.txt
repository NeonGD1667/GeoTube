
cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(GeoTube VERSION 1.0.0)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})


# =========================
# WebView2 - Windows only
# =========================

if (WIN32)

    set(WEBVIEW2_VERSION "1.0.4191.47")
    set(WEBVIEW2_PACKAGE
        "${CMAKE_BINARY_DIR}/Microsoft.Web.WebView2.${WEBVIEW2_VERSION}.nupkg"
    )
    set(WEBVIEW2_DIR
        "${CMAKE_BINARY_DIR}/webview2"
    )

    if (NOT EXISTS "${WEBVIEW2_DIR}/build/native/include/WebView2.h")

        message(STATUS
            "Downloading Microsoft.Web.WebView2 ${WEBVIEW2_VERSION}..."
        )

        file(DOWNLOAD
            "https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/${WEBVIEW2_VERSION}"
            "${WEBVIEW2_PACKAGE}"
            SHOW_PROGRESS
            STATUS WEBVIEW2_DOWNLOAD_STATUS
        )

        list(GET WEBVIEW2_DOWNLOAD_STATUS 0 WEBVIEW2_DOWNLOAD_RESULT)

        if (NOT WEBVIEW2_DOWNLOAD_RESULT EQUAL 0)
            message(FATAL_ERROR
                "Failed to download Microsoft.Web.WebView2"
            )
        endif()

        file(MAKE_DIRECTORY "${WEBVIEW2_DIR}")

        execute_process(
            COMMAND
                "${CMAKE_COMMAND}"
                -E tar
                xf
                "${WEBVIEW2_PACKAGE}"
            WORKING_DIRECTORY
                "${WEBVIEW2_DIR}"
            RESULT_VARIABLE WEBVIEW2_EXTRACT_RESULT
        )

        if (NOT WEBVIEW2_EXTRACT_RESULT EQUAL 0)
            message(FATAL_ERROR
                "Failed to extract Microsoft.Web.WebView2"
            )
        endif()

    endif()


    find_path(WEBVIEW2_INCLUDE_DIR
        NAMES WebView2.h
        PATHS
            "${WEBVIEW2_DIR}/build/native/include"
        NO_DEFAULT_PATH
    )

    if (NOT WEBVIEW2_INCLUDE_DIR)
        message(FATAL_ERROR
            "WebView2.h was not found!"
        )
    endif()


    find_library(WEBVIEW2_LIBRARY
        NAMES WebView2LoaderStatic140
        PATHS
            "${WEBVIEW2_DIR}/build/native"
        PATH_SUFFIXES
            x64
        NO_DEFAULT_PATH
    )


    if (NOT WEBVIEW2_LIBRARY)
        message(FATAL_ERROR
            "WebView2Loader library was not found!"
        )
    endif()


    message(STATUS
        "Found WebView2 headers: ${WEBVIEW2_INCLUDE_DIR}"
    )

    message(STATUS
        "Found WebView2 library: ${WEBVIEW2_LIBRARY}"
    )


    target_include_directories(${PROJECT_NAME}
        PRIVATE
            "${WEBVIEW2_INCLUDE_DIR}"
    )

    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            "${WEBVIEW2_LIBRARY}"
            ole32
            oleaut32
            shlwapi
    )

endif()

