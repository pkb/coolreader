/***************************************************************************
 *   CoolReader engine                                                     *
 *   Copyright (C) 2007,2009,2012,2013 Vadim Lopatin <coolreader.org@gmail.com>
 *   Copyright (C) 2018,2020 Aleksey Chernov <valexlin@gmail.com>          *
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

#ifndef __LV_STRINGCOLLECTION_H_INCLUDED__
#define __LV_STRINGCOLLECTION_H_INCLUDED__

#include "lvstring.h"

#include <vector>
#include <algorithm>

template <typename StringT>
class lStringCollection
{
private:
    std::vector<StringT> strings;

public:
    lStringCollection() = default;

    lStringCollection(const lStringCollection& src) = default;

    lStringCollection(const StringT& str, const StringT& delimiter)
    {
        split(str, delimiter);
    }

    void reserve(int space)
    {
        strings.reserve(static_cast<size_t>(space));
    }

    int add(const StringT& str)
    {
        strings.push_back(str);
        return static_cast<int>(strings.size());
    }

    int add(const char* str)
    {
        return add(StringT(str));
    }

    void addAll(const lStringCollection& src)
    {
        strings.insert(strings.end(), src.strings.begin(), src.strings.end());
    }

    // Calculate a hash value for the collection (combines hashes of all strings)
    lUInt32 getHash() const
    {
        lUInt32 hash = 0;
        for (const auto& s : strings)
        {
            hash = hash * 31 + s.getHash();
        }
        return hash;
    }

    // Split a string by delimiter and add the resulting substrings
    void split(const StringT& str, const StringT& delimiter)
    {
        if (str.empty() || delimiter.empty())
            return;

        size_t startpos = 0;
        while (startpos < str.length())
        {
            size_t pos = str.find(delimiter, startpos);
            if (pos == std::string::npos)
                pos = str.length();

            // Emplace substring from startpos to pos (exclusive)
            strings.emplace_back(str, startpos, pos - startpos);

            startpos = pos + delimiter.length();
        }
    }

    void addToken(StringT token, bool flgTrim)
    {
        if ( flgTrim )
            token.trimDoubleSpaces(false, false, false);
        if ( !flgTrim || !token.empty() )
            add( token );
    }

    void parse(StringT str, lChar8 delimiter, bool flgTrim )
    {
        size_t start = 0;
        size_t end = str.find(delimiter);

        while (end != std::string::npos)
        {
            addToken(str.substr(start, end - start), flgTrim);
            start = end + 1;
            end = str.find(delimiter, start);
        }
        addToken(str.substr(start), flgTrim);
    }

    /// parse delimiter-separated string
    void parse(StringT str, StringT delimiter, bool flgTrim )
    {
        if (delimiter.empty())
        {
            addToken(str, flgTrim);
            return;
        }

        size_t start = 0;
        size_t end = str.find(delimiter);

        while (end != std::string::npos)
        {
            addToken(str.substr(start, end - start), flgTrim);
            start = end + delimiter.length();
            end = str.find(delimiter, start);
        }
        addToken(str.substr(start), flgTrim);
    }

    // Remove count elements starting at offset
    void erase(int offset, int count)
    {
        if (offset < 0 || count <= 0 || offset >= static_cast<int>(strings.size()))
            return;
        size_t pos = static_cast<size_t>(offset);
        size_t len = static_cast<size_t>(count);
        if (pos + len > strings.size())
            len = strings.size() - pos;
        strings.erase(strings.begin() + pos, strings.begin() + pos + len);
    }

    const StringT& at(int index) const
    {
        return strings.at(static_cast<size_t>(index));
    }

    const StringT& operator[](int index) const
    {
        return strings[static_cast<size_t>(index)];
    }

    StringT& operator[](int index)
    {
        return strings[static_cast<size_t>(index)];
    }

    lStringCollection& operator=(const lStringCollection& other) = default;

    bool operator==(const lStringCollection& other) const
    {
        return strings == other.strings;
    }

    bool contains( StringT value )
    {
        auto pos = std::find(strings.begin(), strings.end(), value);

        return pos != strings.end();
    }

    int insert( int pos, const StringT & str )
    {
        if (pos<0 || pos>=length())
            return add(str);
        strings.insert(strings.begin() + pos, str);
        return static_cast<int>(strings.size());
    }

    void sort()
    {
        std::sort(strings.begin(), strings.end());
    }

    void sort(int(comparator)(StringT& s1, StringT & s2))
    {
        std::sort(strings.begin(), strings.end(),
            [comparator](StringT& str1, StringT& str2) {
                 return comparator(str1, str2) < 0;
            }
        );
    }

    bool operator!=(const lStringCollection& other) const
    {
        return !(*this == other);
    }

    int length() const
    {
        return static_cast<int>(strings.size());
    }

    void clear()
    {
        strings.clear();
    }

    bool empty() const
    {
        return strings.empty();
    }

    ~lStringCollection() = default;
};

#endif //_LV_STRINGCOLLECTION_H_INCLUDED__
