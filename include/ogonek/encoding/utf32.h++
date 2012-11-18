// Ogonek
//
// Written in 2012 by Martinho Fernandes <martinho.fernandes@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

// UTF-32 encoding form

#ifndef OGONEK_ENCODING_UTF32_HPP
#define OGONEK_ENCODING_UTF32_HPP

#include "iterator.h++"
#include "../types.h++"
#include "../validation.h++"
#include "../detail/partial_array.h++"

#include <boost/range/sub_range.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <utility>

namespace ogonek {
    struct utf32 {
        using code_unit = char32_t;
        static constexpr bool is_fixed_width = true;
        static constexpr std::size_t max_width = 1;
        static constexpr bool is_self_synchronizing = true;
        struct state {};

        template <typename SinglePassRange, typename ValidationPolicy,
                  typename Iterator = typename boost::range_const_iterator<SinglePassRange>::type,
                  typename EncodingIterator = encoding_iterator<utf32, Iterator, ValidationPolicy>>
        static boost::iterator_range<EncodingIterator> encode(SinglePassRange const& r, ValidationPolicy) {
            return boost::make_iterator_range(
                    EncodingIterator { boost::begin(r), boost::end(r) },
                    EncodingIterator { boost::end(r), boost::end(r) });
        }

        template <typename SinglePassRange, typename ValidationPolicy,
                  typename Iterator = typename boost::range_const_iterator<SinglePassRange>::type,
                  typename DecodingIterator = decoding_iterator<utf32, Iterator, ValidationPolicy>>
        static boost::iterator_range<DecodingIterator> decode(SinglePassRange const& r, ValidationPolicy) {
            return boost::make_iterator_range(
                    DecodingIterator { boost::begin(r), boost::end(r) },
                    DecodingIterator { boost::end(r), boost::end(r) });
        }

        template <typename ValidationPolicy>
        static detail::coded_character<utf32> encode_one(codepoint u, state&, ValidationPolicy) {
            return { u };
        }
        template <typename SinglePassRange>
        static boost::sub_range<SinglePassRange> decode_one(SinglePassRange const& r, codepoint& out, state&, skip_validation_t) {
            auto first = boost::begin(r);
            out = *first++;
            return { first, boost::end(r) };
        }
        template <typename SinglePassRange, typename ValidationPolicy>
        static boost::sub_range<SinglePassRange> decode_one(SinglePassRange const& r, codepoint& out, state& s, ValidationPolicy) {
            auto first = boost::begin(r);
            auto u = *first++;

            auto is_surrogate = [](codepoint u) { return u >= 0xD800 && u <= 0xDFFF; };

            if(u > 0x10FFFF || is_surrogate(u)) {
                return ValidationPolicy::template apply_decode<utf32>(r, s, out);
            }

            out = u;
            return { first, boost::begin(r) };
        }
    };
} // namespace ogonek

#endif // OGONEK_ENCODING_UTF32_HPP

