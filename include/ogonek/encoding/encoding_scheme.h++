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

// Generic encoding scheme

#ifndef OGONEK_ENCODING_ENCODING_SCHEME_HPP
#define OGONEK_ENCODING_ENCODING_SCHEME_HPP

#include <ogonek/encoding/iterator.h++>
#include <ogonek/types.h++>
#include <ogonek/detail/ranges.h++>
#include <ogonek/detail/container/partial_array.h++>
#include <ogonek/encoding/detail/encoded_character.h++>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/sub_range.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/empty.hpp>

#include <cstdint>
#include <iterator>
#include <type_traits>

namespace ogonek {
    namespace detail {
        template <typename T, std::size_t N = sizeof(T)>
        struct uint;
        template <typename T>
        struct uint<T, 1> { using type = std::uint8_t; };
        template <typename T>
        struct uint<T, 2> { using type = std::uint16_t; };
        template <typename T>
        struct uint<T, 4> { using type = std::uint32_t; };
        template <typename T>
        struct uint<T, 8> { using type = std::uint64_t; };
        template <typename T>
        using Uint = typename uint<T>::type;

        template <typename ByteOrder, typename Integer, typename Iterator>
        struct byte_ordered_iterator // TODO: support input iterators
        : boost::iterator_facade<
            byte_ordered_iterator<ByteOrder, Integer, Iterator>,
            Integer,
            std::forward_iterator_tag,
            Integer
        > {
            byte_ordered_iterator(Iterator it) : it(it) {}
            Integer dereference() const {
                Integer i;
                ByteOrder::unmap(it, i);
                return i;
            }
            bool equal(byte_ordered_iterator const& that) const {
                return it == that.it;
            }
            void increment() {
                Integer dummy;
                it = ByteOrder::unmap(it, dummy);
            }

            Iterator it;
        };

        template <typename ByteOrder, typename Integer, typename Range>
        using byte_ordered_range = boost::iterator_range<
                                       byte_ordered_iterator<
                                           ByteOrder,
                                           Integer,
                                           typename boost::range_iterator<Range>::type>>;
    } // namespace detail

    template <typename EncodingForm, typename ByteOrder>
    struct encoding_scheme {
        static constexpr bool is_fixed_width = EncodingForm::is_fixed_width;
        static constexpr std::size_t max_width = EncodingForm::max_width * sizeof(typename EncodingForm::code_unit);
        using state = typename EncodingForm::state;
        using code_unit = ogonek::byte;

        template <typename ErrorHandler>
        static detail::encoded_character<encoding_scheme<EncodingForm, ByteOrder>> encode_one(code_point u, state& s, ErrorHandler) {
            std::array<byte, max_width> result;
            auto encoded = EncodingForm::encode_one(u, s, ErrorHandler{});
            auto out = result.begin();
            for(auto it = encoded.begin(); it != encoded.end(); ++it) {
                auto bytes = ByteOrder::map(static_cast<detail::Uint<CodeUnit<EncodingForm>>>(*it));
                out = std::copy(bytes.begin(), bytes.end(), out);
            }
            return { result, std::size_t(out - result.begin()) };
        }
        template <typename Range, typename ErrorHandler>
        static boost::sub_range<Range> decode_one(Range const& r, code_point& out, state& s, ErrorHandler) {
            using code_unit_range = detail::byte_ordered_range<ByteOrder, typename EncodingForm::code_unit, Range>;
            using iterator = typename boost::range_iterator<code_unit_range>::type;
            code_unit_range range {
                iterator { boost::begin(r) }, iterator { boost::end(r) }
            };
            auto remaining = EncodingForm::decode_one(range, out, s, ErrorHandler{});
            return { remaining.begin().it, r.end() };
        }
    };
} // namespace ogonek

#endif // OGONEK_ENCODING_ENCODING_SCHEME_HPP

