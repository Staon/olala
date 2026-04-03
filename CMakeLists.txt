cmake_minimum_required(VERSION 3.20)

include(cmake/olala-utils.cmake)

project(OLala)

enable_testing()

# -- Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# -- compiler setting
set(CMAKE_CXX_STANDARD 17)
include_directories(include)

# -- the OLala library
add_library(olala)

# -- library sources
addSources(olala src PRIVATE
    adaptable.cpp
    adaptableexception.cpp
    error.cpp
    inputsequence.cpp
    lookaheadstate.cpp
    inputsequencestream.cpp
    parsercontext.cpp
    semanticstack.cpp
    symbol.cpp
    symbolterminal.cpp
    symbolcontainer.cpp
    symbolnonterminal.cpp
    symbolsequence.cpp
    symbolalternation.cpp
    symbolaction.cpp
    symbolepsilon.cpp
    symbolidentifier.cpp
    value.cpp
    valuestring.cpp
)

# -- library headers
addSources(olala include/olala PUBLIC
    adaptable.h
    adaptableexception.h
    error.h
    inputsequence.h
    lookaheadstate.h
    lookaheadstateptr.h
    inputsequencestream.h
    parsercontext.h
    semanticstack.h
    symbol.h
    symbolptr.h
    symbolterminal.h
    symbolcontainer.h
    symbolnonterminal.h
    symbolsequence.h
    symbolalternation.h
    symbolaction.h
    symbolepsilon.h
    symbolidentifier.h
    value.h
    valueptr.h
    valuestring.h
)

# -- tests
add_subdirectory(test)
