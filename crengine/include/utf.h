/***************************************************************************
 *   CoolReader engine                                                     *
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
 * \file utf.h
 * \brief string classes interface
 */

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

struct utf8_decoder 
{
    template<class Iterator>
    decode_result<Iterator> decode(Iterator current, Iterator end) const
    {
        //TODO
        return { 0; end; }
    }
};

struct utf8_encoder {
    static constexpr size_t codePointSize(lChar32 cp) noexcept {
        if (cp < 0x80) return 1;
        if (cp < 0x800) return 2;
        if (cp < 0x10000) return 3;
        return 4;
    }

    template <typename OutputIt>
    OutputIt encode(lChar32 cp, OutputIt it) const {
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

struct utf32_decoder
{
    template <class Iterator>
    decode_result<Iterator> decode(Iterator current, Iterator end) const {
        if (current == end) return { 0, end };
        Iterator next_it = current;
        lChar32 cp = *next_it++;
        return { cp, next_it };
    }
};

template<class Decoder, class CodeUnitIterator>
class code_point_iterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = lChar32;
    using difference_type = std::ptrdiff_t;
    using pointer = const lChar32*;
    using reference = const lChar32&;

    code_point_iterator() = default; // Default constructs the "end" sentinel
    
    code_point_iterator(CodeUnitIterator current, CodeUnitIterator end, Decoder decoder = {})
        : m_current(current), m_end(end), m_decoder(decoder)
    {
        advance(); // Decode the first character immediately upon creation
    }

    reference operator*() const { return m_code_point; }
    pointer operator->() const { return &m_code_point; }

    code_point_iterator& operator++()
    {
        m_current = m_next; // Jump to the next byte sequence
        advance();
        return *this;
    }

    code_point_iterator operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const code_point_iterator& other) const
    {
        // Comparing against a default-constructed iterator (the sentinel) works here
        return m_current == other.m_current; 
    }

    bool operator!=(const code_point_iterator& other) const {
        return !(*this == other);
    }

private:
    void advance()
    {
        if (m_current != m_end) {
            auto res = m_decoder.decode(m_current, m_end);
            m_code_point = res.code_point;
            m_next = res.next;
        } else {
            // Reached the end; become the default-constructed sentinel
            m_current = CodeUnitIterator{}; 
        }
    }

    CodeUnitIterator m_current{};
    CodeUnitIterator m_end{};
    CodeUnitIterator m_next{};
    Decoder m_decoder{};
    lChar32 m_code_point = 0;
};

template<class BaseIt, class Transform>
class code_point_transform_iterator {
public:
    using base_value_type = typename std::iterator_traits<BaseIt>::value_type;
    
    // C++17: Deduce the return type of the transform function
    using value_type = std::invoke_result_t<Transform, base_value_type>;
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;

    code_point_transform_iterator(BaseIt base, Transform transform)
        : m_base(base), m_transform(std::move(transform)), m_dirty(true) {}

    reference operator*() const
    { 
        update(); 
        return m_cached; 
    }

    code_point_transform_iterator& operator++()
    {
        ++m_base;
        m_dirty = true;
        return *this; 
    }

    code_point_transform_iterator operator++(int)
    {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const code_point_transform_iterator& other) const { return m_base == other.m_base; }
    bool operator!=(const code_point_transform_iterator& other) const { return m_base != other.m_base; }

private:
    void update() const
    {
        if (m_dirty) { 
            // C++17: std::invoke handles lambdas, member functions, and functors uniformly
            //m_cached = std::invoke(m_transform, *m_base);
            m_cached = m_transform(*m_base);
            m_dirty = false; 
        }
    }

    BaseIt m_base;
    Transform m_transform;
    mutable bool m_dirty = true;
    mutable value_type m_cached{};
};


template <class Decoder, class CodeUnitIterator>
class code_point_view
{
public:
    using iterator = code_point_iterator<Decoder, CodeUnitIterator>;

    code_point_view(CodeUnitIterator current, CodeUnitIterator end, Decoder decoder = {})
        : begin_(current), end_(end), decoder_(decoder) {}

    iterator begin() const {
        return iterator(begin_, end_, decoder_);
    }

    iterator end() const {
        return iterator(end_, end_, decoder_);
    }

private:
    CodeUnitIterator begin_;
    CodeUnitIterator end_;
    Decoder          decoder_;
};


template <typename SrcDecoder, typename DstEncoder, typename SrcIt>
size_t measureCodeUnitsCount(SrcIt first, SrcIt last)
{
    size_t total = 0;
    auto it = code_point_iterator<SrcDecoder, SrcIt>(first, last);
    auto end = code_point_iterator<SrcDecoder, SrcIt>();
    while (it != end) {
        total += DstEncoder::codePointSize(*it);
        ++it;
    }
    return total;
}

template <typename SrcDecoder, typename DstEncoder, typename SrcIt, typename DstContainer>
void reencode(SrcIt first, SrcIt last, DstContainer& dst)
{
    size_t required = measureCodeUnitsCount<SrcDecoder, DstEncoder>(first, last);
    
    dst.reserve(dst.size() + required);
    
    DstEncoder encoder;
    auto out = std::back_inserter(dst);
    
    auto view = code_point_view<SrcDecoder, SrcIt>(first, last, SrcDecoder{});
    for (auto it = view.begin(); it != view.end(); ++it) {
        out = encoder.encode(*it, out);
    }
}
#endif /* __LV_UTF_H_INCLUDED__ */
