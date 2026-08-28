/***************************************************************************
 *   CoolReader engine                                                     *
 *   Copyright (C) 2019,2020 Konstantin Potapov <pkbo@users.sourceforge.net>
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

#ifndef __LV_UTF_H_INCLUDED__
#define __LV_UTF_H_INCLUDED__

#include "lvtypes.h"

#include <string>
#include <iterator>
#include <type_traits>
#include <utility>

template<class Iterator>
struct decode_result
{
    lChar32 code_point;
    Iterator next;
};

constexpr lChar32 REPLACEMENT_CHARACTER = static_cast<lChar32>(0xFFFD);

constexpr bool is_unicode_scalar(lChar32 cp) noexcept
{
    const lUInt32 u = static_cast<lUInt32>(cp);

    return u <= 0x10FFFF
        && !(u >= 0xD800 && u <= 0xDFFF);
}

static lChar32 utf8_decode_next(const lUInt8*& src, const lUInt8* end)
{
    if (src >= end) return 0;

    lUInt8 lead = *src++;
    lUInt32 cp = lead;
    size_t bytes = 0;

    if (lead < 0x80) {
        return lead;
    } else if (lead >= 0xC0 && lead < 0xE0) {
        cp = lead & 0x1F; bytes = 2;
    } else if (lead >= 0xE0 && lead < 0xF0) {
        cp = lead & 0x0F; bytes = 3;
    } else if (lead >= 0xF0 && lead < 0xF8) {
        cp = lead & 0x07; bytes = 4;
    } else {
        // Invalid lead byte (0x80..0xBF, 0xF8..0xFF)
        return REPLACEMENT_CHARACTER;
    }

    // Check if we have enough bytes left in the buffer
    if (src + (bytes - 1) > end) {
        src = end; // Consume the truncated sequence
        return REPLACEMENT_CHARACTER;
    }

    // Read continuation bytes
    for (size_t i = 1; i < bytes; ++i) {
        lUInt8 cont = *src; // Peek
        if ((cont & 0xC0) != 0x80) {
            // Invalid continuation byte. Do NOT consume it.
            // It will be processed as the lead byte of the next sequence.
            return REPLACEMENT_CHARACTER;
        }
        ++src; // Valid continuation, consume it.
        cp = (cp << 6) | (cont & 0x3F);
    }

    // --- Validation ---
    // Overlong check
    if ((bytes == 2 && cp < 0x80) ||
        (bytes == 3 && cp < 0x800) ||
        (bytes == 4 && cp < 0x10000)) {
        return REPLACEMENT_CHARACTER;
    }

    // Surrogate check
    if (cp >= 0xD800 && cp <= 0xDFFF) {
        return REPLACEMENT_CHARACTER;
    }

    // Max scalar check
    if (cp > 0x10FFFF) {
        return REPLACEMENT_CHARACTER;
    }
    return cp;
}

struct utf8_decoder
{
    template<class Iterator>
    decode_result<Iterator> decode(Iterator current, Iterator end) const
    {
        if (current == end) return {0, end};

        // C++17 helper to get a raw pointer from contiguous iterators
        // (Works for const char*, const uint8_t*, std::string_view::iterator, etc.)
        auto get_ptr = [](auto it) -> const lUInt8* {
            if constexpr (std::is_pointer_v<decltype(it)>) {
                return reinterpret_cast<const lUInt8*>(it);
            } else {
                return reinterpret_cast<const lUInt8*>(&*it);
            }
        };

        const lUInt8* src_ptr = get_ptr(current);
        const lUInt8* end_ptr = get_ptr(end);

        // Call the raw pointer decoder
        lChar32 cp = utf8_decode_next(src_ptr, end_ptr);

        // Calculate how many bytes were consumed to advance the generic iterator
        auto bytes_consumed = src_ptr - get_ptr(current);

        Iterator next_it = current;
        std::advance(next_it, bytes_consumed);

        return { cp, next_it };
    }
};

struct utf8_encoder
{
    static constexpr size_t codePointSize(lChar32 cp) noexcept
    {
        if (cp < 0x80) return 1;
        if (cp < 0x800) return 2;
        if (cp < 0x10000) return 3;
        return 4;
    }

    template <typename OutputIt>
    OutputIt encode(lChar32 cp, OutputIt it) const
    {
        if (cp < 0x80) {
            *it++ = static_cast<char>(cp);
        } else if (cp < 0x800) {
            *it++ = static_cast<char>(0xC0 | (cp >> 6));
            *it++ = static_cast<char>(0x80 | (cp & 0x3F));
        } else if (cp < 0x10000) {
            *it++ = static_cast<char>(0xE0 | (cp >> 12));
            *it++ = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            *it++ = static_cast<char>(0x80 | (cp & 0x3F));
        } else {
            *it++ = static_cast<char>(0xF0 | (cp >> 18));
            *it++ = static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
            *it++ = static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
            *it++ = static_cast<char>(0x80 | (cp & 0x3F));
        }
        return it;
    }
};

struct utf16_decoder
{
    template<class Iterator>
    decode_result<Iterator> decode(Iterator current, Iterator end) const
    {
        if (current == end) {
            return { 0, current };
        }

        Iterator next = current;

        const lUInt16 lead = static_cast<lUInt16>(*next);
        ++next;

        if (lead < 0xD800 || lead > 0xDFFF) {
            // Normal BMP code point.
            return { static_cast<lChar32>(lead), next };
        }

        if (lead <= 0xDBFF) {
            // High surrogate.
            if (next == end) {
                // Truncated high surrogate.
                return { REPLACEMENT_CHARACTER, next };
            }

            const lUInt16 trail = static_cast<lUInt16>(*next);

            if (trail >= 0xDC00 && trail <= 0xDFFF) {
                ++next;

                const lChar32 cp =
                    static_cast<lChar32>(0x10000)
                    + (static_cast<lChar32>(lead - 0xD800) << 10)
                    + static_cast<lChar32>(trail - 0xDC00);

                return { cp, next };
            }

            // Invalid trail surrogate.
            //
            // Important: do not consume it.
            // It will be processed as the start of the next subsequence.
            return { REPLACEMENT_CHARACTER, next };
        }

        // Isolated low surrogate.
        return { REPLACEMENT_CHARACTER, next };
    }
};

struct utf16_encoder
{
    static constexpr std::size_t codePointSize(lChar32 cp) noexcept
    {
        if (!is_unicode_scalar(cp)) {
            return 1; // replacement character
        }

        return cp < static_cast<lChar32>(0x10000) ? 1 : 2;
    }

    template<class OutputIt>
    OutputIt encode(lChar32 cp, OutputIt it) const
    {
        if (!is_unicode_scalar(cp)) {
            cp = REPLACEMENT_CHARACTER;
        }

        if (cp < static_cast<lChar32>(0x10000)) {
            *it++ = static_cast<lUInt16>(cp);
            return it;
        }

        const lChar32 v = cp - static_cast<lChar32>(0x10000);

        *it++ = static_cast<lUInt16>(0xD800 + (v >> 10));
        *it++ = static_cast<lUInt16>(0xDC00 + (v & 0x3FF));

        return it;
    }
};

struct utf32_decoder
{
    template<class Iterator>
    decode_result<Iterator> decode(Iterator current, Iterator end) const
    {
        if (current == end) {
            return { 0, current };
        }

        Iterator next = current;

        const lUInt32 unit = static_cast<lUInt32>(*next);
        ++next;

        if (unit <= 0x10FFFF && !(unit >= 0xD800 && unit <= 0xDFFF)) {
            return { static_cast<lChar32>(unit), next };
        }

        return { REPLACEMENT_CHARACTER, next };
    }
};

struct utf32_encoder
{
    static constexpr std::size_t codePointSize(lChar32) noexcept
    {
        return 1;
    }

    template<class OutputIt>
    OutputIt encode(lChar32 cp, OutputIt it) const
    {
        if (!is_unicode_scalar(cp)) {
            cp = REPLACEMENT_CHARACTER;
        }

        *it++ = static_cast<lUInt32>(cp);
        return it;
    }
};
#endif /* __LV_UTF_H_INCLUDED__ */
