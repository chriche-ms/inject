project(inject_test)

include(GoogleTest)

set(SOURCE src/container_tests.cpp
           src/factory_tests.cpp)

add_executable(inject_test ${SOURCE})

target_link_libraries(inject_test PUBLIC inject gtest gtest_main gmock)

target_compile_features(inject_test PUBLIC cxx_std_17)
set_target_properties(inject_test PROPERTIES CXX_EXTENSIONS OFF)

gtest_discover_tests(inject_test)
