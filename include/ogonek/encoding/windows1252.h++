// Ogonek
//
// Written in 2013 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

// This file was automatically generated on 2013-03-11T05:08:08.0265100Z

// Windows-1252 encoding form

#ifndef OGONEK_WINDOWS1252_HPP
#define OGONEK_WINDOWS1252_HPP

#include <ogonek/types.h++>
#include <ogonek/encoding/codepage_encoding.h++>

namespace ogonek {
    struct windows1252_codepage {
        static code_point to_unicode[256];
        static codepage_entry from_unicode[251];
    };

    using windows1252 = codepage_encoding<windows1252_codepage>;
} // namespace ogonek
#endif // OGONEK_WINDOWS1252_HPP

