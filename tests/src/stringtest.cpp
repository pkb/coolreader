#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "lvstring.h"

TEST_CASE("Testing basic_lstring API Completeness") {
    lString32 str("  0x1F  ");

    SUBCASE("Trimming implementation") {
        str.trim();
        CHECK(str == "0x1F");
    }

    SUBCASE("atoi parsing functionality") {
        int val = 0;
        str.trim();
        CHECK(str.atoi(val) == true);
        CHECK(val == 31);
    }
}

#if 0
TEST_CASE("Micro-benchmarking string algorithms") {
    // Generate a long string to evaluate heap performance and bypass SSO
    lString8 longStr("This is a remarkably long string used to profile operations.");
    
    doctest::Timer timer;
    
    // Warm up/Run loop
    for (int i = 0; i < 500000; ++i) {
        longStr.lowercase();
    }

    double elapsed_seconds = timer.getElapsedSeconds();
    MESSAGE("Elapsed time for 500,000 case transitions: ", elapsed_seconds, "s");
}
#endif