cmake_minimum_required(VERSION 3.25)
project(SetarReference VERSION 0.1.0 LANGUAGES CXX)

option(SETAR_BUILD_TESTS "Build independent numerical reference tests" ON)
option(SETAR_BUILD_RENDERER "Build the offline reference renderer" ON)
option(SETAR_BUILD_NUMERICAL_EXPERIMENTS "Build independent offline solver/coupling experiments" ON)
option(SETAR_BUILD_REPORT_TESTS "Test renderer, WAV/CSV, and Python report integration" ON)
option(SETAR_WARNINGS_AS_ERRORS "Treat compiler warnings as errors for project targets" ON)
option(SETAR_ENABLE_SANITIZERS "Enable address/undefined sanitizers (GCC/Clang only)" OFF)
option(SETAR_ENABLE_JUCE "Build an optional JUCE synthetic-reference diagnostic plugin" OFF)
option(SETAR_BUILD_INSTRUMENT "Build the playable four-string setar prototype engine" ON)
option(SETAR_ENABLE_JUCE_INSTRUMENT "Build the optional JUCE adapter for the setar prototype" OFF)
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
    add_executable(setar_loss_study apps/loss_study.cpp)
    target_link_libraries(setar_loss_study PRIVATE setar::numerical_prototype setar::reference)
    target_include_directories(setar_loss_study PRIVATE tests)
    setar_project_options(setar_loss_study)
endif()

if(SETAR_BUILD_INSTRUMENT)
    # Separate static libraries keep module ownership and link dependencies explicit.
    function(setar_instrument_library name source)
        add_library(${name} STATIC ${source})
        target_include_directories(${name} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>")
        target_compile_features(${name} PUBLIC cxx_std_20)
        set_target_properties(${name} PROPERTIES POSITION_INDEPENDENT_CODE ON)
        setar_project_options(${name})
    endfunction()
    setar_instrument_library(setar_instrument_numerics src/instrument/numerics.cpp)
    setar_instrument_library(setar_instrument_engine src/instrument/instrument.cpp)
    target_sources(setar_instrument_engine PRIVATE src/instrument/config.cpp)
    target_link_libraries(setar_instrument_engine PUBLIC setar_instrument_numerics)
    setar_instrument_library(setar_instrument_body src/instrument/body_model.cpp)
    setar_instrument_library(setar_instrument_preset src/instrument/setar_preset.cpp)
    target_link_libraries(setar_instrument_preset PUBLIC setar_instrument_body setar_instrument_engine)
    setar_instrument_library(setar_instrument_performance src/instrument/performance.cpp)
    add_library(setar_instrument INTERFACE)
    target_link_libraries(setar_instrument INTERFACE setar_instrument_engine setar_instrument_body
                                                     setar_instrument_preset setar_instrument_performance)
    add_library(setar::instrument ALIAS setar_instrument)

    add_executable(setar_play apps/setar_play.cpp)
    target_link_libraries(setar_play PRIVATE setar::instrument)
    setar_project_options(setar_play)
    add_executable(setar_benchmark apps/setar_benchmark.cpp)
    target_link_libraries(setar_benchmark PRIVATE setar::instrument)
    setar_project_options(setar_benchmark)
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
    foreach(case_name IN ITEMS parameters frequencies analytic_response wavenumber_loss lossless_energy
                               passivity pluck_projection convergence reset block_partition
                               resonator endpoints)
        add_test(NAME "core.${case_name}" COMMAND setar_core_tests "${case_name}")
        set_tests_properties("core.${case_name}" PROPERTIES LABELS "core;numerical" TIMEOUT 30)
    endforeach()
    if(SETAR_BUILD_NUMERICAL_EXPERIMENTS)
        add_executable(setar_numerical_tests tests/numerical_prototype_tests.cpp)
        # The analytic modal reference is linked only into the TEST so that the
        # finite-difference library cannot depend on, and cannot be tuned against, its oracle.
        target_link_libraries(setar_numerical_tests PRIVATE setar::numerical_prototype setar::reference)
        target_include_directories(setar_numerical_tests PRIVATE tests)
        setar_project_options(setar_numerical_tests)
        foreach(case_name IN ITEMS string_spatial_convergence force_interpolation_adjoint
                                   stability_envelope discrete_energy_ledger
                                   dispersion_grid_refinement explicit_cfl_rejection
                                   explicit_work_ledger explicit_implicit_comparison
                                   loss_law_passivity loss_decay_estimator loss_decay_oracle
                                   loss_modal_reference_crosscheck invalid_parameters)
            add_test(NAME "numerics.${case_name}" COMMAND setar_numerical_tests "${case_name}")
            set_tests_properties("numerics.${case_name}" PROPERTIES
                LABELS "numerical;prototype" TIMEOUT 60)
        endforeach()
        # Dense O(N^2)-per-step references: about 7 s in Debug and 18 s under ASan/UBSan on
        # Linux. The longer limit leaves headroom for MSVC checked-iterator Debug builds,
        # whose speed has not been measured; it is not permission for unbounded work.
        set_tests_properties(numerics.loss_decay_oracle numerics.loss_modal_reference_crosscheck
            PROPERTIES TIMEOUT 240)
        foreach(case_name IN ITEMS two_mass_oracle port_power_balance)
            add_test(NAME "coupling.${case_name}" COMMAND setar_numerical_tests "${case_name}")
            set_tests_properties("coupling.${case_name}" PROPERTIES
                LABELS "numerical;coupling;prototype" TIMEOUT 60)
        endforeach()
    endif()
    if(SETAR_BUILD_INSTRUMENT)
        # Each runner receives the full CTest name as its single argument.
        # Each runner links only the modules it exercises.
        function(setar_instrument_tests runner source labels)
            cmake_parse_arguments(PARSE_ARGV 3 ARG "" "" "LIBS;TESTS")
            add_executable(${runner} ${source})
            target_link_libraries(${runner} PRIVATE ${ARG_LIBS})
            target_include_directories(${runner} PRIVATE tests)
            setar_project_options(${runner})
            foreach(test_name IN LISTS ARG_TESTS)
                add_test(NAME ${test_name} COMMAND ${runner} ${test_name})
                set_tests_properties(${test_name} PROPERTIES LABELS "instrument;${labels}" TIMEOUT 120)
            endforeach()
        endfunction()
        setar_instrument_tests(setar_instrument_numerics_tests tests/instrument/numerics_tests.cpp
            "numerical;solver"
            LIBS setar_instrument_numerics
            TESTS
            solver.modal_exact_free_response solver.modal_energy_identity
            solver.modal_forced_convergence solver.sine_shape_accuracy
            solver.discrete_gradient_identity solver.contact_law_monotonicity
            solver.newton_scalar_oracle solver.newton_coupled_oracle
            solver.point_spring_eigen_oracle solver.invalid_inputs)
        setar_instrument_tests(setar_instrument_engine_tests tests/instrument/engine_tests.cpp
            "numerical;engine"
            LIBS setar_instrument_engine
            TESTS
            engine.config_validation engine.lossless_energy_ledger engine.lossy_energy_ledger
            engine.open_string_pitch engine.fretted_pitch engine.event_ordering
            engine.newton_failure_reporting
            contact.nail_passivity contact.release_convergence contact.fret_crossing_work
            contact.release_afterlength coupling.cross_string_transfer coupling.uncoupled_limit
            performance.persistent_state performance.restrike_memory
            acoustics.observation_invariance controls.sweep_continuity)
        setar_instrument_tests(setar_instrument_body_tests tests/instrument/body_tests.cpp
            "numerical;body"
            LIBS setar_instrument_body
            TESTS
            materials.reciprocity materials.positive_energy body.plate_frequency_oracle
            body.cavity_coupling_oracle body.mobility_reciprocity body.mobility_passivity
            body.invalid_design)
        setar_instrument_tests(setar_instrument_preset_tests tests/instrument/preset_tests.cpp
            "preset"
            LIBS setar_instrument_preset
            TESTS
            preset.tension_inverse preset.fret_positions preset.config_admissibility
            preset.tuning_cents preset.invalid_inputs)
        setar_instrument_tests(setar_instrument_performance_tests tests/instrument/performance_tests.cpp
            "performance"
            LIBS setar_instrument_performance setar_instrument_preset
            TESTS
            score.parse_valid score.parse_errors score.schedule_ordering score.fret_choice
            score.course_sweep score.midi_mapper)
        setar_instrument_tests(setar_instrument_realtime_tests tests/instrument/realtime_tests.cpp
            "realtime"
            LIBS setar_instrument_preset setar_instrument_performance
            TESTS
            realtime.allocation_hook realtime.block_partition realtime.max_work
            realtime.capacity_overflow contact.gesture_replay)
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
        if(SETAR_BUILD_INSTRUMENT)
            add_test(NAME integration.setar_play
                COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_setar_play.py"
                        --player "$<TARGET_FILE:setar_play>"
                        --analyzer "${CMAKE_CURRENT_SOURCE_DIR}/tools/analyze_render.py"
                        --scores "${CMAKE_CURRENT_SOURCE_DIR}/data/scores")
            set_tests_properties(integration.setar_play PROPERTIES
                LABELS "integration;instrument" TIMEOUT 900)
        endif()
        if(SETAR_BUILD_NUMERICAL_EXPERIMENTS)
            add_test(NAME integration.solver_evidence
                COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_solver_report.py"
                        --solver "$<TARGET_FILE:setar_compare_solvers>"
                        --generator "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_solver_report.py")
            set_tests_properties(integration.solver_evidence PROPERTIES
                LABELS "integration;numerical;visualization" TIMEOUT 60)
            add_test(NAME integration.loss_evidence
                COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/test_loss_report.py"
                        --study "$<TARGET_FILE:setar_loss_study>"
                        --generator "${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_loss_report.py")
            set_tests_properties(integration.loss_evidence PROPERTIES
                LABELS "integration;numerical;visualization" TIMEOUT 300)
        endif()
    endif()
endif()

if(SETAR_ENABLE_JUCE)
    include(cmake/JuceDiagnostic.cmake)
endif()

if(SETAR_ENABLE_JUCE_INSTRUMENT)
    if(NOT SETAR_BUILD_INSTRUMENT)
        message(FATAL_ERROR "SETAR_ENABLE_JUCE_INSTRUMENT requires SETAR_BUILD_INSTRUMENT=ON")
    endif()
    add_subdirectory(plugin)
endif()

