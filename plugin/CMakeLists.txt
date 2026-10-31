# Optional JUCE adapter for the playable setar prototype (SETAR_ENABLE_JUCE_INSTRUMENT=ON).
# JUCE is an explicit, externally supplied boundary dependency: it is never downloaded
# here and the headless engine never links it. Review JUCE's licence terms before
# building or distributing anything produced by this target.
if(NOT JUCE_SOURCE_DIR OR NOT EXISTS "${JUCE_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "SETAR_ENABLE_JUCE_INSTRUMENT needs -DJUCE_SOURCE_DIR=/absolute/path/to/an/existing/JUCE/checkout. "
        "No dependency is downloaded.")
endif()

# The headless project is C++-only; JUCE's CMake also compiles C sources (and its
# juceaide helper), so C is enabled here, the highest directory using it.
enable_language(C)
# SETAR_ENABLE_JUCE may already have added the same JUCE checkout.
if(NOT COMMAND juce_add_plugin)
    add_subdirectory("${JUCE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/juce" EXCLUDE_FROM_ALL)
endif()

juce_add_plugin(setar_prototype_plugin
    VERSION "${PROJECT_VERSION}"
    COMPANY_NAME "Setar Research"
    BUNDLE_ID "org.setar.prototype.synthetic"
    PLUGIN_MANUFACTURER_CODE StRf
    PLUGIN_CODE StPr
    PRODUCT_NAME "Setar Prototype Synthetic"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    FORMATS VST3 Standalone
    VST3_CATEGORIES Instrument Synth)
target_sources(setar_prototype_plugin PRIVATE
    Source/SetarProcessor.cpp
    Source/SetarEditor.cpp)
target_compile_definitions(setar_prototype_plugin PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(setar_prototype_plugin
    PRIVATE setar::instrument juce::juce_audio_utils
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)
