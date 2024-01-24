cmake_minimum_required (VERSION 3.21)

set (TARGET_NAME straw_demo)
project (${TARGET_NAME} VERSION 0.0.1)

include (FetchContent)
set (FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

# Set browsable modules in IDE
set_property (GLOBAL PROPERTY USE_FOLDERS YES)
option (JUCE_ENABLE_MODULE_SOURCE_GROUPS "Enable Module Source Groups" ON)

# Fetch juce
FetchContent_Declare (JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG origin/master
    GIT_SHALLOW 1
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/JUCE)
FetchContent_MakeAvailable (JUCE)

# Fetch popsicle
FetchContent_Declare (popsicle
    GIT_REPOSITORY https://github.com/kunitoki/popsicle.git
    GIT_TAG origin/dev/reworked_version_1
    GIT_SHALLOW 1
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/popsicle)
FetchContent_Populate (popsicle)
add_subdirectory ("${popsicle_SOURCE_DIR}/modules" popsicle)

# Fetch melatonin inspector
FetchContent_Declare (melatonin_inspector
  GIT_REPOSITORY https://github.com/sudara/melatonin_inspector.git
  GIT_TAG origin/main
  GIT_SHALLOW 1
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/melatonin_inspector)
FetchContent_MakeAvailable (melatonin_inspector)

# Add the straw modules
get_filename_component (MODULES_PATH "${CMAKE_CURRENT_LIST_DIR}/../Modules" ABSOLUTE)
add_subdirectory (${MODULES_PATH} ./Modules)

# Configure python
set (Python_ROOT_DIR "/Library/Frameworks/Python.framework/Versions/Current")
set (Python_USE_STATIC_LIBS TRUE)
find_package (Python REQUIRED Development.Embed)

juce_add_gui_app (${TARGET_NAME}
    PRODUCT_NAME "Automation Example"
    VERSION "1.0.0"
    BUNDLE_ID "org.kunitoki.strawdemo")
juce_generate_juce_header (${TARGET_NAME})

target_sources (${TARGET_NAME} PRIVATE
    Main.cpp
    SimpleAutomation.cpp
    SimpleAutomation.h)

set_target_properties (${TARGET_NAME} PROPERTIES JUCE_TARGET_KIND_STRING "App")
set_target_properties (${TARGET_NAME} PROPERTIES CXX_STANDARD 17)

if (APPLE)
    set_target_properties (${TARGET_NAME} PROPERTIES OSX_ARCHITECTURES "arm64;x86_64")
    set (STRAW_LTO "juce::juce_recommended_lto_flags")
else()
    set (STRAW_LTO "")
endif()

target_compile_definitions (${TARGET_NAME} PRIVATE
    JUCE_STANDALONE_APPLICATION=1
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_MODAL_LOOPS_PERMITTED=1
    JUCE_WEB_BROWSER=0
    JUCE_LOAD_CURL_SYMBOLS_LAZILY=1
    JUCE_ALLOW_STATIC_NULL_VARIABLES=0
    JUCE_LOG_ASSERTIONS=1
    JUCE_STRICT_REFCOUNTEDPOINTER=1)

target_link_libraries (${TARGET_NAME} PRIVATE
    juce::juce_analytics
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_cryptography
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_opengl
    juce::juce_osc
    juce::juce_video
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
    Python::Python
    popsicle::juce_python
    popsicle::juce_python_recommended_warning_flags
    straw::juce_straw
    straw::juce_straw_recommended_warning_flags
    Melatonin::Inspector
    ${STRAW_LTO})
