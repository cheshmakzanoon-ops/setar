cmake_minimum_required(VERSION 3.25)
project(SetarReference VERSION 0.1.0 LANGUAGES CXX)

option(SETAR_BUILD_TESTS "Build independent numerical reference tests" ON)
option(SETAR_BUILD_RENDERER "Build the offline reference renderer" ON)
option(SETAR_BUILD_REPORT_TESTS "Test renderer, WAV/CSV, and Python report integration" ON)
option(SETAR_WARNINGS_AS_ERRORS "Treat compiler warnings as errors for project targets" ON)
option(SETAR_ENABLE_SANITIZERS "Enable address/undefined sanitizers (GCC/Clang only)" OFF)
option(SETAR_ENABLE_JUCE "Build an optional JUCE synthetic-reference diagnostic plugin" OFF)
set(JUCE_SOURCE_DIR "" CACHE PATH "Existing licensed JUCE checkout; never downloaded automatically")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/ProjectOptions.cmake)
add_library(setar_reference src/reference.cpp)
add_library(setar::reference ALIAS setar_reference)
target_include_directories(setar_reference PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
target_compile_features(setar_reference PUBLIC cxx_std_20)
set_target_properties(setar_reference PROPERTIES POSITION_INDEPENDENT_CODE ON)
setar_project_options(setar_reference)

if(SETAR_BUILD_RENDERER)
    add_executable(setar_render apps/render_reference.cpp)
    target_link_libraries(setar_render PRIVATE setar::reference)
    setar_project_options(setar_render)
endif()

if(SETAR_BUILD_TESTS)
    enable_testing()
    add_executable(setar_core_tests tests/reference_tests.cpp)
    target_link_libraries(setar_core_tests PRIVATE setar::reference)
    setar_project_options(setar_core_tests)
    foreach(case_name IN ITEMS parameters frequencies analytic_response lossless_energy passivity
                               pluck_projection convergence reset block_partition resonator endpoints)
        add_test(NAME "core.${case_name}" COMMAND setar_core_tests "${case_name}")
        set_tests_properties("core.${case_name}" PROPERTIES LABELS "core;numerical" TIMEOUT 30)
    endforeach()
    if(SETAR_BUILD_REPORT_TESTS)
        if(NOT SETAR_BUILD_RENDERER)
            message(FATAL_ERROR "SETAR_BUILD_REPORT_TESTS requires SETAR_BUILD_RENDERER=ON")
        endif()
        find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)
        add_test(NAME integration.report
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_report.py"
                    --renderer "$<TARGET_FILE:setar_render>"
                    --report-script "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_report.py")
        set_tests_properties(integration.report PROPERTIES LABELS "integration;report" TIMEOUT 60)
        add_test(NAME documentation.audit_self_tests
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_audit.py")
        set_tests_properties(documentation.audit_self_tests PROPERTIES LABELS "documentation" TIMEOUT 30)
    endif()
endif()

if(SETAR_ENABLE_JUCE)
    include(cmake/JuceDiagnostic.cmake)
endif()

