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

// Range utilities

#ifndef OGONEK_DETAIL_RANGES_HPP
#define OGONEK_DETAIL_RANGES_HPP

#include <ogonek/types.h++>
#include <ogonek/traits.h++>
#include <ogonek/error.h++>

#include <wheels/meta.h++>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/const_iterator.hpp>
#include <boost/range/mutable_iterator.hpp>

#include <iterator>

namespace ogonek {
    template <typename EncodingForm, typename Iterator, typename ErrorHandler>
    struct encoding_iterator;
    template <typename EncodingForm, typename Iterator, typename ErrorHandler>
    struct decoding_iterator;

    namespace detail {
        template <typename Container>
        using Iterator = typename Container::iterator;
        template <typename Container>
        using ConstIterator = typename Container::const_iterator;
        template <typename Container>
        using ValueType = typename Container::value_type;
        template <typename Iterator>
        using IteratorValue = typename std::iterator_traits<Iterator>::value_type;

        template <typename T>
        struct range_iterator
        : wheels::Conditional<std::is_const<T>,
                              boost::range_const_iterator<wheels::RemoveConst<T>>,
                              boost::range_mutable_iterator<T>> {};
        
        template <typename Range>
        using RangeIterator = typename range_iterator<Range>::type;
        template <typename Range>
        using RangeConstIterator = RangeIterator<Range const>;
    } // namespace detail

    template <typename EncodingForm, typename Range, typename ErrorHandler>
    using encoding_range = boost::iterator_range<encoding_iterator<EncodingForm, detail::RangeIterator<Range>, ErrorHandler>>;
    template <typename EncodingForm, typename Range, typename ErrorHandler>
    using decoding_range = boost::iterator_range<decoding_iterator<EncodingForm, detail::RangeIterator<Range>, ErrorHandler>>;

    class utf16;
    class utf32;

    namespace detail {
        struct range_value_tester {
            template <typename T, typename = IteratorValue<RangeIterator<T>>>
            std::true_type static test(int);
            template <typename>
            std::false_type static test(...);
        };
        template <typename Range>
        using has_range_value = wheels::TraitOf<range_value_tester, Range>;
        template <typename Range>
        using RangeValue = IteratorValue<RangeIterator<Range>>;

        template <typename Range, typename Value, bool = has_range_value<Range>()>
        struct is_range_of : std::is_same<RangeValue<Range>, Value> {};
        template <typename Range, typename Value>
        struct is_range_of<Range, Value, false> : std::false_type {};
        
        template <typename T>
        struct is_unicode_sequence_impl : is_range_of<T, code_point> {};
        template <std::size_t N>
        struct is_unicode_sequence_impl<char32_t[N]> : std::true_type {};
        template <>
        struct is_unicode_sequence_impl<char32_t const*> : std::true_type {};
        template <std::size_t N>
        struct is_unicode_sequence_impl<char16_t[N]> : std::true_type {};
        template <>
        struct is_unicode_sequence_impl<char16_t const*> : std::true_type {};
    
        template <typename T>
        struct is_unicode_sequence : detail::is_unicode_sequence_impl<wheels::Unqualified<T>> {};

        template <typename Iterator, typename Range>
        boost::iterator_range<Iterator> wrap_range(Range&& range) {
            return {
                Iterator { boost::begin(range), boost::end(range) },
                Iterator { boost::end(range), boost::end(range) }
            };
        }
        
        template <typename Char>
        struct null_terminated_range_iterator
        : boost::iterator_facade<
            null_terminated_range_iterator<Char>,
            Char,
            std::bidirectional_iterator_tag
        > {
        public:
            null_terminated_range_iterator() : ptr(nullptr) {}
            null_terminated_range_iterator(Char* ptr) : ptr(ptr) {}

        private:
            friend class boost::iterator_core_access;

            Char& dereference() const { return *ptr; }
            void increment() { ++ptr; }
            void decrement() { --ptr; }
            void advance(std::size_t n) { ptr += n; }
            std::ptrdiff_t distance_to(null_terminated_range_iterator const& that) const {
                return that.ptr - ptr;
            }

            bool equal(null_terminated_range_iterator const& that) const {
                return ptr == that.ptr
                    || (is_end() && that.is_end());
            }

            bool is_end() const { return !ptr || !*ptr; }

            Char* ptr;
        };

        template <typename Char>
        using null_terminated_range = boost::iterator_range<null_terminated_range_iterator<Char>>;
        
        template <typename Char>
        null_terminated_range<Char> null_terminated(Char* str) {
                return null_terminated_range<Char>(str, {});
        }
        template <typename ErrorHandler = default_error_handler_t>
        using null_terminated_utf16 = decoding_range<utf16, null_terminated_range<char16_t const>, ErrorHandler>;
        template <typename ErrorHandler = default_error_handler_t>
        using null_terminated_utf32 = decoding_range<utf32, null_terminated_range<char32_t const>, ErrorHandler>;
        
        template <typename UnicodeSequence, typename ErrorHandler,
                  wheels::EnableIf<is_unicode_sequence<UnicodeSequence>>...,
                  wheels::EnableIf<always_validated<UnicodeSequence>>...,
                  wheels::EnableIf<is_error_handler<ErrorHandler>>...>
        UnicodeSequence&& as_code_point_range(UnicodeSequence&& sequence, ErrorHandler) {
            return std::forward<UnicodeSequence>(sequence);
        }
        template <typename UnicodeSequence, typename ErrorHandler,
                  wheels::EnableIf<is_unicode_sequence<UnicodeSequence>>...,
                  wheels::DisableIf<always_validated<UnicodeSequence>>...,
                  wheels::EnableIf<is_error_handler<ErrorHandler>>...,
                  typename Iterator = decoding_iterator<utf32, RangeIterator<wheels::RemoveReference<UnicodeSequence>>, ErrorHandler>>
        boost::iterator_range<Iterator> as_code_point_range(UnicodeSequence&& sequence, ErrorHandler) {
            return wrap_range<Iterator>(sequence);
        }
        template <typename UnicodeSequence,
                  wheels::EnableIf<is_unicode_sequence<UnicodeSequence>>...>
        UnicodeSequence&& as_code_point_range(UnicodeSequence&& sequence, assume_valid_t) {
            return std::forward<UnicodeSequence>(sequence);
        }
        template <typename ErrorHandler>
        null_terminated_utf16<ErrorHandler> as_code_point_range(char16_t const* sequence, ErrorHandler);
        template <typename ErrorHandler>
        null_terminated_utf32<ErrorHandler> as_code_point_range(char32_t const* sequence, ErrorHandler);
        null_terminated_range<char32_t const> as_code_point_range(char32_t const* sequence, assume_valid_t);
        
        template <typename UnicodeSequence, typename ErrorHandler = default_error_handler_t>
        using UnicodeSequenceIterator = decltype(boost::begin(as_code_point_range(std::declval<UnicodeSequence>(), ErrorHandler{})));
        
        struct validated_tag {
            static constexpr bool validated = true;
        };
        template <typename NormalForm>
        struct normalized_tag : validated_tag {
            using normal_form = NormalForm;
        };
        struct casefolded_tag : validated_tag {
            static constexpr bool casefolded = true;
        };

        template <typename Iterator, typename... Tags>
        struct tagged_iterator_range
        : boost::iterator_range<Iterator>, Tags... {
            tagged_iterator_range(Iterator first, Iterator last)
            : boost::iterator_range<Iterator>(first, last) {}
        };

        template <typename Iterator, typename... Tags, typename Range>
        tagged_iterator_range<Iterator, Tags...> wrap_tagged_range(Range&& range) {
            return {
                Iterator { boost::begin(range), boost::end(range) },
                Iterator { boost::end(range), boost::end(range) }
            };
        }
        
    } // namespace detail
} // namespace ogonek

#endif // OGONEK_DETAIL_RANGES_HPP
