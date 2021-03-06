// Ogonek
//
// Written in 2012-2013 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

// Wide encoding form

#ifndef OGONEK_ENCODING_WIDE_HPP
#define OGONEK_ENCODING_WIDE_HPP

#include <ogonek/encoding/utf16.h++>
#include <ogonek/encoding/utf32.h++>

#include <wheels/meta.h++>

#include <type_traits>

namespace ogonek {
    struct wide : wheels::Conditional<sizeof(wchar_t)==2, utf16, utf32> {
        using code_unit = wchar_t;
    };
} // namespace ogonek

#endif // OGONEK_ENCODING_WIDE_HPP
