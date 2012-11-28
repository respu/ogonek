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

// Tests for <ogonek/encoding/latin1.h++>

#include <ogonek/encoding/latin1.h++>
#include <ogonek/types.h++>

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

#include <catch.h++>

TEST_CASE("latin1", "ISO-8859-1 encoding form") {
    using namespace ogonek::literal;

    SECTION("encode", "Encoding ISO-8859-1") {
        auto decoded = { 0x0041_u, 0x0082_u };
        auto range = ogonek::latin1::encode(decoded, ogonek::skip_validation);
        std::vector<ogonek::byte> encoded(boost::begin(range), boost::end(range));
        REQUIRE(encoded.size() == 2);
        CHECK(encoded[0] == 0x41_b);
        CHECK(encoded[1] == 0x82_b);
    }
    SECTION("decode", "Decoding ISO-8859-1") {
        auto encoded = { 0x41_b, 0x82_b };
        auto range = ogonek::latin1::decode(encoded, ogonek::skip_validation);
        std::vector<ogonek::codepoint> decoded(boost::begin(range), boost::end(range));
        REQUIRE(decoded.size() == 2);
        CHECK(decoded[0] == U'\x0041');
        CHECK(decoded[1] == U'\x0082');
    }
    SECTION("validation", "Validating ISO-8859-1") {
        auto encoded = { 0x41_b, 0x82_b, 0xFF_b };
        auto range = ogonek::latin1::decode(encoded, ogonek::use_replacement_character);
        std::vector<ogonek::codepoint> decoded(boost::begin(range), boost::end(range));
        REQUIRE(decoded.size() == 3);
        CHECK(decoded[0] == U'\x0041');
        CHECK(decoded[1] == U'\x0082');
        CHECK(decoded[2] == U'\x00FF');
    }
    SECTION("replacement", "ISO-8859-1's custom replacement character (?)") {
        auto decoded = { 0x41_u, 0x32_u, 0x1F4A9_u };
        auto range = ogonek::latin1::encode(decoded, ogonek::use_replacement_character);
        std::vector<ogonek::latin1::code_unit> encoded(boost::begin(range), boost::end(range));
        REQUIRE(encoded.size() == 3);
        CHECK(encoded[0] == 0x41_b);
        CHECK(encoded[1] == 0x32_b);
        CHECK(encoded[2] == '?');
    }
}


