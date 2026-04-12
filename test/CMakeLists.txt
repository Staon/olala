include(GoogleTest)

include(../cmake/olala-utils.cmake)

add_executable(olala-test)

target_link_libraries(olala-test PRIVATE
    olala
    GTest::gtest_main
)

addSources(olala-test . PRIVATE
    symbolidentifier.cpp
    symbolwhitespace.cpp
    symboldouble.cpp
    symbolinteger.cpp
)

gtest_discover_tests(olala-test)
