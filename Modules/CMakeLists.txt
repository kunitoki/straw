juce_add_modules(juce_straw juce_python)

add_library(juce_straw_recommended_warning_flags INTERFACE)
if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR (CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC"))
    #target_compile_options(juce_straw_recommended_warning_flags INTERFACE "/W4")
elseif((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    target_compile_options(juce_straw_recommended_warning_flags INTERFACE
        -Wno-shadow-field-in-constructor
        -Wno-shadow-field
        -Wno-shadow-uncaptured-local
        -Wno-self-assign-overloaded
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-float-equal)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(juce_straw_recommended_warning_flags INTERFACE
        -Wno-shadow-field-in-constructor
        -Wno-shadow-field
        -Wno-shadow-uncaptured-local
        -Wno-self-assign-overloaded
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-float-equal)
endif()

add_library(straw::juce_straw ALIAS juce_straw)
add_library(straw::juce_python ALIAS juce_python)
add_library(straw::juce_straw_recommended_warning_flags ALIAS juce_straw_recommended_warning_flags)
