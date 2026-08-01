/***************************************************************************
 *   CoolReader engine                                                     *
 *   Copyright (C) 2007-2015,2018 Vadim Lopatin <coolreader.org@gmail.com> *
 *   Copyright (C) 2014 Huang Xin <chrox.huang@gmail.com>                  *
 *   Copyright (C) 2015 Yifei(Frank) ZHU <fredyifei@gmail.com>             *
 *   Copyright (C) 2016 Bob Gordon <bobgordon62@gmail.com>                 *
 *   Copyright (C) 2018-2021 poire-z <poire-z@users.noreply.github.com>    *
 *   Copyright (C) 2019-2021 Aleksey Chernov <valexlin@gmail.com>          *
 *   Copyright (C) 2021 Dmitry Atamanov <datamanrb@gmail.com>              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License           *
 *   as published by the Free Software Foundation; either version 2        *
 *   of the License, or (at your option) any later version.                *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,            *
 *   MA 02110-1301, USA.                                                   *
 ***************************************************************************/

/**
 * \file lvstring.h
 * \brief string classes interface
 */

#ifndef __LV_STRING_H_INCLUDED__
#define __LV_STRING_H_INCLUDED__

#include "lvtypes.h"
#include "lvmemman.h"

#if (USE_UTF8PROC==1)
#include <utf8proc.h>
#endif

#include <string>
#include <cstring>
#include <charconv>
#include <type_traits>
#include <algorithm>

// (Note: some of these 0x have lowercase hex digit, to avoid
// 'redefined' warnings as they are already defined in lowercase
// in antiword/wordconst.h.)

/// Unicode spaces
#define UNICODE_NO_BREAK_SPACE            0x00A0
#define UNICODE_ZERO_WIDTH_NO_BREAK_SPACE 0xfeff
#define UNICODE_WORD_JOINER      0x2060
// All chars from U+2000 to U+200B allow wrap after, except U+2007
#define UNICODE_EN_QUAD          0x2000
#define UNICODE_FIGURE_SPACE     0x2007
#define UNICODE_ZERO_WIDTH_SPACE 0x200b

#ifdef USE_ATOMIC_REFCOUNT
#include <atomic>
#endif
/// Unicode hyphens
#define UNICODE_SOFT_HYPHEN_CODE 0x00AD
#define UNICODE_ARMENIAN_HYPHEN  0x058A
// All chars from U+2010 to U+2014 allow deprecated wrap after, except U+2011
#define UNICODE_HYPHEN           0x2010
#define UNICODE_NO_BREAK_HYPHEN  0x2011
#define UNICODE_EM_DASH          0x2014

// Punctuation and CJK ranges
#define UNICODE_GENERAL_PUNCTUATION_BEGIN 0x2000
#define UNICODE_GENERAL_PUNCTUATION_END 0x206F
#define UNICODE_CJK_IDEOGRAPHS_BEGIN 0x3041
#define UNICODE_CJK_IDEOGRAPHS_END 0x02CEAF
#define UNICODE_CJK_IDEOGRAPHIC_SPACE 0x3000
#define UNICODE_CJK_PUNCTUATION_BEGIN 0x3000
#define UNICODE_CJK_PUNCTUATION_END 0x303F
// These may be wrong as this block contain katakana and hangul
// letters, as well as ascii full-width chars:
#define UNICODE_CJK_PUNCTUATION_HALF_AND_FULL_WIDTH_BEGIN 0xFF01
#define UNICODE_CJK_PUNCTUATION_HALF_AND_FULL_WIDTH_END 0xFFEE

#define UNICODE_ASCII_FULL_WIDTH_BEGIN 0xFF01
#define UNICODE_ASCII_FULL_WIDTH_END 0xFF5E
#define UNICODE_ASCII_FULL_WIDTH_OFFSET 0xFEE0 // substract or add to convert to/from ASCII


/// strlen for lChar16
int lStr_len(const lChar16 * str);
/// strlen for lChar32
int lStr_len(const lChar32 * str);
/// strlen for lChar8
int lStr_len(const lChar8 * str);
/// strnlen for lChar16
int lStr_nlen(const lChar16 * str, int maxcount);
/// strnlen for lChar32
int lStr_nlen(const lChar32 * str, int maxcount);
/// strnlen for lChar8
int lStr_nlen(const lChar8 * str, int maxcount);
/// strcpy for lChar16
int lStr_cpy(lChar16 * dst, const lChar16 * src);
/// strcpy for lChar32
int lStr_cpy(lChar32 * dst, const lChar32 * src);
/// strcpy for lChar16 -> lChar8
int lStr_cpy(lChar16 * dst, const lChar8 * src);
/// strcpy for lChar32 -> lChar8
int lStr_cpy(lChar32 * dst, const lChar8 * src);
/// strcpy for lChar8
int lStr_cpy(lChar8 * dst, const lChar8 * src);
/// strncpy for lChar16
int lStr_ncpy(lChar16 * dst, const lChar16 * src, int maxcount);
/// strncpy for lChar32
int lStr_ncpy(lChar32 * dst, const lChar32 * src, int maxcount);
/// strncpy for lChar8
int lStr_ncpy(lChar8 * dst, const lChar8 * src, int maxcount);
/// memcpy for lChar16
void   lStr_memcpy(lChar16 * dst, const lChar16 * src, int count);
/// memcpy for lChar32
void   lStr_memcpy(lChar32 * dst, const lChar32 * src, int count);
/// memcpy for lChar8
void   lStr_memcpy(lChar8 * dst, const lChar8 * src, int count);
/// memset for lChar16
void   lStr_memset(lChar16 * dst, lChar16 value, int count);
/// memset for lChar32
void   lStr_memset(lChar32 * dst, lChar32 value, int count);
/// memset for lChar8
void   lStr_memset(lChar8 * dst, lChar8 value, int count);
/// strcmp for lChar16
int    lStr_cmp(const lChar16 * str1, const lChar16 * str2);
/// strcmp for lChar32
int    lStr_cmp(const lChar32 * str1, const lChar32 * str2);
/// strcmp for lChar32 <> lChar8
int    lStr_cmp(const lChar32 * str1, const lChar8 * str2);
/// strcmp for lChar16 <> lChar8
int    lStr_cmp(const lChar16 * str1, const lChar8 * str2);
/// strcmp for lChar8 <> lChar16
int    lStr_cmp(const lChar8 * str1, const lChar16 * str2);
/// strcmp for lChar16 <> lChar32
int    lStr_cmp(const lChar16 * str1, const lChar32 * str2);
/// strcmp for lChar8 <> lChar32
int    lStr_cmp(const lChar8 * str1, const lChar32 * str2);
/// strcmp for lChar32 <> lChar16
int    lStr_cmp(const lChar32 * str1, const lChar16 * str2);
/// strcmp for lChar8
int    lStr_cmp(const lChar8 * str1, const lChar8 * str2);
/// convert string to uppercase
void lStr_uppercase( lChar8 * str, int len );
/// convert string to lowercase
void lStr_lowercase( lChar8 * str, int len );
/// convert string to uppercase
void lStr_uppercase( lChar32 * str, int len );
/// convert string to lowercase
void lStr_lowercase( lChar32 * str, int len );
/// convert string to be capitalized
void lStr_capitalize( lChar32 * str, int len );
/// convert string to use full width chars
void lStr_fullWidthChars( lChar32 * str, int len );
/// calculates CRC32 for buffer contents
lUInt32 lStr_crc32( lUInt32 prevValue, const void * buf, int size );

// returns 0..15 if c is hex digit, -1 otherwise
int hexDigit( int c );
// decode LEN hex digits, return decoded number, -1 if invalid
int decodeHex( const lChar32 * str, int len );
// decode LEN decimal digits, return decoded number, -1 if invalid
int decodeDecimal( const lChar32 * str, int len );


#define CH_PROP_UPPER       0x0001 ///< uppercase alpha character flag
#define CH_PROP_LOWER       0x0002 ///< lowercase alpha character flag
#define CH_PROP_ALPHA       0x0003 ///< alpha flag is combination of uppercase and lowercase flags
#define CH_PROP_DIGIT       0x0004 ///< digit character flag
#define CH_PROP_PUNCT       0x0008 ///< pubctuation character flag
#define CH_PROP_SPACE       0x0010 ///< space character flag
#define CH_PROP_HYPHEN      0x0020 ///< hyphenation character flag
#define CH_PROP_VOWEL       0x0040 ///< vowel character flag
#define CH_PROP_CONSONANT   0x0080 ///< consonant character flag
#define CH_PROP_SIGN        0x0100 ///< sign character flag
#define CH_PROP_ALPHA_SIGN  0x0200 ///< alpha sign character flag
#define CH_PROP_DASH        0x0400 ///< minus, emdash, endash, ... (- signs)
#define CH_PROP_CJK         0x0800 ///< CJK ideographs
#define CH_PROP_RTL         0x1000 ///< RTL character
#define CH_PROP_AVOID_WRAP_AFTER   0x2000 ///< avoid wrap on following space
#define CH_PROP_AVOID_WRAP_BEFORE  0x4000 ///< avoid wrap on preceding space

/// retrieve character properties mask array for wide c-string
void lStr_getCharProps( const lChar32 * str, int sz, lUInt16 * props );
/// retrieve character properties mask for single wide character
lUInt16 lGetCharProps( lChar32 ch );
/// find alpha sequence bounds
void lStr_findWordBounds( const lChar32 * str, int sz, int pos, int & start, int & end, bool & has_rtl );
// is char a word separator
bool lStr_isWordSeparator( lChar32 ch );

namespace detail {
    std::basic_string<lChar16> Utf8ToUtf16(const lChar8* str, size_t len);
    std::basic_string<lChar32> Utf8ToUtf32(const lChar8* str, size_t len);
    std::basic_string<lChar8> Utf16ToUtf8(const lChar16* str, size_t len);
    std::basic_string<lChar32> Utf16ToUtf32(const lChar16* str, size_t len);
    std::basic_string<lChar8> Utf32ToUtf8(const lChar32* str, size_t len);
    std::basic_string<lChar16> Utf32ToUtf16(const lChar32* str, size_t len);
}

namespace fmt {
    class decimal {
        lInt64 value;
    public:
        explicit decimal(lInt64 v) : value(v) { }
        lInt64 get() const { return value; }
    };

    class hex {
        lUInt64 value;
    public:
        explicit hex(lInt64 v) : value(v) { }
        lUInt64 get() const { return value; }
    };
}

// Helper for hex digits
template <typename CharT>
constexpr CharT toHexDigit(int digit) noexcept
{
    if (digit < 10) {
        return static_cast<CharT>('0' + digit);
    } else {
        return static_cast<CharT>('a' + ((digit & 0xF) - 10));
    }
}

template <typename StringT, typename NumT>
inline bool StringToNum(const StringT& s, NumT& n) noexcept
{
    std::string str;
    using CharT = typename StringT::value_type;

    if constexpr (std::is_same_v<CharT, lChar8>) {
        str = static_cast<std::string>(s);
    }
    else if constexpr (std::is_same_v<CharT, lChar16>) {
        str = detail::Utf16ToUtf8(s.data(), s.size());
    }
    else {
        str = detail::Utf32ToUtf8(s.data(), s.size());
    }

    const char* p = str.data();
    const char* end = p + str.size();

    while (p < end && (*p == ' ' || *p == '\t')) ++p;
    if (p == end) return false;

    std::from_chars_result result;

    if constexpr (std::is_floating_point_v<NumT>) {
        // Floating point logic (does not accept a base argument)
        result = std::from_chars(p, end, n, std::chars_format::general);
    }
    else if constexpr (std::is_integral_v<NumT>) {
        if (end - p >= 2 && p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
            p += 2;
            result = std::from_chars(p, end, n, 16);
        } else {
            result = std::from_chars(p, end, n, 10);
        }
    }
    else {
        static_assert(std::is_arithmetic_v<NumT>, "Unsupported type passed to StringToNum.");
    }

    if (result.ec != std::errc() || result.ptr == p) return false;

    //FIXME: original code did not check for trailing whitespaces properly
    const char* trailing = result.ptr;
    while (trailing < end && (*trailing == ' ' || *trailing == '\t')) {
        ++trailing;
    }

    return trailing == end;
}

template <typename T>
std::string NumToString(T value, bool hex = false)
{
    // A 64-byte buffer is safely large enough for any 64-bit integer or double-precision float
    const size_t buf_size = 64;
    lChar8 buf[buf_size]{};
    std::to_chars_result result;

    if constexpr (std::is_floating_point_v<T>) {
        // Floating point values (float/double) - std::to_chars ignores fmt flags here
        result = std::to_chars(buf, buf + buf_size, value);
    }
    else if constexpr (std::is_integral_v<T>) {
        // Integral values (int, int64_t, uint32_t, etc.)
        int base = hex ? 16 : 10;
        result = std::to_chars(buf, buf + buf_size, value, base);
    }
    else {
        static_assert(std::is_arithmetic_v<T>, "to_lstring only supports numeric types.");
    }

    // Handle conversion failure
    if (result.ec != std::errc{}) {
        return {};
    }
    return std::string(buf, result.ptr - buf);
}

template <typename StringT>
void appendString(StringT& dest, const std::string& str)
{
    using CharT = typename StringT::value_type;

    // Conditionally compile the correct conversion path based on CharT
    if constexpr (std::is_same_v<CharT, lChar8>) {
        dest.append(str.data(), str.size());
    }
    else if constexpr (std::is_same_v<CharT, lChar16>) {
        auto converted = detail::Utf8ToUtf16(str.data(), str.size());
        dest.append(converted.data(), converted.size());
    }
    else {
        auto converted = detail::Utf8ToUtf32(str.data(), str.size());
        dest.append(converted.data(), converted.size());
    }
}

template <typename StringT>
void appendDecimal(StringT& str, lInt64 val)
{
    appendString(str, NumToString(val));
}

template <typename StringT>
void appendHex(StringT& str, lInt64 val)
{
    appendString(str, NumToString(val, true));
}

#define STRING_HASH_MULT 31

template <typename StringT>
inline lUInt32 getHash(const StringT& s)
{
    // 1. Extract the raw character type (char, char16_t, etc.)
    using CharT = typename StringT::value_type;

    // 2. Get its safe unsigned equivalent to prevent signed char extensions
    using UnsignedCharT = std::make_unsigned_t<CharT>;

    lUInt32 res = 0;
    for (auto c : s)
    {
        // 3. Cast to unsigned first, then safely scale up to lUInt32
        res = res * STRING_HASH_MULT + static_cast<lUInt32>(static_cast<UnsignedCharT>(c));
    }
    return res;
}

template <typename Derived, typename CharT>
class basic_lstring_operators : public std::basic_string<CharT>
{
public:
    using base = std::basic_string<CharT>;
    using base::basic_string;

    // Cast helper to return references to the final derived class
    Derived& derived() { return static_cast<Derived&>(*this); }

    Derived& operator<<(CharT c) { base::push_back(c); return derived(); }
    Derived& operator<<(const CharT* s) { if (s) base::append(s); return derived(); }
    Derived& operator<<(const base& s) { base::append(s); return derived(); }
    Derived& operator<<(const Derived& s) { base::append(s); return derived(); }
    Derived& operator<<(std::basic_string_view<CharT> sv) { base::append(sv.data(), sv.size()); return derived(); }
    Derived& operator<<(fmt::decimal v) { appendDecimal(v.get()); return derived(); }
    Derived& operator<<(fmt::hex v) { appendHex(v.get()); return derived(); }
    template <typename T = CharT, typename = std::enable_if_t<!std::is_same_v<T, lChar8>>>
    Derived& operator<<(const lChar8* s) { appendLiteral(s); return derived();  }
    Derived& appendDecimal(lInt64 n) { ::appendDecimal(derived(), n); return derived(); }
    Derived& appendHex(lUInt64 n) { ::appendHex(derived(), n); return derived(); }

    Derived& operator+=( CharT c ) { base::push_back(c); return derived(); }
    Derived& operator+=( const CharT * s ) { if (s) base::append(s); return derived(); }
    Derived& operator+=(const base& s) { base::append(s); return derived(); }
    Derived& operator+=(const Derived& s) { base::append(s); return derived(); }
    Derived& operator+=(std::basic_string_view<CharT> sv) { base::append(sv.data(), sv.size()); return derived(); }
    template <typename T = CharT, typename = std::enable_if_t<!std::is_same_v<T, lChar8>>>
    Derived& operator+=(const lChar8* s) { appendLiteral(s); return derived();  }
    Derived& operator+=(fmt::decimal v) { appendDecimal(v.get()); return derived(); }
    Derived& operator+=(fmt::hex v) { appendHex(v.get()); return derived(); }
private:
    void appendLiteral(const lChar8* s)
    {
        if (!s || *s == '\0')
            return;
        appendString(derived(), std::string(s));
    }
};

template <typename CharT>
class basic_lstring : public basic_lstring_operators<basic_lstring<CharT>, CharT>
{
public:
    using base = basic_lstring_operators<basic_lstring<CharT>, CharT>;
    using size_type = typename base::size_type;
    using base::base;
    using base::compare;
    using base::append;

    basic_lstring(const basic_lstring& other) = default;
    basic_lstring(basic_lstring&& other) noexcept = default;

    basic_lstring& operator=(const basic_lstring& other) = default;
    basic_lstring& operator=(basic_lstring&& other) noexcept = default;
    template <typename T = CharT, typename = std::enable_if_t<!std::is_same_v<T, lChar8>>>
    basic_lstring& operator=(const lChar8* other) noexcept
    {
        if (!other) {
            base::clear();
            return *this;
        }
        appendString(*this, std::string(other));
        return *this;
    }

    explicit basic_lstring(int size) : basic_lstring(static_cast<size_type>(size), 0) {}

    explicit basic_lstring(const lChar8* str)
        : basic_lstring(str, str ? lStr_len(str) : 0) {}

    explicit basic_lstring(const lChar8* str, size_type count)
    {
        if (!str || count == 0) return;

        if constexpr (std::is_same_v<CharT, lChar8>) {
            base::assign(str, count);
        }
        else if constexpr (std::is_same_v<CharT, lChar16>) {
            auto converted = detail::Utf8ToUtf16(str, count);
            base::assign(converted.data(), converted.size());
        }
        else if constexpr (std::is_same_v<CharT, lChar32>) {
            auto converted = detail::Utf8ToUtf32(str, count);
            base::assign(converted.data(), converted.size());
        }
    }

    explicit basic_lstring(const lChar16* str)
        : basic_lstring(str, str ? lStr_len(str) : 0) {}

    explicit basic_lstring(const lChar16* str, size_type count)
    {
        if (!str || count == 0) return;

        if constexpr (std::is_same_v<CharT, lChar16>) {
            base::assign(str, count);
        }
        else if constexpr (std::is_same_v<CharT, lChar8>) {
            auto converted = detail::Utf16ToUtf8(str, count);
            base::assign(converted.data(), converted.size());
        }
        else if constexpr (std::is_same_v<CharT, lChar32>) {
            auto converted = detail::Utf16ToUtf32(str, count);
            base::assign(converted.data(), converted.size());
        }
    }

    explicit basic_lstring(const lChar32* str)
        : basic_lstring(str, str ? lStr_len(str) : 0) {}

    explicit basic_lstring(const lChar32* str, size_type count)
    {
        if (!str || count == 0) return;

        if constexpr (std::is_same_v<CharT, lChar32>) { // Assuming lChar32 type alignment
            base::assign(str, count);
        }
        else if constexpr (std::is_same_v<CharT, lChar8>) {
            auto converted = detail::Utf32ToUtf8(str, count);
            base::assign(converted.data(), converted.size());
        }
        else if constexpr (std::is_same_v<CharT, lChar16>) {
            auto converted = detail::Utf32ToUtf16(str, count);
            base::assign(converted.data(), converted.size());
        }
    }
    lUInt32 getHash() const
    {
        return ::getHash(*this);
    }
    // --- Substring (Overrides standard to return the wrapper class rather than raw std::string) ---
    basic_lstring substr(size_type pos = 0, size_type count = base::npos) const
    {
        auto raw_sub = base::substr(pos, count);
        return basic_lstring(raw_sub.data(), raw_sub.size());
    }
    basic_lstring& replace(size_type p0, size_type n0, const CharT* str)
    {
        base::replace(p0, n0, str);
        return *this;
    }
    basic_lstring& replace(size_type p0, size_type n0, const CharT* str, size_type count)
    {
        base::replace(p0, n0, str, count);
        return *this;
    }
    basic_lstring& replace(size_type p0, size_type n0, const basic_lstring& str)
    {
        base::replace(p0, n0, str);
        return *this;
    }
    basic_lstring& replace(size_type p0, size_type n0, const basic_lstring& str, size_type offset, size_type count)
    {
        base::replace(p0, n0, str, offset, count);
        return *this;
    }
    basic_lstring& replace(size_type p0, size_type n0, size_type count, CharT ch)
    {
        base::replace(p0, n0, count, ch);
        return *this;
    }
    basic_lstring& replace(CharT before, CharT after)
    {
        std::replace(base::begin(), base::end(), before, after);
        return *this;
    }
    bool replace(const basic_lstring& findStr, const basic_lstring& replaceStr)
    {
        if(findStr.empty())
            return false;
        size_type n = base::find(findStr);
        if(base::npos == n)
            return false;
        replace(n, findStr.length(), replaceStr);
        return true;
    }

    bool replaceParam(int index, const basic_lstring& value)
    {
        return replace(basic_lstring("") + fmt::decimal(index), value);
    }

    bool replaceIntParam(int index, int replaceNumber)
    {
        return replaceParam( index, basic_lstring::itoa(replaceNumber));
    }

    // SFINAE-constrained splits: Only compiles/exists when instantiated for lChar32
    template <typename T = CharT, typename = std::enable_if_t<std::is_same_v<T, lChar32>>>
    bool split2(const basic_lstring<lChar32>& delim, basic_lstring<lChar32>& value1, basic_lstring<lChar32>& value2) const
    {
        auto pos = base::find(delim);
        if (pos == base::npos) return false;

        value1 = substr(0, pos);
        value2 = substr(pos + delim.length());
        return true;
    }

    template <typename T = CharT, typename = std::enable_if_t<std::is_same_v<T, lChar32>>>
    bool split2(const lChar32* delim, basic_lstring<lChar32>& value1, basic_lstring<lChar32>& value2) const
    {
        if (!delim) return false;
        auto pos = base::find(delim);
        if (pos == base::npos) return false;

        value1 = substr(0, pos);
        value2 = substr(pos + lStr_len(delim)); // assuming lStr_len works on lChar32*
        return true;
    }

    template <typename T = CharT, typename = std::enable_if_t<std::is_same_v<T, lChar32>>>
    bool split2(const lChar8* delim, basic_lstring<lChar32>& value1, basic_lstring<lChar32>& value2) const
    {
        if (!delim) return false;
        // Convert the 8-bit delimiter to 32-bit to perform the find matching operations
        auto converted_delim = detail::Utf8ToUtf32(delim, lStr_len(delim));
        auto pos = base::find(converted_delim.data(), 0, converted_delim.size());
        if (pos == base::npos) return false;

        value1 = substr(0, pos);
        value2 = substr(pos + converted_delim.size());
        return true;
    }

    static basic_lstring itoa(lInt64 n)
    {
        auto string_value = NumToString(n);
        return basic_lstring(string_value.c_str(), string_value.size());
    }

    basic_lstring& uppercase()
    {
        //TODO
        return *this;
    }

    /// make string lowercase
    basic_lstring& lowercase()
    {
        //TODO
        return *this;
    }

    basic_lstring& capitalize()
    {
        //TODO
        return *this;
    }

    // trims spaces at beginning and end of string (modifies and returns *this)
    basic_lstring& trim()
    {
        const CharT ws[] = { ' ', '\t', '\0' };

        // Left trim
        size_t start = base::find_first_not_of(ws);
        if (start != base::npos) {
            base::erase(0, start);
        } else {
            base::clear(); // String is entirely whitespace
            return *this;
        }

        // Right trim
        size_t end = base::find_last_not_of(ws);
        if (end != base::npos) {
            base::erase(end + 1);
        }
        return *this;
    }

    // trims non-alpha at beginning and end of string
    basic_lstring& trimNonAlpha()
    {
        //TODO

        return *this;
    }

    template <typename T = CharT, typename = std::enable_if_t<std::is_same_v<T, lChar32>>>
    basic_lstring& trimDoubleSpaces( bool allowStartSpace, bool allowEndSpace, bool removeEolHyphens=false )
    {
        //TODO
        return *this;
    }
    // ------------------------------------------------------------------------
    // Conversion Functions (String -> Integer)
    // ------------------------------------------------------------------------

    // converts to integer, returns 0 on failure (classic legacy behavior)
    int atoi() const
    {
        int n = 0;
        bool res = atoi(n);
        return res ? n : 0;
    }

    lInt64 atoi64() const
    {
        lInt64 n = 0;
        return StringToNum(*this, n) ? n : 0;
    }

    // converts to integer, returns true if success
    bool atoi(int& n) const
    {
        n = 0;
        return StringToNum(*this, n);
    }

    // converts to 64 bit integer, returns true if success
    bool atoi(lInt64& n) const
    {
        n = 0;
        return StringToNum(*this, n);
    }

    /// convert to double
    double atod() const
    {
        double d = 0.0;
        bool res = atod(d, '.');
        return res ? d : 0.0;
    }
    /// convert to double, returns true if success
    bool atod( double &d, char dp = '.' ) const
    {
        d = 0.0;
        return StringToNum(*this, d);
    }
    /// find position of char inside string, -1 if not found
    int pos(CharT ch, int start = 0) const
    {
        size_t res = base::find(ch, static_cast<size_t>(start));
        return (res == base::npos) ? -1 : static_cast<int>(res);
    }
    /// find position of substring inside string, -1 if not found
    int pos(const basic_lstring& subStr, int start = 0) const
    {
        size_t res = base::find(subStr, static_cast<size_t>(start));
        return (res == base::npos) ? -1 : static_cast<int>(res);
    }
    int pos(basic_lstring& subStr, int start = 0) const
    {
        size_t res = base::find(subStr, static_cast<size_t>(start));
        return (res == base::npos) ? -1 : static_cast<int>(res);
    }
    int pos(const CharT* subStr, int startPos = 0) const
    {
        if (!subStr)
            return -1;
        size_t res = base::find(subStr, static_cast<size_t>(startPos));
        return (res == base::npos) ? -1 : static_cast<int>(res);
    }
    template <typename T = CharT, typename = std::enable_if_t<!std::is_same_v<T, lChar8>>>
    int pos(const lChar8* subStr, int startPos = 0) const
    {
        if (!subStr)
            return -1;
        basic_lstring converted(subStr); // Construct temp matching encoding
        return pos(converted, startPos);
    }
    int rpos(CharT ch) const
    {
        size_t res = base::rfind(ch);
        return (res == base::npos) ? -1 : static_cast<int>(res);
    }
    int rpos(CharT* substr) const
    {
        if (!substr)
            return -1;
        size_t res = base::rfind(substr);
        return (res == base::npos) ? -1 : static_cast<int>(res);
    }
    template <typename T = CharT, typename = std::enable_if_t<!std::is_same_v<T, lChar8>>>
    int rpos(const lChar8* substr) const
    {
        if (!substr)
            return -1;
        basic_lstring converted(substr);
        return rpos(converted.c_str());
    }
    int rpos(const basic_lstring& substr) const
    {
        size_t res = base::rfind(substr);
        return (res == base::npos) ? -1 : static_cast<int>(res);
    }

        /// returns true if string starts with specified substring
    bool startsWith (const basic_lstring& substring) const
    {
        //TODO
        return false;
    }
    /// returns true if string starts with specified substring
    bool startsWith (const CharT* substring) const
    {
        //TODO
        return false;
    }
    template <typename T = CharT, typename = std::enable_if_t<!std::is_same_v<T, lChar8>>>
    int compare(const lChar8 *s) const  { return lStr_cmp(base::c_str(), s); }
    basic_lstring& append(const basic_lstring& str)
    {
        base::append(str);
        return *this;
    }
    template <typename T = CharT, typename = std::enable_if_t<!std::is_same_v<T, lChar8>>>
    basic_lstring& append(const lChar8 * str)
    {
        if (str)
            appendString(*this, std::string(str));
        return *this;
    }
    template <typename T = CharT, typename = std::enable_if_t<!std::is_same_v<T, lChar8>>>
    basic_lstring& append(const lChar8 * str, size_type count)
    {
        if (str && count > 0)
            appendString(*this, std::string(str, count));
        return *this;
    }
    /// returns true if string starts with specified substring (8bit ASCII only)
    template <typename T = CharT, typename = std::enable_if_t<!std::is_same_v<T, lChar8>>>
    bool startsWith (const lChar8 * substring) const
    {
        //TODO
        return false;
    }
    /// returns true if string ends with specified substring
    bool endsWith(const basic_lstring& substring) const
    {
        //TODO
        return false;
    }
    /// returns true if string ends with specified substring (8-bit ASCII only)
    bool endsWith(const CharT* substring) const
    {
        //TODO
        return false;
    }
    /// returns true if string ends with specified substring (8-bit ASCII only)
    template <typename T = CharT, typename = std::enable_if_t<!std::is_same_v<T, lChar8>>>
    bool endsWith(const lChar8* substring) const
    {
        //TODO
        return false;
    }
    /// returns true if string starts with specified substring, case insensitive
    bool startsWithNoCase(const basic_lstring& substring) const
    {
        //TODO
        return false;
    }
    /// returns last character
    CharT lastChar() { return base::empty() ? 0 : base::at(base::length()-1); }
    /// returns first character
    CharT firstChar() { return base::empty() ? 0 : base::at(0); }

    CharT* modify() { return base::data(); }
    basic_lstring& pack() { return *this; }
    /// clear string, set buffer size
    void  reset( size_type size )
    {
        //TODO
    }
    /// erase all extra characters from end of string after size
    void  limit( size_type size )
    {
        //TODO
    }

    //static const basic_lstring<CharT> empty_str;
    static inline const basic_lstring<CharT> empty_str{};
};

//template <typename CharT>
//static inline const basic_lstring<CharT> empty_str{};

using lString8  = basic_lstring<lChar8>;
using lString16 = basic_lstring<lChar16>;
using lString32 = basic_lstring<lChar32>;

// 1. string + string (Same Type)
template <typename CharT>
inline basic_lstring<CharT> operator+(basic_lstring<CharT> s1, const basic_lstring<CharT>& s2) {
    s1.append(s2); // Or s1 += s2; if you mapped += to append
    return s1;     // Compiler uses RVO / Move on return
}

// 2. string + C-string (Same Type)
template <typename CharT>
inline basic_lstring<CharT> operator+(basic_lstring<CharT> s1, const CharT* s2) {
    if (s2) s1.append(s2);
    return s1;
}

// 3. string + Foreign Pointer (Mixed-Width Conversion)
template <typename CharT, typename OtherCharT,
          typename = std::enable_if_t<!std::is_same_v<CharT, OtherCharT>>>
inline basic_lstring<CharT> operator+(basic_lstring<CharT> s1, const OtherCharT* s2) {
    if (s2) {
        s1.append(basic_lstring<CharT>(s2));
    }
    return s1;
}

// 4. string + fmt::decimal
template <typename CharT>
inline basic_lstring<CharT> operator+(basic_lstring<CharT> s1, fmt::decimal v) {
    appendDecimal(s1, v.get());
    return s1;
}

// 5. string + fmt::hex
template <typename CharT>
inline basic_lstring<CharT> operator+(basic_lstring<CharT> s1, fmt::hex v) {
    appendHex(s1, v.get());
    return s1;
}

// C-string + string (Same Width)
template <typename CharT>
inline basic_lstring<CharT> operator+(const CharT* s1, basic_lstring<CharT> s2) {
    if (s1) s2.insert(0, s1);
    return s2;
}

// Foreign C-string + string (Mixed Width)
template <typename CharT, typename OtherCharT,
          typename = std::enable_if_t<!std::is_same_v<CharT, OtherCharT>>>
inline basic_lstring<CharT> operator+(const OtherCharT* s1, basic_lstring<CharT> s2) {
    if (s1) {
        basic_lstring<CharT> converted_left(s1);
        converted_left.append(s2);
        return converted_left;
    }
    return s2;
}

template <typename CharT>
inline bool operator==(const basic_lstring<CharT>& lhs, const CharT* rhs)
{
    if (!rhs)
        return lhs.empty();
    return lhs.compare(rhs) == 0;
}
template <typename CharT>
inline bool operator==(const CharT* lhs, const basic_lstring<CharT>& rhs)
{
    return rhs == lhs; // Reuses the operator above
}

template <typename CharT, typename = std::enable_if_t<!std::is_same_v<CharT, lChar8>>>
inline bool operator==(const basic_lstring<CharT>& lhs, const lChar8* rhs)
{
    if (!rhs) return lhs.empty();

    // Convert the incoming UTF-8 literal to match this string's encoding
    size_t len = lStr_len(rhs);
    if constexpr (std::is_same_v<CharT, lChar16>) {
        return lhs == detail::Utf8ToUtf16(rhs, len).c_str();
    } else {
        return lhs == detail::Utf8ToUtf32(rhs, len).c_str();
    }
}

template <typename CharT, typename = std::enable_if_t<!std::is_same_v<CharT, lChar8>>>
inline bool operator==(const lChar8* lhs, const basic_lstring<CharT>& rhs)
{
    return rhs == lhs; // Reuses the operator above
}

template <typename CharT, typename = std::enable_if_t<!std::is_same_v<CharT, lChar8>>>
inline bool operator==(const basic_lstring<CharT>& lhs, const basic_lstring<lChar8>& rhs)
{
    return lhs == rhs.c_str();
}

template <typename CharT, typename = std::enable_if_t<!std::is_same_v<CharT, lChar8>>>
inline bool operator==(const basic_lstring<lChar8>& lhs, const basic_lstring<CharT>& rhs)
{
    return rhs == lhs; // Reuses the operator above
}

template <typename CharT>
inline bool operator!=(const basic_lstring<CharT>& lhs, const CharT* rhs)
{
    return !(lhs == rhs); // Leverages the operator== you already wrote
}

template <typename CharT>
inline bool operator!=(const CharT* lhs, const basic_lstring<CharT>& rhs)
{
    return !(rhs == lhs);
}

template <typename CharT, typename = std::enable_if_t<!std::is_same_v<CharT, lChar8>>>
inline bool operator!=(const basic_lstring<CharT>& lhs, const lChar8* rhs)
{
    return !(lhs == rhs); // Leverages the templated operator==
}

template <typename CharT, typename = std::enable_if_t<!std::is_same_v<CharT, lChar8>>>
inline bool operator!=(const lChar8* lhs, const basic_lstring<CharT>& rhs)
{
    return !(rhs == lhs);
}

#define cs32(str) lString32(str)
#define cs8(str) lString8(str)

/// calculates hash for wide c-string
lUInt32 calcStringHash( const lChar16 * s );
lUInt32 calcStringHash( const lChar32 * s );

lString8  UnicodeToTranslit( const lString32 & str );
/// converts wide unicode string to local 8-bit encoding
lString8  UnicodeToLocal( const lString32 & str );
/// converts wide unicode string to utf-8 string
inline lString8  UnicodeToUtf8( const lString32 & str ) { return lString8(str.c_str()); }
/// converts wide unicode string to utf-16 string
inline lString16  UnicodeToUtf16( const lString32 & str ) { return lString16(str.c_str()); }
/// converts wide unicode string to utf-8 string
inline lString8 UnicodeToUtf8(const lChar32 * s, int count) { return lString8(s, count); }
/// converts wide unicode string to utf-16 string
inline lString16 UnicodeToUtf16(const lChar32 * s, int count) { return lString16(s, count); }
/// converts wide unicode string to wtf-8 string
lString8  UnicodeToWtf8( const lString32 & str );
/// converts wide unicode string to wtf-8 string
lString8 UnicodeToWtf8(const lChar32 * s, int count);
/// converts unicode string to 8-bit string using specified conversion table
lString8  UnicodeTo8Bit( const lString32 & str, const lChar8 * * table );
/// converts 8-bit string to unicode string using specified conversion table for upper 128 characters
lString32 ByteToUnicode( const lString8 & str, const lChar32 * table );
/// converts 8-bit string in local encoding to wide unicode string
lString32 LocalToUnicode( const lString8 & str );
/// converts utf-8 string to wide unicode string
inline lString32 Utf8ToUnicode( const lString8 & str ) { return lString32(str.c_str()); }
/// converts utf-8 c-string to wide unicode string
inline lString32 Utf8ToUnicode( const char * s ) { return lString32(s); }
/// converts utf-8 string fragment to wide unicode string
inline lString32 Utf8ToUnicode( const char * s, int sz ) { return lString32(s, sz); }
/// converts utf-8 string fragment to wide unicode string
void Utf8ToUnicode(const lUInt8 * src,  int &srclen, lChar32 * dst, int &dstlen);
/// converts utf-16 string to wide unicode string
inline lString32 Utf16ToUnicode( const lString16 & str ) { return lString32(str.c_str()); }
/// converts utf-16 c-string to wide unicode string
inline lString32 Utf16ToUnicode( const lChar16 * s ) { return lString32(s); }
/// converts utf-16 string fragment to wide unicode string
inline lString32 Utf16ToUnicode( const lChar16 * s, int sz ) { return lString32(s, sz); }
/// converts utf-16 string fragment to wide unicode string
void Utf16ToUnicode(const lChar16 * src,  int &srclen, lChar32 * dst, int &dstlen);
/// converts wtf-8 string to wide unicode string
lString32 Wtf8ToUnicode( const lString8 & str );
/// converts utf-8 c-string to wide unicode string
lString32 Wtf8ToUnicode( const char * s );
/// converts utf-8 string fragment to wide unicode string
lString32 Wtf8ToUnicode( const char * s, int sz );
/// decodes path like "file%20name" to "file name"
lString32 DecodeHTMLUrlString( lString32 s );
/// truncates string by specified size, appends ... if truncated, prefers to wrap whole words
void limitStringSize(lString32 & str, int maxSize);

int TrimDoubleSpaces(lChar32 * buf, int len,  bool allowStartSpace, bool allowEndSpace, bool removeEolHyphens);

/// remove soft-hyphens from string
lString32 removeSoftHyphens( lString32 s );


#define LCSTR(x) (UnicodeToUtf8(x).c_str())
bool splitIntegerList( lString32 s, lString32 delim, int & value1, int & value2 );

#endif  // __LV_STRING_H_INCLUDED__
