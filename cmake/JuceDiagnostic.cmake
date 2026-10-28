if(NOT JUCE_SOURCE_DIR OR NOT EXISTS "${JUCE_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "SETAR_ENABLE_JUCE needs -DJUCE_SOURCE_DIR=/absolute/path/to/an/existing/JUCE/checkout. "
        "No dependency is downloaded. Review JUCE's terms before building or distributing.")
endif()

# JUCE is an explicit optional boundary dependency; the headless core never links it.
add_subdirectory("${JUCE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/juce" EXCLUDE_FROM_ALL)
juce_add_plugin(setar_diagnostic
    VERSION "${PROJECT_VERSION}"
    COMPANY_NAME "Setar Research"
    BUNDLE_ID "org.setar.reference.diagnostic"
    PLUGIN_MANUFACTURER_CODE StRf
    PLUGIN_CODE StDg
    PRODUCT_NAME "Setar Reference Diagnostic"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    FORMATS Standalone VST3
    VST3_CATEGORIES Instrument Synth)
juce_generate_juce_header(setar_diagnostic)
target_sources(setar_diagnostic PRIVATE apps/juce_diagnostic.cpp)
target_compile_definitions(setar_diagnostic PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(setar_diagnostic
    PRIVATE setar::reference juce::juce_audio_utils
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)

