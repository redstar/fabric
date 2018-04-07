find_package (Boost COMPONENTS system filesystem unit_test_framework REQUIRED)
include_directories (${cppshim_SOURCE_DIR}/include
                     ${cppshim_SOURCE_DIR}/src
                     ${Boost_INCLUDE_DIRS}
                     )
add_definitions (-DBOOST_TEST_DYN_LINK)
add_executable (AllTests main.cc fsm_test.cc channel_test.cc)
target_link_libraries (AllTests
                       cppshim
                       ${Boost_FILESYSTEM_LIBRARY}
                       ${Boost_SYSTEM_LIBRARY}
                       ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                       )

add_test (NAME CppShim_Tests COMMAND AllTests)
