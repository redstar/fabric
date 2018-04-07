find_package (Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
include_directories (${cppshim_SOURCE_DIR}/include
                     ${Boost_INCLUDE_DIRS}
                     )
add_executable (fabcar fabcar.cc)
target_link_libraries (fabcar
                       cppshim
                       grpc++
                       grpc++_unsecure
                       grpc++_reflection
                       )

