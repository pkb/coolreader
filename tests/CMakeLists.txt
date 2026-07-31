SET (CR3TEST_SOURCES 
    src/stringtest.cpp
)

include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest
  GIT_TAG v2.4.11 # tag, branch or even commit SHA
)
FetchContent_MakeAvailable(doctest)

enable_testing()

# Include doctest_discover_tests(<target>) function
include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")

ADD_EXECUTABLE(crengine_test ${CR3TEST_SOURCES})

target_compile_features(crengine_test PRIVATE cxx_std_17)

target_link_libraries(crengine_test crengine doctest::doctest ${STD_LIBS})