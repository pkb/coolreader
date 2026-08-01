#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "lvstring.h"

#include <locale>
#include <codecvt>
#include <iostream>
#include <string>

std::string toStdString(const lString32& v) 
{
    std::u32string uv(v.c_str());
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
    std::string narrow = converter.to_bytes(uv);
    return narrow;
}

std::string toStdString(const lString16& v) 
{
    std::u16string uv(v.c_str());
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
    std::string narrow = converter.to_bytes(uv);
    return narrow;
}

std::string toStdString(const lString8& v) 
{
    std::string narrow = std::string(v.c_str());
    return narrow;
}

TEST_CASE_TEMPLATE("trim tests", T, lString32, lString16, lString8) {
    T str("  0x1F  ");
    T es;
    T es1("    	   ");
    T str1("      aaaaaaaa");
    T str2("aaaaaaaa  	");
    T str3("  aaaaaaaa  	   ");
    T str4("  aaa a aaaa  	   ");


    SUBCASE("Trimming implementation") {
        str.trim();
        CHECK(toStdString(str) == "0x1F");
    }

    SUBCASE("Trimming empty string") {
        es.trim();
        CHECK(toStdString(es) == "");
    }

    SUBCASE("Trimming string with whitespaces only") {
        es1.trim();
        CHECK(es1.empty() == true);
    }

    SUBCASE("Trim left") {
        str1.trim();
        CHECK(toStdString(str1) == "aaaaaaaa");
    }

    SUBCASE("Trim right") {
        str2.trim();
        CHECK(toStdString(str2) == "aaaaaaaa");
    }

    SUBCASE("Trim both") {
        str3.trim();
        CHECK(toStdString(str3) == "aaaaaaaa");
    }

    SUBCASE("Trim both - keep middle") {
        str4.trim();
        CHECK(toStdString(str4) == "aaa a aaaa");
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