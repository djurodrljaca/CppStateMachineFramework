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

# --------------------------------------------------------------------------------------------------
# Custom (meta) targets
# --------------------------------------------------------------------------------------------------
add_custom_target(all_unit_tests)

# --------------------------------------------------------------------------------------------------
# Helper methods
# --------------------------------------------------------------------------------------------------
function(CppStateMachineFramework_AddUnitTest)
    # Function parameters
    set(options)                # Boolean parameters
    set(oneValueParams          # Parameters with one value
            TEST_NAME
        )
    set(multiValueParams)       # Parameters with multiple values

    cmake_parse_arguments(PARAM "${options}" "${oneValueParams}" "${multiValueParams}" ${ARGN})

    # Add test
    CppStateMachineFramework_AddTest(${ARGN} LABELS CPPSTATEMACHINEFRAMEWORK_UNIT_TESTS)

    # Add test to target "all_unit_tests"
    add_dependencies(all_unit_tests ${PARAM_TEST_NAME})
endfunction()

# --------------------------------------------------------------------------------------------------
# Unit tests
# --------------------------------------------------------------------------------------------------
add_subdirectory(Event)
add_subdirectory(EventWithData)
add_subdirectory(StateMachine)

# --------------------------------------------------------------------------------------------------
# Code Coverage
# --------------------------------------------------------------------------------------------------
if (CppStateMachineFramework_CodeCoverage MATCHES ON)
    # Prepare targets for code coverage
    SETUP_TARGET_FOR_COVERAGE(
            NAME         ctest_coverage_all_unit_tests
            EXECUTABLE   ctest -L CPPSTATEMACHINEFRAMEWORK_UNIT_TESTS
            DEPENDENCIES all_unit_tests
            PROJECT_PATH ${PROJECT_SOURCE_DIR}
            EXCLUDES     ${CppStateMachineFramework_CodeCoverage_EXCLUDES}
        )
endif()
