cmake_minimum_required(VERSION 3.31)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AppInfo.cmake)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/resources/8mbapp.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/8mbapp.rc @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/resources/setup.iss.in ${CMAKE_CURRENT_SOURCE_DIR}/setup/setup.iss @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/resources/Globals.hxx.in ${CMAKE_CURRENT_SOURCE_DIR}/src/Globals.hxx @ONLY)

project(8mbapp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.1/msvc2022_64/lib/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR}/bin/RelWithDebInfo)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR}/bin/MinSizeRel)

set(RESOURCE_FILES "resources/icons.qrc" "resources/images.qrc")

set(APP_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/8mbapp.rc")

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets
        Network
        StateMachine)

qt_standard_project_setup()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.cxx src/*.h src/*.hxx)

add_executable(8mbapp ${SOURCES} ${APP_RESOURCES} ${RESOURCE_FILES})

target_include_directories(8mbapp PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(8mbapp
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Network
        Qt::StateMachine
)

if (CMAKE_BUILD_TYPE MATCHES "MinSizeRel|Release")
    set_target_properties(8mbapp PROPERTIES
            WIN32_EXECUTABLE TRUE
    )
endif ()
