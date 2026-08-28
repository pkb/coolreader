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

#ifndef LV_UNICODE_CODE_POINT_HPP
#define LV_UNICODE_CODE_POINT_HPP

#include <iterator>
#include <type_traits>
#include <utility>
#include <functional>

#include "lvtypes.h"
#include "utf.h"

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
            m_cached = std::invoke(m_transform, *m_base);
            m_dirty = false;
        }
    }

    BaseIt m_base;
    mutable Transform m_transform;
    mutable bool m_dirty = true;
    mutable value_type m_cached{};
};


template<class BaseIt, class Pred>
class filter_iterator
{
public:
    using iterator_category = std::input_iterator_tag;
    using value_type        = typename std::iterator_traits<BaseIt>::value_type;
    using difference_type   = std::ptrdiff_t;
    using reference         = typename std::iterator_traits<BaseIt>::reference;
    using pointer           = const value_type*;

    filter_iterator(BaseIt current, BaseIt end, Pred pred)
        : m_current(std::move(current))
        , m_end(std::move(end))
        , m_pred(std::move(pred))
    {
        satisfy();
    }

    reference operator*() const
    {
        return *m_current;
    }

    filter_iterator& operator++()
    {
        ++m_current;
        satisfy();
        return *this;
    }

    filter_iterator operator++(int)
    {
        filter_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    friend bool operator==(const filter_iterator& lhs, const filter_iterator& rhs)
    {
        return lhs.m_current == rhs.m_current;
    }

    friend bool operator!=(const filter_iterator& lhs, const filter_iterator& rhs)
    {
        return !(lhs == rhs);
    }

private:
    void satisfy()
    {
        while (m_current != m_end && !std::invoke(m_pred, *m_current)) {
            ++m_current;
        }
    }

    BaseIt m_current;
    BaseIt m_end;
    Pred   m_pred;
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

template<class BaseView, class Transform>
class transform_view
{
public:
    using base_iterator = decltype(std::declval<const BaseView&>().begin());
    using iterator      = code_point_transform_iterator<base_iterator, Transform>;

    transform_view(BaseView base, Transform transform)
        : m_base(std::move(base))
        , m_transform(std::move(transform))
    {
    }

    iterator begin() const
    {
        return iterator(m_base.begin(), m_transform);
    }

    iterator end() const
    {
        return iterator(m_base.end(), m_transform);
    }

private:
    BaseView  m_base;
    Transform m_transform;
};

template<class BaseView, class Pred>
class filter_view
{
public:
    using base_iterator = decltype(std::declval<const BaseView&>().begin());
    using iterator      = filter_iterator<base_iterator, Pred>;

    filter_view(BaseView base, Pred pred)
        : m_base(std::move(base))
        , m_pred(std::move(pred))
    {
    }

    iterator begin() const
    {
        return iterator(m_base.begin(), m_base.end(), m_pred);
    }

    iterator end() const
    {
        return iterator(m_base.end(), m_base.end(), m_pred);
    }

private:
    BaseView m_base;
    Pred     m_pred;
};

struct pipe_adaptor_base {};

template<class Pred>
struct filter_adaptor : pipe_adaptor_base
{
    Pred pred;

    explicit filter_adaptor(Pred p)
        : pred(std::move(p))
    {
    }

    template<class View>
    auto operator()(View&& view) const
        -> filter_view<std::decay_t<View>, Pred>
    {
        return filter_view<std::decay_t<View>, Pred>(
            std::forward<View>(view),
            pred
        );
    }
};

template<class Pred>
filter_adaptor<std::decay_t<Pred>> filtered(Pred&& pred)
{
    return filter_adaptor<std::decay_t<Pred>>(std::forward<Pred>(pred));
}

template<class Transform>
struct transform_adaptor : pipe_adaptor_base
{
    Transform transform;

    explicit transform_adaptor(Transform t)
        : transform(std::move(t))
    {
    }

    template<class View>
    auto operator()(View&& view) const
        -> transform_view<std::decay_t<View>, Transform>
    {
        return transform_view<std::decay_t<View>, Transform>(
            std::forward<View>(view),
            transform
        );
    }
};

template<class Transform>
transform_adaptor<std::decay_t<Transform>> transformed(Transform&& transform)
{
    return transform_adaptor<std::decay_t<Transform>>(std::forward<Transform>(transform));
}

template<
    class View,
    class Adaptor,
    std::enable_if_t<
        std::is_base_of_v<pipe_adaptor_base, std::decay_t<Adaptor>>,
        int
    > = 0
>
auto operator|(View&& view, Adaptor&& adaptor)
    -> decltype(std::forward<Adaptor>(adaptor)(std::forward<View>(view)))
{
    return std::forward<Adaptor>(adaptor)(std::forward<View>(view));
}

template<typename DstEncoder, typename View>
size_t measureView(const View& view)
{
    size_t total = 0;

    for (auto cp : view) {
        total += DstEncoder::codePointSize(cp);
    }

    return total;
}

template <typename DstEncoder, typename DstContainer, typename View>
void reencodeView(const View& view, DstContainer& dst)
{
    size_t required = measureView<DstEncoder, View>(view);

    dst.reserve(dst.size() + required);

    DstEncoder encoder;
    auto out = std::back_inserter(dst);

    for (lChar32 cp : view) {
        out = encoder.encode(cp, out);
    }
}
#endif // LV_UNICODE_CODE_POINT_HPP
