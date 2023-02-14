project(inject_test)

set(SOURCE src/factory_test.cpp)

add_executable(inject_test ${SOURCE})

target_link_libraries(inject_test PUBLIC inject_inject gtest gtest_main gmock)

target_compile_features(inject_test PUBLIC cxx_std_17)
set_target_properties(inject_test PROPERTIES CXX_EXTENSIONS OFF)