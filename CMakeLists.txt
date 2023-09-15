cmake_minimum_required(VERSION 3.21)

project(straw VERSION 0.0.1 LANGUAGES CXX
        DESCRIPTION "JUCE module for automated integration testing"
        HOMEPAGE_URL "https://github.com/kunitoki/straw")

set(PROJECT_NAME straw)
project(${PROJECT_NAME})

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Demo")
