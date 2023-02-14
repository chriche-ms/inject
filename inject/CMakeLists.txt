project(inject_inject)

set(INCLUDE include)

set(HEADER include/inject/factory.h
           include/inject/factory_exception.h
           include/inject/function_traits.h
           include/inject/type_id.h)

add_library(inject_inject INTERFACE)

target_sources(inject_inject INTERFACE ${HEADER}) # required to associate files with an INTERFACE library

target_include_directories(inject_inject INTERFACE ${INCLUDE})

target_compile_features(inject_inject INTERFACE cxx_std_17)