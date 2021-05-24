# This file is part of C++ State Machine Framework.
#
# C++ State Machine Framework is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# C++ State Machine Framework is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
# PURPOSE. See the GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License along with C++ State
# Machine Framework. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.5.0)

project(CppStateMachineFramework VERSION 1.0.0)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)

# --------------------------------------------------------------------------------------------------
# Code Coverage
# --------------------------------------------------------------------------------------------------
option(CppStateMachineFramework_CodeCoverage "C++ State Machine Framework Code Coverage" OFF)

if (CppStateMachineFramework_CodeCoverage MATCHES ON)
    message("C++ State Machine Framework: Code coverage enabled")

    include(cmake/CodeCoverage.cmake)
    APPEND_COVERAGE_COMPILER_FLAGS()

    set(excludes_cmake_prefix_path)
    foreach(item ${CMAKE_PREFIX_PATH})
        get_filename_component(item ${item} ABSOLUTE)
        list(APPEND excludes_cmake_prefix_path "'${item}/*'")
    endforeach()

    set(CppStateMachineFramework_CodeCoverage_EXCLUDES
            "'/opt/*'"
            "'/usr/*'"
            ${excludes_cmake_prefix_path}
            "'${CMAKE_SOURCE_DIR}/tests/*'"
            "'${CMAKE_BINARY_DIR}/*'"
        )
endif()

# --------------------------------------------------------------------------------------------------
# CppStateMachineFramework library
# --------------------------------------------------------------------------------------------------
add_library(CppStateMachineFramework SHARED
        inc/CppStateMachineFramework/Event.hpp
        inc/CppStateMachineFramework/HashFunctions.hpp
        inc/CppStateMachineFramework/StateMachine.hpp

        src/Event.cpp
        src/StateMachine.cpp
    )

set_target_properties(CppStateMachineFramework PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

include(GenerateExportHeader)
generate_export_header(CppStateMachineFramework
    EXPORT_FILE_NAME exports/CppStateMachineFramework/CppStateMachineFrameworkExport.hpp)

target_include_directories(CppStateMachineFramework PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/inc>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/exports>
        $<INSTALL_INTERFACE:include>
    )

target_link_libraries(CppStateMachineFramework PUBLIC
        Qt5::Core
    )

set_target_properties(CppStateMachineFramework PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )

# --------------------------------------------------------------------------------------------------
# Package
# --------------------------------------------------------------------------------------------------
add_library(CppStateMachineFramework::CppStateMachineFramework ALIAS CppStateMachineFramework)

install(TARGETS CppStateMachineFramework EXPORT CppStateMachineFrameworkTargets DESTINATION
        LIBRARY  DESTINATION lib
        ARCHIVE  DESTINATION lib
        RUNTIME  DESTINATION bin
        INCLUDES DESTINATION include
    )

install(
        DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}/inc/CppStateMachineFramework
        DESTINATION include
        COMPONENT   Devel
    )

install(
        DIRECTORY   ${PROJECT_BINARY_DIR}/exports/CppStateMachineFramework
        DESTINATION include
        COMPONENT   Devel
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/CppStateMachineFramework/CppStateMachineFrameworkConfigVersion.cmake"
        VERSION ${CppStateMachineFramework_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

export(EXPORT CppStateMachineFrameworkTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/CppStateMachineFramework/CppStateMachineFrameworkTargets.cmake"
        NAMESPACE CppStateMachineFramework::
    )

configure_file(cmake/CppStateMachineFrameworkConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/CppStateMachineFramework/CppStateMachineFrameworkConfig.cmake"
        COPYONLY
    )

set(ConfigPackageLocation lib/cmake/CppStateMachineFramework)

install(
        EXPORT      CppStateMachineFrameworkTargets
        FILE        CppStateMachineFrameworkTargets.cmake
        NAMESPACE   CppStateMachineFramework::
        DESTINATION ${ConfigPackageLocation}
    )

install(
        FILES
            cmake/CppStateMachineFrameworkConfig.cmake
            "${CMAKE_CURRENT_BINARY_DIR}/CppStateMachineFramework/CppStateMachineFrameworkConfigVersion.cmake"
        DESTINATION ${ConfigPackageLocation}
        COMPONENT   Devel
    )

# --------------------------------------------------------------------------------------------------
# Tests
# --------------------------------------------------------------------------------------------------
enable_testing()
add_subdirectory(tests)
