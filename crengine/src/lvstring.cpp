/***************************************************************************
 *   CoolReader engine                                                     *
 *   Copyright (C) 2007-2015,2018 Vadim Lopatin <coolreader.org@gmail.com> *
 *   Copyright (C) 2014,2016 Huang Xin <chrox.huang@gmail.com>             *
 *   Copyright (C) 2015 Yifei(Frank) ZHU <fredyifei@gmail.com>             *
 *   Copyright (C) 2016 Hzj_jie <hzj_jie@hotmail.com>                      *
 *   Copyright (C) 2020 Konstantin Potapov <pkbo@users.sourceforge.net>    *
 *   Copyright (C) 2018-2021 Aleksey Chernov <valexlin@gmail.com>          *
 *   Copyright (C) 2021 zwim <martin.zwicknagl@kirchbichl.net>             *
 *   Copyright (C) 2018-2021 poire-z <poire-z@users.noreply.github.com>    *
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
 * \file lvstring.cpp
 * \brief string classes implementation
 */

#include "../include/lvstring.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>

#if !defined(__SYMBIAN32__) && defined(_WIN32)
extern "C" {
#include <windows.h>
}
#endif

#if (USE_ZLIB==1)
#include <zlib.h>
#endif

#if (USE_UTF8PROC==1)
#include <utf8proc.h>
#endif

#if !defined(__SYMBIAN32__) && defined(_WIN32)
extern "C" {
#include <windows.h>
#include "lvstring.h"
}
#endif

#define LS_DEBUG_CHECK

// set to 1 to enable debugging
#define DEBUG_STATIC_STRING_ALLOC 0

////////////////////////////////////////////////////////////////////////////
// Utility functions
////////////////////////////////////////////////////////////////////////////

inline int _lStr_len(const lChar16 * str)
{
    int len;
    for (len=0; *str; str++)
        len++;
    return len;
}

inline int _lStr_len(const lChar32 * str)
{
    int len;
    for (len=0; *str; str++)
        len++;
    return len;
}

inline int _lStr_len(const lChar8 * str)
{
    int len;
    for (len=0; *str; str++)
        len++;
    return len;
}

inline int _lStr_nlen(const lChar16 * str, int maxcount)
{
    int len;
    for (len=0; len<maxcount && *str; str++)
        len++;
    return len;
}

inline int _lStr_nlen(const lChar32 * str, int maxcount)
{
    int len;
    for (len=0; len<maxcount && *str; str++)
        len++;
    return len;
}

inline int _lStr_nlen(const lChar8 * str, int maxcount)
{
    int len;
    for (len=0; len<maxcount && *str; str++)
        len++;
    return len;
}

inline int _lStr_cpy(lChar16 * dst, const lChar16 * src)
{
    int count;
    for ( count=0; (*dst++ = *src++); count++ )
        ;
    return count;
}

inline int _lStr_cpy(lChar32 * dst, const lChar32 * src)
{
    int count;
    for ( count=0; (*dst++ = *src++); count++ )
        ;
    return count;
}

inline int _lStr_cpy(lChar8 * dst, const lChar8 * src)
{
    int count;
    for ( count=0; (*dst++ = *src++); count++ )
        ;
    return count;
}

inline int _lStr_cpy(lChar16 * dst, const lChar8 * src)
{
    int count;
    for ( count=0; (*dst++ = *src++); count++ )
        ;
    return count;
}

inline int _lStr_cpy(lChar32 * dst, const lChar8 * src)
{
    int count;
    for ( count=0; (*dst++ = *src++); count++ )
        ;
    return count;
}

inline int _lStr_cpy(lChar8 * dst, const lChar16 * src)
{
    int count;
    for ( count=0; (*dst++ = (lChar8)*src++); count++ )
        ;
    return count;
}

inline int _lStr_cpy(lChar8 * dst, const lChar32 * src)
{
    int count;
    for ( count=0; (*dst++ = (lChar8)*src++); count++ )
        ;
    return count;
}

inline int _lStr_ncpy(lChar32 * dst, const lChar32 * src, int maxcount)
{
    int count = 0;
    do
    {
        if (++count > maxcount)
        {
            *dst = 0;
            return count;
        }
    } while ((*dst++ = *src++));
    return count;
}

inline int _lStr_ncpy(lChar16 * dst, const lChar16 * src, int maxcount)
{
    int count = 0;
    do
    {
        if (++count > maxcount)
        {
            *dst = 0;
            return count;
        }
    } while ((*dst++ = *src++));
    return count;
}

inline int _lStr_ncpy(lChar16 * dst, const lChar8 * src, int maxcount)
{
    int count = 0;
    do
    {
        if (++count > maxcount)
        {
            *dst = 0;
            return count;
        }
    } while ((*dst++ = (unsigned char)*src++));
    return count;
}

inline int _lStr_ncpy(lChar32 * dst, const lChar8 * src, int maxcount)
{
    int count = 0;
    do
    {
        if (++count > maxcount)
        {
            *dst = 0;
            return count;
        }
    } while ((*dst++ = (unsigned char)*src++));
    return count;
}

inline int _lStr_ncpy(lChar8 * dst, const lChar8 * src, int maxcount)
{
    int count = 0;
    do
    {
        if (++count > maxcount)
        {
            *dst = 0;
            return count;
        }
    } while ((*dst++ = *src++));
    return count;
}

inline void _lStr_memcpy(lChar16 * dst, const lChar16 * src, int count)
{
    while ( count-- > 0)
        (*dst++ = *src++);
}

inline void _lStr_memcpy(lChar32 * dst, const lChar32 * src, int count)
{
    while ( count-- > 0)
        (*dst++ = *src++);
}

inline void _lStr_memcpy(lChar8 * dst, const lChar8 * src, int count)
{
    memcpy(dst, (const lChar8 *) src, count);
}

inline void _lStr_memset(lChar16 * dst, lChar16 value, int count)
{
    while ( count-- > 0)
        *dst++ = value;
}

inline void _lStr_memset(lChar32 * dst, lChar32 value, int count)
{
    while ( count-- > 0)
        *dst++ = value;
}

inline void _lStr_memset(lChar8 * dst, lChar8 value, int count)
{
    memset(dst, (lChar8) value, count);
}

int lStr_len(const lChar16 * str)
{
    return _lStr_len(str);
}

int lStr_len(const lChar32 * str)
{
    return _lStr_len(str);
}

int lStr_len(const lChar8 * str)
{
    return _lStr_len(str);
}

int lStr_nlen(const lChar16 * str, int maxcount)
{
    return _lStr_nlen(str, maxcount);
}

int lStr_nlen(const lChar32 * str, int maxcount)
{
    return _lStr_nlen(str, maxcount);
}

int lStr_nlen(const lChar8 * str, int maxcount)
{
    return _lStr_nlen(str, maxcount);
}

int lStr_cpy(lChar16 * dst, const lChar16 * src)
{
    return _lStr_cpy(dst, src);
}

int lStr_cpy(lChar32 * dst, const lChar32 * src)
{
    return _lStr_cpy(dst, src);
}

int lStr_cpy(lChar8 * dst, const lChar8 * src)
{
    return _lStr_cpy(dst, src);
}

int lStr_cpy(lChar16 * dst, const lChar8 * src)
{
    return _lStr_cpy(dst, src);
}

int lStr_cpy(lChar32 * dst, const lChar8 * src)
{
    return _lStr_cpy(dst, src);
}

int lStr_ncpy(lChar16 * dst, const lChar16 * src, int maxcount)
{
    return _lStr_ncpy(dst, src, maxcount);
}

int lStr_ncpy(lChar32 * dst, const lChar32 * src, int maxcount)
{
    return _lStr_ncpy(dst, src, maxcount);
}

int lStr_ncpy(lChar8 * dst, const lChar8 * src, int maxcount)
{
    return _lStr_ncpy(dst, src, maxcount);
}

void lStr_memcpy(lChar16 * dst, const lChar16 * src, int count)
{
    _lStr_memcpy(dst, src, count);
}

void lStr_memcpy(lChar32 * dst, const lChar32 * src, int count)
{
    _lStr_memcpy(dst, src, count);
}

void lStr_memcpy(lChar8 * dst, const lChar8 * src, int count)
{
    _lStr_memcpy(dst, src, count);
}

void lStr_memset(lChar16 * dst, lChar16 value, int count)
{
    _lStr_memset(dst, value, count);
}

void lStr_memset(lChar32 * dst, lChar32 value, int count)
{
    _lStr_memset(dst, value, count);
}

void lStr_memset(lChar8 * dst, lChar8 value, int count)
{
    _lStr_memset(dst, value, count);
}

int lStr_cmp(const lChar16 * dst, const lChar16 * src)
{
    if (dst == src)
        return 0;
    if (!dst)
        return -1;
    else if (!src)
        return 1;
    while ( *dst == *src)
    {
        if (! *dst )
            return 0;
        ++dst;
        ++src;
    }
    if ( *dst > *src )
        return 1;
    else
        return -1;
}

int lStr_cmp(const lChar32 * dst, const lChar32 * src)
{
    if (dst == src)
        return 0;
    if (!dst)
        return -1;
    else if (!src)
        return 1;
    while ( *dst == *src)
    {
        if (! *dst )
            return 0;
        ++dst;
        ++src;
    }
    if ( *dst > *src )
        return 1;
    else
        return -1;
}

int lStr_cmp(const lChar8 * dst, const lChar8 * src)
{
    if (dst == src)
        return 0;
    if (!dst)
        return -1;
    else if (!src)
        return 1;
    while ( *dst == *src)
    {
        if (! *dst )
            return 0;
        ++dst;
        ++src;
    }
    if ( *dst > *src )
        return 1;
    else
        return -1;
}

int lStr_cmp(const lChar16 * dst, const lChar8 * src)
{
    if (!dst && !src)
        return 0;
    if (!dst)
        return -1;
    else if (!src)
        return 1;
    while ( *dst == (lChar16)*src)
    {
        if (! *dst )
            return 0;
        ++dst;
        ++src;
    }
    if ( *dst > (lChar16)*src )
        return 1;
    else
        return -1;
}

int lStr_cmp(const lChar32 * dst, const lChar8 * src)
{
    if (!dst && !src)
        return 0;
    if (!dst)
        return -1;
    else if (!src)
        return 1;
    while ( *dst == (lChar32)*src)
    {
        if (! *dst )
            return 0;
        ++dst;
        ++src;
    }
    if ( *dst > (lChar32)*src )
        return 1;
    else
        return -1;
}

int lStr_cmp(const lChar8 * dst, const lChar16 * src)
{
    if (!dst && !src)
        return 0;
    if (!dst)
        return -1;
    else if (!src)
        return 1;
    while ( (lChar16)*dst == *src)
    {
        if (! *dst )
            return 0;
        ++dst;
        ++src;
    }
    if ( (lChar16)*dst > *src )
        return 1;
    else
        return -1;
}

int lStr_cmp(const lChar8 * dst, const lChar32 * src)
{
    if (!dst && !src)
        return 0;
    if (!dst)
        return -1;
    else if (!src)
        return 1;
    while ( (lChar32)*dst == *src)
    {
        if (! *dst )
            return 0;
        ++dst;
        ++src;
    }
    if ( (lChar32)*dst > *src )
        return 1;
    else
        return -1;
}

int lStr_cmp(const lChar32 * dst, const lChar16 * src) {
    if (!dst && !src)
        return 0;
    if (!dst)
        return -1;
    else if (!src)
        return 1;
    while ( *dst == (lChar32)*src)
    {
        if (! *dst )
            return 0;
        ++dst;
        ++src;
    }
    if ( *dst > (lChar32)*src )
        return 1;
    else
        return -1;
}

int lStr_cmp(const lChar16 * dst, const lChar32 * src)
{
    if (!dst && !src)
        return 0;
    if (!dst)
        return -1;
    else if (!src)
        return 1;
    while ( (lChar32)*dst == *src)
    {
        if (! *dst )
            return 0;
        ++dst;
        ++src;
    }
    if ( (lChar32)*dst > *src )
        return 1;
    else
        return -1;
}

bool isAlNum(lChar32 ch) {
    lUInt16 props = lGetCharProps(ch);
    return (props & (CH_PROP_ALPHA | CH_PROP_DIGIT)) != 0;
}


// returns 0..15 if c is hex digit, -1 otherwise
int hexDigit( int c )
{
    if ( c>='0' && c<='9')
        return c-'0';
    if ( c>='a' && c<='f')
        return c-'a'+10;
    if ( c>='A' && c<='F')
        return c-'A'+10;
    return -1;
}

// decode LEN hex digits, return decoded number, -1 if invalid
int decodeHex( const lChar32 * str, int len ) {
    int n = 0;
    for ( int i=0; i<len; i++ ) {
        if ( !str[i] )
            return -1;
        int d = hexDigit(str[i]);
        if ( d==-1 )
            return -1;
        n = (n<<4) | d;
    }
    return n;
}

// decode LEN decimal digits, return decoded number, -1 if invalid
int decodeDecimal( const lChar32 * str, int len ) {
    int n = 0;
    for ( int i=0; i<len; i++ ) {
        if ( !str[i] )
            return -1;
        int d = str[i] - '0';
        if ( d<0 || d>9 )
            return -1;
        n = n*10 + d;
    }
    return n;
}

lUInt32 calcStringHash( const lChar32 * s )
{
    lUInt32 a = 2166136261u;
    while (*s)
    {
        a = a * 16777619 ^ (*s++);
    }
    return a;
}

/// calculates CRC32 for buffer contents
lUInt32 lStr_crc32( lUInt32 prevValue, const void * buf, int size )
{
#if (USE_ZLIB==1)
    return crc32( prevValue, (const lUInt8 *)buf, size );
#else
    // TODO:
    return 0;
#endif
}

lUInt32 calcStringHash( const lChar16 * s )
{
    lUInt32 a = 2166136261u;
    while (*s)
    {
        a = a * 16777619 ^ (*s++);
    }
    return a;
}

bool lvUnicodeIsAlpha( lChar32 ch )
{
    if ( ch<128 ) {
        if ( (ch>='a' && ch<='z') || (ch>='A' && ch<='Z') )
            return true;
    } else if ( ch>=0xC0 && ch<=0x1ef9) {
        return true;
    }
    return false;
}

void lStr_uppercase( lChar8 * str, int len )
{
    for ( int i=0; i<len; i++ ) {
        lChar32 ch = str[i];
        if ( ch>='a' && ch<='z' ) {
            str[i] = ch - 0x20;
        } else if ( ch>=0xE0 && ch<=0xFF ) {
            str[i] = ch - 0x20;
        }
    }
}

void lStr_lowercase( lChar8 * str, int len )
{
    for ( int i=0; i<len; i++ ) {
        lChar32 ch = str[i];
        if ( ch>='A' && ch<='Z' ) {
            str[i] = ch + 0x20;
        } else if ( ch>=0xC0 && ch<=0xDF ) {
            str[i] = ch + 0x20;
        }
    }
}

void lStr_uppercase( lChar32 * str, int len )
{
    for ( int i=0; i<len; i++ ) {
        lChar32 ch = str[i];
#if (USE_UTF8PROC==1)
        str[i] = utf8proc_toupper(ch);
#else
        if ( ch>='a' && ch<='z' ) {
            str[i] = ch - 0x20;
        } else if ( ch>=0xE0 && ch<=0xFF ) {
            str[i] = ch - 0x20;
        } else if ( ch>=0x430 && ch<=0x44F ) {
            str[i] = ch - 0x20;
        } else if ( ch>=0x3b0 && ch<=0x3cF ) {
            str[i] = ch - 0x20;
        } else if ( (ch >> 8)==0x1F ) { // greek
            lChar32 n = ch & 255;
            if (n<0x70) {
                str[i] = ch | 8;
            } else if (n<0x80) {

            } else if (n<0xF0) {
                str[i] = ch | 8;
            }
        }
#endif
    }
}

void lStr_lowercase( lChar32 * str, int len )
{
    for ( int i=0; i<len; i++ ) {
        lChar32 ch = str[i];
#if (USE_UTF8PROC==1)
        str[i] = utf8proc_tolower(ch);
#else
        if ( ch>='A' && ch<='Z' ) {
            str[i] = ch + 0x20;
        } else if ( ch>=0xC0 && ch<=0xDF ) {
            str[i] = ch + 0x20;
        } else if ( ch>=0x410 && ch<=0x42F ) {
            str[i] = ch + 0x20;
        } else if ( ch>=0x390 && ch<=0x3aF ) {
            str[i] = ch + 0x20;
        } else if ( (ch >> 8)==0x1F ) { // greek
            lChar32 n = ch & 255;
            if (n<0x70) {
                str[i] = ch & (~8);
            } else if (n<0x80) {

            } else if (n<0xF0) {
                str[i] = ch & (~8);
            }
        }
#endif
    }
}

void lStr_fullWidthChars( lChar32 * str, int len )
{
    for ( int i=0; i<len; i++ ) {
        lChar32 ch = str[i];
        if ( ch>=0x21 && ch<=0x7E ) {
            // full-width versions of ascii chars 0x21-0x7E are at 0xFF01-0Xff5E
            str[i] = ch + UNICODE_ASCII_FULL_WIDTH_OFFSET;
        } else if ( ch==0x20 ) {
            str[i] = UNICODE_CJK_IDEOGRAPHIC_SPACE; // full-width space
        }
    }
}

void lStr_capitalize( lChar32 * str, int len )
{
    bool prev_is_word_sep = true; // first char of string will be capitalized
    for ( int i=0; i<len; i++ ) {
        lChar32 ch = str[i];
        if (prev_is_word_sep) {
            // as done as in lStr_uppercase()
#if (USE_UTF8PROC==1)
            str[i] = utf8proc_toupper(ch);
#else
            if ( ch>='a' && ch<='z' ) {
                str[i] = ch - 0x20;
            } else if ( ch>=0xE0 && ch<=0xFF ) {
                str[i] = ch - 0x20;
            } else if ( ch>=0x430 && ch<=0x44F ) {
                str[i] = ch - 0x20;
            } else if ( ch>=0x3b0 && ch<=0x3cF ) {
                str[i] = ch - 0x20;
            } else if ( (ch >> 8)==0x1F ) { // greek
                lChar32 n = ch & 255;
                if (n<0x70) {
                    str[i] = ch | 8;
                } else if (n<0x80) {

                } else if (n<0xF0) {
                    str[i] = ch | 8;
                }
            }
#endif
        }
        // update prev_is_word_sep for next char
        prev_is_word_sep = lStr_isWordSeparator(ch);
    }
}


int TrimDoubleSpaces(lChar32 * buf, int len,  bool allowStartSpace, bool allowEndSpace, bool removeEolHyphens)
{
    lChar32 * psrc = buf;
    lChar32 * pdst = buf;
    int state = 0; // 0=beginning, 1=after space, 2=after non-space
    while ((len--) > 0) {
        lChar32 ch = *psrc++;
        if (ch == ' ' || ch == '\t') {
            if ( state==2 ) {
                if ( *psrc || allowEndSpace ) // if not last
                    *pdst++ = ' ';
            } else if ( state==0 && allowStartSpace ) {
                *pdst++ = ' ';
            }
            state = 1;
        } else if ( ch=='\r' || ch=='\n' ) {
            if ( state==2 ) {
                if ( removeEolHyphens && pdst>(buf+1) && *(pdst-1)=='-' && lvUnicodeIsAlpha(*(pdst-2)) )
                    pdst--; // remove hyphen at end of line
                if ( *psrc || allowEndSpace ) // if not last
                    *pdst++ = ' ';
            } else if ( state==0 && allowStartSpace ) {
                *pdst++ = ' ';
            }
            state = 1;
        } else {
            *pdst++ = ch;
            state = 2;
        }
    }
    return (int)(pdst - buf);
}

int Utf8CharCount( const lChar8 * str )
{
    int count = 0;
    lUInt8 ch;
    while ( (ch=*str++) ) {
        if ( (ch & 0x80) == 0 ) {
        } else if ( (ch & 0xE0) == 0xC0 ) {
            if ( !(*str++) )
                break;
        } else if ( (ch & 0xF0) == 0xE0 ) {
            if ( !(*str++) )
                break;
            if ( !(*str++) )
                break;
        } else if ( (ch & 0xF8) == 0xF0 ) {
            if ( !(*str++) )
                break;
            if ( !(*str++) )
                break;
            if ( !(*str++) )
                break;
        } else {
            // In Unicode standard maximum length of UTF-8 sequence is 4 byte!
            // invalid first byte in UTF-8 sequence, just leave as is
            ;
        }
        count++;
    }
    return count;
}

int Utf8CharCount( const lChar8 * str, int len )
{
    if (len == 0)
        return 0;
    int count = 0;
    lUInt8 ch;
    const lChar8 * endp = str + len;
    while ((ch=*str++)) {
        if ( (ch & 0x80) == 0 ) {
        } else if ( (ch & 0xE0) == 0xC0 ) {
            str++;
        } else if ( (ch & 0xF0) == 0xE0 ) {
            str+=2;
        } else if ( (ch & 0xF8) == 0xF0 ) {
            str+=3;
        } else {
            // invalid first byte of UTF-8 sequence, just leave as is
            ;
        }
        if (str > endp)
            break;
        count++;
    }
    return count;
}

int Utf16CharCount( const lChar16 * str )
{
    int count = 0;
    lUInt16 ch;
    while ( (ch=*str++) ) {
        if ( (ch >=0 && ch <= 0xD7FF) || (ch >= 0xE000 && ch <= 0xFFFF) ) {
        } else if ( ch >= 0xD800 && ch <= 0xDBFF ) {
            if ( !(*str++) )
                break;
        } else {
            // In Unicode standard maximum length of UTF-16 sequence is 2 word!
            // invalid first word in UTF-16 sequence, just leave as is
            ;
        }
        count++;
    }
    return count;
}

int Utf16CharCount( const lChar16 * str, int len )
{
    if (len == 0)
        return 0;
    int count = 0;
    lUInt16 ch;
    const lChar16 * endp = str + len;
    while ( (ch=*str++) ) {
        if ( (ch >=0 && ch <= 0xD7FF) || (ch >= 0xE000 && ch <= 0xFFFF) ) {
        } else if ( ch >= 0xD800 && ch <= 0xDBFF ) {
            str++;
        } else {
            // invalid first word of UTF-16 sequence, just leave as is
            ;
        }
        if (str > endp)
            break;
        count++;
    }
    return count;
}

int Wtf8CharCount( const lChar8 * str )
{
    int count = 0;
    lUInt8 ch;
    lUInt32 p;
    while ( (ch=*str++) ) {
        if ( (ch & 0x80) == 0 ) {
        } else if ( (ch & 0xE0) == 0xC0 ) {
            if ( !(*str++) )
                break;
        } else if ( (ch & 0xF0) == 0xE0 ) {
            p = (ch & 0x0F) << 12;
            if ( !(ch=*str++) )
                break;
            p |= (ch & 0x3F) << 6;
            if ( !(ch=*str++) )
                break;
            p |= ch & 0x3F;
            if (p >= 0xD800 && p <= 0xDBFF) {           // high surrogate
                ch = *str;
                if ((ch & 0xF0) == 0xE0) {
                    p = (ch & 0x0F) << 12;
                    if ( !(ch=*(str+1)) )
                        break;
                    p |= (ch & 0x3F) << 6;
                    if ( !(ch=*(str+2)) )
                        break;
                    p |= ch & 0x3F;
                    if (p >= 0xDC00 && p <= 0xDFFF) {   // low surrogate
                        str += 3;
                    }
                }
            }
        } else if ( (ch & 0xF8) == 0xF0 ) {
            // Mostly unused
            if ( !(*str++) )
                break;
            if ( !(*str++) )
                break;
            if ( !(*str++) )
                break;
        } else {
            // invalid first byte in UTF-8 sequence, just leave as is
            ;
        }
        count++;
    }
    return count;
}

int Wtf8CharCount( const lChar8 * str, int len )
{
    if (len == 0)
        return 0;
    int count = 0;
    lUInt8 ch;
    const lChar8 * endp = str + len;
    while ((ch=*str)) {
        if ( (ch & 0x80) == 0 ) {
            str++;
        } else if ( (ch & 0xE0) == 0xC0 ) {
            str+=2;
        } else if ( (ch & 0xF0) == 0xE0 ) {
            str+=3;
            ch=*str;
            if ( (ch & 0xF0) == 0xE0 ) {
                str+=3;
            }
        } else if ( (ch & 0xF8) == 0xF0 ) {
            // Mostly unused
            str+=4;
        } else {
            // invalid first byte of UTF-8 sequence, just leave as is
            str++;
        }
        if (str > endp)
            break;
        count++;
    }
    return count;
}

inline int charUtf8ByteCount(lUInt32 ch) {
    if (!(ch & ~0x7F))
        return 1;
    if (!(ch & ~0x7FF))
        return 2;
    if (!(ch & ~0xFFFF))
        return 3;
    if (!(ch & ~0x1FFFFF))
        return 4;
    // In Unicode Standard codepoint must be in range U+0000..U+10FFFF
    // return invalid codepoint as one byte
    return 1;
}

inline int charUtf16WordCount(lUInt32 ch) {
    if (!(ch & ~0xFFFF))
        return 1;
    if (!(ch & ~0x1FFFFF))
        return 2;
    // In Unicode Standard codepoint must be in range U+0000..U+10FFFF
    // return invalid codepoint as one word
    return 1;
}

int Utf8ByteCount(const lChar32 * str)
{
    int count = 0;
    lUInt32 ch;
    while ( (ch=*str++) ) {
        count += charUtf8ByteCount(ch);
    }
    return count;
}

inline int charWtf8ByteCount(lUInt32 ch) {
    if (!(ch & ~0x7F))
        return 1;
    if (!(ch & ~0x7FF))
        return 2;
    if (!(ch & ~0xFFFF))
        return 3;
    if (!(ch & ~0x1FFFFF))
        return 6;
    return 1;
}

int Utf8ByteCount(const lChar32 * str, int len)
{
    int count = 0;
    lUInt32 ch;
    while ((len--) > 0) {
        ch = *str++;
        count += charUtf8ByteCount(ch);
    }
    return count;
}

int Utf16WordCount(const lChar32 * str, int len)
{
    int count = 0;
    lUInt32 ch;
    while ((len--) > 0) {
        ch = *str++;
        count += charUtf16WordCount(ch);
    }
    return count;
}

int Wtf8ByteCount(const lChar32 * str, int len)
{
    int count = 0;
    lUInt32 ch;
    while ((len--) > 0) {
        ch = *str++;
        count += charWtf8ByteCount(ch);
    }
    return count;
}

#define CONT_BYTE(index,shift) (((lChar32)(s[index]) & 0x3F) << shift)

static void DecodeUtf8(const char * s,  lChar32 * p, int len)
{
    lChar32 * endp = p + len;
    lUInt32 ch;
    while (p < endp) {
        ch = *s++;
        if ( (ch & 0x80) == 0 ) {
            *p++ = (char)ch;
        } else if ( (ch & 0xE0) == 0xC0 ) {
            *p++ = ((ch & 0x1F) << 6)
                    | CONT_BYTE(0,0);
            s++;
        } else if ( (ch & 0xF0) == 0xE0 ) {
            *p++ = ((ch & 0x0F) << 12)
                | CONT_BYTE(0,6)
                | CONT_BYTE(1,0);
            s += 2;
        } else if ( (ch & 0xF8) == 0xF0 ) {
            *p++ = ((ch & 0x07) << 18)
                | CONT_BYTE(0,12)
                | CONT_BYTE(1,6)
                | CONT_BYTE(2,0);
            s += 3;
        } else {
            // Invalid first byte in UTF-8 sequence
            // Pass with mask 0x7F, to resolve exception around env->NewStringUTF()
            *p++ = (char) (ch & 0x7F);
        }
    }
}

static void DecodeWtf8(const char * s,  lChar32 * p, int len)
{
    lChar32 * endp = p + len;
    lUInt32 ch;
    while (p < endp) {
        ch = *s;
        bool matched = false;
        if ( (ch & 0x80) == 0 ) {
            matched = true;
            *p++ = (char)ch;
            s++;
        } else if ( (ch & 0xE0) == 0xC0 ) {
            matched = true;
            *p++ = ((ch & 0x1F) << 6)
                    | CONT_BYTE(1,0);
            s += 2;
        } else if ( (ch & 0xF0) == 0xE0 ) {
            matched = true;
            *p++ = ((ch & 0x0F) << 12)
                | CONT_BYTE(1,6)
                | CONT_BYTE(2,0);
            s += 3;
            if (*(p-1) >= 0xD800 && *(p-1) <= 0xDBFF) {     // what we wrote is a high surrogate,
                lUInt32 next = *s;                          // and there's room next for a low surrogate
                if ( (next & 0xF0) == 0xE0) {               // is a 3-bytes sequence
                    next = ((next & 0x0F) << 12) | CONT_BYTE(1,6) | CONT_BYTE(2,0);
                    if (next >= 0xDC00 && next <= 0xDFFF) { // is a low surrogate: valid surrogates sequence
                        ch = 0x10000 + ((*(p-1) & 0x3FF)<<10) + (next & 0x3FF);
                        p--; // rewind to override what we wrote
                        *p++ = ch;
                        s += 3;
                    }
                }
            }
        } else if ( (ch & 0xF8) == 0xF0 ) {
            // Mostly unused
            matched = true;
            *p++ = ((ch & 0x07) << 18)
                | CONT_BYTE(1,12)
                | CONT_BYTE(2,6)
                | CONT_BYTE(3,0);
            s += 4;
        } else {
            // Invalid first byte in UTF-8 sequence
            // Pass with mask 0x7F, to resolve exception around env->NewStringUTF()
            *p++ = (char) (ch & 0x7F);
            s++;
            matched = true; // just to avoid next if
        }

        // unexpected character
        if (!matched) {
            *p++ = '?';
            s++;
        }
    }
}

static void DecodeUtf16(const lChar16 * s,  lChar32 * p, int len)
{
    lChar32 * endp = p + len;
    lUInt16 ch;
    while (p < endp) {
        ch = *s++;
        if ( (ch >=0 && ch <= 0xD7FF) || (ch >= 0xE000 && ch <= 0xFFFF) ) {
            *p++ = (lChar32)ch;
        } else if ( ch >= 0xD800 && ch < 0xDC00 ) {
            lUInt16 next = (lUInt16)*s;
            if (next >= 0xDC00 && next < 0xE000) {
                // convert surrogate pair into unicode code point
                // 110110wwwwxxxxxx, 110111xxxxxxxxxx => 000uuuuuxxxxxxxxxxxxxxxx
                //  where uuuuu = wwww+1
                *p++ = ( ( ( (ch & 0x03C0) >> 6 ) + 1 ) << 16 ) | ((ch & 0x3F) << 10) | (next & 0x3FF);
            } else {
                // Invalid second word in UTF-16 sequence (including '\0')
                // Pass with mask 0x7F, to resolve exception around env->NewStringUTF()
                *p++ = (char) (ch & 0x7F);
            }
            s++;
        } else {
            // Invalid first word in UTF-16 sequence
            // Pass with mask 0x7F, to resolve exception around env->NewStringUTF()
            *p++ = (char) (ch & 0x7F);
        }
    }
}

// Top two bits are 10, i.e. original & 11000000(2) == 10000000(2)
#define IS_FOLLOWING(index) ((s[index] & 0xC0) == 0x80)

void Utf8ToUnicode(const lUInt8 * src,  int &srclen, lChar32 * dst, int &dstlen)
{
    const lUInt8 * s = src;
    const lUInt8 * ends = s + srclen;
    lChar32 * p = dst;
    lChar32 * endp = p + dstlen;
    lUInt32 ch;
    bool matched;
    while (p < endp && s < ends) {
        ch = *s;
        matched = false;
        if ( (ch & 0x80) == 0 ) {
            matched = true;
            *p++ = (char)ch;
            s++;
        } else if ( (ch & 0xE0) == 0xC0 ) {
            if (s + 2 > ends)
                break;
            if (IS_FOLLOWING(1)) {
                matched = true;
                *p++ = ((ch & 0x1F) << 6)
                        | CONT_BYTE(1,0);
                s += 2;
            }
        } else if ( (ch & 0xF0) == 0xE0 ) {
            if (s + 3 > ends)
                break;
            if (IS_FOLLOWING(1) && IS_FOLLOWING(2)) {
                matched = true;
                *p++ = ((ch & 0x0F) << 12)
                    | CONT_BYTE(1,6)
                    | CONT_BYTE(2,0);
                s += 3;
                // Supports WTF-8 : https://en.wikipedia.org/wiki/UTF-8#WTF-8
                // a superset of UTF-8, that includes UTF-16 surrogates
                // in UTF-8 bytes (forbidden in well-formed UTF-8).
                // We may get that from bad producers or converters.
                // As these shouldn't be there in UTF-8, if we find
                // these surrogates in the right sequence, we might as well
                // convert the char they represent to the right Unicode
                // codepoint and display it instead of a '?'.
                //   Surrogates are code points from two special ranges of
                //   Unicode values, reserved for use as the leading, and
                //   trailing values of paired code units in UTF-16. Leading,
                //   also called high, surrogates are from D800 to DBFF, and
                //   trailing, or low, surrogates are from DC00 to DFFF. They
                //   are called surrogates, since they do not represent
                //   characters directly, but only as a pair.
                if (*(p-1) >= 0xD800 && *(p-1) <= 0xDBFF && s+2 < ends) { // what we wrote is a high surrogate,
                    lUInt32 next = *s;                            // and there's room next for a low surrogate
                    if ( (next & 0xF0) == 0xE0 && IS_FOLLOWING(1) && IS_FOLLOWING(2)) { // is a valid 3-bytes sequence
                        next = ((next & 0x0F) << 12) | CONT_BYTE(1,6) | CONT_BYTE(2,0);
                        if (next >= 0xDC00 && next <= 0xDFFF) { // is a low surrogate: valid surrogates sequence
                            ch = 0x10000 + ((*(p-1) & 0x3FF)<<10) + (next & 0x3FF);
                            p--; // rewind to override what we wrote
                            *p++ = ch;
                            s += 3;
                        }
                    }
                }
            }
        } else if ( (ch & 0xF8) == 0xF0 ) {
            if (s + 4 > ends)
                break;
            if (IS_FOLLOWING(1) && IS_FOLLOWING(2) && IS_FOLLOWING(3)) {
                matched = true;
                *p++ = ((ch & 0x07) << 18)
                    | CONT_BYTE(1,12)
                    | CONT_BYTE(2,6)
                    | CONT_BYTE(3,0);
                s += 4;
            }
        } else {
            // Invalid first byte in UTF-8 sequence
            // Pass with mask 0x7F, to resolve exception around env->NewStringUTF()
            *p++ = (char) (ch & 0x7F);
            s++;
            matched = true; // just to avoid next if
        }
        // unexpected character
        if (!matched) {
            *p++ = '?';
            s++;
        }
    }
    srclen = (int)(s - src);
    dstlen = (int)(p - dst);
}

void Utf16ToUnicode(const lChar16 * src,  int &srclen, lChar32 * dst, int &dstlen)
{
    const lChar16 * s = src;
    const lChar16 * ends = s + srclen;
    lChar32 * p = dst;
    lChar32 * endp = p + dstlen;
    lUInt32 ch;
    bool matched;
    while (p < endp && s < ends) {
        ch = *s;
        matched = false;
        if ( (ch >=0 && ch <= 0xD7FF) || (ch >= 0xE000 && ch <= 0xFFFF) ) {
            matched = true;
            *p++ = (lChar32)ch;
            s++;
        } else if ( ch >= 0xD800 && ch < 0xDC00 ) {
            if (s + 2 > ends)
                break;
            lUInt16 next = *s;
            if (next >= 0xDC00 && next < 0xE000) {
                matched = true;
                // convert surrogate pair into unicode code point
                // 110110wwwwxxxxxx, 110111xxxxxxxxxx => 000uuuuuxxxxxxxxxxxxxxxx
                //  where uuuuu = wwww+1
                *p++ = ( ( ( (ch & 0x03C0) >> 6 ) + 1 ) << 16 ) | ((ch & 0x3F) << 10) | (next & 0x3FF);
                s += 2;
            }
        } else {
            // Invalid first word in UTF-16 sequence
            // Pass with mask 0x7F, to resolve exception around env->NewStringUTF()
            *p++ = (char) (ch & 0x7F);
            s++;
            matched = true; // just to avoid next if
        }
        // unexpected character
        if (!matched) {
            *p++ = '?';
            s++;
        }
    }
    srclen = (int)(s - src);
    dstlen = (int)(p - dst);
}

lString32 Wtf8ToUnicode( const lString8 & str )
{
    return Wtf8ToUnicode( str.c_str() );
}

lString32 Wtf8ToUnicode( const char * s ) {
    if (!s || !s[0])
      return {};
    int len = Wtf8CharCount( s );
    if (!len)
      return {};
    lString32 dst(len, 0);
    lChar32 * p = dst.data();
    DecodeWtf8(s, p, len);
    return dst;
}

lString32 Wtf8ToUnicode( const char * s, int sz ) {
    if (!s || !s[0] || sz <= 0)
      return {};
    int len = Utf8CharCount( s, sz );
    if (!len)
      return {};
    lString32 dst(len, 0);
    lChar32 * p = dst.data();
    DecodeWtf8(s, p, len);
    return dst;
}

lString8 UnicodeToWtf8(const lChar32 * s, int count)
{
    if (count <= 0)
      return {};
    int len = Wtf8ByteCount(s, count);
    if (len <= 0)
      return {};
    lString8 dst(len, 0);
    lChar8 * buf = dst.data();
    {
        lUInt32 ch;
        while ((count--) > 0) {
            ch = *s++;
            if (!(ch & ~0x7F)) {
                *buf++ = ( (lUInt8)ch );
            } else if (!(ch & ~0x7FF)) {
                *buf++ = ( (lUInt8) ( ((ch >> 6) & 0x1F) | 0xC0 ) );
                *buf++ = ( (lUInt8) ( ((ch ) & 0x3F) | 0x80 ) );
            } else if (!(ch & ~0xFFFF)) {
                *buf++ = ( (lUInt8) ( ((ch >> 12) & 0x0F) | 0xE0 ) );
                *buf++ = ( (lUInt8) ( ((ch >> 6) & 0x3F) | 0x80 ) );
                *buf++ = ( (lUInt8) ( ((ch ) & 0x3F) | 0x80 ) );
            } else if (!(ch & ~0x1FFFFF)) {
                //   UTF-16 Scalar Value
                // 000uuuuu xxxxxxxxxxxxxxxx
                //   UTF-16
                // 110110wwwwxxxxxx 110111xxxxxxxxxx
                // wwww = uuuuu - 1
                lUInt16 wwww = (ch >> 16) - 1;
                lUInt16 low = ch & 0xFFFF;
                lUInt32 hiSurr = 0xD800 | (wwww << 6) | (low >> 10);    // high surrogate
                lUInt32 lowSurr = 0xDC00 | (low & 0x3FF);               // low surrogate
                *buf++ = ( (lUInt8) ( ((hiSurr >> 12) & 0x0F) | 0xE0 ) );
                *buf++ = ( (lUInt8) ( ((hiSurr >> 6) & 0x3F) | 0x80 ) );
                *buf++ = ( (lUInt8) ( ((hiSurr ) & 0x3F) | 0x80 ) );
                *buf++ = ( (lUInt8) ( ((lowSurr >> 12) & 0x0F) | 0xE0 ) );
                *buf++ = ( (lUInt8) ( ((lowSurr >> 6) & 0x3F) | 0x80 ) );
                *buf++ = ( (lUInt8) ( ((lowSurr ) & 0x3F) | 0x80 ) );
            } else {
                // invalid codepoint
                // In Unicode Standard codepoint must be in range U+0000 .. U+10FFFF
                *buf++ = '?';
            }
        }
    }
    return dst;
}

lString8 UnicodeToWtf8( const lString32 & str )
{
    return UnicodeToWtf8(str.c_str(), str.length());
}

lString8 UnicodeTo8Bit( const lString32 & str, const lChar8 * * table )
{
    lString8 buf;
    buf.reserve( str.length() );
    for (int i=0; i < str.length(); i++) {
        lChar32 ch = str[i];
        const lChar8 * p = table[ (ch>>8) & 255 ];
        if ( p ) {
            buf += p[ ch&255 ];
        } else {
            buf += '?';
        }
    }
    return buf;
}

lString32 ByteToUnicode( const lString8 & str, const lChar32 * table )
{
    lString32 buf;
    buf.reserve( str.length() );
    for (int i=0; i < str.length(); i++) {
        lChar32 ch = (unsigned char)str[i];
        lChar32 ch32 = ((ch & 0x80) && table) ? table[ (ch&0x7F) ] : ch;
        buf += ch32;
    }
    return buf;
}


#if !defined(__SYMBIAN32__) && defined(_WIN32)

lString8 UnicodeToLocal( const lString32 & str )
{
   if (str.empty())
      return {};
   lString16 utf16 = UnicodeToUtf16(str);
   CHAR def_char = '?';
   BOOL usedDefChar = FALSE;
   int len = WideCharToMultiByte(
      CP_ACP,
      WC_COMPOSITECHECK | WC_DISCARDNS
       | WC_SEPCHARS | WC_DEFAULTCHAR,
      utf16.c_str(),
      utf16.length(),
      NULL,
      0,
      &def_char,
      &usedDefChar
      );
   if (len)
   {
      lString8 dst(len, 0);
      WideCharToMultiByte(
         CP_ACP,
         WC_COMPOSITECHECK | WC_DISCARDNS
          | WC_SEPCHARS | WC_DEFAULTCHAR,
         utf16.c_str(),
         utf16.length(),
         dst.data(),
         len,
         &def_char,
         &usedDefChar
         );
   }
   return {};
}

lString32 LocalToUnicode( const lString8 & str )
{
   if (str.empty())
      return lString32::empty_str;
   int len = MultiByteToWideChar(
      CP_ACP,
      0,
      str.c_str(),
      str.length(),
      NULL,
      0
      );
   if (len)
   {
      lString16 utf16(len, 0)
      MultiByteToWideChar(
         CP_ACP,
         0,
         str.c_str(),
         str.length(),
         utf16.data(),
         len
         );
   }
   return {};
}

#else

lString8 UnicodeToLocal( const lString32 & str )
{
    return UnicodeToUtf8( str );
}

lString32 LocalToUnicode( const lString8 & str )
{
    return Utf8ToUnicode( str );
}

#endif

//0x410
static const char * russian_capital[32] =
{
"A", "B", "V", "G", "D", "E", "ZH", "Z", "I", "j", "K", "L", "M", "N", "O", "P", "R",
"S", "T", "U", "F", "H", "TS", "CH", "SH", "SH", "\'", "Y", "\'", "E", "YU", "YA"
};
static const char * russian_small[32] =
{
"a", "b", "v", "g", "d", "e", "zh", "z", "i", "j", "k", "l", "m", "n", "o", "p", "r",
"s", "t", "u", "f", "h", "ts", "ch", "sh", "sh", "\'", "y", "\'", "e", "yu", "ya"
};

static const char * latin_1[64] =
{
"A", // U+00C0	LATIN CAPITAL LETTER A WITH GRAVE
"A", // U+00C1	LATIN CAPITAL LETTER A WITH ACUTE
"A", // U+00C2	LATIN CAPITAL LETTER A WITH CIRCUMFLEX
"A", // U+00C3	LATIN CAPITAL LETTER A WITH TILDE
"AE",// U+00C4	LATIN CAPITAL LETTER A WITH DIAERESIS
"A", // U+00C5	LATIN CAPITAL LETTER A WITH RING ABOVE
"AE",// U+00C6	LATIN CAPITAL LETTER AE
"C", // U+00C7	LATIN CAPITAL LETTER C WITH CEDILLA
"E", // U+00C8	LATIN CAPITAL LETTER E WITH GRAVE
"E", // U+00C9	LATIN CAPITAL LETTER E WITH ACUTE
"E", // U+00CA	LATIN CAPITAL LETTER E WITH CIRCUMFLEX
"E", // U+00CB	LATIN CAPITAL LETTER E WITH DIAERESIS
"I", // U+00CC	LATIN CAPITAL LETTER I WITH GRAVE
"I", // U+00CD	LATIN CAPITAL LETTER I WITH ACUTE
"I", // U+00CE	LATIN CAPITAL LETTER I WITH CIRCUMFLEX
"I", // U+00CF	LATIN CAPITAL LETTER I WITH DIAERESIS
"D", // U+00D0	LATIN CAPITAL LETTER ETH
"N", // U+00D1	LATIN CAPITAL LETTER N WITH TILDE
"O", // U+00D2	LATIN CAPITAL LETTER O WITH GRAVE
"O", // U+00D3	LATIN CAPITAL LETTER O WITH ACUTE
"O", // U+00D4	LATIN CAPITAL LETTER O WITH CIRCUMFLEX
"O", // U+00D5	LATIN CAPITAL LETTER O WITH TILDE
"OE",// U+00D6	LATIN CAPITAL LETTER O WITH DIAERESIS
"x", // U+00D7	MULTIPLICATION SIGN
"O", // U+00D8	LATIN CAPITAL LETTER O WITH STROKE
"U", // U+00D9	LATIN CAPITAL LETTER U WITH GRAVE
"U", // U+00DA	LATIN CAPITAL LETTER U WITH ACUTE
"U", // U+00DB	LATIN CAPITAL LETTER U WITH CIRCUMFLEX
"UE",// U+00DC	LATIN CAPITAL LETTER U WITH DIAERESIS
"Y", // U+00DD	LATIN CAPITAL LETTER Y WITH ACUTE
"p", // U+00DE	LATIN CAPITAL LETTER THORN
"SS",// U+00DF	LATIN SMALL LETTER SHARP S
"a", // U+00E0	LATIN SMALL LETTER A WITH GRAVE
"a", // U+00E1	LATIN SMALL LETTER A WITH ACUTE
"a", // U+00E2	LATIN SMALL LETTER A WITH CIRCUMFLEX
"a", // U+00E3	LATIN SMALL LETTER A WITH TILDE
"ae",// U+00E4	LATIN SMALL LETTER A WITH DIAERESIS
"a", // U+00E5	LATIN SMALL LETTER A WITH RING ABOVE
"ae",// U+00E6	LATIN SMALL LETTER AE
"c", // U+00E7	LATIN SMALL LETTER C WITH CEDILLA
"e", // U+00E8	LATIN SMALL LETTER E WITH GRAVE
"e", // U+00E9	LATIN SMALL LETTER E WITH ACUTE
"e", // U+00EA	LATIN SMALL LETTER E WITH CIRCUMFLEX
"e", // U+00EB	LATIN SMALL LETTER E WITH DIAERESIS
"i", // U+00EC	LATIN SMALL LETTER I WITH GRAVE
"i", // U+00ED	LATIN SMALL LETTER I WITH ACUTE
"i", // U+00EE	LATIN SMALL LETTER I WITH CIRCUMFLEX
"i", // U+00EF	LATIN SMALL LETTER I WITH DIAERESIS
"d", // U+00F0	LATIN SMALL LETTER ETH
"n", // U+00F1	LATIN SMALL LETTER N WITH TILDE
"o", // U+00F2	LATIN SMALL LETTER O WITH GRAVE
"o", // U+00F3	LATIN SMALL LETTER O WITH ACUTE
"o", // U+00F4	LATIN SMALL LETTER O WITH CIRCUMFLEX
"oe",// U+00F5	LATIN SMALL LETTER O WITH TILDE
"o", // U+00F6	LATIN SMALL LETTER O WITH DIAERESIS
"x", // U+00F7	DIVISION SIGN
"o", // U+00F8	LATIN SMALL LETTER O WITH STROKE
"u", // U+00F9	LATIN SMALL LETTER U WITH GRAVE
"u", // U+00FA	LATIN SMALL LETTER U WITH ACUTE
"u", // U+00FB	LATIN SMALL LETTER U WITH CIRCUMFLEX
"ue",// U+00FC	LATIN SMALL LETTER U WITH DIAERESIS
"y", // U+00FD	LATIN SMALL LETTER Y WITH ACUTE
"p", // U+00FE	LATIN SMALL LETTER THORN
"y", // U+00FF	LATIN SMALL LETTER Y WITH DIAERESIS
};

static const char * getCharTranscript( lChar32 ch )
{
    if ( ch>=0x410 && ch<0x430 )
        return russian_capital[ch-0x410];
    else if (ch>=0x430 && ch<0x450)
        return russian_small[ch-0x430];
    else if (ch>=0xC0 && ch<0xFF)
        return latin_1[ch-0xC0];
    else if (ch==0x450)
        return "E";
    else if ( ch==0x451 )
        return "e";
    return "?";
}

lString8  UnicodeToTranslit( const lString32 & str )
{
    lString8 buf;
    if ( str.empty() )
        return buf;
    buf.reserve( str.length()*5/4 );
    for ( int i=0; i<str.length(); i++ ) {
        lChar32 ch = str[i];
        if ( ch>=32 && ch<=127 ) {
            buf.append( 1, (lChar8)ch );
        } else {
            const char * trans = getCharTranscript(ch);
            buf.append( trans );
        }
    }
    return buf;
}


// Note:
// CH_PROP_UPPER and CH_PROP_LOWER make out CH_PROP_ALPHA, which is,
// with CH_PROP_CONSONANT, CH_PROP_VOWEL and CH_PROP_ALPHA_SIGN,
// used only for detecting a word candidate to hyphenation.
// CH_PROP_PUNCT and CH_PROP_DASH are used each once in some obscure places.
// Others seem not used anywhere: CH_PROP_SIGN, CH_PROP_DIGIT, CH_PROP_SPACE
static lUInt16 char_props[] = {
// 0x0000:
0,0,0,0, 0,0,0,0, CH_PROP_SPACE,CH_PROP_SPACE,CH_PROP_SPACE,0, CH_PROP_SPACE,CH_PROP_SPACE,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x0020:
CH_PROP_SPACE, // ' '
CH_PROP_PUNCT | CH_PROP_AVOID_WRAP_BEFORE, // '!'
0, // '\"'
CH_PROP_SIGN, // '#'
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_BEFORE | CH_PROP_AVOID_WRAP_AFTER, // '$'
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_BEFORE, // '%'
CH_PROP_SIGN, // '&'
CH_PROP_SIGN, // '\''
CH_PROP_AVOID_WRAP_AFTER, // '('
CH_PROP_AVOID_WRAP_BEFORE, // ')'
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_BEFORE | CH_PROP_AVOID_WRAP_AFTER, // '*'
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_BEFORE | CH_PROP_AVOID_WRAP_AFTER, // '+'
CH_PROP_PUNCT | CH_PROP_AVOID_WRAP_BEFORE, // ','
CH_PROP_SIGN | CH_PROP_DASH | CH_PROP_AVOID_WRAP_BEFORE, // '-'
CH_PROP_PUNCT | CH_PROP_AVOID_WRAP_BEFORE, // '.'
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_BEFORE, // '/'
// 0x0030:
CH_PROP_DIGIT, // '0'
CH_PROP_DIGIT, // '1'
CH_PROP_DIGIT, // '2'
CH_PROP_DIGIT, // '3'
CH_PROP_DIGIT, // '4'
CH_PROP_DIGIT, // '5'
CH_PROP_DIGIT, // '6'
CH_PROP_DIGIT, // '7'
CH_PROP_DIGIT, // '8'
CH_PROP_DIGIT, // '9'
CH_PROP_PUNCT | CH_PROP_AVOID_WRAP_BEFORE, // ':'
CH_PROP_PUNCT | CH_PROP_AVOID_WRAP_BEFORE, // ';'
CH_PROP_SIGN  | CH_PROP_AVOID_WRAP_BEFORE | CH_PROP_AVOID_WRAP_AFTER,  // '<'
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_BEFORE | CH_PROP_AVOID_WRAP_AFTER,  // '='
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_BEFORE | CH_PROP_AVOID_WRAP_AFTER,  // '>'
CH_PROP_PUNCT | CH_PROP_AVOID_WRAP_BEFORE, // '?'
// 0x0040:
CH_PROP_SIGN,  // '@'
CH_PROP_UPPER | CH_PROP_VOWEL,     // 'A'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'B'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'C'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'D'
CH_PROP_UPPER | CH_PROP_VOWEL, // 'E'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'F'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'G'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'H'
CH_PROP_UPPER | CH_PROP_VOWEL, // 'I'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'J'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'K'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'L'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'M'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'N'
CH_PROP_UPPER | CH_PROP_VOWEL, // 'O'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'P'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'Q'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'R'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'S'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'T'
CH_PROP_UPPER | CH_PROP_VOWEL, // 'U'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'V'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'W'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'X'
CH_PROP_UPPER | CH_PROP_VOWEL, // 'Y'
CH_PROP_UPPER | CH_PROP_CONSONANT, // 'Z'
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_AFTER, // '['
CH_PROP_SIGN, // '\'
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_BEFORE, // ']'
CH_PROP_SIGN, // '^'
CH_PROP_SIGN, // '_'
// 0x0060:
CH_PROP_SIGN,  // '`'
CH_PROP_LOWER | CH_PROP_VOWEL,     // 'a'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'b'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'c'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'd'
CH_PROP_LOWER | CH_PROP_VOWEL, // 'e'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'f'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'g'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'h'
CH_PROP_LOWER | CH_PROP_VOWEL, // 'i'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'j'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'k'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'l'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'm'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'n'
CH_PROP_LOWER | CH_PROP_VOWEL, // 'o'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'p'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'q'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'r'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 's'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 't'
CH_PROP_LOWER | CH_PROP_VOWEL, // 'u'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'v'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'w'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'x'
CH_PROP_LOWER | CH_PROP_VOWEL, // 'y'
CH_PROP_LOWER | CH_PROP_CONSONANT, // 'z'
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_AFTER, // '{'
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_BEFORE | CH_PROP_AVOID_WRAP_AFTER, // '|'
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_BEFORE, // '}'
CH_PROP_SIGN, // '~'
CH_PROP_SIGN, // ' '
// 0x0080:
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x0090:
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x00A0:
CH_PROP_SPACE, // 00A0 nbsp
CH_PROP_PUNCT, // 00A1 inverted !
CH_PROP_SIGN,  // 00A2
CH_PROP_SIGN,  // 00A3
CH_PROP_SIGN,  // 00A4
CH_PROP_SIGN,  // 00A5
CH_PROP_SIGN,  // 00A6
CH_PROP_SIGN,  // 00A7
CH_PROP_SIGN,  // 00A8
CH_PROP_SIGN,  // 00A9
CH_PROP_SIGN,  // 00AA
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_AFTER,  // 00AB «
CH_PROP_SIGN,  // 00AC
CH_PROP_HYPHEN,// 00AD soft-hyphen (UNICODE_SOFT_HYPHEN_CODE)
CH_PROP_SIGN,  // 00AE
CH_PROP_SIGN,  // 00AF
// 0x00A0:
CH_PROP_SIGN,  // 00B0 degree
CH_PROP_SIGN,  // 00B1
CH_PROP_SIGN,  // 00B2
CH_PROP_SIGN,  // 00B3
CH_PROP_SIGN,  // 00B4
CH_PROP_SIGN,  // 00B5
CH_PROP_SIGN,  // 00B6
CH_PROP_SIGN,  // 00B7
CH_PROP_SIGN,  // 00B8
CH_PROP_SIGN,  // 00B9
CH_PROP_SIGN,  // 00BA
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_BEFORE,  // 00BB »
CH_PROP_SIGN,  // 00BC
CH_PROP_SIGN,  // 00BD
CH_PROP_SIGN,  // 00BE
CH_PROP_PUNCT, // 00BF
// 0x00C0:
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00C0 A`
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00C1 A'
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00C2 A^
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00C3 A"
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00C4 A:
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00C5 Ao
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00C6 AE
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 00C7 C~
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00C8 E`
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00C9 E'
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00CA E^
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00CB E:
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00CC I`
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00CD I'
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00CE I^
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00CF I:
// 0x00D0:
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 00D0 D-
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 00D1 N-
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00D2 O`
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00D3 O'
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00D4 O^
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00D5 O"
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00D6 O:
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_BEFORE | CH_PROP_AVOID_WRAP_AFTER,  // 00D7 x (multiplication sign)
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00D8 O/
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00D9 U`
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00DA U'
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00DB U^
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00DC U:
CH_PROP_UPPER | CH_PROP_VOWEL,  // 00DD Y'
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 00DE P thorn
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 00DF ss
// 0x00E0:
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00E0 a`
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00E1 a'
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00E2 a^
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00E3 a"
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00E4 a:
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00E5 ao
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00E6 ae
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 00E7 c~
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00E8 e`
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00E9 e'
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00EA e^
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00EB e:
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00EC i`
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00ED i'
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00EE i^
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00EF i:
// 0x00F0:
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 00F0 eth
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 00F1 n~
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00F2 o`
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00F3 o'
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00F4 o^
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00F5 o"
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00F6 o:
CH_PROP_SIGN | CH_PROP_AVOID_WRAP_BEFORE | CH_PROP_AVOID_WRAP_AFTER,  // 00F7 (division sign %)
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00F8 o/
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00F9 u`
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00FA u'
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00FB u^
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00FC u:
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00FD y'
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 00FE p thorn
CH_PROP_LOWER | CH_PROP_VOWEL,  // 00FF y:
// 0x0100:
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0100 A_
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0101 a_
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0102 Au
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0103 au
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0104 A,
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0105 a,
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0106 C'
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0107 c'
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0108 C^
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0109 c^
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 010A C.
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 010B c.
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 010C Cu
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 010D cu
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 010E Du
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 010F d'

CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0110 D-
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0111 d-
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0112 E_
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0113 e_
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0114 Eu
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0115 eu
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0116 E.
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0117 e.
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0118 E,
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0119 e,
CH_PROP_UPPER | CH_PROP_VOWEL,  // 011A Ev
CH_PROP_LOWER | CH_PROP_VOWEL,  // 011B ev
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 011C G^
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 011D g^
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 011E Gu
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 011F Gu

CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0120 G.
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0121 g.
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0122 G,
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0123 g,
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0124 H^
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0125 h^
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0126 H-
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0127 h-
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0128 I~
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0129 i~
CH_PROP_UPPER | CH_PROP_VOWEL,  // 012A I_
CH_PROP_LOWER | CH_PROP_VOWEL,  // 012B i_
CH_PROP_UPPER | CH_PROP_VOWEL,  // 012C Iu
CH_PROP_LOWER | CH_PROP_VOWEL,  // 012D iu
CH_PROP_UPPER | CH_PROP_VOWEL,  // 012E I,
CH_PROP_LOWER | CH_PROP_VOWEL,  // 012F i,

CH_PROP_UPPER | CH_PROP_VOWEL,  // 0130 I.
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0131 i-.
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0132 IJ
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0133 ij
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0134 J^
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0135 j^
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0136 K,
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0137 k,
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0138 k (kra)
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0139 L'
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 013A l'
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 013B L,
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 013C l,
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 013D L'
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 013E l'
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 013F L.

CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0140 l.
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0141 L/
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0142 l/
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0143 N'
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0144 n'
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0145 N,
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0146 n,
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0147 Nv
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0148 nv
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0149 `n
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 014A Ng
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 014B ng
CH_PROP_UPPER | CH_PROP_VOWEL,  // 014C O_
CH_PROP_LOWER | CH_PROP_VOWEL,  // 014D o-.
CH_PROP_UPPER | CH_PROP_VOWEL,  // 014E Ou
CH_PROP_LOWER | CH_PROP_VOWEL,  // 014F ou

CH_PROP_UPPER | CH_PROP_VOWEL,  // 0150 O"
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0151 o"
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0152 Oe
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0153 oe
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0154 R'
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0155 r'
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0156 R,
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0157 r,
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0158 Rv
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0159 rv
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 015A S'
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 015B s'
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 015C S^
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 015D s^
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 015E S,
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 015F s,

CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0160 Sv
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0161 sv
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0162 T,
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0163 T,
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0164 Tv
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0165 Tv
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0166 T-
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0167 T-
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0168 U~
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0169 u~
CH_PROP_UPPER | CH_PROP_VOWEL,  // 016A U_
CH_PROP_LOWER | CH_PROP_VOWEL,  // 016B u_
CH_PROP_UPPER | CH_PROP_VOWEL,  // 016C Uu
CH_PROP_LOWER | CH_PROP_VOWEL,  // 016D uu
CH_PROP_UPPER | CH_PROP_VOWEL,  // 016E Uo
CH_PROP_LOWER | CH_PROP_VOWEL,  // 016F uo

CH_PROP_UPPER | CH_PROP_VOWEL,  // 0170 U"
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0171 u"
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0172 U,
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0173 u,
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0174 W^
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0175 w^
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0176 Y,
CH_PROP_LOWER | CH_PROP_VOWEL,  // 0177 y,
CH_PROP_UPPER | CH_PROP_VOWEL,  // 0178 Y:
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0179 Z'
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 017A z'
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 017B Z.
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 017C z.
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 017D Zv
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 017E zv
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 017F s long
// 0x0180:
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x0190:
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x01A0:
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x01B0:
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x01C0:
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x01D0:
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x01E0:
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x01F0:
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x0200:
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x0300:
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x0380:
0,0,0,0,
CH_PROP_VOWEL, //    GREEK TONOS 	0384
CH_PROP_VOWEL, //    GREEK DIALYTIKA TONOS 	0385
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER ALPHA WITH TONOS 	0386
CH_PROP_UPPER | CH_PROP_PUNCT, //    GREEK ANO TELEIA 	0387
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER EPSILON WITH TONOS 	0388
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER ETA WITH TONOS 	0389
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER IOTA WITH TONOS 	038A
0,//038b
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER OMICRON WITH TONOS 	038C
0,//038d
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER UPSILON WITH TONOS 	038E
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER OMEGA WITH TONOS 	038F
// 0x0390:
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS 	0390
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER ALPHA	Α	0391 	&Alpha;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER BETA	0392 	&Beta;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER GAMMA	0393 	&Gamma;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER DELTA	0394 	&Delta;
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER EPSILON	0395 	&Epsilon;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER ZETA	0396 	&Zeta;
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER ETA	0397 	&Eta;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER THETA	0398 	&Theta;
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER IOTA	0399 	&Iota;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER KAPPA	039A 	&Kappa;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER LAM(B)DA	039B 	&Lambda;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER MU	039C 	&Mu;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER NU	039D 	&Nu;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER XI	039E 	&Xi;
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER OMICRON	039F 	&Omicron;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER PI	03A0 	&Pi;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER RHO	03A1 	&Rho;
0, // 03a2
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER SIGMA	03A3 	&Sigma;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER TAU	03A4 	&Tau;
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER UPSILON	03A5 	&Upsilon;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER PHI	03A6 	&Phi;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER CHI	03A7 	&Chi;
CH_PROP_UPPER | CH_PROP_CONSONANT, //    GREEK CAPITAL LETTER PSI	03A8 	&Psi;
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER OMEGA	03A9 	&Omega;
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER IOTA WITH DIALYTIKA 	03AA
CH_PROP_UPPER | CH_PROP_VOWEL, //    GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA 	03AB
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER ALPHA WITH TONOS 	03AC
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER EPSILON WITH TONOS 	03AD
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER ETA WITH TONOS 	03AE
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER IOTA WITH TONOS 	03AF

// 03B0
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS 	03B0
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER ALPHA   03B1 	&alpha;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER BETA	03B2 	&beta;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER GAMMA	03B3 	&gamma;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER DELTA	03B4 	&delta;
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER EPSILON	03B5 	&epsilon;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER ZETA	03B6 	&zeta;
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER ETA     03B7 	&eta;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER THETA	03B8 	&theta;
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER IOTA	03B9 	&iota;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER KAPPA	03BA 	&kappa;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER LAM(B)DA	03BB 	&lambda;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER MU      03BC 	&mu;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER NU      03BD 	&nu;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER XI      03BE 	&xi;
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER OMICRON	03BF 	&omicron;

CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER PI      03C0 	&pi;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER RHO     03C1 	&rho;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER FINAL SIGMA	03C2
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER SIGMA	03C3 	&sigma;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER TAU     03C4 	&tau;
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER UPSILON	03C5 	&upsilon;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER PHI     03C6 	&phi;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER CHI     03C7 	&chi;
CH_PROP_LOWER | CH_PROP_CONSONANT, //    GREEK SMALL LETTER PSI     03C8 	&psi;
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER OMEGA   03C9 	&omega;
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER IOTA WITH DIALYTIKA 	03CA
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER UPSILON WITH DIALYTIKA 	03CB
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER OMICRON WITH TONOS 	03CC
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER UPSILON WITH TONOS 	03CD
CH_PROP_LOWER | CH_PROP_VOWEL, //    GREEK SMALL LETTER OMEGA WITH TONOS 	03CE
0, //03cf
// 03d0
CH_PROP_CONSONANT, //    GREEK BETA SYMBOL (cursive) 	03D0
CH_PROP_CONSONANT, //    GREEK THETA SYMBOL (cursive) 	03D1
CH_PROP_VOWEL, //    GREEK UPSILON WITH HOOK SYMBOL	03D2
CH_PROP_VOWEL, //    GREEK UPSILON WITH ACUTE AND HOOK SYMBOL	03D3
CH_PROP_VOWEL, //    GREEK UPSILON WITH DIAERESIS AND HOOK SYMBOL	03D4
CH_PROP_CONSONANT, //    GREEK PHI SYMBOL (cursive) 	03D5
CH_PROP_CONSONANT, //    GREEK PI SYMBOL	03D6
CH_PROP_CONSONANT, //    GREEK KAI SYMBOL	03D7
0, // 03d8
0, // 03d9
CH_PROP_CONSONANT, //    GREEK LETTER STIGMA	03DA
CH_PROP_CONSONANT, //    GREEK SMALL LETTER STIGMA	03DB
CH_PROP_CONSONANT, //    GREEK LETTER DIGAMMA (F)	03DC
CH_PROP_CONSONANT, //    GREEK SMALL LETTER DIGAMMA (f)	03DD
CH_PROP_CONSONANT, //    GREEK LETTER KOPPA	03DE
CH_PROP_CONSONANT, //    GREEK SMALL LETTER KOPPA	03DF
// 03e0
CH_PROP_CONSONANT, //    GREEK LETTER SAMPI	03E0
CH_PROP_CONSONANT, //    GREEK SMALL LETTER SAMPI	03E1
// 03e2
    0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x0400:
0,  // 0400
CH_PROP_UPPER | CH_PROP_VOWEL,      // 0401 cyrillic E:
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0402 cyrillic Dje
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0403 cyrillic Gje
CH_PROP_UPPER | CH_PROP_VOWEL,      // 0404 cyrillic ukr Ie
CH_PROP_UPPER | CH_PROP_CONSONANT,  // 0405 cyrillic Dze
CH_PROP_UPPER | CH_PROP_VOWEL,      // 0406 cyrillic ukr I
CH_PROP_UPPER | CH_PROP_VOWEL,      // 0407 cyrillic ukr I:
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0408 cyrillic J
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0409 cyrillic L'
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 040A cyrillic N'
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 040B cyrillic Th
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 040C cyrillic K'
0,      // 040D cyrillic
CH_PROP_UPPER | CH_PROP_VOWEL,      // 040E cyrillic Yu
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 040F cyrillic Dzhe
// 0x0410:
CH_PROP_UPPER | CH_PROP_VOWEL,      // 0410 cyrillic A
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0411 cyrillic B
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0412 cyrillic V
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0413 cyrillic G
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0414 cyrillic D
CH_PROP_UPPER | CH_PROP_VOWEL,      // 0415 cyrillic E
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0416 cyrillic Zh
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0417 cyrillic Z
CH_PROP_UPPER | CH_PROP_VOWEL,      // 0418 cyrillic I
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0419 cyrillic YI
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 041A cyrillic K
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 041B cyrillic L
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 041C cyrillic M
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 041D cyrillic N
CH_PROP_UPPER | CH_PROP_VOWEL,      // 041E cyrillic O
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 041F cyrillic P
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0420 cyrillic R
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0421 cyrillic S
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0422 cyrillic T
CH_PROP_UPPER | CH_PROP_VOWEL,      // 0423 cyrillic U
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0424 cyrillic F
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0425 cyrillic H
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0426 cyrillic C
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0427 cyrillic Ch
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0428 cyrillic Sh
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0429 cyrillic Sch
CH_PROP_UPPER | CH_PROP_ALPHA_SIGN,      // 042A cyrillic Hard sign
CH_PROP_UPPER | CH_PROP_VOWEL,      // 042B cyrillic Y
CH_PROP_UPPER | CH_PROP_ALPHA_SIGN,      // 042C cyrillic Soft sign
CH_PROP_UPPER | CH_PROP_VOWEL,      // 042D cyrillic EE
CH_PROP_UPPER | CH_PROP_VOWEL,      // 042E cyrillic Yu
CH_PROP_UPPER | CH_PROP_VOWEL,      // 042F cyrillic Ya
// 0x0430:
CH_PROP_LOWER | CH_PROP_VOWEL,      // 0430 cyrillic A
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0431 cyrillic B
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0432 cyrillic V
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0433 cyrillic G
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0434 cyrillic D
CH_PROP_LOWER | CH_PROP_VOWEL,      // 0435 cyrillic E
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0436 cyrillic Zh
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0437 cyrillic Z
CH_PROP_LOWER | CH_PROP_VOWEL,      // 0438 cyrillic I
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0439 cyrillic YI
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 043A cyrillic K
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 043B cyrillic L
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 043C cyrillic M
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 043D cyrillic N
CH_PROP_LOWER | CH_PROP_VOWEL,      // 043E cyrillic O
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 043F cyrillic P
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0440 cyrillic R
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0441 cyrillic S
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0442 cyrillic T
CH_PROP_LOWER | CH_PROP_VOWEL,      // 0443 cyrillic U
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0444 cyrillic F
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0445 cyrillic H
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0446 cyrillic C
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0447 cyrillic Ch
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0448 cyrillic Sh
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0449 cyrillic Sch
CH_PROP_LOWER | CH_PROP_ALPHA_SIGN,     // 044A cyrillic Hard sign
CH_PROP_LOWER | CH_PROP_VOWEL,      // 044B cyrillic Y
CH_PROP_LOWER | CH_PROP_ALPHA_SIGN,     // 044C cyrillic Soft sign
CH_PROP_LOWER | CH_PROP_VOWEL,      // 044D cyrillic EE
CH_PROP_LOWER | CH_PROP_VOWEL,      // 044E cyrillic Yu
CH_PROP_LOWER | CH_PROP_VOWEL,      // 044F cyrillic Ya
0,      // 0450 cyrillic
CH_PROP_LOWER | CH_PROP_VOWEL,      // 0451 cyrillic e:
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0452 cyrillic Dje
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0453 cyrillic Gje
CH_PROP_LOWER | CH_PROP_VOWEL,      // 0454 cyrillic ukr Ie
CH_PROP_LOWER | CH_PROP_CONSONANT,  // 0455 cyrillic Dze
CH_PROP_LOWER | CH_PROP_VOWEL,      // 0456 cyrillic ukr I
CH_PROP_LOWER | CH_PROP_VOWEL,      // 0457 cyrillic ukr I:
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0458 cyrillic J
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0459 cyrillic L'
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 045A cyrillic N'
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 045B cyrillic Th
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 045C cyrillic K'
0,      // 045D cyrillic
CH_PROP_LOWER | CH_PROP_VOWEL,      // 045E cyrillic Yu
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 045F cyrillic Dzhe
// 0x0460:
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
// 0x0490:
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0490 cyrillic G'
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0491 cyrillic g'
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0492 cyrillic G-
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0493 cyrillic g-
0,      // 0494 cyrillic
0,      // 0495 cyrillic
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 0496 cyrillic Zh,
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 0497 cyrillic zh,
0,      // 0498 cyrillic
0,      // 0499 cyrillic
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 049A cyrillic K,
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 049B cyrillic k,
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 049C cyrillic K|
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 049D cyrillic k|
0,      // 049E cyrillic
0,      // 049F cyrillic
0,      // 04A0 cyrillic
0,      // 04A1 cyrillic
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 04A2 cyrillic H,
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 04A3 cyrillic n,
0,      // 04A4 cyrillic
0,      // 04A5 cyrillic
0,      // 04A6 cyrillic
0,      // 04A7 cyrillic
0,      // 04A8 cyrillic
0,      // 04A9 cyrillic
0,      // 04AA cyrillic
0,      // 04AB cyrillic
0,      // 04AC cyrillic
0,      // 04AD cyrillic
CH_PROP_UPPER | CH_PROP_VOWEL,      // 04AE cyrillic Y
CH_PROP_LOWER | CH_PROP_VOWEL,      // 04AF cyrillic y
CH_PROP_UPPER | CH_PROP_VOWEL,      // 04B0 cyrillic Y-
CH_PROP_LOWER | CH_PROP_VOWEL,      // 04B1 cyrillic y-
CH_PROP_UPPER | CH_PROP_CONSONANT,      // 04B2 cyrillic X,
CH_PROP_LOWER | CH_PROP_CONSONANT,      // 04B3 cyrillic x,
};


static lUInt16 char_props_1f00[] = {
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH PSILI 1F00
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH DASIA 1F01
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA 1F02
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA 1F03
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA 1F04
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA 1F05
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI 1F06
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI 1F07
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH PSILI 1F08
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH DASIA 1F09
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA 1F0A
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA 1F0B
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA 1F0C
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA 1F0D
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI 1F0E
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI 1F0F
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER EPSILON WITH PSILI 1F10
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER EPSILON WITH DASIA 1F11
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER EPSILON WITH PSILI AND VARIA 1F12
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER EPSILON WITH DASIA AND VARIA 1F13
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER EPSILON WITH PSILI AND OXIA 1F14
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA 1F15
0, 0,
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER EPSILON WITH PSILI 1F18
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER EPSILON WITH DASIA 1F19
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA 1F1A
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA 1F1B
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA 1F1C
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA 1F1D
0, 0,
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH PSILI 1F20
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH DASIA 1F21
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH PSILI AND VARIA 1F22
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH DASIA AND VARIA 1F23
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH PSILI AND OXIA 1F24
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH DASIA AND OXIA 1F25
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI 1F26
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI 1F27
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH PSILI 1F28
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH DASIA 1F29
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA 1F2A
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA 1F2B
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA 1F2C
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA 1F2D
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI 1F2E
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI 1F2F
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH PSILI 1F30
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH DASIA 1F31
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH PSILI AND VARIA 1F32
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH DASIA AND VARIA 1F33
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH PSILI AND OXIA 1F34
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH DASIA AND OXIA 1F35
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH PSILI AND PERISPOMENI 1F36
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH DASIA AND PERISPOMENI 1F37
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER IOTA WITH PSILI 1F38
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER IOTA WITH DASIA 1F39
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA 1F3A
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA 1F3B
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA 1F3C
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA 1F3D
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI 1F3E
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI 1F3F
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMICRON WITH PSILI 1F40
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMICRON WITH DASIA 1F41
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMICRON WITH PSILI AND VARIA 1F42
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMICRON WITH DASIA AND VARIA 1F43
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMICRON WITH PSILI AND OXIA 1F44
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA 1F45
0, 0,
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMICRON WITH PSILI 1F48
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMICRON WITH DASIA 1F49
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA 1F4A
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA 1F4B
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA 1F4C
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA 1F4D
0, 0,
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH PSILI 1F50
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH DASIA 1F51
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH PSILI AND VARIA 1F52
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH DASIA AND VARIA 1F53
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH PSILI AND OXIA 1F54
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH DASIA AND OXIA 1F55
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH PSILI AND PERISPOMENI 1F56
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI 1F57
0,
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER UPSILON WITH DASIA 1F59
0,
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA 1F5B
0,
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA 1F5D
0,
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI 1F5F
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH PSILI 1F60
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH DASIA 1F61
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA 1F62
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA 1F63
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA 1F64
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA 1F65
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI 1F66
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI 1F67
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH PSILI 1F68
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH PSILI 1F69
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH PSILI 1F6A
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA 1F6B
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA 1F6C
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA 1F6D
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI 1F6E
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI 1F6F
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH VARIA 1F70
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH OXIA 1F71
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER EPSILON WITH VARIA 1F72
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER EPSILON WITH OXIA 1F73
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH VARIA 1F74
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH OXIA 1F75
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH VARIA 1F76
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH OXIA 1F77
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMICRON WITH VARIA 1F78
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMICRON WITH OXIA 1F79
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH VARIA 1F7A
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH OXIA 1F7B
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH VARIA 1F7C
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH OXIA 1F7D
0, 0,
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH PSILI AND YPOGEGRAMMENI 1F80
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH DASIA AND YPOGEGRAMMENI 1F81
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH PSILI AND VARIA AND YPOGEGRAMMENI 1F82
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH DASIA AND VARIA AND YPOGEGRAMMENI 1F83
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH PSILI AND OXIA AND YPOGEGRAMMENI 1F84
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH DASIA AND OXIA AND YPOGEGRAMMENI 1F85
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI 1F86
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI 1F87
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI 1F88
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI 1F89
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI 1F8A
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI 1F8B
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI 1F8C
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI 1F8D
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI 1F8E
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI 1F8F
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH PSILI AND YPOGEGRAMMENI 1F90
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH DASIA AND YPOGEGRAMMENI 1F91
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH PSILI AND VARIA AND YPOGEGRAMMENI 1F92
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH DASIA AND VARIA AND YPOGEGRAMMENI 1F93
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH PSILI AND OXIA AND YPOGEGRAMMENI 1F94
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH DASIA AND OXIA AND YPOGEGRAMMENI 1F95
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI 1F96
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI 1F97
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI 1F98
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI 1F99
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI 1F9A
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI 1F9B
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI 1F9C
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI 1F9D
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI 1F9E
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI 1F9F
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH PSILI AND YPOGEGRAMMENI 1FA0
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH DASIA AND YPOGEGRAMMENI 1FA1
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH PSILI AND VARIA AND YPOGEGRAMMENI 1FA2
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH DASIA AND VARIA AND YPOGEGRAMMENI 1FA3
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH PSILI AND OXIA AND YPOGEGRAMMENI 1FA4
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH DASIA AND OXIA AND YPOGEGRAMMENI 1FA5
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH PSILI AND PERISPOMENI AND YPOGEGRAMMENI 1FA6
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH DASIA AND PERISPOMENI AND YPOGEGRAMMENI 1FA7
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI 1FA8
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI 1FA9
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI 1FAA
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI 1FAB
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI 1FAC
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI 1FAD
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI 1FAE
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI 1FAF
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH VRACHY 1FB0
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH MACRON 1FB1
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH VARIA AND YPOGEGRAMMENI 1FB2
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH YPOGEGRAMMENI 1FB3
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI 1FB4
0,
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH PERISPOMENI 1FB6
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ALPHA WITH PERISPOMENI AND YPOGEGRAMMENI 1FB7
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH VRACHY 1FB8
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH MACRON 1FB9
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH VARIA 1FBA
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH OXIA 1FBB
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI 1FBC
0, 0, 0,
0, 0,
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI 1FC2
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH YPOGEGRAMMENI 1FC3
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI 1FC4
0,
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH PERISPOMENI 1FC6
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER ETA WITH PERISPOMENI AND YPOGEGRAMMENI 1FC7
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER EPSILON WITH VARIA 1FC8
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER EPSILON WITH OXIA 1FC9
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH VARIA 1FCA
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH OXIA 1FCB
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI 1FCC
0, 0, 0,
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH VRACHY 1FD0
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH MACRON 1FD1
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH DIALYTIKA AND VARIA 1FD2
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH DIALYTIKA AND OXIA 1FD3
0, 0,
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH PERISPOMENI 1FD6
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER IOTA WITH DIALYTIKA AND PERISPOMENI 1FD7
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER IOTA WITH VRACHY 1FD8
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER IOTA WITH MACRON 1FD9
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER IOTA WITH VARIA 1FDA
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER IOTA WITH OXIA 1FDB
0, 0, 0, 0,
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH VRACHY 1FE0
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH MACRON 1FE1
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND VARIA 1FE2
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND OXIA 1FE3
CH_PROP_LOWER | CH_PROP_CONSONANT, // GREEK SMALL LETTER RHO WITH PSILI 1FE4
CH_PROP_LOWER | CH_PROP_CONSONANT, // GREEK SMALL LETTER RHO WITH DASIA 1FE5
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH PERISPOMENI 1FE6
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND PERISPOMENI 1FE7
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER UPSILON WITH VRACHY 1FE8
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER UPSILON WITH MACRON 1FE9
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER UPSILON WITH VARIA 1FEA
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER UPSILON WITH OXIA 1FEB
CH_PROP_UPPER | CH_PROP_CONSONANT, // GREEK CAPITAL LETTER RHO WITH DASIA 1FEC
0, 0, 0,
0, 0,
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI 1FF2
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH YPOGEGRAMMENI 1FF3
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI 1FF4
0,
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH PERISPOMENI 1FF6
CH_PROP_LOWER | CH_PROP_VOWEL, // GREEK SMALL LETTER OMEGA WITH PERISPOMENI AND YPOGEGRAMMENI 1FF7
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMICRON WITH VARIA 1FF8
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMICRON WITH OXIA 1FF9
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH VARIA 1FFA
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH OXIA 1FFB
CH_PROP_UPPER | CH_PROP_VOWEL, // GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI 1FFC
0, 0, 0
};

inline lUInt16 getCharProp(lChar32 ch) {
    static const lChar32 maxchar = sizeof(char_props) / sizeof( lUInt16 );
    if (ch<maxchar)
        return char_props[ch];
    else if ((ch>>8) == 0x1F)
        return char_props_1f00[ch & 255];
    else if (ch>=0x2012 && ch<=0x2015)
        return CH_PROP_DASH|CH_PROP_SIGN;
    else if (ch==0x201C) // left double quotation mark
        return CH_PROP_AVOID_WRAP_AFTER;
    else if (ch==0x201D) // right double quotation mark
        return CH_PROP_AVOID_WRAP_BEFORE;
    else if (ch>=UNICODE_CJK_IDEOGRAPHS_BEGIN && ch<=UNICODE_CJK_IDEOGRAPHS_END&&(ch<=UNICODE_CJK_PUNCTUATION_HALF_AND_FULL_WIDTH_BEGIN||
                                                                                  ch>=UNICODE_CJK_PUNCTUATION_HALF_AND_FULL_WIDTH_END))
        return CH_PROP_CJK;
    else if ((ch>=UNICODE_CJK_PUNCTUATION_BEGIN && ch<=UNICODE_CJK_PUNCTUATION_END) ||
             (ch>=UNICODE_GENERAL_PUNCTUATION_BEGIN && ch<=UNICODE_GENERAL_PUNCTUATION_END) ||
             (ch>=UNICODE_CJK_PUNCTUATION_HALF_AND_FULL_WIDTH_BEGIN && ch<=UNICODE_CJK_PUNCTUATION_HALF_AND_FULL_WIDTH_END))
        return CH_PROP_PUNCT;

    // Try to guess a few other things about other chars we don't handle above
    lUInt16 prop = 0;
#if (USE_UTF8PROC==1)
    // For other less known ranges, fallback to detecting letters with utf8proc,
    // which is enough to be able to ensure hyphenation for Armenian and Georgian.
    utf8proc_category_t cat = utf8proc_category(ch);
    switch (cat) {
        case UTF8PROC_CATEGORY_LU:
        case UTF8PROC_CATEGORY_LT:
            prop |= CH_PROP_UPPER;
            break;
        case UTF8PROC_CATEGORY_LL:
        case UTF8PROC_CATEGORY_LM:
        case UTF8PROC_CATEGORY_LO:
            prop |= CH_PROP_LOWER;
            break;
        default:
            break;
    }
#endif
    // Detect RTL (details in lvtextfm.cpp)
    if ( ch >= 0x0590 && ch <= 0x08FF ) // Hebrew, Arabic, Syriac, Thaana, Nko, Samaritan...
        prop |= CH_PROP_RTL;
    else if ( ch >= 0xFB1D ) { // Try to balance the searches
        if ( ch <= 0xFDFF )     // FB1D>FDFF Hebrew and Arabic presentation forms
            prop |= CH_PROP_RTL;
        else if ( ch <= 0xFEFF ) {
            if ( ch >= 0xFE70)   // FE70>FEFF Arabic presentation forms
                prop |= CH_PROP_RTL;
        }
        else if ( ch <= 0x1EEBB ) {
            if (ch >= 0x1E800)   // 1E800>1EEBB Other rare scripts possibly RTL
                prop |= CH_PROP_RTL;
            else if ( ch <= 0x10FFF && ch >= 0x10800 ) // 10800>10FFF Other rare scripts possibly RTL
                prop |= CH_PROP_RTL;
        }
    }
    return prop;
}

void lStr_getCharProps( const lChar32 * str, int sz, lUInt16 * props )
{
    for ( int i=0; i<sz; i++ ) {
        lChar32 ch = str[i];
        props[i] = getCharProp(ch);
    }
}

bool lStr_isWordSeparator( lChar32 ch )
{
    // ASCII letters and digits are NOT word separators
    if (ch >= 0x61 && ch <= 0x7A) return false; // lowercase ascii letters
    if (ch >= 0x41 && ch <= 0x5A) return false; // uppercase ascii letters
    if (ch >= 0x30 && ch <= 0x39) return false; // digits
    if (ch == 0xAD ) return false; // soft-hyphen, considered now as part of word
    // All other below 0xC0 are word separators:
    //   < 0x30 space, !"#$%&'()*+,-./
    //   < 0x41 :;<=>?@
    //   < 0x61 [\]^_`
    //   < 0xC0 {|}~ and control characters and other signs
    if (ch < 0xC0 ) return true;
    // 0xC0 to 0xFF, except 0xD7 and 0xF7, are latin accentuated letters.
    // Above 0xFF are other alphabets. Let's consider all above 0xC0 unicode
    // characters as letters, except the adequately named PUNCTUATION ranges.
    // There may be exceptions in some alphabets, that we can individually
    // add here :
    if (ch == 0xD7 ) return true;  // multiplication sign
    if (ch == 0xF7 ) return true;  // division sign
    // this one includes em-dash & friends, and other quotation marks
    if (ch>=UNICODE_GENERAL_PUNCTUATION_BEGIN && ch<=UNICODE_GENERAL_PUNCTUATION_END) return true;
    // CJK puncutation
    if (ch>=UNICODE_CJK_PUNCTUATION_BEGIN && ch<=UNICODE_CJK_PUNCTUATION_END) return true;
    if (ch>=UNICODE_CJK_PUNCTUATION_HALF_AND_FULL_WIDTH_BEGIN && ch<=UNICODE_CJK_PUNCTUATION_HALF_AND_FULL_WIDTH_END) return true;
    // Some others(from https://www.cs.tut.fi/~jkorpela/chars/spaces.html)
    if (ch == 0x1680 ) return true;  // OGHAM SPACE MARK
    if (ch == 0x180E ) return true;  // MONGOLIAN VOWEL SEPARATOR
    if (ch == 0xFEFF ) return true;  // ZERO WIDTH NO-BREAK SPACE
    // All others are considered part of a word, thus not word separators
    return false;
}

/// find alpha sequence bounds
void lStr_findWordBounds( const lChar32 * str, int sz, int pos, int & start, int & end, bool & has_rtl )
{
    int hwStart, hwEnd;
    has_rtl = false;

    // 20180615: don't split anymore on UNICODE_SOFT_HYPHEN_CODE, consider
    // it like an alpha char of zero width not drawn.
    // Only hyphenation code will care about it
    // We don't use lStr_isWordSeparator() here, but we exclusively look
    // for ALPHA chars or soft-hyphens, as this function is and should
    // only be used before calling hyphenate() to find a real word to
    // give to the hyphenation algorithms.

//    // skip spaces
//    for (hwStart=pos-1; hwStart>0; hwStart--)
//    {
//        lChar32 ch = str[hwStart];
//        if ( ch<(int)maxchar ) {
//            lUInt16 props = char_props[ch];
//            if ( !(props & CH_PROP_SPACE) )
//                break;
//        }
//    }
//    // skip punctuation signs and digits
//    for (; hwStart>0; hwStart--)
//    {
//        lChar32 ch = str[hwStart];
//        if ( ch<(int)maxchar ) {
//            lUInt16 props = char_props[ch];
//            if ( !(props & (CH_PROP_PUNCT|CH_PROP_DIGIT)) )
//                break;
//        }
//    }
    // skip until first alpha
    for (hwStart = pos-1; hwStart > 0; hwStart--)
    {
        lChar32 ch = str[hwStart];
        lUInt16 props = getCharProp(ch);
        if ( props & (CH_PROP_ALPHA|CH_PROP_HYPHEN) )
            break;
    }
    if ( hwStart<0 ) {
        // no alphas found
        start = end = pos;
        return;
    }
    hwEnd = hwStart+1;
    // skipping while alpha
    for (; hwStart>0; hwStart--)
    {
        lChar32 ch = str[hwStart];
        //int lastAlpha = -1;
        lUInt16 props = getCharProp(ch);
        if ( props & (CH_PROP_ALPHA|CH_PROP_HYPHEN) ) {
            if ( props & CH_PROP_RTL )
                has_rtl = true;
            //lastAlpha = hwStart;
        } else {
            hwStart++;
            break;
        }
    }
//    if ( lastAlpha<0 ) {
//        // no alphas found
//        start = end = pos;
//        return;
//    }
    for (hwEnd=hwStart+1; hwEnd<sz; hwEnd++) // 20080404
    {
        lChar32 ch = str[hwEnd];
        lUInt16 props = getCharProp(ch);
        if ( !(props & (CH_PROP_ALPHA|CH_PROP_HYPHEN)) )
            break;
        ch = str[hwEnd-1];
        if ( ch==' ' ) // || ch==UNICODE_SOFT_HYPHEN_CODE) )
            break;
    }
    start = hwStart;
    end = hwEnd;
    //CRLog::debug("Word bounds: '%s'", LCSTR(lString32(str+start, end-start)));
}

lUInt16 lGetCharProps( lChar32 ch )
{
    return getCharProp(ch);
}

bool splitIntegerList( lString32 s, lString32 delim, int &value1, int &value2 )
{
    if ( s.empty() )
        return false;
    lString32 s1, s2;
    if ( !s.split2( delim, s1, s2 ) )
        return false;
    int n1, n2;
    if ( !s1.atoi(n1) )
        return false;
    if ( !s2.atoi(n2) )
        return false;
    value1 = n1;
    value2 = n2;
    return true;
}

static int decodeHex( lChar32 ch )
{
    if ( ch>='0' && ch<='9' )
        return ch-'0';
    else if ( ch>='a' && ch<='f' )
        return ch-'a'+10;
    else if ( ch>='A' && ch<='F' )
        return ch-'A'+10;
    return -1;
}

static lChar8 decodeHTMLChar( const lChar32 * s )
{
    if (s[0] == '%') {
        int d1 = decodeHex( s[1] );
        if (d1 >= 0) {
            int d2 = decodeHex( s[2] );
            if (d2 >= 0) {
                return (lChar8)(d1*16 + d2);
            }
        }
    }
    return 0;
}

/// decodes path like "file%20name%C3%A7" to "file nameç"
lString32 DecodeHTMLUrlString( lString32 s )
{
    const lChar32 * str = s.c_str();
    for ( int i=0; str[i]; i++ ) {
        if ( str[i]=='%'  ) {
            lChar8 ch = decodeHTMLChar( str + i );
            if ( ch==0 ) {
                continue;
            }
            // HTML encoded char found
            lString8 res;
            res.reserve(s.length());
            res.append(UnicodeToUtf8(str, i));
            res.append(1, ch);
            i+=3;

            // continue conversion
            for ( ; str[i]; i++ ) {
                if ( str[i]=='%'  ) {
                    ch = decodeHTMLChar( str + i );
                    if ( ch==0 ) {
                        res.append(1, (lChar8)str[i]);
                        continue;
                    }
                    res.append(1, ch);
                    i+=2;
                } else {
                    res.append(1, (lChar8)str[i]);
                }
            }
            return Utf8ToUnicode(res);
        }
    }
    return s;
}

void limitStringSize(lString32 & str, int maxSize) {
    if (str.length() < maxSize)
        return;
    int lastSpace = -1;
    for (int i = str.length() - 1; i > 0; i--)
        if (str[i] == ' ') {
            while (i > 0 && str[i - 1] == ' ')
                i--;
            lastSpace = i;
            break;
        }
    int split = lastSpace > 0 ? lastSpace : maxSize;
    str = str.substr(0, split);
    str += "...";
}

/// remove soft-hyphens from string
lString32 removeSoftHyphens( lString32 s )
{
    lChar32 hyphen = lChar32(UNICODE_SOFT_HYPHEN_CODE);
    int start = 0;
    while (true) {
        int p = -1;
        int len = s.length();
        for (int i = start; i < len; i++) {
            if (s[i] == hyphen) {
                p = i;
                break;
            }
        }
        if (p == -1)
            break;
        start = p;
        lString32 s1 = s.substr( 0, p );
        lString32 s2 = p < len-1 ? s.substr( p+1, len-p-1 ) : lString32::empty_str;
        s = s1 + s2;
    }
    return s;
}

std::basic_string<lChar16> detail::Utf8ToUtf16(const lChar8* s, size_t count)
{
    //FIXME: double conversion, but seems ok, because we don't use
    // this conversion in practice
    std::basic_string<lChar32> utf32 = detail::Utf8ToUtf32(s, count);
    return detail::Utf32ToUtf16(utf32.c_str(), lStr_len(utf32.c_str()));
}

std::basic_string<lChar32> detail::Utf8ToUtf32(const lChar8* s, size_t count)
{
    if (!s || !s[0] || count == 0)
      return {};
    int len = Utf8CharCount( s );
    if (!len)
      return {};
    std::basic_string<lChar32> dst(len, 0);
    lChar32 * p = dst.data();
    DecodeUtf8(s, p, len);
    return dst;
}

std::basic_string<lChar8> detail::Utf16ToUtf8(const lChar16* s, size_t count)
{
    //FIXME: double conversion, but seems ok, because we don't use
    // this conversion in practice
    std::basic_string<lChar32> utf32 = detail::Utf16ToUtf32(s, count);
    return detail::Utf32ToUtf8(utf32.c_str(), lStr_len(utf32.c_str()));

}

std::basic_string<lChar32> detail::Utf16ToUtf32(const lChar16* s, size_t count)
{
    if (!s || !s[0] || count <= 0)
      return {};
    int len = Utf16CharCount( s, count );
    if (!len)
      return {};
    std::basic_string<lChar32> dst(len, 0);
    lChar32 * p = dst.data();
    DecodeUtf16(s, p, len);
    return dst;
}

std::basic_string<lChar8> detail::Utf32ToUtf8(const lChar32* s, size_t count)
{
    if (count <= 0)
      return {};
    int len = Utf8ByteCount(s, count);
    if (len <= 0)
      return {};
    std::basic_string<lChar8> dst(len, 0);
    lChar8 * buf = dst.data();
    {
        lUInt32 ch;
        while ((count--) > 0) {
            ch = *s++;
            if (!(ch & ~0x7F)) {
                *buf++ = ( (lUInt8)ch );
            } else if (!(ch & ~0x7FF)) {
                *buf++ = ( (lUInt8) ( ((ch >> 6) & 0x1F) | 0xC0 ) );
                *buf++ = ( (lUInt8) ( ((ch ) & 0x3F) | 0x80 ) );
            } else if (!(ch & ~0xFFFF)) {
                *buf++ = ( (lUInt8) ( ((ch >> 12) & 0x0F) | 0xE0 ) );
                *buf++ = ( (lUInt8) ( ((ch >> 6) & 0x3F) | 0x80 ) );
                *buf++ = ( (lUInt8) ( ((ch ) & 0x3F) | 0x80 ) );
            } else if (!(ch & ~0x1FFFFF)) {
                *buf++ = ( (lUInt8) ( ((ch >> 18) & 0x07) | 0xF0 ) );
                *buf++ = ( (lUInt8) ( ((ch >> 12) & 0x3F) | 0x80 ) );
                *buf++ = ( (lUInt8) ( ((ch >> 6) & 0x3F) | 0x80 ) );
                *buf++ = ( (lUInt8) ( ((ch ) & 0x3F) | 0x80 ) );
            } else {
                // invalid codepoint
                // In Unicode Standard codepoint must be in range U+0000 .. U+10FFFF
                *buf++ = '?';
            }
        }
    }
    return dst;
}

std::basic_string<lChar16> detail::Utf32ToUtf16(const lChar32* s, size_t count)
{
    if (!s || count <= 0)
        return {};
    int len = Utf16WordCount(s, count);
    if (len <= 0)
      return {};
    std::basic_string<lChar16> dst(len, 0);
    lChar16 * buf = dst.data();
    {
        lUInt32 ch;
        while ((count--) > 0) {
            ch = *s++;
            if (!(ch & ~0xFFFF)) {
                *buf++ = (lChar16)ch;
            } else if (!(ch & ~0x1FFFFF)) {
                // put into a surrogate pair
                // 000uuuuuxxxxxxxxxxxxxxxx => 110110wwwwxxxxxx, 110111xxxxxxxxxx
                //   where wwww = uuuuu - 1
                // first word
                *buf++ = (lChar16) ( 0xD800 | ( ( ( (ch >> 16) & 0x1F ) - 1 ) << 6 ) | ( (ch >> 10) & 0x3F ) );
                // second word
                *buf++ = (lChar16) ( 0xDC00 | (ch & 0x3FF) );
            } else {
                // invalid codepoint
                // In Unicode Standard codepoint must be in range U+0000 .. U+10FFFF
                *buf++ = L'?';
            }
        }
    }
    return dst;
}
