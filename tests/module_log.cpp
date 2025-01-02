// _______________ TEST FRAMEWORK & MODULE  _______________

#include <sstream>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "thirdparty/doctest.h"

#include "test.hpp"

#include "module_log.hpp"

// _______________________ INCLUDES _______________________

#include <array>         // testing stringification
#include <complex>       // testing stringification
#include <cstdint>       // testing stringification
#include <filesystem>    // testing stringification
#include <map>           // testing stringification
#include <set>           // testing stringification
#include <unordered_map> // testing stringification
#include <unordered_set> // testing stringification
#include <vector>        // testing stringification

// ____________________ DEVELOPER DOCS ____________________

// NOTE: DOCS

// ____________________ IMPLEMENTATION ____________________

using namespace utl;

// =============================
// --- Stringifier API tests ---
// =============================

TEST_CASE("Stringifier functor, static method and global function behave the same") {
    const auto value           = std::make_tuple(true, 'a', 1, 0.5, std::array{1, 2});
    const auto expected_result = "< true, a, 1, 0.5, { 1, 2 } >";

    CHECK(log::Stringifier{}(value) == expected_result);
    CHECK(log::Stringifier::stringify(value) == expected_result);
    CHECK(log::stringify(value) == expected_result);
}

// =============================
// --- Stringification tests ---
// =============================

TEST_CASE("Stringifier correctly handles bools") {
    CHECK(log::stringify(false) == "false");
    CHECK(log::stringify(true) == "true");
}

TEST_CASE("Stringifier correctly handles strings") {
    // Char
    CHECK(log::stringify('g') == "g");

    // String view-convertible
    CHECK(log::stringify("lorem ipsum") == "lorem ipsum");
    CHECK(log::stringify("lorem ipsum"s) == "lorem ipsum");
    CHECK(log::stringify("lorem ipsum"sv) == "lorem ipsum");

    // String-convertible
    CHECK(log::stringify(fs::path("lorem/ipsum")) == "lorem/ipsum");
}

TEST_CASE("Stringifier correctly handles integers") {
    CHECK(log::stringify(0) == "0");
    CHECK(log::stringify(-450) == "-450");

    CHECK(log::stringify(-17) == "-17");
    CHECK(log::stringify(17u) == "17");
    CHECK(log::stringify(-17l) == "-17");
    CHECK(log::stringify(17ul) == "17");
    CHECK(log::stringify(-17ll) == "-17");
    CHECK(log::stringify(17ull) == "17");

    CHECK(log::stringify(nlim<std::int8_t>::min()) == std::to_string(nlim<std::int8_t>::min()));
    CHECK(log::stringify(nlim<std::int16_t>::min()) == std::to_string(nlim<std::int16_t>::min()));
    CHECK(log::stringify(nlim<std::int32_t>::min()) == std::to_string(nlim<std::int32_t>::min()));
    CHECK(log::stringify(nlim<std::int64_t>::min()) == std::to_string(nlim<std::int64_t>::min()));

    CHECK(log::stringify(nlim<std::int8_t>::max()) == std::to_string(nlim<std::int8_t>::max()));
    CHECK(log::stringify(nlim<std::int16_t>::max()) == std::to_string(nlim<std::int16_t>::max()));
    CHECK(log::stringify(nlim<std::int32_t>::max()) == std::to_string(nlim<std::int32_t>::max()));
    CHECK(log::stringify(nlim<std::int64_t>::max()) == std::to_string(nlim<std::int64_t>::max()));

    CHECK(log::stringify(nlim<std::uint8_t>::min()) == std::to_string(nlim<std::uint8_t>::min()));
    CHECK(log::stringify(nlim<std::uint16_t>::min()) == std::to_string(nlim<std::uint16_t>::min()));
    CHECK(log::stringify(nlim<std::uint32_t>::min()) == std::to_string(nlim<std::uint32_t>::min()));
    CHECK(log::stringify(nlim<std::uint64_t>::min()) == std::to_string(nlim<std::uint64_t>::min()));

    CHECK(log::stringify(nlim<std::uint8_t>::max()) == std::to_string(nlim<std::uint8_t>::max()));
    CHECK(log::stringify(nlim<std::uint16_t>::max()) == std::to_string(nlim<std::uint16_t>::max()));
    CHECK(log::stringify(nlim<std::uint32_t>::max()) == std::to_string(nlim<std::uint32_t>::max()));
    CHECK(log::stringify(nlim<std::uint64_t>::max()) == std::to_string(nlim<std::uint64_t>::max()));
}

TEST_CASE("Stringifier correctly handles floats") {
    CHECK(log::stringify(0.5) == "0.5");
    CHECK(log::stringify(-1.5) == "-1.5");
    CHECK(log::stringify(0.) == "0");
    CHECK(!check_if_throws([] { return log::stringify(nlim<float>::max()); }));
    CHECK(!check_if_throws([] { return log::stringify(nlim<float>::min()); }));
    CHECK(!check_if_throws([] { return log::stringify(nlim<double>::max()); }));
    CHECK(!check_if_throws([] { return log::stringify(nlim<double>::min()); }));
    CHECK(!check_if_throws([] { return log::stringify(nlim<long double>::max()); }));
    CHECK(!check_if_throws([] { return log::stringify(nlim<long double>::min()); }));
}

TEST_CASE("Stringifier correctly handles complex") { CHECK(log::stringify(std::complex{1, 2}) == "1 + 2 i"); }

TEST_CASE("Stringifier correctly handles arrays") {
    CHECK(log::stringify(std::array{1, 2, 3}) == "{ 1, 2, 3 }");
    CHECK(log::stringify(std::vector{1, 2, 3}) == "{ 1, 2, 3 }");
    CHECK(log::stringify(std::set{1, 2, 3}) == "{ 1, 2, 3 }");
}

TEST_CASE("Stringifier correctly handles tuples") {
    CHECK(log::stringify(std::pair{1, 2}) == "< 1, 2 >");
    CHECK(log::stringify(std::tuple{"lorem", 2, "ipsum"}) == "< lorem, 2, ipsum >");
}

struct Printable {};

std::ostream& operator<<(std::ostream& os, Printable value) { return os << "printable_value"; }

TEST_CASE("Stringifier correctly handles printables") { CHECK(log::stringify(Printable{}) == "printable_value"); }

TEST_CASE("Stringifier correctly handles compound types") {
    CHECK(log::stringify(std::map{
              std::pair{"k1", 1},
              std::pair{"k2", 2}
    }) == "{ < k1, 1 >, < k2, 2 > }");
    CHECK(log::stringify(std::vector{
              std::vector{1, 2},
              std::vector{3}
    }) == "{ { 1, 2 }, { 3 } }");
    CHECK(log::stringify(std::vector<std::vector<std::vector<const char*>>>{{{"lorem"}}}) == "{ { { lorem } } }");
}

TEST_CASE("Stringifier correctly handles alignment wrappers") {
    // Left-padded values
    CHECK(log::stringify(log::PadLeft{"lorem", 10}) == "     lorem");
    CHECK(log::stringify(log::PadLeft{"lorem", 2}) == "lorem");

    // Right-padded values
    CHECK(log::stringify(log::PadRight{"lorem", 10}) == "lorem     ");
    CHECK(log::stringify(log::PadRight{"lorem", 2}) == "lorem");

    // Padded (center-aligned) values
    CHECK(log::stringify(log::Pad{"lorem", 9}) == "  lorem  ");
    CHECK(log::stringify(log::Pad{"lorem", 10}) == "  lorem   ");
    CHECK(log::stringify(log::Pad{"lorem", 2}) == "lorem");
}

// =======================================
// --- Stringifier customization tests ---
// =======================================

// Create customization of stringifier that also adds '$' on both sides of the floats.
template <class Derived>
struct DecoratedStringifierBase : public log::StringifierBase<Derived> {
    using base = log::StringifierBase<Derived>;

    template <class T>
    static void append_float(std::string& buffer, const T& value) {
        buffer += '$';
        base::append_float(buffer, value);
        buffer += '$';
    }
};

struct DecoratedStringifier : public DecoratedStringifierBase<DecoratedStringifier> {};

// Create another customization on top of prev. one so we can check that multi-layered logic works as expected
struct DoubleDecoratedStringifier : public DecoratedStringifierBase<DoubleDecoratedStringifier> {
    using base = DecoratedStringifierBase<DoubleDecoratedStringifier>;

    template <class T>
    static void append_float(std::string& buffer, const T& value) {
        buffer += '$';
        base::append_float(buffer, value);
        buffer += '$';
    }
};

TEST_CASE("Stringifier customization correctly decorates values") {
    CHECK(DecoratedStringifier{}(0.5) == "$0.5$");
    CHECK(DoubleDecoratedStringifier{}(0.5) == "$$0.5$$");

    CHECK(DecoratedStringifier{}(std::array{0.5, 1.5}) == "{ $0.5$, $1.5$ }");
}

// Create customization of stringifier that overrides formatting for specific type
struct OverridingStringifier : public log::StringifierBase<OverridingStringifier> {
    using base = log::StringifierBase<OverridingStringifier>;

    // Bring base class 'append()' here so we don't shadow it
    using base::append;

    // Declare overloads for types with specific formatting
    static void append(std::string& buffer, const std::vector<int>& value) {
        buffer += "integers: [ ";
        for (auto e : value) buffer += std::to_string(e) + " ";
        buffer += "]";
    }
};

TEST_CASE("Stringifier customization correctly overrides formatting of specific types") {
    CHECK(OverridingStringifier{}(std::array{1, 2, 3}) == "{ 1, 2, 3 }");
    CHECK(OverridingStringifier{}(std::vector{1, 2, 3}) == "integers: [ 1 2 3 ]");
    CHECK(OverridingStringifier{}(std::set{1, 2, 3}) == "{ 1, 2, 3 }");
}

// ===============================
// --- Logger formatting tests ---
// ===============================

TEST_CASE("Logger correctly formats deterministic columns") {
    std::ostringstream oss;
    
    // Now is also a goot time to test sink setters
    auto& sink = log::add_ostream_sink(oss);
    // sink.set_colors(log::Colors::DISABLE);
    
    const auto expected_output = "";
    
    // CHECK(oss.str() == expected_output);
}