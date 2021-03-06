// Ogonek
//
// Written in 2012-2013 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
//
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

// Array slice

#ifndef OGONEK_DETAIL_ARRAY_SLICE_HPP
#define OGONEK_DETAIL_ARRAY_SLICE_HPP

#include <boost/range/iterator_range.hpp>

namespace ogonek {
    namespace detail {
        template <typename T>
        using array_slice = boost::iterator_range<T*>;
    } // namespace detail
} // namespace ogonek

#endif // OGONEK_DETAIL_ARRAY_SLICE_HPP

