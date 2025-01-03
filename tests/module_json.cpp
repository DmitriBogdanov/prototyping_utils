// _______________ TEST FRAMEWORK & MODULE  _______________

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "thirdparty/doctest.h"

#include "module_json.hpp"

// _______________________ INCLUDES _______________________

#include <array>            // testing JSON array conversion
#include <deque>            // testing JSON array conversion
#include <filesystem>       // iteration over the test suite files
#include <forward_list>     // testing JSON array conversion
#include <initializer_list> // testing JSON array conversion
#include <list>             // testing JSON array conversion
#include <set>              // testing JSON array conversion
#include <vector>           // testing JSON array conversion

// ____________________ DEVELOPER DOCS ____________________

// NOTE: DOCS

// ____________________ IMPLEMENTATION ____________________

using namespace utl;
namespace fs = std::filesystem;

template <class Func>
bool check_if_throws(Func f) {
    bool throws = false;
    try {
        f();
    } catch (...) { throws = true; }
    return throws;
}

// ==================================
// --- RFC-8259 conformance tests ---
// ==================================

TEST_CASE("JSON validation test suite (accept)") {
    const fs::path test_suite_path = "tests/data/json_test_suite/should_accept/";

    std::cout << "Running test case from path: " << fs::current_path() << '\n' << std::flush;

    for (const auto& test_suite_entry : fs::directory_iterator(test_suite_path)) {
        std::cout << "Parsing file named: " << test_suite_entry.path() << '\n' << std::flush;

        const bool throws = check_if_throws([&]() { json::from_file(test_suite_entry.path()); });

        CHECK(!throws);
    }
}

TEST_CASE("JSON validation test suite (reject)") {
    const fs::path test_suite_path = "tests/data/json_test_suite/should_reject/";

    std::cout << "Running test case from path: " << fs::current_path() << '\n' << std::flush;

    // JSON specification is rather strict when it comes to numeric format, not allowing a lot of the valid IEEE 754
    // floats that work fine with C++ formatting facilities. Since there is no real benefit in prohibiting the parsing
    // of such values, we explicitly mark them as "acceptable to parse".
    const std::unordered_set<std::string> acceptable_cases = {"n_number_0.e1.json",
                                                              "n_number_2.e3.json",
                                                              "n_number_2.e+3.json",
                                                              "n_number_2.e-3.json",
                                                              "n_number_-01.json",
                                                              "n_number_-2..json",
                                                              "n_number_-NaN.json",
                                                              "n_number_minus_infinity.json",
                                                              "n_number_neg_int_starting_with_zero.json",
                                                              "n_number_neg_real_without_int_part.json",
                                                              "n_number_real_without_fractional_part.json",
                                                              "n_number_with_leading_zero.json"};

    for (const auto& test_suite_entry : fs::directory_iterator(test_suite_path)) {
        std::cout << "Parsing file named: " << test_suite_entry.path() << '\n' << std::flush;

        const bool throws = check_if_throws([&]() { json::from_file(test_suite_entry.path()); });

        if (!acceptable_cases.count(test_suite_entry.path().filename())) CHECK(throws);
    }
}

// =============================
// --- Type conversion tests ---
// =============================

TEST_CASE_TEMPLATE("To-JSON-array conversions", T, //
                   json::Array,                    //
                   std::vector<int>,               //
                   std::list<int>,                 //
                   std::initializer_list<int>,     //
                   std::deque<int>,                //
                   std::forward_list<int>,         //
                   std::set<int>                   //
) {
    json::Node json;
    json["array"] = T{1, 2, 3};

    const auto& arr = json.at("array").get_array(); // will throw if node is not an array

    CHECK(arr.size() == 3);

    int expected_value = 0;
    for (auto it = arr.begin(); it != arr.end(); ++it) CHECK(it->get_number() == ++expected_value);
}

TEST_CASE("To-JSON-array conversions (multidimensional arrays)") {
    json::Node json;
    json["array_1D"] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    json["array_2D"] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    json["array_3D"] = {
        {{1, 2},    {3, 4}},
        {{5, 6}, {7, 8, 9}}
    };

    CHECK(json.at("array_1D").to_string(json::Format::MINIMIZED) == "[1,2,3,4,5,6,7,8,9]");
    CHECK(json.at("array_2D").to_string(json::Format::MINIMIZED) == "[[1,2,3],[4,5,6],[7,8,9]]");
    CHECK(json.at("array_3D").to_string(json::Format::MINIMIZED) == "[[[1,2],[3,4]],[[5,6],[7,8,9]]]");
}

TEST_CASE_TEMPLATE("To-JSON-object conversions", T,     //
                   json::Object,                        //
                   std::map<std::string, int>,          //
                   std::unordered_map<std::string, int> //
) {
    json::Node json;
    json["object"] = T{
        {"key_1", 1},
        {"key_2", 2}
    };

    const auto& obj = json.at("object").get_object(); // will throw if node is not an array

    CHECK(obj.size() == 2);
    CHECK(obj.at("key_1").get_number() == 1);
    CHECK(obj.at("key_2").get_number() == 2);
}

TEST_CASE_TEMPLATE("To-JSON-string conversions", T, //
                   json::String,                    //
                   std::string,                     //
                   std::string_view                 //
) {
    json::Node json;
    json["string"] = T{"lorem ipsum"};
    CHECK(json.at("string").get_string() == "lorem ipsum");
}

TEST_CASE("To-JSON-string conversions (literals)") {
    json::Node json;
    json["string"] = "lorem ipsum";
    CHECK(json.at("string").get_string() == "lorem ipsum");
}

TEST_CASE_TEMPLATE("To-JSON-number conversions", T, //
                   json::Number,                    //
                   float,                           //
                   double,                          //
                   int,                             //
                   unsigned int,                    //
                   std::size_t,                     //
                   long double,                     //
                   char                             //
) {
    json::Node json;
    json["number"] = T(2);
    CHECK(json.at("number").get_number() == 2);
}

TEST_CASE_TEMPLATE("To-JSON-bool conversions", T, //
                   json::Bool,                    //
                   bool                           //
) {
    json::Node json;
    json["bool"] = T(true);
    CHECK(json.at("bool").get_bool() == true);
}

TEST_CASE_TEMPLATE("To-JSON-null conversions", T, //
                   json::Null                     //
) {
    json::Node json;
    json["null"] = T();
    CHECK(json.at("null").get_null() == json::Null());
    // nothing else converts to 'Null' so the only thing we really test is that 'Null == Null' works
}

// =============================
// --- Object node API tests ---
// =============================

TEST_CASE_TEMPLATE("JSON node can be implicitly converted to object only for 'Null'", T, //
                   json::Object,
                   json::Array,
                   json::String,
                   json::Number,
                   json::Bool,
                   json::Null
) {
    json::Node json = T();
    
    const bool throws = check_if_throws([&]() { json["key"] = "value"; });
    
    // Only 'Null' converts to 'Object' implicitly after 'operator[]'
    if (json.is_null() || json.is_object()) {
        CHECK(!throws);
        CHECK(json.is_object());
    }
    // other types should throw on conversion
    else {
        CHECK(throws);
    }
}

TEST_CASE("JSON object node API basics work as intended"
) {
    auto json = json::from_string(R"(
        {
            "string": "lorem ipsum",
            "number": 17,
            "null": null
        }
    )");
    
    CHECK(check_if_throws([&](){ auto val = json.at("non_existent_key"); }));
    CHECK(json.contains("string"));
    CHECK(json.at("string").get_string() == "lorem ipsum");
    CHECK(json.value_or("number", -5.) == 17.);
    CHECK(json.value_or("non_existent_key", -5.) == -5.);
}