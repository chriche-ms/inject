project(inject)

set(INCLUDE include)

set(HEADER include/inject/container.h
           include/inject/factory.h
           include/inject/factory_exception.h
           include/inject/function_traits.h
           include/inject/type_id.h)

add_library(inject INTERFACE)

target_sources(inject INTERFACE ${HEADER}) # required to associate files with an INTERFACE library

target_include_directories(inject INTERFACE ${INCLUDE})

target_compile_features(inject INTERFACE cxx_std_17)
