cmake_minimum_required(VERSION 3.25)
project(SetarReference VERSION 0.1.0 LANGUAGES CXX)

option(SETAR_BUILD_TESTS "Build independent numerical reference tests" ON)
option(SETAR_BUILD_RENDERER "Build the offline reference renderer" ON)
option(SETAR_BUILD_NUMERICAL_EXPERIMENTS "Build independent offline solver/coupling experiments" ON)
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

if(SETAR_BUILD_NUMERICAL_EXPERIMENTS)
    add_library(setar_numerical_prototype src/numerical_prototype.cpp)
    add_library(setar::numerical_prototype ALIAS setar_numerical_prototype)
    target_include_directories(setar_numerical_prototype PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
    target_compile_features(setar_numerical_prototype PUBLIC cxx_std_20)
    setar_project_options(setar_numerical_prototype)
    add_executable(setar_compare_solvers apps/compare_solvers.cpp)
    target_link_libraries(setar_compare_solvers PRIVATE setar::numerical_prototype)
    setar_project_options(setar_compare_solvers)
endif()

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
    if(SETAR_BUILD_NUMERICAL_EXPERIMENTS)
        add_executable(setar_numerical_tests tests/numerical_prototype_tests.cpp)
        target_link_libraries(setar_numerical_tests PRIVATE setar::numerical_prototype)
        setar_project_options(setar_numerical_tests)
        foreach(case_name IN ITEMS string_spatial_convergence force_interpolation_adjoint
                                   stability_envelope discrete_energy_ledger
                                   dispersion_grid_refinement explicit_cfl_rejection
                                   explicit_work_ledger explicit_implicit_comparison invalid_parameters)
            add_test(NAME "numerics.${case_name}" COMMAND setar_numerical_tests "${case_name}")
            set_tests_properties("numerics.${case_name}" PROPERTIES
                LABELS "numerical;prototype" TIMEOUT 60)
        endforeach()
        foreach(case_name IN ITEMS two_mass_oracle port_power_balance)
            add_test(NAME "coupling.${case_name}" COMMAND setar_numerical_tests "${case_name}")
            set_tests_properties("coupling.${case_name}" PROPERTIES
                LABELS "numerical;coupling;prototype" TIMEOUT 60)
        endforeach()
    endif()
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
        add_test(NAME documentation.implementation_traceability
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_implementation.py")
        set_tests_properties(documentation.implementation_traceability PROPERTIES
            LABELS "documentation;traceability" TIMEOUT 30)
        add_test(NAME documentation.implementation_mutations
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_implementation_audit.py")
        set_tests_properties(documentation.implementation_mutations PROPERTIES
            LABELS "documentation;traceability" TIMEOUT 30)
        add_test(NAME data.acquisition_manifest_integrity
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_evidence_gates.py")
        set_tests_properties(data.acquisition_manifest_integrity PROPERTIES
            LABELS "data;qualification;provenance" TIMEOUT 30)
        add_test(NAME documentation.executable_coverage_integrity
            COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/audit_executable_coverage.py")
        set_tests_properties(documentation.executable_coverage_integrity PROPERTIES
            LABELS "documentation;traceability" TIMEOUT 30)
        if(SETAR_BUILD_NUMERICAL_EXPERIMENTS)
            add_test(NAME integration.solver_evidence
                COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_solver_report.py"
                        --solver "$<TARGET_FILE:setar_compare_solvers>"
                        --generator "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_solver_report.py")
            set_tests_properties(integration.solver_evidence PROPERTIES
                LABELS "integration;numerical;visualization" TIMEOUT 60)
        endif()
    endif()
endif()

if(SETAR_ENABLE_JUCE)
    include(cmake/JuceDiagnostic.cmake)
endif()

