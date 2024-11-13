// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Macro-Module:  UTL_DEFINE
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/MACRO_DEFINE.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMACRO_DEFINE)
#ifndef UTLHEADERGUARD_DEFINE
#define UTLHEADERGUARD_DEFINE

// _______________________ INCLUDES _______________________

#include <algorithm>   // fill_n()
#include <cctype>      // isspace()
#include <cstdlib>     // exit()
#include <iostream>    // cerr
#include <iterator>    // ostreambuf_iterator<>
#include <ostream>     // endl
#include <sstream>     // istringstream
#include <string>      // string, getline()
#include <string_view> // string_view
#include <type_traits> // remove_const<>
#include <utility>     // declval<>()

// ____________________ DEVELOPER DOCS ____________________

// Macros for automatic codegen, such as codegen for type traits, loops,
// operators, enum bitflags, enum <=> string conversions and etc.
//
// Implementation can be rather sketchy at times due to trying to do things that
// aren't really supposed to be done, but nothing straight-up illegal.
//
// # UTL_DEFINE_CURRENT_OS_STRING #
// Evaluates to current platform name detected through compiler macros. Can evaluate to:
// "Windows64", "Windows32", "Windows (CYGWIN)", "Android", "Linux", "Unix-like OS", "MacOS", ""
//
// # UTL_DEFINE_IS_DEBUG #
// Resolves to 'true' in debug mode and to 'false' in release.
// Useful for some debug-only 'contexpr if ()' expressions.
//
// # UTL_DEFINE_REPEAT(N) #
// Equivalent to 'for(int i=0; i<N; ++i)'
//
// # UTL_DEFINE_VA_ARGS_COUNT(args...) #
// Counts how many comma-separated arguments are passed. Works in a true 'preprocessor' evaluating arguments
// as arbitrary text that doesn't have to be a valid identifier. Useful for a lot of variadic macros.
// Note: Since macro evaluates arbitrary preprocessor text, language construct with additional comma should
// be surrounded by parentheses aka '(std::pair<int, int>{4, 5})', since 'std::pair<int, int>{4, 5}' gets
// interpreted as 3 separate args 'std::pair<int', ' int>{4', ' 5}' by any variadic macro.
//
// # UTL_DEFINE_ENUM_WITH_STRING_CONVERSION(enum_name, enum_values...) #
// Create enum 'enum_name' with given enum_values and methods 'to_string()', 'from_string()' inside 'enum_name'
// namespace. Note: Enums can't be declared inside functions.
//
// # UTL_DEFINE_IS_FUNCTION_PRESENT(function_name, return_type, argumet_types...) #
// Creates integral constant named 'is_function_present_{function_name}' that
// returns in "::value" whether function with given name and mask is a valid identifier.
// Useful for detecting existance of platform-specific methods.
// Note: This is kinda pushing the limits of compiler, but possible through SFINAE, which is the only
// mechanism that can perform that kind check. This is the reason implementing an inline FUNC_EXISTS(...)
// kind of macro is impossible and integral constant has to be created.
//
// # UTL_DEFINE_EXIT(message, exit_code) #
// Performs 'std::exit()' with some additional decorators. Displays formatted exit message, exit code,
// file, function and line of callsite to std::cerr. If ommited replaces message & exit_code with default values.

// ____________________ IMPLEMENTATION ____________________

// ===============================
// --- Compilation Info Macros ---
// ===============================

// - Detect OS -
#if defined(_WIN64) // _WIN64 implies _WIN32 so it should be first
#define UTL_DEFINE_CURRENT_OS_STRING "Windows64"
#elif defined(_WIN32)
#define UTL_DEFINE_CURRENT_OS_STRING "Windows32"
#elif defined(__CYGWIN__) && !defined(_WIN32)
#define UTL_DEFINE_CURRENT_OS_STRING "Windows (CYGWIN)" // Cygwin POSIX under Microsoft Window
#elif defined(__ANDROID__)                              // __ANDROID__ implies __linux__ so it should be first
#define UTL_DEFINE_CURRENT_OS_STRING "Android"
#elif defined(__linux__)
#define UTL_DEFINE_CURRENT_OS_STRING "Linux"
#elif defined(unix) || defined(__unix__) || defined(__unix)
#define UTL_DEFINE_CURRENT_OS_STRING "Unix-like OS"
#elif defined(__APPLE__) && defined(__MACH__)
#define UTL_DEFINE_CURRENT_OS_STRING "MacOS" // Apple OSX and iOS (Darwin)
#else
#define UTL_DEFINE_CURRENT_OS_STRING ""
#endif

// - Debug as bool constant -
#ifdef _DEBUG
#define UTL_DEFINE_IS_DEBUG true
#else
#define UTL_DEFINE_IS_DEBUG false
#endif

// =========================
// --- Automatic Codegen ---
// =========================

//  Repeat loop -
#define UTL_DEFINE_REPEAT(repeats_)                                                                                    \
    for (std::remove_const<decltype(repeats_)>::type count_ = 0; count_ < repeats_; ++count_)

// - Size of __VA_ARGS__ in variadic macros -
#define _utl_expand_va_args(x_) x_ // a fix for MSVC bug not expanding __VA_ARGS__ properly

#define _utl_va_args_count_impl(x01_, x02_, x03_, x04_, x05_, x06_, x07_, x08_, x09_, x10_, x11_, x12_, x13_, x14_,    \
                                x15_, x16_, x17_, x18_, x19_, x20_, x21_, x22_, x23_, x24_, x25_, x26_, x27_, x28_,    \
                                x29_, x30_, x31_, x32_, x33_, x34_, x35_, x36_, x37_, x38_, x39_, x40_, x41_, x42_,    \
                                x43_, x44_, x45_, x46_, x47_, x48_, x49_, N_, ...)                                     \
    N_

#define UTL_DEFINE_VA_ARGS_COUNT(...)                                                                                  \
    _utl_expand_va_args(_utl_va_args_count_impl(__VA_ARGS__, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36,   \
                                                35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19,    \
                                                18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

// - Enum with string conversion -
[[nodiscard]] inline std::string _utl_trim_enum_string(const std::string& str) {
    std::string::const_iterator left_it = str.begin();
    while (left_it != str.end() && std::isspace(*left_it)) ++left_it;

    std::string::const_reverse_iterator right_it = str.rbegin();
    while (right_it.base() != left_it && std::isspace(*right_it)) ++right_it;

    return std::string(left_it, right_it.base()); // return string with whitespaces trimmed at both sides
}

inline void _utl_split_enum_args(const char* va_args, std::string* strings, int count) {
    std::istringstream ss(va_args);
    std::string        buffer;

    for (int i = 0; ss.good() && (i < count); ++i) {
        std::getline(ss, buffer, ',');
        strings[i] = _utl_trim_enum_string(buffer);
    }
};

#define UTL_DEFINE_ENUM_WITH_STRING_CONVERSION(enum_name_, ...)                                                        \
    namespace enum_name_ {                                                                                             \
    enum enum_name_ { __VA_ARGS__, _count };                                                                           \
                                                                                                                       \
    inline std::string _strings[_count];                                                                               \
                                                                                                                       \
    inline std::string to_string(enum_name_ enum_val) {                                                                \
        if (_strings[0].empty()) { _utl_split_enum_args(#__VA_ARGS__, _strings, _count); }                             \
        return _strings[enum_val];                                                                                     \
    }                                                                                                                  \
                                                                                                                       \
    inline enum_name_ from_string(const std::string& enum_str) {                                                       \
        if (_strings[0].empty()) { _utl_split_enum_args(#__VA_ARGS__, _strings, _count); }                             \
        for (int i = 0; i < _count; ++i) {                                                                             \
            if (_strings[i] == enum_str) { return static_cast<enum_name_>(i); }                                        \
        }                                                                                                              \
        return _count;                                                                                                 \
    }                                                                                                                  \
    }
// IMPLEMENTATION COMMENTS:
// We declare namespace with enum inside to simulate enum-class while having '_strings' array
// and 'to_string()', 'from_string()' methods bundled with it.
//
// To count number of enum elements we add fake '_count' value at the end, which ends up being enum size
//
// '_strings' is declared compile-time, but gets filled through lazy evaluation upon first
// 'to_string()' or 'from_string()' call. To fill it we interpret #__VA_ARGS__ as a single string
// with some comma-separated identifiers. Those identifiers get split by commas, trimmed from
// whitespaces and added to '_strings'
//
// Upon further calls (enum -> string) conversion is done though taking '_strings[enum_val]',
// while (string -> enum) conversion requires searching through '_strings' to find enum index


// - is_function_present type trait -
#define UTL_DEFINE_IS_FUNCTION_PRESENT(function_name_, return_type_, ...)                                              \
    template <typename ReturnType, typename... ArgTypes>                                                               \
    class _utl_is_function_present_impl_##function_name_ {                                                             \
    private:                                                                                                           \
        typedef char no[sizeof(ReturnType) + 1];                                                                       \
                                                                                                                       \
        template <typename... C>                                                                                       \
        static auto test(C... arg) -> decltype(function_name_(arg...));                                                \
                                                                                                                       \
        template <typename... C>                                                                                       \
        static no& test(...);                                                                                          \
                                                                                                                       \
    public:                                                                                                            \
        enum { value = (sizeof(test<ArgTypes...>(std::declval<ArgTypes>()...)) == sizeof(ReturnType)) };               \
    };                                                                                                                 \
                                                                                                                       \
    using is_function_present_##function_name_ =                                                                       \
        _utl_is_function_present_impl_##function_name_<return_type_, __VA_ARGS__>;
// TASK:
// We need to detect at compile time if function FUNC(ARGS...) exists.
// FUNC identifier isn't guaranteed to be declared.
//
// Ideal method would look like UTL_FUNC_EXISTS(FUNC, RETURN_TYPE, ARG_TYPES...) -> true/false
// This does not seem to be possible, we have to declare integral constant instead, see explanation below.
//
// WHY IS IT SO HARD:
// (1) Can this be done through preprocessor macros?
// No, preprocessor has no way to tell whether C++ identifier is defined or not.
//
// (2) Is there a compiler-specific way to do it?
// Doesn't seem to be the case.
//
// (3) Why not use some sort of template with FUNC as a parameter?
// Essentially we have to evaluate undeclared identifier, while compiler exits with error upon
// encountering anything undeclared. The only way to detect whether undeclared identifier exists
// or not seems to be through SFINAE.
//
// IMPLEMENTATION COMMENTS:
// We declate integral constant class with 2 functions 'test()', first one takes priority during overload
// resolution and compiles if FUNC(ARGS...) is defined, otherwise it's {Substitution Failure} which is
// {Is Not An Error} and second function compiles.
//
// To resolve which overload of 'test()' was selected we check the sizeof() return type, 2nd overload
// has a return type 'char[sizeof(ReturnType) + 1]' so it's always different from 1st overload.
// Resolution result (true/false) gets stored to '::value'.
//
// Note that we can't pass 'ReturnType' and 'ArgTypes' directly through '__VA_ARGS__' because
// to call function 'test(ARGS...)' in general case we have to 'std::declval<>()' all 'ARGS...'.
// To do so we can use variadic template syntax and then just forward '__VA_ARGS__' to the template
// through 'using is_function_present = is_function_present_impl<ReturnType, __VA_ARGS__>'.
//
// ALTERNATIVES: Perhaps some sort of tricky inline SFINAE can be done through C++14 generic lambdas.
//
// Note: Some versions of 'clangd' give a 'bugprone-sizeof-expression' warning for sizeof(*A),
// this is a false alarm.

// - Exit with decorators -
inline void _utl_exit_with_message(std::string_view file, int line, std::string_view func,
                                   std::string_view message = "<NO MESSAGE>", int code = 1) {
    constexpr int  HLINE_WIDTH  = 50;
    constexpr char HLINE_SYMBOL = '-';

    const std::string_view filename = file.substr(file.find_last_of("/\\") + 1);

    // Create some more space
    std::cerr << std::endl;

    // Horizontal line
    std::fill_n(std::ostreambuf_iterator<char>(std::cerr), HLINE_WIDTH, HLINE_SYMBOL);
    std::cerr << std::endl;

    // Text
    std::cerr << "Exit triggered on [" << filename << ":" << line << ", " << func << "()] with:" << std::endl
              << "Message => " << message << std::endl
              << "Code    => " << code << std::endl;

    // Horizontal line
    std::fill_n(std::ostreambuf_iterator<char>(std::cerr), HLINE_WIDTH, HLINE_SYMBOL);
    std::cerr << std::endl;

    std::exit(code);
}

#define UTL_DEFINE_EXIT(...) _utl_exit_with_message(__FILE__, __LINE__, __func__, __VA_ARGS__);

#endif
#endif // macro-module UTL_DEFINE






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Macro-Module:  UTL_LOG
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/MACRO_LOG.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMACRO_LOG)
#ifndef UTLHEADERGUARD_LOG
#define UTLHEADERGUARD_LOG

// _______________________ INCLUDES _______________________

#include <iostream>    // cout
#include <ostream>     // ostream
#include <string_view> // string_view

// ____________________ DEVELOPER DOCS ____________________

// A very minimalistic logger with source file info. Mainly used for debugging.
// NOTE: Perhaps can be expanded to something faster and more production-usable.
//
// # UTL_LOG_SET_OUTPUT() #
// Select ostream used by LOG macros.
//
// # UTL_LOG(), UTL_LOG_DEBUG() #
// Print message to selected ostream prefixed with [<filename>:<line> (<function>)].
// Accepts multiple args (with defined operator <<) that get concatenated into a single message.
// DEBUG version compiles to nothing in release.

// ____________________ IMPLEMENTATION ____________________

// ======================
// --- Logging Macros ---
// ======================

inline std::ostream* _utl_log_ostream = &std::cout;

#define UTL_LOG_SET_OUTPUT(new_stream_) _utl_log_ostream = &new_stream_;

template <typename... Args>
inline void _utl_log_print(std::string_view file, int line, std::string_view func, const Args&... args) {
    const std::string_view filename = file.substr(file.find_last_of("/\\") + 1);

    ///(*_utl_log_ostream) << "\033[31;1m"; // Supported by Linux and Windows10+, but prints to files, figure out a fix
    (*_utl_log_ostream) << "[" << filename << ":" << line << ", " << func << "()]";
    ///(*_utl_log_ostream) << "\033[0m";

    (*_utl_log_ostream) << " ";
    ((*_utl_log_ostream) << ... << args);
    (*_utl_log_ostream) << '\n';
}

#define UTL_LOG(...) _utl_log_print(__FILE__, __LINE__, __func__, __VA_ARGS__)

#ifdef _DEBUG
#define UTL_LOG_DEBUG(...) _utl_log_print(__FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define UTL_LOG_DEBUG(...)
#endif

#endif
#endif // macro-module UTL_LOG






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Macro-Module:  UTL_PROFILER
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/MACRO_PROFILER.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <functional>
#if !defined(UTL_PICK_MODULES) || defined(UTLMACRO_PROFILER)
#ifndef UTLHEADERGUARD_PROFILER
#define UTLHEADERGUARD_PROFILER

// _______________________ INCLUDES _______________________

#include <algorithm>   // sort()
#include <chrono>      // chrono::steady_clock, chrono::duration_cast<>, std::chrono::milliseconds
#include <cstdlib>     // atexit()
#include <fstream>     // ofstream
#include <iomanip>     // setprecision(), setw()
#include <ios>         // streamsize, fixed,
#include <iostream>    // cout
#include <ostream>     // ostream
#include <sstream>     // ostringstream
#include <string>      // string
#include <string_view> // string_view
#include <vector>      // vector<>

// ____________________ DEVELOPER DOCS ____________________

// Macros for quick code profiling.
// Trivially simple, yet effective way of finding bottlenecks without any tooling.
//
// Can also be used to benchmark stuff "quick & dirty" due to doing all the time measuring
// and table formatting one would usually implement in their benchmarks. A proper bechmark
// suite of course would include support for automatic reruns and gather statistical data,
// but in prototying this is often not necessary.
//
// # UTL_PROFILER_REROUTE_TO_FILE(filepath) #
// Reroutes profiler summary from std::cout to a file with a given name.
//
// # UTL_PROFILER, UTL_PROFILER_LABELED() #
// Profiles the following expression/scope. If profiled scope was entered at any point of the program,
// upon exiting 'main()' the table with profiling results will be printed. Profiling results include:
// - Total program runtime
// - Total runtime of each profiled scope
// - % of total runtime taken by each profiled scope
// - Profiler labels (if using 'UTL_PROFILE_LABELED()', otherwise the label is set to "<NONE>")
// - Profiler locations: file, function, line

// ____________________ IMPLEMENTATION ____________________

namespace utl::profiler {

// ==========================
// --- Profiler Internals ---
// ==========================

inline std::string _format_call_site(std::string_view file, int line, std::string_view func) {
    const std::string_view filename = file.substr(file.find_last_of("/\\") + 1);

    return (std::ostringstream() << filename << ":" << line << ", " << func << "()").str();
}

using _clock      = std::chrono::steady_clock;
using _duration   = _clock::duration;
using _time_point = _clock::time_point;

inline const _time_point _program_entry_time_point = _clock::now();

struct _record {
    const char* file;
    int         line;
    const char* func;
    const char* label;
    _duration   accumulated_time;
};

inline void _utl_profiler_atexit(); // predeclaration, implementation has circular dependency with 'RecordManager'

// =========================
// --- Profiler Classess ---
// =========================

class RecordManager {
private:
    _record data;

public:
    inline static std::vector<_record> records;
    bool                               busy;

    void add_time(_duration time) noexcept { this->data.accumulated_time += time; }

    RecordManager() = delete;

    RecordManager(const char* file, int line, const char* func, const char* label)
        : data({file, line, func, label, _duration(0)}), busy(false) {
        // 'file', 'func', 'label' are guaranteed to be string literals, since we want to
        // have as little overhead as possible during runtime, we can just save raw pointers
        // and convert them to nicer types like 'std::string_view' later in the formatting stage

        // Profiler ever gets called => register result output at std::exit()
        static bool first_call = true;
        if (first_call) {
            std::atexit(_utl_profiler_atexit);
            first_call = false;
        }
    }

    ~RecordManager() { records.emplace_back(this->data); }
};

class ScopeTimer {
private:
    _time_point    start;
    RecordManager* record_manager;
    // we could use 'std::optional<std::reference_wrapper<RecordManager>>',
    // but that would inctroduce more dependencies for no real reason

public:
    constexpr operator bool() const noexcept { return true; }
    // we needed so we can use 'if constexpr (const auto x = ScopeTimer())' construct

    ScopeTimer(RecordManager* manager) {
        // Busy manager => we're in recursion, don't count time
        if (manager->busy) {
            this->record_manager = nullptr;
            return;
        }
        this->record_manager       = manager;
        this->record_manager->busy = true;
        this->start                = _clock::now();
    }

    ~ScopeTimer() {
        if (!this->record_manager) return;
        this->record_manager->add_time(_clock::now() - this->start);
        this->record_manager->busy = false;
    }
};

// ==================================
// --- Profiler Exit & Formatting ---
// ==================================

inline void _utl_profiler_atexit() {

    const auto total_runtime = _clock::now() - _program_entry_time_point;

    std::ostream* ostr = &std::cout;

    // Convenience functions
    const auto duration_to_sec = [](_duration duration) -> double {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() / 1e9;
    };

    const auto duration_percentage = [&](double duration_sec) -> double {
        const double total_runtime_sec = duration_to_sec(total_runtime);
        return (duration_sec / total_runtime_sec) * 100.;
    };

    const auto float_printed_size = [](double value, std::streamsize precision, decltype(std::fixed) format,
                                       std::string_view postfix) -> std::streamsize {
        std::ostringstream ss;
        ss << std::setprecision(precision) << format << value << postfix;
        return ss.str().size(); // can be done faster but we don't really care here
    };

    const auto repeat_hline_symbol = [](std::streamsize repeats) -> std::string {
        return std::string(static_cast<size_t>(repeats), '-');
    };

    constexpr std::streamsize  duration_precision = 2;
    constexpr auto             duration_format    = std::fixed;
    constexpr std::string_view duration_postfix   = " s";

    constexpr std::streamsize  percentage_precision = 1;
    constexpr auto             percentage_format    = std::fixed;
    constexpr std::string_view percentage_postfix   = "%";

    // Sort records by their accumulated time
    std::sort(RecordManager::records.begin(), RecordManager::records.end(),
              [](const _record& l, const _record& r) { return l.accumulated_time < r.accumulated_time; });

    // Collect max length of each column (for proper formatting)
    constexpr std::string_view column_name_call_site  = "Call Site";
    constexpr std::string_view column_name_label      = "Label";
    constexpr std::string_view column_name_duration   = "Time";
    constexpr std::string_view column_name_percentage = "Time %";

    std::streamsize max_length_call_site  = column_name_call_site.size();
    std::streamsize max_length_label      = column_name_label.size();
    std::streamsize max_length_duration   = column_name_duration.size();
    std::streamsize max_length_percentage = column_name_percentage.size();

    for (const auto& record : RecordManager::records) {
        const std::string call_site    = _format_call_site(record.file, record.line, record.func);
        const std::string label        = record.label;
        const double      duration_sec = duration_to_sec(record.accumulated_time);

        // 'Call Site' column
        const std::streamsize length_call_site = call_site.size();
        if (max_length_call_site < length_call_site) max_length_call_site = length_call_site;

        // 'Label' column
        const std::streamsize length_label = label.size();
        if (max_length_label < length_label) max_length_label = length_label;

        // 'Time' column
        const std::streamsize length_duration =
            float_printed_size(duration_sec, duration_precision, duration_format, duration_postfix);
        if (max_length_duration < length_duration) max_length_duration = length_duration;

        // 'Time %' column
        const auto            percentage = duration_percentage(duration_sec);
        const std::streamsize length_percentage =
            float_printed_size(percentage, percentage_precision, percentage_format, percentage_postfix);
        if (max_length_percentage < length_percentage) max_length_percentage = length_percentage;
    }

    // Print formatted profiler header
    constexpr std::string_view HEADER_TEXT = " UTL PROFILING RESULTS ";

    const std::streamsize total_table_length = sizeof("| ") - 1 + max_length_call_site + sizeof(" | ") - 1 +
                                               max_length_label + sizeof(" | ") - 1 + max_length_duration +
                                               sizeof(" | ") - 1 + max_length_percentage + sizeof(" |") -
                                               1; // -1 because sizeof(char[]) accounts for invisible '\0' at the end

    const std::streamsize header_text_length = HEADER_TEXT.size();
    const std::streamsize header_left_pad    = (total_table_length - header_text_length) / 2;
    const std::streamsize header_right_pad   = total_table_length - header_text_length - header_left_pad;

    *ostr << "\n"
          << repeat_hline_symbol(header_left_pad + 1) << HEADER_TEXT << repeat_hline_symbol(header_right_pad + 1)
          << '\n'
          // + 1 makes header hline extend 1 character past the table on both sides
          << "\n"
          << " Total runtime -> " << std::setprecision(duration_precision) << duration_format
          << duration_to_sec(total_runtime) << " sec\n"
          << "\n";

    // Print formatted table header
    *ostr << " | " << std::setw(max_length_call_site) << column_name_call_site << " | " << std::setw(max_length_label)
          << column_name_label << " | " << std::setw(max_length_duration) << column_name_duration << " | "
          << std::setw(max_length_percentage) << column_name_percentage << " |\n";

    *ostr << " |"
          << repeat_hline_symbol(max_length_call_site + 2) // add 2 to account for delimers not having spaces in hline
          << "|" << repeat_hline_symbol(max_length_label + 2) << "|" << repeat_hline_symbol(max_length_duration + 2)
          << "|" << repeat_hline_symbol(max_length_percentage + 2) << "|\n";

    *ostr << std::setfill(' '); // reset the fill so we don't mess with table contents


    // Print formatted table contents
    for (const auto& record : RecordManager::records) {
        const std::string call_site    = _format_call_site(record.file, record.line, record.func);
        const std::string label        = record.label;
        const double      duration_sec = duration_to_sec(record.accumulated_time);
        const double      percentage   = duration_percentage(duration_sec);

        // Join floats with their postfixes into a single string so they are properly handled by std::setw()
        // (which only affects the first value leading to a table misaligned by postfix size)
        std::ostringstream ss_duration;
        ss_duration << std::setprecision(duration_precision) << duration_format << duration_sec << duration_postfix;

        std::ostringstream ss_percentage;
        ss_percentage << std::setprecision(percentage_precision) << percentage_format << percentage
                      << percentage_postfix;

        *ostr << " | " << std::setw(max_length_call_site) << call_site << " | " << std::setw(max_length_label) << label
              << " | " << std::setw(max_length_duration) << ss_duration.str() << " | "
              << std::setw(max_length_percentage) << ss_percentage.str() << " |\n";
    }
}

// ========================
// --- Profiler Codegen ---
// ========================

#define _utl_profiler_concat_tokens(a, b) a##b
#define _utl_profiler_concat_tokens_wrapper(a, b) _utl_profiler_concat_tokens(a, b)
#define _utl_profiler_add_uuid(varname_) _utl_profiler_concat_tokens_wrapper(varname_, __LINE__)
// This macro creates token 'varname_##__LINE__' from 'varname_'.
//
// The reason we can't just write it as is, is that function-macros only expands their macro-arguments
// if neither the stringizing operator # nor the token-pasting operator ## are applied to the arguments
// inside the macro body.
//
// Which means in a simple 'varname_##__LINE__' macro '__LINE__' doesn't expand to it's value.
//
// We can get around this fact by introducing indirection,
// '__LINE__' gets expanded in '_utl_profiler_concat_tokens_wrapper()'
// and then tokenized and concatenated in '_utl_profiler_concat_tokens()'

#define UTL_PROFILER(label_)                                                                                           \
    constexpr bool _utl_profiler_add_uuid(utl_profiler_macro_guard_) = true;                                           \
                                                                                                                       \
    static_assert(_utl_profiler_add_uuid(utl_profiler_macro_guard_), "UTL_PROFILE is a multi-line macro.");            \
                                                                                                                       \
    static utl::profiler::RecordManager _utl_profiler_add_uuid(utl_profiler_record_manager_)(__FILE__, __LINE__,       \
                                                                                             __func__, label_);        \
                                                                                                                       \
    if constexpr (const utl::profiler::ScopeTimer _utl_profiler_add_uuid(utl_profiler_scope_timer_){                   \
                      &_utl_profiler_add_uuid(utl_profiler_record_manager_)})
// Note 1:
//
//    constexpr bool ... = true;
//    static_assert(..., "UTL_PROFILE is a multi-line macro.");
//
// is reponsible for preventing accidental errors caused by using macro like this:
//
//    for (...) UTL_PROFILER("") function(); // will only loop the first line of the multi-line macro
//
// If someone tries to write it like this, the constexpr bool variable will be "pulled" into a narrower scope,
// causing 'static_assert()' to fail due to using undeclared identifier. Since the line with undeclared identifier
// gets expanded, user will be able to see the assert message.
//
// Note 2:
//
// By separating "record management" into a static variable and "actual timing" into a non-static one,
// we can avoid additional overhead from having to locate the record, corresponding to the profiled source location.
// (an operation that require a non-trivial vector/map lookup with string comparisons)
//
// Static variable initializes its record once and timer does the bare minimum of work - 2 calls to 'now()' to get
// timing, one addition to accumulated time and a check for recursion (so it can skip time appropriately).
//
//  Note 3:
//
// _utl_profiler_add_uuid(...) ensures no identifier collisions when several profilers exist in a single scope.
// Since in this context 'uuid' is a line number, the only case in which ids can collide is when multiple profilers
// are declated on the same line, which I assume no sane person would do. And even if they would, that would simply
// lead to a compiler error. Can't really do better than that without resorting to non-standard macros like
// '__COUNTER__' for 'uuid' creation

} // namespace utl::profiler

#endif
#endif // macro-module UTL_PROFILER






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::json
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/module_json.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_JSON)
#ifndef UTLHEADERGUARD_JSON
#define UTLHEADERGUARD_JSON

// _______________________ INCLUDES _______________________

#include <array>            // array<>
#include <charconv>         // to_chars(), from_chars()
#include <cmath>            // isfinite()
#include <cstddef>          // size_t
#include <fstream>          // ifstream, ofstream
#include <initializer_list> // initializer_list<>
#include <limits>           // numeric_limits<>::max_digits10, numeric_limits<>::max_exponent10
#include <map>              // map<>
#include <stdexcept>        // runtime_error
#include <string>           // string
#include <string_view>      // string_view
#include <system_error>     // errc()
#include <type_traits>      // enable_if_t<>, void_t, is_convertible_v<>, is_same_v<>
#include <utility>          // move(), declval<>()
#include <variant>          // variant<>
#include <vector>           // vector<>

// ____________________ DEVELOPER DOCS ____________________

// NOTE: DOCS
// TEMP:

// ____________________ IMPLEMENTATION ____________________

namespace utl::json {

// ===================
// --- Misc. Utils ---
// ===================

inline std::string _read_file_to_string(const std::string& path) {
    using namespace std::string_literals;

    // This seems the to be the fastest way of reading a text file
    // into 'std::string' without invoking OS-specific methods
    // See this StackOverflow thread:
    // [https://stackoverflow.com/questions/32169936/optimal-way-of-reading-a-complete-file-to-a-string-using-fstream]
    // And attached benchmarks:
    // [https://github.com/Sqeaky/CppFileToStringExperiments]

    std::ifstream file(path, std::ios::ate); // open file and immediately seek to the end
    if (!file.good()) throw std::runtime_error("Could not open file {"s + path + "."s);
    // NOTE: Add a way to return errors if file doesn't exist, exceptions aren't particularly good here

    const auto file_size = file.tellg(); // returns cursor pos, which is the end of file
    file.seekg(std::ios::beg);           // seek to the beginning
    std::string chars(file_size, 0);     // allocate string of appropriate size
    file.read(chars.data(), file_size);  // read into the string
    return chars;
}

template <typename T>
constexpr int _log_10_ceil(T num) {
    return num < 10 ? 1 : 1 + _log_10_ceil(num / 10);
}

// Workaround for 'static_assert(false)' making program ill-formed even
// when placed inide an 'if constexpr' branch that never compiles.
// 'static_assert(_always_false_v<T)' on the the other hand doesn't,
// which means we can use it to mark branches that should never compile.
template <class>
inline constexpr bool _always_false_v = false;

// Type trait that checks existence of '.begin()', 'end()' members
template <typename Type, typename = void, typename = void>
struct _is_const_iterable_through : std::false_type {};

template <typename Type>
struct _is_const_iterable_through<Type, std::void_t<decltype(++std::declval<Type>().begin())>,
                                  std::void_t<decltype(std::declval<Type>().end())>> : std::true_type {};

// Type trait that checks existence of '::key_type', '::mapped_type' members
template <typename Type, typename = void, typename = void>
struct _is_assotiative : std::false_type {};

template <typename Type>
struct _is_assotiative<Type, std::void_t<typename Type::key_type>, std::void_t<typename Type::mapped_type>>
    : std::true_type {};

// ===================================
// --- JSON type conversion traits ---
// ===================================

template <class T>
using _object_type_impl = std::map<std::string, T, std::less<>>;
// 'std::less<>' declares map as transparent, which means we can `.find()` for `std::string_view` keys
template <class T>
using _array_type_impl  = std::vector<T>;
using _string_type_impl = std::string;
using _number_type_impl = double;
using _bool_type_impl   = bool;
class _null_type_impl {};

template <class T>
constexpr bool is_object_like_v = _is_const_iterable_through<T>::value && _is_assotiative<T>::value;
// NOTE: Also check for 'key_type' being convertible to 'std::string'
template <class T>
constexpr bool is_array_like_v = _is_const_iterable_through<T>::value;
template <class T>
constexpr bool is_string_like_v = std::is_convertible_v<T, std::string_view>;
template <class T>
constexpr bool is_numeric_like_v = std::is_convertible_v<T, _number_type_impl>;
template <class T>
constexpr bool is_bool_like_v = std::is_same_v<T, _bool_type_impl>;
template <class T>
constexpr bool is_null_like_v = std::is_same_v<T, _null_type_impl>;

template <class T>
constexpr bool is_json_type_convertible_v = is_object_like_v<T> || is_array_like_v<T> || is_string_like_v<T> ||
                                            is_numeric_like_v<T> || is_bool_like_v<T> || is_null_like_v<T>;

// ==================
// --- Node class ---
// ==================

enum class Format { PRETTY, MINIMIZED };

class Node;
inline void _serialize_json_to_buffer(std::string& chars, const Node& node, Format format);

class Node {
public:
    using object_type = _object_type_impl<Node>;
    using array_type  = _array_type_impl<Node>;
    using string_type = _string_type_impl;
    using number_type = _number_type_impl;
    using bool_type   = _bool_type_impl;
    using null_type   = _null_type_impl;

private:
    using variant_type = std::variant<null_type, object_type, array_type, string_type, number_type, bool_type>;
    // 'null_type' should go first to ensure default-initialization creates 'null' nodes

    variant_type data{};

public:
    // -- Getters --
    // -------------

    template <class T>
    [[nodiscard]] T& get() {
        return std::get<T>(this->data);
    }

    template <class T>
    [[nodiscard]] const T& get() const {
        return std::get<T>(this->data);
    }

    [[nodiscard]] object_type& get_object() { return this->get<object_type>(); }
    [[nodiscard]] array_type&  get_array() { return this->get<array_type>(); }
    [[nodiscard]] string_type& get_string() { return this->get<string_type>(); }
    [[nodiscard]] number_type& get_number() { return this->get<number_type>(); }
    [[nodiscard]] bool_type&   get_bool() { return this->get<bool_type>(); }
    [[nodiscard]] null_type&   get_null() { return this->get<null_type>(); }

    [[nodiscard]] const object_type& get_object() const { return this->get<object_type>(); }
    [[nodiscard]] const array_type&  get_array() const { return this->get<array_type>(); }
    [[nodiscard]] const string_type& get_string() const { return this->get<string_type>(); }
    [[nodiscard]] const number_type& get_number() const { return this->get<number_type>(); }
    [[nodiscard]] const bool_type&   get_bool() const { return this->get<bool_type>(); }
    [[nodiscard]] const null_type&   get_null() const { return this->get<null_type>(); }

    template <class T>
    [[nodiscard]] bool is() const noexcept {
        return std::holds_alternative<T>(this->data);
    }

    [[nodiscard]] bool is_object() const noexcept { return this->is<object_type>(); }
    [[nodiscard]] bool is_array() const noexcept { return this->is<array_type>(); }
    [[nodiscard]] bool is_string() const noexcept { return this->is<string_type>(); }
    [[nodiscard]] bool is_number() const noexcept { return this->is<number_type>(); }
    [[nodiscard]] bool is_bool() const noexcept { return this->is<bool_type>(); }
    [[nodiscard]] bool is_null() const noexcept { return this->is<null_type>(); }

    template <class T>
    [[nodiscard]] T* get_if() noexcept {
        return std::get_if<T>(&this->data);
    }

    template <class T>
    [[nodiscard]] const T* get_if() const noexcept {
        return std::get_if<T>(&this->data);
    }

    // -- Object methods --
    // --------------------

    Node& operator[](std::string_view key) {
        // 'std::map<K, V>::operator[]()' and 'std::map<K, V>::at()' don't support
        // support heterogeneous lookup, we have to reimplement them manually
        if (this->is_null()) this->data = object_type(); // only 'null' converts to object automatically on 'json[key]'
        auto& object = this->get_object();
        auto  it     = object.find(key);
        if (it == object.end()) it = object.insert(std::make_pair(std::string(key), Node{})).first;
        return it->second;
    }

    [[nodiscard]] const Node& operator[](std::string_view key) const {
        // 'std::map<K, V>::operator[]()' and 'std::map<K, V>::at()' don't support
        // support heterogeneous lookup, we have to reimplement them manually
        const auto& object = this->get_object();
        const auto  it     = object.find(key);
        if (it == object.end()) throw std::runtime_error("Accessing non-existent key in JSON object.");
        return it->second;
    }

    [[nodiscard]] Node& at(std::string_view key) {
        // Non-const 'operator[]' inserts non-existent keys, '.at()' should throw instead
        auto&      object = this->get_object();
        const auto it     = object.find(key);
        if (it == object.end()) throw std::runtime_error("Accessing non-existent key in JSON object.");
        return it->second;
    }

    [[nodiscard]] const Node& at(std::string_view key) const { return this->operator[](key); }

    [[nodiscard]] bool contains(std::string_view key) const {
        const auto& object = this->get_object();
        const auto  it     = object.find(key);
        return it != object.end();
    }

    template <class T>
    [[nodiscard]] T& value_or(std::string_view key, const T& else_value) {
        const auto& object = this->get_object();
        const auto  it     = object.find(key);
        if (it != object.end()) return it->second.get<T>();
        return else_value;
        // same thing as 'this->contains(key) ? json.at(key).get<T>() : else_value' but without a second map lookup
    }

    // -- Assignment --
    // ----------------

    // Converting assignment
    template <class T, std::enable_if_t<
                           !std::is_same_v<std::decay_t<T>, Node> && !std::is_same_v<std::decay_t<T>, object_type> &&
                               !std::is_same_v<std::decay_t<T>, array_type> &&
                               !std::is_same_v<std::decay_t<T>, string_type> && is_json_type_convertible_v<T>,
                           bool> = true>
    Node& operator=(const T& value) {
        // Don't take types that decay to Node/object/array/string to prevent
        // shadowing native copy/move assignment for those types

        constexpr bool is_object_like  = _is_const_iterable_through<T>::value && _is_assotiative<T>::value;
        constexpr bool is_array_like   = _is_const_iterable_through<T>::value;
        constexpr bool is_string_like  = std::is_convertible_v<T, std::string_view>;
        constexpr bool is_numeric_like = std::is_convertible_v<T, number_type>;
        constexpr bool is_bool_like    = std::is_same_v<T, bool_type>;
        constexpr bool is_null_like    = std::is_same_v<T, null_type>;

        // Several "type-like' characteristics can be true at the same time,
        // to resolve ambiguity we assign the following conversion priority:
        // string > object > array > bool > null > numeric

        if constexpr (is_string_like) {
            this->data.emplace<string_type>(value);
        } else if constexpr (is_object_like) {
            this->data.emplace<object_type>();
            auto& object = this->get_object();
            for (const auto& [key, val] : value) object[key] = val;
        } else if constexpr (is_array_like) {
            this->data.emplace<array_type>();
            auto& array = this->get_array();
            for (const auto& elem : value) array.emplace_back(elem);
        } else if constexpr (is_bool_like) {
            this->data.emplace<bool_type>(value);
        } else if constexpr (is_null_like) {
            this->data.emplace<null_type>(value);
        } else if constexpr (is_numeric_like) {
            this->data.emplace<number_type>(value);
        } else {
            static_assert(_always_false_v<T>, "Method is a non-exhaustive visitor of std::variant<>.");
        }

        return *this;
    }

    // "native" copy/move semantics for types that support it
    Node& operator=(const object_type& value) {
        this->data = value;
        return *this;
    }
    Node& operator=(object_type&& value) {
        this->data = std::move(value);
        return *this;
    }

    Node& operator=(const array_type& value) {
        this->data = value;
        return *this;
    }
    Node& operator=(array_type&& value) {
        this->data = std::move(value);
        return *this;
    }

    Node& operator=(const string_type& value) {
        this->data = value;
        return *this;
    }
    Node& operator=(string_type&& value) {
        this->data = std::move(value);
        return *this;
    }

    // Support for 'std::initializer_list' type deduction,
    // (otherwise the call is ambiguous)
    template <class T>
    Node& operator=(std::initializer_list<T> ilist) {
        // We can't just do 'return *this = array_type(value);' because compiler doesn't realize it can
        // convert 'std::initializer_list<T>' to 'std::vector<Node>' for all 'T' convertable to 'Node',
        // we have to invoke 'Node()' constructor explicitly (here it happens in 'emplace_back()')
        array_type array_value;
        array_value.reserve(ilist.size());
        for (const auto& e : ilist) array_value.emplace_back(e);
        this->data = std::move(array_value);
        return *this;
    }

    // -- Constructors --
    // ------------------

    Node& operator=(const Node&) = default;
    Node& operator=(Node&&)      = default;

    Node()            = default;
    Node(const Node&) = default;
    Node(Node&&)      = default;

    // Converting ctor
    template <class T, std::enable_if_t<
                           !std::is_same_v<std::decay_t<T>, Node> && !std::is_same_v<std::decay_t<T>, object_type> &&
                               !std::is_same_v<std::decay_t<T>, array_type> &&
                               !std::is_same_v<std::decay_t<T>, string_type> && is_json_type_convertible_v<T>,
                           bool> = true>
    Node(const T& value) {
        *this = value;
    }

    Node(const object_type& value) { this->data = value; }
    Node(object_type&& value) { this->data = std::move(value); }
    Node(const array_type& value) { this->data = value; }
    Node(array_type&& value) { this->data = std::move(value); }
    Node(std::string_view value) { this->data = string_type(value); }
    Node(const string_type& value) { this->data = value; }
    Node(string_type&& value) { this->data = std::move(value); }
    Node(number_type value) { this->data = value; }
    Node(bool_type value) { this->data = value; }
    Node(null_type value) { this->data = value; }

    [[nodiscard]] std::string to_string(Format format = Format::PRETTY) const {
        std::string buffer;
        _serialize_json_to_buffer(buffer, *this, format);
        return buffer;
    }
};

// Public typedefs
using Object = Node::object_type;
using Array  = Node::array_type;
using String = Node::string_type;
using Number = Node::number_type;
using Bool   = Node::bool_type;
using Null   = Node::null_type;

// =====================
// --- Lookup Tables ---
// =====================

constexpr std::size_t _number_of_char_values = 256;
// always true since 'sizeof(char) == 1' is guaranteed by the standard

// Lookup table used to check if number should be escaped and get a replacement char on at the same time.
// This allows us to replace multiple checks and if's with a single array lookup that.
//
// Instead of:
//    if (c == '"') { chars += '"' }
//    ...
//    else if (c == '\t') { chars += 't' }
// we get:
//    if (const char replacement = _lookup_serialized_escaped_chars[c]) { chars += replacement; }
//
// which ends up being quite noticeably faster.
//
constexpr std::array<char, _number_of_char_values> _lookup_serialized_escaped_chars = [] {
    std::array<char, _number_of_char_values> res{};
    // default-initialized chars get initialized to '\0',
    // as far as I understand ('\0' == 0) is mandated by the standard,
    // which is why we can use it inside an 'if' condition
    res['"']  = '"';
    res['\\'] = '\\';
    // res['/']  = '/'; escaping forward slash in JSON is allowed, but redundant
    res['\b'] = 'b';
    res['\f'] = 'f';
    res['\n'] = 'n';
    res['\r'] = 'r';
    res['\t'] = 't';
    return res;
}();

// Lookup table used to determine "insignificant whitespace" characters when
// skipping whitespace during parser. Seems to be either similar or marginally
// faster in performance than a regular condition check.
constexpr std::array<bool, _number_of_char_values> _lookup_whitespace_chars = [] {
    std::array<bool, _number_of_char_values> res{};
    // "Insignificant whitespace" according to the JSON spec:
    // [https://ecma-international.org/wp-content/uploads/ECMA-404.pdf]
    // constitues following symbols:
    // - Whitespace      (aka ' ' )
    // - Tabs            (aka '\t')
    // - Carriage return (aka '\r')
    // - Newline         (aka '\n')
    res[' ']  = true;
    res['\t'] = true;
    res['\r'] = true;
    res['\n'] = true;
    return res;
}();

// Lookup table used to get an appropriate char for the escaped char in a 2-char JSON escape sequence.
// Allows us to avoid a chain of 8 if-else'es which ends up beings faster.
constexpr std::array<char, _number_of_char_values> _lookup_parsed_escaped_chars = [] {
    std::array<char, _number_of_char_values> res{};
    res['"']  = '"';
    res['\\'] = '\\';
    res['/']  = '/';
    res['b']  = '\b';
    res['f']  = '\f';
    res['n']  = '\n';
    res['r']  = '\r';
    res['t']  = '\t';
    return res;
}();

// ====================
// --- JSON Parsing ---
// ====================

struct _parser {
    const std::string& chars;

    _parser() = delete;
    _parser(const std::string& chars) : chars(chars) {}

    // Parser state
    std::size_t skip_nonsignificant_whitespace(std::size_t cursor) {
        using namespace std::string_literals;

        while (cursor < this->chars.size()) {
            if (!_lookup_whitespace_chars[this->chars[cursor]]) return cursor;
            ++cursor;
        }

        throw std::runtime_error("JSON parser reached the end of buffer at pos "s + std::to_string(cursor) +
                                 " while skipping insignificant whitespace segment."s);
    }

    // Parsing methods
    std::pair<std::size_t, Node> parse_node(std::size_t cursor) {
        using namespace std::string_literals;

        // Node selector assumes it is starting at a significant symbol
        // which is the first symbol of the node to be parsed

        const char c = this->chars[cursor];

        // Assuming valid JSON, we can determine node type based on a single first character
        if (c == '{') {
            // UTL_PROFILER_LABELED("Objects")
            return this->parse_object(cursor);
        } else if (c == '[') {
            // UTL_PROFILER_LABELED("Arrays")
            return this->parse_array(cursor);
        } else if (c == '"') {
            // UTL_PROFILER_LABELED("Strings")
            return this->parse_string(cursor);
        } else if (('0' <= c && c <= '9') || (c == '-')) {
            // UTL_PROFILER_LABELED("Numbers")
            return this->parse_number(cursor);
        } else if (c == 't') {
            // UTL_PROFILER_LABELED("Trues")
            return this->parse_true(cursor);
        } else if (c == 'f') {
            // UTL_PROFILER_LABELED("Falses")
            return this->parse_false(cursor);
        } else if (c == 'n') {
            // UTL_PROFILER_LABELED("Nulls")
            return this->parse_null(cursor);
        }
        throw std::runtime_error("JSON node selector encountered unexpected marker symbol {"s + this->chars[cursor] +
                                 "} at pos "s + std::to_string(cursor) + " (should be one of {0123456789{[\"tfn})."s);

        // Note: using a lookup table instead of an 'if' chain doesn't seem to offer any performance benefits here
    }

    std::size_t parse_object_pair(std::size_t cursor, Object& parent) {
        using namespace std::string_literals;

        // Object pair parser assumes it is starting at a '"'

        // Parse pair key
        std::string key; // allocating a string here is fine since we will std::move() into a map key
        std::tie(cursor, key) = this->parse_string(cursor); // may point 'this->current_node' to a new node

        // Handle stuff inbetween
        cursor = this->skip_nonsignificant_whitespace(cursor);
        if (this->chars[cursor] != ':')
            throw std::runtime_error("JSON object node encountered unexpected symbol {"s + this->chars[cursor] +
                                     "} after the pair key at pos "s + std::to_string(cursor) + " (should be {:})."s);
        ++cursor; // move past the colon ':'
        cursor = this->skip_nonsignificant_whitespace(cursor);

        // Parse pair value
        Node value;
        std::tie(cursor, value) = this->parse_node(cursor);
        
        parent.emplace(std::pair{std::move(key), std::move(value)});

        return cursor;
    }

    std::pair<std::size_t, Object> parse_object(std::size_t cursor) {
        using namespace std::string_literals;

        ++cursor; // move past the opening brace '{'

        // Empty object that will accumulate child nodes as we parse them
        Object object_value;

        // Handle 1st pair
        cursor = this->skip_nonsignificant_whitespace(cursor);
        if (this->chars[cursor] != '}') {
            cursor = this->parse_object_pair(cursor, object_value);
        } else {
            ++cursor; // move past the closing brace '}'
            return {cursor, std::move(object_value)};
        }

        // Handle other pairs

        // Since we are staring past the first pair, all following pairs are gonna be preceded by a comma.
        //
        // Strictly speaking, commas in objects aren't necessary for decoding a JSON, this is
        // a case of redundant information, included into the format to make it more human-readable.
        // { "key_1":1 "key_1":2 "key_3":"value" } <- enough information to parse without commas.
        //
        // However commas ARE in fact necessary for array parsing. By using commas to detect when we have
        // to parse another pair, we can reuse the same algorithm for both objects pairs and array elements.
        //
        // Doing so also has a benefit of inherently adding comma-presense validation which we would have
        // to do manually otherwise.

        while (cursor < this->chars.size()) {
            cursor       = this->skip_nonsignificant_whitespace(cursor);
            const char c = this->chars[cursor];

            if (c == ',') {
                ++cursor; // move past the comma ','
                cursor = this->skip_nonsignificant_whitespace(cursor);
                cursor = this->parse_object_pair(cursor, object_value);
            } else if (c == '}') {
                ++cursor; // move past the closing brace '}'
                return {cursor, std::move(object_value)};
            }
        }

        throw std::runtime_error("JSON object node reached the end of buffer while parsing object contents.");
    }

    std::size_t parse_array_element(std::size_t cursor, Array& parent) {
        using namespace std::string_literals;

        // Array element parser assumes it is starting at the first symbol of some JSON node

        // Parse pair key
        Node value;
        std::tie(cursor, value) = this->parse_node(cursor);

        parent.emplace_back(std::move(value));

        return cursor;
    }

    std::pair<std::size_t, Array> parse_array(std::size_t cursor) {
        using namespace std::string_literals;

        ++cursor; // move past the opening bracket '['

        // Empty object that will accumulate child nodes as we parse them
        Array array_value;

        // Handle 1st pair
        cursor = this->skip_nonsignificant_whitespace(cursor);
        if (this->chars[cursor] != ']') {
            cursor = this->parse_array_element(cursor, array_value);
        } else {
            ++cursor; // move past the closing bracket ']'
            return {cursor, std::move(array_value)};
        }

        // Handle other pairs
        // (the exact same way we do with objects, see the note here)
        while (cursor < this->chars.size()) {
            cursor       = this->skip_nonsignificant_whitespace(cursor);
            const char c = this->chars[cursor];

            if (c == ',') {
                ++cursor; // move past the comma ','
                cursor = this->skip_nonsignificant_whitespace(cursor);
                cursor = this->parse_array_element(cursor, array_value);
            } else if (c == ']') {
                ++cursor; // move past the closing bracket ']'
                return {cursor, std::move(array_value)};
            }
        }

        throw std::runtime_error("JSON array node reached the end of buffer while parsing object contents.");
    }

    std::pair<std::size_t, String> parse_string(std::size_t cursor) {
        using namespace std::string_literals;

        // Empty string that will accumulate characters as we parse them
        std::string string_value;

        ++cursor; // move past the opening quote '"'

        // Serialize string while handling escape sequences.
        //
        // Doing 'string_value += c' for every char is ~50-60% slower than appending whole string at once,
        // which is why we 'buffer' appends by keeping track of 'segment_start' and 'cursor', and appending
        // whole chunks of the buffer to 'string_value' when we encounter an escape sequence or end of the string.
        //
        for (std::size_t segment_start = cursor; cursor < this->chars.size(); ++cursor) {
            const char c = this->chars[cursor];

            // Handle escape sequences inside the string
            if (c == '\\') {
                ++cursor; // move past the backslash '\'

                const char escaped_char = this->chars[cursor];

                // 2-character escape sequences
                if (const char replacement_char = _lookup_parsed_escaped_chars[escaped_char]) {
                    if (cursor >= this->chars.size())
                        throw std::runtime_error("JSON string node reached the end of buffer while"s +
                                                 "parsing a 2-character escape sequence at pos "s +
                                                 std::to_string(cursor) + "."s);

                    string_value.append(this->chars.data() + segment_start, cursor - segment_start - 1);
                    string_value += replacement_char;
                }
                // 5-character escape sequences (escaped unicode HEX codepoints)
                else if (escaped_char == 'u')
                    if (cursor >= this->chars.size() + 4)
                        throw std::runtime_error("JSON string node reached the end of buffer while"s +
                                                 "parsing a 5-character escape sequence at pos "s +
                                                 std::to_string(cursor) + "."s);
                    // TODO:
                    else
                        throw std::runtime_error(
                            "JSON string node encountered unicode hex value while parsing an escape sequence at pos "s +
                            std::to_string(cursor) + ", which is not currently supported."s);
                else
                    throw std::runtime_error("JSON string node encountered unexpected character {"s + escaped_char +
                                             "} while parsing an escape sequence at pos "s + std::to_string(cursor) +
                                             "."s);

                // This covers all non-hex escape sequences according to ECMA-404 specification
                // [https://ecma-international.org/wp-content/uploads/ECMA-404.pdf] (page 4)

                // moving past the escaped character will be done by the loop '++cursor'
                segment_start = cursor + 1;
                continue;
            }
            // NOTE: Hex escape sequences are not supported yet

            // Reached the end of the string
            if (c == '"') {
                string_value.append(this->chars.data() + segment_start, cursor - segment_start);
                ++cursor; // move past the closing quote '"'
                return {cursor, std::move(string_value)};
            }
        }

        throw std::runtime_error("JSON string node reached the end of buffer while parsing string contents.");
    }

    std::pair<std::size_t, Number> parse_number(std::size_t cursor) {
        using namespace std::string_literals;

        Number number_value;

        const auto [numer_end_ptr, error_code] =
            std::from_chars(this->chars.data() + cursor, this->chars.data() + this->chars.size(), number_value);

        // Note:
        // std::from_chars() converts the first complete number it finds in the string,
        // for example "42 meters" would be converted to 42. We rely on that behaviour here.

        if (error_code != std::errc()) {
            // std::errc(0) is a valid enumeration value that represents success
            // even though it does not appear in the enumerator list (which starts at 1)
            if (error_code == std::errc::invalid_argument)
                throw std::runtime_error("JSON number node could not be parsed as a number at pos "s +
                                         std::to_string(cursor) + "."s);
            else if (error_code == std::errc::result_out_of_range)
                throw std::runtime_error(
                    "JSON number node parsed to number larger than its possible binary representation at pos "s +
                    std::to_string(cursor) + "."s);
        }

        return {numer_end_ptr - this->chars.data(), number_value};
    }

    std::pair<std::size_t, Bool> parse_true(std::size_t cursor) {
        using namespace std::string_literals;
        constexpr std::size_t token_length = 4;

        if (cursor + token_length >= this->chars.size())
            throw std::runtime_error("JSON bool node reached the end of buffer while parsing {true}.");

        const bool parsed_correctly =         //
            this->chars[cursor + 0] == 't' && //
            this->chars[cursor + 1] == 'r' && //
            this->chars[cursor + 2] == 'u' && //
            this->chars[cursor + 3] == 'e';   //

        if (!parsed_correctly)
            throw std::runtime_error("JSON bool node could not parse {true} at pos "s + std::to_string(cursor) + "."s);

        return {cursor + token_length, Bool(true)};
    }

    std::pair<std::size_t, Bool> parse_false(std::size_t cursor) {
        using namespace std::string_literals;
        constexpr std::size_t token_length = 5;

        if (cursor + token_length >= this->chars.size())
            throw std::runtime_error("JSON bool node reached the end of buffer while parsing {false}.");

        const bool parsed_correctly =         //
            this->chars[cursor + 0] == 'f' && //
            this->chars[cursor + 1] == 'a' && //
            this->chars[cursor + 2] == 'l' && //
            this->chars[cursor + 3] == 's' && //
            this->chars[cursor + 4] == 'e';   //

        if (!parsed_correctly)
            throw std::runtime_error("JSON bool node could not parse {false} at pos "s + std::to_string(cursor) + "."s);

        return {cursor + token_length, Bool(false)};
    }

    std::pair<std::size_t, Null> parse_null(std::size_t cursor) {
        using namespace std::string_literals;
        constexpr std::size_t token_length = 4;

        if (cursor + token_length >= this->chars.size())
            throw std::runtime_error("JSON null node reached the end of buffer while parsing {null}.");

        const bool parsed_correctly =         //
            this->chars[cursor + 0] == 'n' && //
            this->chars[cursor + 1] == 'u' && //
            this->chars[cursor + 2] == 'l' && //
            this->chars[cursor + 3] == 'l';   //

        if (!parsed_correctly)
            throw std::runtime_error("JSON null node could not parse {null} at pos "s + std::to_string(cursor) + "."s);

        return {cursor + token_length, Null()};
    }
};


// ========================
// --- JSON Serializing ---
// ========================

template <bool prettify>
inline void _serialize_json_recursion(const Node& node, std::string& chars, unsigned int indent_level = 0,
                                      bool skip_first_indent = false) {
    using namespace std::string_literals;
    constexpr std::size_t indent_level_size = 4;
    const std::size_t     indent_size       = indent_level_size * indent_level;

    // first indent should be skipped when printing after a key
    //
    // Example:
    //
    // {
    //     "object": {              <- first indent skipped (Object)
    //         "something": null    <- first indent skipped (Null)
    //     },
    //     "array": [               <- first indent skipped (Array)
    //          1,                  <- first indent NOT skipped (Number)
    //          2                   <- first indent NOT skipped (Number)
    //     ]
    // }
    //
    // We handle 'prettify' segments through 'if constexpr'
    // to avoid  any "trace" overhead on non-prettified serializing
    //

    // Note:
    // The fastest way to append strings to a preallocated buffer seems to be with '+=':
    //    > chars += string_1; chars += string_2; chars += string_3;
    //
    // Using operator '+' slows things down due to additional allocations:
    //    > chars +=  string_1 + string_2 + string_3; // slow
    //
    // '.append()' performs exactly the same as '+=', but has no overload for appending single chars.
    // However it does have an overload for appending N of some character, which is why we use if for indentation.
    //
    // 'std::ostringstream' is painfully slow compared to regular appends
    // so it's out of the question.

    if constexpr (prettify)
        if (!skip_first_indent) chars.append(indent_size, ' ');

    // JSON Object
    if (auto* ptr = node.get_if<Object>()) {
        const auto& object_value = *ptr;

        // Skip all logic for empty objects
        if (object_value.empty()) {
            chars += "{}";
            return;
        }

        chars += '{';
        if constexpr (prettify) chars += '\n';

        for (auto it = object_value.cbegin(); it != object_value.cend();) {
            if constexpr (prettify) chars.append(indent_size + indent_level_size, ' ');
            // Key
            chars += '"';
            chars += it->first;
            if constexpr (prettify) chars += "\": ";
            else chars += "\":";
            // Value
            _serialize_json_recursion<prettify>(it->second, chars, indent_level + 1, true);
            // Comma
            if (++it != object_value.cend()) chars += ','; // prevents trailing comma
            if constexpr (prettify) chars += '\n';
        }

        if constexpr (prettify) chars.append(indent_size, ' ');
        chars += '}';
    }
    // JSON Array
    else if (auto* ptr = node.get_if<Array>()) {
        const auto& array_value = *ptr;

        // Skip all logic for empty arrays
        if (array_value.empty()) {
            chars += "[]";
            return;
        }

        chars += '[';
        if constexpr (prettify) chars += '\n';

        for (auto it = array_value.cbegin(); it != array_value.cend();) {
            // Node
            _serialize_json_recursion<prettify>(*it, chars, indent_level + 1);
            // Comma
            if (++it != array_value.cend()) chars += ','; // prevents trailing comma
            if constexpr (prettify) chars += '\n';
        }
        if constexpr (prettify) chars.append(indent_size, ' ');
        chars += ']';
    }
    // String
    else if (auto* ptr = node.get_if<String>()) {
        const auto& string_value = *ptr;

        chars += '"';
        
        // Serialize string while handling escape sequences.
        /// Without escape sequences we could just do 'chars += string_value'.
        //
        // Since appending individual characters is ~twice as slow as appending the whole string, we use a
        // "buffered" way of appending, appending whole segments up to the currently escaped char.
        // Strings with no escaped chars get appended in a single call.
        //
        std::size_t segment_start = 0;
        for (std::size_t i = 0; i < string_value.size(); ++i) {
            if (const char escaped_char_replacement = _lookup_serialized_escaped_chars[string_value[i]]) {
                chars.append(string_value.data() + segment_start, i - segment_start);
                chars += '\\';
                chars += escaped_char_replacement;
                segment_start = i + 1; // skip over the "actual" technical character in the string
            }
        }
        chars.append(string_value.data() + segment_start, string_value.size() - segment_start);

        chars += '"';
    }
    // Number
    else if (auto* ptr = node.get_if<Number>()) {
        const auto& number_value = *ptr;

        constexpr int max_exponent = std::numeric_limits<Number>::max_exponent10;
        constexpr int max_digits =
            4 + std::numeric_limits<Number>::max_digits10 + std::max(2, _log_10_ceil(max_exponent));
        // should be the smallest buffer size to account for all possible 'std::to_chars()' outputs,
        // see [https://stackoverflow.com/questions/68472720/stdto-chars-minimal-floating-point-buffer-size]

        std::array<char, max_digits> buffer;

        auto [number_end_ptr, error_code] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), number_value);

        if (error_code != std::errc())
            throw std::runtime_error(
                "JSON serializing encountered std::to_chars() formatting error while serializing value {"s +
                std::to_string(number_value) + "}."s);

        const std::string_view number_string(buffer.data(), number_end_ptr - buffer.data());

        // Save NaN/Inf cases as strings, since JSON spec doesn't include IEEE 754.
        // (!) May result in non-homogenous arrays like [ 1.0, "inf" , 3.0, 4.0, "nan" ]
        if (std::isfinite(number_value)) {
            chars.append(buffer.data(), number_end_ptr - buffer.data());
        } else {
            chars += '"';
            chars.append(buffer.data(), number_end_ptr - buffer.data());
            chars += '"';
        }
    }
    // Bool
    else if (auto* ptr = node.get_if<Bool>()) {
        const auto& bool_value = *ptr;
        chars += (bool_value ? "true" : "false");
    }
    // Null
    else if (node.is<Null>()) {
        chars += "null";
    }
}

inline void _serialize_json_to_buffer(std::string& chars, const Node& node, Format format) {
    if (format == Format::PRETTY) _serialize_json_recursion<true>(node, chars);
    else _serialize_json_recursion<false>(node, chars);
}

// ==========================================
// --- Parsing / Serializing API wrappers ---
// ==========================================

inline Node import_string(const std::string& buffer) {
    _parser           parser(buffer);
    const std::size_t json_start = parser.skip_nonsignificant_whitespace(0); // skip leading whitespace
    auto result_node = parser.parse_node(json_start).second; // starts parsing recursively from the root node

    return result_node;
}

inline void export_string(std::string& buffer, const Node& node, Format format = Format::PRETTY) {
    _serialize_json_to_buffer(buffer, node, format);
    // NOTE: Allocating here kinda defeats the whole purpose of saving to a pre-existing buffer,
    // but for now I'll keep it for the sake of having a more uniform API
}

inline Node import_file(const std::string& filepath) {
    const std::string chars = _read_file_to_string(filepath);

    return import_string(chars);
}

inline void export_file(const std::string& filepath, const Node& node, Format format = Format::PRETTY) {
    auto chars = node.to_string(format);
    std::ofstream(filepath).write(chars.data(), chars.size());
    // a little faster than doing 'std::ofstream(path) << node.to_string(format)'
}

namespace literals {
inline Node operator""_utl_json(const char* c_str, std::size_t c_str_size) {
    return import_string(std::string(c_str, c_str_size));
}
} // namespace literals

} // namespace utl::json

#endif
#endif // module utl::json






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::math
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/module_math.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_MATH)
#ifndef UTLHEADERGUARD_MATH
#define UTLHEADERGUARD_MATH

// _______________________ INCLUDES _______________________

#include <cassert>     // assert()
#include <cstddef>     // size_t
#include <functional>  // function<>
#include <type_traits> // enable_if_t<>, void_t<>, is_floating_point<>, is_arithmetic<>,
                       // conditional_t<>, is_integral<>, true_type, false_type
#include <utility>     // declval<>()
#include <vector>      // vector<>

// ____________________ DEVELOPER DOCS ____________________

// Coordinate transformations, mathematical constants and technical helper functions.
// A bit of a mix-bag-of-everything, but in the end pretty useful.
//
// # ::PI, ::PI_TWO, ::PI_HALF, ::E, ::GOLDEN_RATION #
// Constants.
//
// # ::is_addable_with_itself<Type> #
// Integral constant, returns in "::value" whether Type supports 'operator()+' with itself.
//
// # ::is_multipliable_by_scalar<Type> #
// Integral constant, returns in "::value" whether Type supports 'operator()*' with double.
//
// # ::is_sized<Type> #
// Integral constant, returns in "::value" whether Type supports '.size()' method.
//
// # ::abs(), ::sign(), ::sqr(), ::cube(), ::midpoint(), deg_to_rad(), rad_to_deg() #
// Constexpr templated math functions, useful when writing expressions with a "textbook form" math.
//
// # ::uint_difference() #
// Returns abs(uint - uint) with respect to uint size and possible overflow.
//
// # ::linspace() #
// Tabulates [min, max] range with N evenly spaced points and returns it as a vector.
//
// # ::ssize() #
// Returns '.size()' of the argument casted to 'int'.
// Essentially a shortcut for verbose 'static_cast<int>(container.size())'.
//
// # ::ternary_branchless() #
// Branchless ternary operator. Slightly slower that regular ternary on most CPUs.
// Should not be used unless branchess qualifier is necessary (like in GPU computation).
//
// # ::ternary_bitselect() #
// Faster branchless ternary for integer types.
// If 2nd return is ommited, 0 is assumed, which allows for significant optimization.

// ____________________ IMPLEMENTATION ____________________

namespace utl::math {

// =================
// --- Constants ---
// =================

constexpr double PI           = 3.14159265358979323846;
constexpr double PI_TWO       = 2. * PI;
constexpr double PI_HALF      = 0.5 * PI;
constexpr double E            = 2.71828182845904523536;
constexpr double GOLDEN_RATIO = 1.6180339887498948482;

// ===================
// --- Type Traits ---
// ===================

template <typename Type, typename = void>
struct is_addable_with_itself : std::false_type {};

template <typename Type>
struct is_addable_with_itself<Type, std::void_t<decltype(std::declval<Type>() + std::declval<Type>())>
                              // perhaps check that resulting type is same as 'Type', but that can cause issues
                              // with classes like Eigen::MatrixXd that return "foldables" that convert back to
                              // objects in the end
                              > : std::true_type {};

template <typename Type, typename = void>
struct is_multipliable_by_scalar : std::false_type {};

template <typename Type>
struct is_multipliable_by_scalar<Type, std::void_t<decltype(std::declval<Type>() * std::declval<double>())>>
    : std::true_type {};

template <typename Type, typename = void>
struct is_sized : std::false_type {};

template <typename Type>
struct is_sized<Type, std::void_t<decltype(std::declval<Type>().size())>> : std::true_type {};

template <typename FuncType, typename Signature>
using is_function_with_signature = std::is_convertible<FuncType, std::function<Signature>>;

// ======================
// --- Math functions ---
// ======================

template <typename Type, std::enable_if_t<std::is_scalar<Type>::value, bool> = true>
[[nodiscard]] constexpr Type abs(Type x) {
    return (x > Type(0)) ? x : -x;
}

template <typename Type, std::enable_if_t<std::is_scalar<Type>::value, bool> = true>
[[nodiscard]] constexpr Type sign(Type x) {
    return (x > Type(0)) ? Type(1) : Type(-1);
}

template <typename Type, std::enable_if_t<std::is_arithmetic<Type>::value, bool> = true>
[[nodiscard]] constexpr Type sqr(Type x) {
    return x * x;
}

template <typename Type, std::enable_if_t<std::is_arithmetic<Type>::value, bool> = true>
[[nodiscard]] constexpr Type cube(Type x) {
    return x * x * x;
}

template <typename Type, std::enable_if_t<utl::math::is_addable_with_itself<Type>::value, bool> = true,
          std::enable_if_t<utl::math::is_multipliable_by_scalar<Type>::value, bool> = true>
[[nodiscard]] constexpr Type midpoint(Type a, Type b) {
    return (a + b) * 0.5;
}

template <typename IntegerType, std::enable_if_t<std::is_integral<IntegerType>::value, bool> = true>
[[nodiscard]] constexpr int kronecker_delta(IntegerType i, IntegerType j) {
    // 'IntegerType' here is necessary to prevent enforcing static_cast<int>(...) on the callsite
    return (i == j) ? 1 : 0;
}

template <typename IntegerType, std::enable_if_t<std::is_integral<IntegerType>::value, bool> = true>
[[nodiscard]] constexpr int power_of_minus_one(IntegerType power) {
    return (power % IntegerType(2)) ? -1 : 1; // is there a faster way of doing it?
}


// --- deg/rad conversion ---
template <typename FloatType, std::enable_if_t<std::is_floating_point<FloatType>::value, bool> = true>
[[nodiscard]] constexpr FloatType deg_to_rad(FloatType degrees) {
    constexpr FloatType FACTOR = FloatType(PI / 180.);
    return degrees * FACTOR;
}

template <typename FloatType, std::enable_if_t<std::is_floating_point<FloatType>::value, bool> = true>
[[nodiscard]] constexpr FloatType rad_to_deg(FloatType radians) {
    constexpr FloatType FACTOR = FloatType(180. / PI);
    return radians * FACTOR;
}

// ====================
// --- Memory Units ---
// ====================

// Workaround for 'static_assert(false)' making program ill-formed even
// when placed inide an 'if constexpr' branch that never compiles.
// 'static_assert(_always_false_v<T)' on the the other hand doesn't,
// which means we can use it to mark branches that should never compile.
template <class>
inline constexpr bool _always_false_v = false;

enum class MemoryUnit { BYTE, KiB, MiB, GiB, TiB, KB, MB, GB, TB };

template <typename T, MemoryUnit units = MemoryUnit::MiB>
[[nodiscard]] constexpr double memory_size(std::size_t count) {
    const double size_in_bytes = count * sizeof(T); // cast to double is critical here
    if constexpr (units == MemoryUnit::BYTE) return size_in_bytes;
    else if constexpr (units == MemoryUnit::KiB) return size_in_bytes / 1024.;
    else if constexpr (units == MemoryUnit::MiB) return size_in_bytes / 1024. / 1024.;
    else if constexpr (units == MemoryUnit::GiB) return size_in_bytes / 1024. / 1024. / 1024.;
    else if constexpr (units == MemoryUnit::TiB) return size_in_bytes / 1024. / 1024. / 1024. / 1024.;
    else if constexpr (units == MemoryUnit::KB) return size_in_bytes / 1000.;
    else if constexpr (units == MemoryUnit::MB) return size_in_bytes / 1000. / 1000.;
    else if constexpr (units == MemoryUnit::GB) return size_in_bytes / 1000. / 1000. / 1000.;
    else if constexpr (units == MemoryUnit::TB) return size_in_bytes / 1000. / 1000. / 1000. / 1000.;
    else static_assert(_always_false_v<T>, "Function is a non-exhaustive visitor of enum class {MemoryUnit}.");
}

// ===============
// --- Meshing ---
// ===============

// Semantic helpers that allow user to directly pass both interval/point counts for grid subdivision,
// without thinking about whether function need +1 or -1 to its argument
struct Points {
    std::size_t count;

    Points() = delete;
    explicit Points(std::size_t count) : count(count) {}
};

struct Intervals {
    std::size_t count;

    Intervals() = delete;
    explicit Intervals(std::size_t count) : count(count) {}
    Intervals(Points points) : count(points.count - 1) {}
};

template <typename FloatType, std::enable_if_t<std::is_floating_point<FloatType>::value, bool> = true>
[[nodiscard]] std::vector<FloatType> linspace(FloatType L1, FloatType L2, Intervals N) {
    assert(L1 < L2);
    assert(N.count >= 1);

    const FloatType step = (L2 - L1) / N.count;

    std::vector<FloatType> res(N.count + 1);

    res[0] = L1;
    for (std::size_t i = 1; i < res.size(); ++i) res[i] = res[i - 1] + step;

    return res;
}

template <typename FloatType, typename FuncType,
          std::enable_if_t<std::is_floating_point<FloatType>::value, bool>                          = true,
          std::enable_if_t<is_function_with_signature<FuncType, FloatType(FloatType)>::value, bool> = true>
[[nodiscard]] FloatType integrate_trapezoidal(FuncType f, FloatType L1, FloatType L2, Intervals N) {
    assert(L1 < L2);
    assert(N.count >= 1);

    const FloatType step = (L2 - L1) / N.count;

    FloatType sum = 0;
    FloatType x   = L1;

    for (std::size_t i = 0; i < N.count; ++i, x += step) sum += f(x) + f(x + step);

    return FloatType(0.5) * sum * step;
}

// ====================
// --- Misc helpers ---
// ====================

template <typename UintType, std::enable_if_t<std::is_integral<UintType>::value, bool> = true>
[[nodiscard]] constexpr UintType uint_difference(UintType a, UintType b) {
    // Cast to widest type if there is a change values don't fit into a regular 'int'
    using WiderIntType = std::conditional_t<(sizeof(UintType) >= sizeof(int)), int64_t, int>;

    return static_cast<UintType>(utl::math::abs(static_cast<WiderIntType>(a) - static_cast<WiderIntType>(b)));
}

template <typename SizedContainer, std::enable_if_t<utl::math::is_sized<SizedContainer>::value, bool> = true>
[[nodiscard]] int ssize(const SizedContainer& container) {
    return static_cast<int>(container.size());
}

template <typename ArithmeticType, std::enable_if_t<std::is_arithmetic<ArithmeticType>::value, bool> = true>
[[nodiscard]] constexpr ArithmeticType ternary_branchless(bool condition, ArithmeticType return_if_true,
                                                          ArithmeticType return_if_false) {
    return (condition * return_if_true) + (!condition * return_if_false);
}

template <typename IntType, std::enable_if_t<std::is_integral<IntType>::value, bool> = true>
[[nodiscard]] constexpr IntType ternary_bitselect(bool condition, IntType return_if_true, IntType return_if_false) {
    return (return_if_true & -IntType(condition)) | (return_if_false & ~(-IntType(condition)));
}

template <typename IntType, std::enable_if_t<std::is_integral<IntType>::value, bool> = true>
[[nodiscard]] constexpr IntType ternary_bitselect(bool condition, IntType return_if_true) {
    return return_if_true & -IntType(condition);
}

} // namespace utl::math

#endif
#endif // module utl::math






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::mvl
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/module_mvl.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_MVL)
#ifndef UTLHEADERGUARD_MVL
#define UTLHEADERGUARD_MVL

// _______________________ INCLUDES _______________________

#include <algorithm>        // swap(), find(), count(), is_sorted(), min_element(),
                            // max_element(), sort(), stable_sort(), min(), max(), remove_if(), copy()
#include <cmath>            // isfinite()
#include <cstddef>          // size_t, ptrdiff_t, nullptr_t
#include <functional>       // reference_wrapper<>, multiplies<>
#include <initializer_list> // initializer_list<>
#include <iomanip>          // setw()
#include <ios>              // right(), boolalpha(), ios::boolalpha
#include <iterator>         // random_access_iterator_tag, reverse_iterator<>
#include <memory>           // unique_ptr<>
#include <numeric>          // accumulate()
#include <ostream>          // ostream
#include <sstream>          // ostringstream
#include <stdexcept>        // out_of_range, invalid_argument
#include <string>           // string
#include <type_traits>      // conditional_t<>, enable_if_t<>, void_t<>, true_type, false_type, remove_reference_t<>
#include <utility>          // move()
#include <vector>           // vector<>

// ____________________ DEVELOPER DOCS ____________________

// This module tries to implement an "unreasonably flexible yet convenient" template for vectors and matrices,
// in order to do so we have to rely heavily on either conditional compilation or automatic code generation
// (or both). There are multiple seemingly viable approaches to it, yet the most of them proved to be either
// unsuitable or unreasonable from the implementation standpoint. Below is a little rundown of implementations
// that were attempted and used/discared for various reasons.
//
// Currently matrix/vector/view/etc code reuse is implemented though a whole buch of conditional compilation with
// SFINAE, abuse of conditional inheritance, conditional types and constexpr if's. While not perfect, this is
// the best working approach so far. Below is a list of approaches that has been considered & tried:
//
// 1) No code reuse, all classes implemented manually - unrealistically cumbersome, huge code duplication
//
//    => [-] UNSUITABLE APPROACH
//
// 2) Regular OOP with virtual classes - having vtables in lightweigh containers is highly undesirable, we can
//    avoid this using CRTP however we run into issues with multiple inheritance (see below)
//
//    => [-] UNSUITABLE APPROACH
//
// 3) CRTP - we unavoidably run into multiple inheritance issues (diamond problem) with concepts like:
//       [mutable-matrix-like] : [const-matrix-like] & [mutable-iterable] where both parents derive from
//       [const-iterable]
//
//    While those issues are resolvable, their handling becomes more and more difficult as parent classes grow larger.
//    A usual solution for this would be virtual inheritance, however it prevents us from calling derived methods inside
//    the base class due to "static downcast via virtual inheritance" rule, which makes 'static_cast<Derived*>(this)'
//    illegal. We can resolve all name collisions manually, however it leads to a code that is absolutely horrific and
//    easy to mess up.
//
//    There is also an issue of extreme boilerplate, since we want to propagate '_types<>' wrapper and the 'Final'
//    return type to all base classes we end up with extremely unwieldy template arguments that make compile errors and
//    refactoring a challenge.
//
//    => [~] SUITABLE, BUT VERY PROBLEMATIC APPROACH
//
// 4) Template-based mixins without CRTP - these allow to add functionality on top of a class, essentially it works
//    like "inverted" CRTP from the code logic POV. While being almost a perfect solution, it doesn't allow us to
//    return objects of derived type from base class methods, which is a requirement for the intended matrix API.
//
//    => [-] UNSUITABLE APPROACH
//
// 5) Macro-based mixins - insert functionality directly in a class, a simple way of doing exactly what we want.
//    "Mixed-in" methods are simply functions implemented in terms of certain "required" methods.
//    Doesn't lead to huge template chains and doesn't have any inheritance at all. The main downside is being
//    preprocessor based, which exposes some identifiers to a global namespace and makes all "connection points"
//    in logic dependant on correct naming inside macros, rather than actual semantical symbols.
//
//    Difficult to work with due to being ignored by the language server, gets increasingly cumbersome as the number of
//    classes grows and still has significant code duplication (vector/matrix * dense/strided/sparse *
//    * container/view/const_view = 18 manually created definitions).
//
//    => [+-] SUITABLE, BUT HEAVILY FLAWED APPROACH
//
// 6) A single class with all pars of API correctly enabled/disabled through SFINAE. Closes thing to a sensible
//    approach with minimal (basically none) code duplication. However it has a number of non-trivial quirks
//    when it comes to conditionally compiling member variables, types & functions (every group has a quirk of
//    its own, functions are the least problematic). Some of such quirks limit possible conditional API (member
//    types suffer from this) or, for example, prevent use of initialization lists on conditionally compiled
//    constructors (big sad).
//
//    A less obvious downside compared to macros, is that it doesn't get handles as well by the language server
//    autocomplete - SFINAE-disabled methods still show up as autocomplete suggestions, even if they get marked
//    as missing immediately upon writing the whole name.
//
//    => [+] DIFFICULT, BUT WORKABLE, BEST APPROACH SO FAR

// ____________________ IMPLEMENTATION ____________________

namespace utl::mvl {

// =======================
// --- Utility Classes ---
// =======================

// Wrapper that allows passing standard set of member types down the CRTP chain
// (types have to be passed through template args one way or another. Trying to access
// 'derived_type::value_type' from the CRTP base class will fail, since in order for it to be available
// the derived class has to be "implemented" which can only happen after the base class is "implemented",
// which requires the type => a logical loop. So we wrap all the types in a class a pass them down
// the chain a "pack with everything" type of deal. This pack then gets "unwrapped" with
// '_utl_storage_define_types' macro in every class => we have all the member types defined).
//
// (!) Since CRTP approach has been deprecated in favor of mixins, the original reasoning is no longer
// accurate, however it is this a useful class whenever a pack of types have to be passed through a
// template (like, for example, in iterator implementation)
template <class T>
struct _types {
    using value_type      = T;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;
};

// A minimal equivalent of the once proposed 'std::observer_ptr<>'.
// Used in views to allow observer pointers with the same interface as std::unique_ptr,
// which means a generic '.data()` can be implemented without having to make a separate version for views and containers
template <class T>
class _observer_ptr {
public:
    using element_type = T;

private:
    element_type* _data = nullptr;

public:
    // - Constructors -
    constexpr _observer_ptr() noexcept = default;
    constexpr _observer_ptr(std::nullptr_t) noexcept {}
    explicit _observer_ptr(element_type* ptr) : _data(ptr) {}

    _observer_ptr& operator=(element_type* ptr) {
        this->_data = ptr;
        return *this;
    }

    // - Interface -
    [[nodiscard]] constexpr element_type* get() const noexcept { return _data; }
};

// =============
// --- Enums ---
// =============

// Parameter enums
// They specify compiler tensor API
enum class Dimension { VECTOR, MATRIX };
enum class Type { DENSE, STRIDED, SPARSE };
enum class Ownership { CONTAINER, VIEW, CONST_VIEW };

// Config enums
// They specify conditional logic for a tensor
// Their combination + parameter enums fully defines a GenericTensor
enum class Checking { NONE, BOUNDS };
enum class Layout { /* 1D */ FLAT, /* 2D */ RC, CR, /* Other */ SPARSE };

// Overload tags (dummy types used to create "overloads" of .for_each(), which changes the way compiler handles
// name based lookup, preventing shadowing of base class methods)
// TODO: Most likely unnecessary after switching away from CRTP, try removing
enum class _for_each_tag { DUMMY };
enum class _for_each_idx_tag { DUMMY };
enum class _for_each_ij_tag { DUMMY };

// =================
// --- Iterators ---
// =================

// Iterator template for 1D iterator over a tensor.
// Reduces code duplication for const & non-const variants by bundling some conditional logic.
// Uses 'operator[]' of 'ParentPointer' for all logic, thus allowing arbitrary tensors that
// don't have to be contiguous or even ordered in memory.
template <class ParentPointer, class Types, bool is_const_iter = false>
class _flat_iterator {
    using parent_pointer = ParentPointer;

public:
    // Iterator reqs: [General]
    // Contains member types      =>
    using iterator_category = std::random_access_iterator_tag;
    using difference_type   = typename Types::difference_type;
    using value_type        = typename Types::value_type;
    using pointer = typename std::conditional_t<is_const_iter, typename Types::const_pointer, typename Types::pointer>;
    using reference =
        typename std::conditional_t<is_const_iter, typename Types::const_reference, typename Types::reference>;

    // Iterator reqs: [General]
    // Constructible
    _flat_iterator(parent_pointer parent, difference_type idx) : _parent(parent), _idx(idx) {}
    // Copy-constructible => by default
    // Copy-assignable    => by default
    // Destructible       => by default
    // Swappable
    friend void swap(_flat_iterator& lhs, _flat_iterator& rhs) { std::swap(lhs._idx, rhs._idx); }

    // Iterator reqs: [General]
    // Can be incremented (prefix & postfix)
    _flat_iterator& operator++() {
        ++_idx;
        return *this;
    } // prefix
    _flat_iterator operator++(int) {
        _flat_iterator temp = *this;
        ++_idx;
        return temp;
    } // postfix

    // Iterator reqs: [Input iterator]
    // Supports equality/inequality comparisons
    friend bool operator==(const _flat_iterator& it1, const _flat_iterator& it2) { return it1._idx == it2._idx; };
    friend bool operator!=(const _flat_iterator& it1, const _flat_iterator& it2) { return it1._idx != it2._idx; };
    // Can be dereferenced as an rvalue
    reference   operator*() const { return _parent->operator[](_idx); }
    pointer     operator->() const { return &_parent->operator[](_idx); }

    // Iterator reqs: [Output iterator]
    // Can be dereferenced as an lvalue (only for mutable iterator types) => not needed

    // Iterator reqs: [Forward iterator]
    // Default-constructible
    _flat_iterator() : _parent(nullptr), _idx(0) {}
    // "Multi-pass" - dereferencing & incrementing does not affects dereferenceability => satisfied

    // Iterator reqs: [Bidirectional iterator]
    // Can be decremented
    _flat_iterator& operator--() {
        --_idx;
        return *this;
    } // prefix
    _flat_iterator operator--(int) {
        _flat_iterator temp = *this;
        --_idx;
        return temp;
    } // postfix

    // Iterator reqs: [Random Access iterator]
    // See: https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
    // Supports arithmetic operators: it + n, n + it, it - n, it1 - it2
    friend _flat_iterator operator+(const _flat_iterator& it, difference_type diff) {
        return _flat_iterator(it._parent, it._idx + diff);
    }
    friend _flat_iterator operator+(difference_type diff, const _flat_iterator& it) {
        return _flat_iterator(it._parent, it._idx + diff);
    }
    friend _flat_iterator operator-(const _flat_iterator& it, difference_type diff) {
        return _flat_iterator(it._parent, it._idx - diff);
    }
    friend difference_type operator-(const _flat_iterator& it1, const _flat_iterator& it2) {
        return it1._idx - it2._idx;
    }
    // Supports inequality comparisons (<, >, <= and >=) between iterators
    friend bool     operator<(const _flat_iterator& it1, const _flat_iterator& it2) { return it1._idx < it2._idx; }
    friend bool     operator>(const _flat_iterator& it1, const _flat_iterator& it2) { return it1._idx > it2._idx; }
    friend bool     operator<=(const _flat_iterator& it1, const _flat_iterator& it2) { return it1._idx <= it2._idx; }
    friend bool     operator>=(const _flat_iterator& it1, const _flat_iterator& it2) { return it1._idx >= it2._idx; }
    // Standard assumption: Both iterators are from the same container => no need to compare '_parent'
    // Supports compound assignment operations += and -=
    _flat_iterator& operator+=(difference_type diff) {
        _idx += diff;
        return *this;
    }
    _flat_iterator& operator-=(difference_type diff) {
        _idx -= diff;
        return *this;
    }
    // Supports offset dereference operator ([])
    reference operator[](difference_type diff) const { return _parent->operator[](_idx + diff); }

private:
    parent_pointer  _parent;
    difference_type _idx; // not size_type because we have to use it in 'difference_type' operations most of the time
};

// ========================
// --- Helper Functions ---
// ========================

template <class T>
[[nodiscard]] std::unique_ptr<T[]> make_unique_ptr_array(size_t size) {
    return std::unique_ptr<T[]>(new T[size]);
}

// Shortuct for labda-type-based SFINAE
template <class FuncType, class Signature>
using _enable_if_signature = std::enable_if_t<std::is_convertible_v<FuncType, std::function<Signature>>, bool>;

// Variadic stringification
template <typename... Args>
[[nodiscard]] std::string _stringify(const Args&... args) {
    std::ostringstream ss;
    (ss << ... << args);
    return ss.str();
}

template <typename T>
[[nodiscard]] std::string default_stringifier(const T& value) {
    std::ostringstream ss;
    ss << std::right << std::boolalpha << value;
    return ss.str();
} // TODO: Check if that function has any reason to exist.

// Marker for uncreachable code
[[noreturn]] inline void _unreachable() {
// (Implementation from https://en.cppreference.com/w/cpp/utility/unreachable)
// Use compiler specific extensions if possible.
// Even if no extension is used, undefined behavior is still raised by
// an empty function body and the noreturn attribute.
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
    __assume(false);
#else // GCC, Clang
    __builtin_unreachable();
#endif
}

// ======================================
// --- Sparse Matrix Pairs & Triplets ---
// ======================================

template <typename T>
struct SparseEntry1D {
    size_t i;
    T      value;
};

template <typename T>
struct SparseEntry2D {
    size_t i;
    size_t j;
    T      value;
};

template <typename T>
[[nodiscard]] bool _sparse_entry_2d_ordering(const SparseEntry2D<T>& left, const SparseEntry2D<T>& right) {
    return (left.i < right.i) && (left.j < right.j);
}

struct Index2D {
    size_t i;
    size_t j;

    bool operator==(const Index2D& other) const noexcept { return (this->i == other.i) && (this->j == other.j); }
};

[[nodiscard]] inline bool _index_2d_sparse_ordering(const Index2D& l, const Index2D& r) noexcept {
    return (l.i < r.i) && (l.j < r.j);
}

// Shortcut template used to deduce type of '_data' based on ownership inside mixins
template <Ownership ownership, typename ContainerResult, typename ViewResult, typename ConstViewResult>
using _choose_based_on_ownership =
    std::conditional_t<ownership == Ownership::CONTAINER, ContainerResult,
                       std::conditional_t<ownership == Ownership::VIEW, ViewResult, ConstViewResult>>;

// ===================
// --- Type Traits ---
// ===================

template <typename Type, typename = void>
struct _supports_stream_output : std::false_type {};

template <typename Type>
struct _supports_stream_output<Type, std::void_t<decltype(std::declval<std::ostream>() << std::declval<Type>())>>
    : std::true_type {};

// While it'd be nice to avoid macro usage alltogether, having a few macros for generating standardized boilerplate
// that gets repeated several dosen times DRASTICALLY improves the maintainability of the whole conditional compilation
// mechanism down the line. They will be later #undef'ed.

#define _utl_define_operator_support_type_trait(trait_name_, operator_)                                                \
    template <typename Type, typename = void>                                                                          \
    struct trait_name_ : std::false_type {};                                                                           \
                                                                                                                       \
    template <typename Type>                                                                                           \
    struct trait_name_<Type, std::void_t<decltype(std::declval<Type>() operator_ std::declval<Type>())>>               \
        : std::true_type {};                                                                                           \
                                                                                                                       \
    static_assert(true)

_utl_define_operator_support_type_trait(_supports_addition, +);
_utl_define_operator_support_type_trait(_supports_multiplication, *);
_utl_define_operator_support_type_trait(_supports_comparison, <);
_utl_define_operator_support_type_trait(_supports_eq_comparison, ==);

// =====================================
// --- Boilerplate Generation Macros ---
// =====================================

#define _utl_template_arg_defs                                                                                         \
    class T, Dimension _dimension, Type _type, Ownership _ownership, Checking _checking, Layout _layout

#define _utl_template_arg_vals T, _dimension, _type, _ownership, _checking, _layout

#define _utl_require(condition_)                                                                                       \
    typename value_type = T, Dimension dimension = _dimension, Type type = _type, Ownership ownership = _ownership,    \
             Checking checking = _checking, Layout layout = _layout, std::enable_if_t<condition_, bool> = true

#define _utl_reqs(condition_) template <_utl_require(condition_)>

// ===========================
// --- Data Member Classes ---
// ===========================

// Unlike class method, member values can't be templated, which prevents us from using regular 'enable_if_t' SFINAE
// for their conditional compilation. The (seemingly) best workaround to compile members conditionally is to inherit
// 'std::contidional<T, EmptyClass>' where 'T' is a "dummy" class with the sole purpose of having data members to
// inherit. This does not introduce virtualiztion (which is good, that how we want it).

template <int id>
struct _nothing {};

template <_utl_template_arg_defs>
class _2d_extents {
private:
    using size_type = typename _types<T>::size_type;

public:
    size_type _rows = 0;
    size_type _cols = 0;
};

template <_utl_template_arg_defs>
class _2d_strides {
private:
    using size_type = typename _types<T>::size_type;

public:
    size_type _row_stride = 0;
    size_type _col_stride = 0;
};

template <_utl_template_arg_defs>
struct _2d_dense_data {
private:
    using value_type = typename _types<T>::value_type;
    using _data_t    = _choose_based_on_ownership<_ownership, std::unique_ptr<value_type[]>, _observer_ptr<value_type>,
                                               _observer_ptr<const value_type>>;

public:
    _data_t _data;
};

template <_utl_template_arg_defs>
struct _2d_sparse_data {
private:
    using value_type = typename _types<T>::value_type;
    using _triplet_t = _choose_based_on_ownership<_ownership, SparseEntry2D<value_type>,
                                                  SparseEntry2D<std::reference_wrapper<value_type>>,
                                                  SparseEntry2D<std::reference_wrapper<const value_type>>>;

public:
    using triplet_type = _triplet_t;

    std::vector<triplet_type> _data;
};

// ===================
// --- Tensor Type ---
// ===================

template <_utl_template_arg_defs>
class GenericTensor
    // Conditionally compile member variables through inheritance
    : public std::conditional_t<_dimension == Dimension::MATRIX, _2d_extents<_utl_template_arg_vals>, _nothing<1>>,
      public std::conditional_t<_dimension == Dimension::MATRIX && _type == Type::STRIDED,
                                _2d_strides<_utl_template_arg_vals>, _nothing<2>>,
      public std::conditional_t<_type == Type::DENSE || _type == Type::STRIDED, _2d_dense_data<_utl_template_arg_vals>,
                                _nothing<3>>,
      public std::conditional_t<_type == Type::SPARSE, _2d_sparse_data<_utl_template_arg_vals>, _nothing<4>>
// > After this point no non-static member variables will be introduced
{
    // --- Parameter reflection ---
    // ----------------------------
public:
    struct params {
        constexpr static auto dimension = _dimension;
        constexpr static auto type      = _type;
        constexpr static auto ownership = _ownership;
        constexpr static auto checking  = _checking;
        constexpr static auto layout    = _layout;

        // Prevent impossible layouts
        static_assert((dimension == Dimension::VECTOR) == (layout == Layout::FLAT), "Flat layout <=> matrix is 1D.");
        static_assert((type == Type::SPARSE) == (layout == Layout::SPARSE), "Sparse layout <=> matrix is sparse.");
    };

    // --- Member types ---
    // --------------------
private:
    using _type_wrapper = _types<T>;

public:
    using self            = GenericTensor;
    using value_type      = typename _type_wrapper::value_type;
    using size_type       = typename _type_wrapper::size_type;
    using difference_type = typename _type_wrapper::difference_type;
    using reference       = typename _type_wrapper::reference;
    using const_reference = typename _type_wrapper::const_reference;
    using pointer         = typename _type_wrapper::pointer;
    using const_pointer   = typename _type_wrapper::const_pointer;

    // --- Iterators ---
    // -----------------
public:
    using const_iterator         = _flat_iterator<const self*, _type_wrapper, true>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using iterator = std::conditional_t<_ownership != Ownership::CONST_VIEW, _flat_iterator<self*, _type_wrapper>,
                                        const_iterator>; // for const views 'iterator' is the same as 'const_iterator'
    using reverse_iterator = std::reverse_iterator<iterator>;

    [[nodiscard]] const_iterator cbegin() const { return const_iterator(this, 0); }
    [[nodiscard]] const_iterator cend() const { return const_iterator(this, this->size()); }
    [[nodiscard]] const_iterator begin() const { return this->cbegin(); }
    [[nodiscard]] const_iterator end() const { return this->cend(); }

    [[nodiscard]] const_reverse_iterator crbegin() const { return const_reverse_iterator(this->cend()); }
    [[nodiscard]] const_reverse_iterator crend() const { return const_reverse_iterator(this->cbegin()); }
    [[nodiscard]] const_reverse_iterator rbegin() const { return this->crbegin(); }
    [[nodiscard]] const_reverse_iterator rend() const { return this->crend(); }

    _utl_reqs(ownership != Ownership::CONST_VIEW)
    [[nodiscard]] iterator begin() { return iterator(this, 0); }

    _utl_reqs(ownership != Ownership::CONST_VIEW)
    [[nodiscard]] iterator end() { return iterator(this, this->size()); }

    _utl_reqs(ownership != Ownership::CONST_VIEW)
    [[nodiscard]] reverse_iterator rbegin() { return reverse_iterator(this->end()); }

    _utl_reqs(ownership != Ownership::CONST_VIEW)
    [[nodiscard]] reverse_iterator rend() { return reverse_iterator(this->begin()); }

    // --- Basic getters ---
    // ---------------------
public:
    _utl_reqs(dimension == Dimension::MATRIX && (type == Type::DENSE || type == Type::STRIDED))
    [[nodiscard]] size_type size() const noexcept { return this->rows() * this->cols(); }

    _utl_reqs(type == Type::SPARSE)
    [[nodiscard]] size_type size() const noexcept { return this->_data.size(); }

    _utl_reqs(dimension == Dimension::MATRIX)
    [[nodiscard]] size_type rows() const noexcept { return this->_rows; }

    _utl_reqs(dimension == Dimension::MATRIX)
    [[nodiscard]] size_type cols() const noexcept { return this->_cols; }

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::DENSE)
    [[nodiscard]] constexpr size_type row_stride() const noexcept {
        if constexpr (self::params::layout == Layout::RC) return 0;
        if constexpr (self::params::layout == Layout::CR) return 1;
        _unreachable();
    }

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::DENSE)
    [[nodiscard]] constexpr size_type col_stride() const noexcept {
        if constexpr (self::params::layout == Layout::RC) return 1;
        if constexpr (self::params::layout == Layout::CR) return 0;
        _unreachable();
    }
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::STRIDED)
    [[nodiscard]] size_type row_stride() const noexcept { return this->_row_stride; }

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::STRIDED)
    [[nodiscard]] size_type col_stride() const noexcept { return this->_col_stride; }

    _utl_reqs(type == Type::DENSE || type == Type::STRIDED)
    [[nodiscard]] const_pointer data() const noexcept { return this->_data.get(); }

    _utl_reqs(ownership != Ownership::CONST_VIEW && (type == Type::DENSE || type == Type::STRIDED))
    [[nodiscard]] pointer data() noexcept { return this->_data.get(); }

    [[nodiscard]] bool empty() const noexcept { return (this->size() == 0); }

    // --- Advanced getters ---
    // ------------------------
    _utl_reqs(_supports_eq_comparison<value_type>::value)
    [[nodiscard]] bool contains(const_reference value) const {
        return std::find(this->cbegin(), this->cend(), value) != this->cend();
    }

    _utl_reqs(_supports_eq_comparison<value_type>::value)
    [[nodiscard]] size_type count(const_reference value) const {
        return std::count(this->cbegin(), this->cend(), value);
    }

    _utl_reqs(_supports_comparison<value_type>::value)
    [[nodiscard]] bool is_sorted() const { return std::is_sorted(this->cbegin(), this->cend()); }

    template <typename Compare>
    [[nodiscard]] bool is_sorted(Compare cmp) const {
        return std::is_sorted(this->cbegin(), this->cend(), cmp);
    }

    [[nodiscard]] std::vector<value_type> to_std_vector() const { return std::vector(this->cbegin(), this->cend()); }

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::DENSE)
    self transposed() const {
        self res(this->cols(), this->rows());
        this->for_each([&](const value_type& element, size_type i, size_type j) { res(j, i) = element; });
        return res;
    }

    _utl_reqs(ownership == Ownership::CONTAINER)
    [[nodiscard]] self clone() const { return *this; }

    _utl_reqs(ownership == Ownership::CONTAINER)
    [[nodiscard]] self move() & { return std::move(*this); }

    template <Type other_type, Ownership other_ownership, Checking other_checking, Layout other_layout>
    [[nodiscard]] bool
    compare_contents(const GenericTensor<value_type, self::params::dimension, other_type, other_ownership,
                                         other_checking, other_layout>& other) const {
        // Surface-level checks
        if ((this->rows() != other.rows()) || (this->cols() != other.cols())) return false;
        // Compare while respecting sparsity
        constexpr bool is_sparse_l = (self::params::type == Type::SPARSE);
        constexpr bool is_sparse_r = (std::remove_reference_t<decltype(other)>::params::type == Type::SPARSE);
        // Same sparsity comparison
        if constexpr (is_sparse_l == is_sparse_r) {
            return this->size() == other.size() &&
                   this->true_for_all([&](const_reference e, size_type i, size_type j) { return e == other(i, j); });
        }
        // Different sparsity comparison
        // TODO: Impl here and use .all_of() OR .any_of()
        return true;
    }

    // --- Indexation ---
    // ------------------
public:
    // Vector API
    [[nodiscard]] const_reference front() const { return this->operator[](0); }
    [[nodiscard]] const_reference back() const { return this->operator[](this->size() - 1); }
    [[nodiscard]] reference       front() { return this->operator[](0); }
    [[nodiscard]] reference       back() { return this->operator[](this->size() - 1); }

private:
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::STRIDED)
    [[nodiscard]] size_type _get_memory_offset_strided_impl(size_type idx, size_type i, size_type j) const {
        if constexpr (self::params::layout == Layout::RC) return idx * this->col_stride() + this->row_stride() * i;
        if constexpr (self::params::layout == Layout::CR) return idx * this->row_stride() + this->col_stride() * j;
        _unreachable();
    }

public:
    _utl_reqs(type == Type::DENSE)
    [[nodiscard]] size_type get_memory_offset_of_idx(size_type idx) const {
        if constexpr (self::params::checking == Checking::BOUNDS) this->_bound_check_idx(idx);
        return idx;
    }

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::DENSE)
    [[nodiscard]] size_type get_memory_offset_of_ij(size_type i, size_type j) const {
        return this->get_idx_of_ij(i, j);
    }

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::STRIDED)
    [[nodiscard]] size_type get_memory_offset_of_idx(size_type idx) const {
        const auto ij = this->get_ij_of_idx(idx);
        return _get_memory_offset_strided_impl(idx, ij.i, ij.j);
    }

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::STRIDED)
    [[nodiscard]] size_type get_memory_offset_of_ij(size_type i, size_type j) const {
        const auto idx = this->get_idx_of_ij(i, j);
        return _get_memory_offset_strided_impl(idx, i, j);
    }

public:
    // - Flat indexation -
    _utl_reqs(ownership != Ownership::CONST_VIEW && dimension == Dimension::MATRIX &&
              (type == Type::DENSE || type == Type::STRIDED))
    [[nodiscard]] reference operator[](size_type idx) { return this->data()[this->get_memory_offset_of_idx(idx)]; }

    _utl_reqs(dimension == Dimension::MATRIX && (type == Type::DENSE || type == Type::STRIDED))
    [[nodiscard]] const_reference operator[](size_type idx) const {
        return this->data()[this->get_memory_offset_of_idx(idx)];
    }

    _utl_reqs(ownership != Ownership::CONST_VIEW && dimension == Dimension::VECTOR || type == Type::SPARSE)
    [[nodiscard]] reference operator[](size_type idx) {
        if constexpr (self::params::checking == Checking::BOUNDS) this->_bound_check_idx(idx);
        return this->_data[idx].value;
    }

    _utl_reqs(dimension == Dimension::VECTOR || type == Type::SPARSE)
    [[nodiscard]] const_reference operator[](size_type idx) const {
        if constexpr (self::params::checking == Checking::BOUNDS) this->_bound_check_idx(idx);
        return this->_data[idx].value;
    }

    // - 2D indexation -
    _utl_reqs(ownership != Ownership::CONST_VIEW && dimension == Dimension::MATRIX &&
              (type == Type::DENSE || type == Type::STRIDED))
    [[nodiscard]] reference operator()(size_type i, size_type j) {
        return this->data()[this->get_memory_offset_of_ij(i, j)];
    }

    _utl_reqs(dimension == Dimension::MATRIX && (type == Type::DENSE || type == Type::STRIDED))
    [[nodiscard]] const_reference operator()(size_type i, size_type j) const {
        return this->data()[this->get_memory_offset_of_ij(i, j)];
    }

    _utl_reqs(ownership != Ownership::CONST_VIEW && dimension == Dimension::MATRIX && type == Type::SPARSE)
    [[nodiscard]] reference operator()(size_type i, size_type j) {
        if constexpr (self::params::checking == Checking::BOUNDS) this->_bound_check_idx(i, j);
        return this->_data[this->get_idx_of_ij(i, j)].value;
    }
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::SPARSE)
    [[nodiscard]] const_reference operator()(size_type i, size_type j) const {
        if constexpr (self::params::checking == Checking::BOUNDS) this->_bound_check_idx(i, j);
        return this->_data[this->get_idx_of_ij(i, j)].value;
    }

    // --- Index conversions ---
    // -------------------------

    // - Bound checking -
private:
    void _bound_check_idx(size_type idx) const {
        if (idx >= this->size())
            throw std::out_of_range(
                _stringify("idx (which is ", idx, ") >= this->size() (which is ", this->size(), ")"));
    }

    _utl_reqs(dimension == Dimension::MATRIX)
    void _bound_check_ij(size_type i, size_type j) const {
        if (i >= this->rows())
            throw std::out_of_range(_stringify("i (which is ", i, ") >= this->rows() (which is ", this->rows(), ")"));
        else if (j >= this->cols())
            throw std::out_of_range(_stringify("j (which is ", j, ") >= this->cols() (which is ", this->cols(), ")"));
    }

    // - Dense & strided implementations -
private:
    _utl_reqs(dimension == Dimension::MATRIX && (type == Type::DENSE || type == Type::STRIDED))
    [[nodiscard]] size_type _unchecked_get_idx_of_ij(size_type i, size_type j) const {
        if constexpr (self::params::layout == Layout::RC) return i * this->cols() + j;
        if constexpr (self::params::layout == Layout::CR) return j * this->rows() + i;
        _unreachable();
    }

    _utl_reqs(dimension == Dimension::MATRIX && (type == Type::DENSE || type == Type::STRIDED))
    [[nodiscard]] Index2D _unchecked_get_ij_of_idx(size_type idx) const {
        if constexpr (self::params::layout == Layout::RC) return {idx / this->cols(), idx % this->cols()};
        if constexpr (self::params::layout == Layout::CR) return {idx % this->rows(), idx / this->rows()};
        _unreachable();
    }

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::STRIDED && ownership == Ownership::CONTAINER)
    [[nodiscard]] size_type _total_allocated_size() const noexcept {
        // Note 1: Allocated size of the strided matrix is NOT equal to .size() (which is same as rows * cols)
        // This is due to all the padding between the actual elements
        // Note 2: The question of whether .size() should return the number of 'strided' elements or the number
        // of actually allocated elements is rather perplexing, while the second option is more "correct" its
        // usefulness is dubious at bets, while the first one does in fact allow convenient 1D iteration over
        // all the elements. Option 1 also provides an API consistent with strided views, which is why it ended
        // up being the one chosen
        //
        // This method returns a true allocated size
        if constexpr (self::params::layout == Layout::RC)
            return (this->rows() - 1) * this->row_stride() + this->rows() * this->cols() * this->col_stride();
        if constexpr (self::params::layout == Layout::CR)
            return (this->cols() - 1) * this->col_stride() + this->rows() * this->cols() * this->row_stride();
        _unreachable();
    }

public:
    _utl_reqs(dimension == Dimension::MATRIX && (type == Type::DENSE || type == Type::STRIDED))
    [[nodiscard]] size_type get_idx_of_ij(size_type i, size_type j) const {
        if constexpr (self::params::checking == Checking::BOUNDS) this->_bound_check_ij(i, j);
        return _unchecked_get_idx_of_ij(i, j);
    }

    _utl_reqs(dimension == Dimension::MATRIX && (type == Type::DENSE || type == Type::STRIDED))
    [[nodiscard]] Index2D get_ij_of_idx(size_type idx) const {
        if constexpr (self::params::checking == Checking::BOUNDS) this->_bound_check_idx(idx);
        return _unchecked_get_ij_of_idx(idx);
    }

    _utl_reqs(dimension == Dimension::MATRIX && (type == Type::DENSE || type == Type::STRIDED))
    [[nodiscard]] size_type extent_major() const noexcept {
        if constexpr (self::params::layout == Layout::RC) return this->rows();
        if constexpr (self::params::layout == Layout::CR) return this->cols();
        _unreachable();
    }

    _utl_reqs(dimension == Dimension::MATRIX && (type == Type::DENSE || type == Type::STRIDED))
    [[nodiscard]] size_type extent_minor() const noexcept {
        if constexpr (self::params::layout == Layout::RC) return this->cols();
        if constexpr (self::params::layout == Layout::CR) return this->rows();
        _unreachable();
    }

    // - Sparse implementations -
private:
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::SPARSE)
    [[nodiscard]] size_type _search_ij(size_type i, size_type j) const noexcept {
        // Returns this->size() if {i, j} wasn't found.
        // Linear search for small .size() (more efficient fue to prediction and cache locality)
        if (true) {
            for (size_type idx = 0; idx < this->size(); ++idx)
                if (this->_data[idx].i == i && this->_data[idx].j == j) return idx;
            return this->size();
        }
        // TODO: Binary search for larger .size() (N(log2(size)) instead of N(size) asymptotically)
    }

public:
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::SPARSE)
    [[nodiscard]] size_type get_idx_of_ij(size_type i, size_type j) const {
        const size_type idx = this->_search_ij(i, j);
        // Return this->size() if {i, j} wasn't found. Throw with bound checking.
        if constexpr (self::params::checking == Checking::BOUNDS)
            if (idx == this->size())
                throw std::out_of_range(_stringify("Index { ", i, ", ", j, "} in not a part of sparse matrix"));
        return idx;
    }

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::SPARSE)
    [[nodiscard]] Index2D get_ij_of_idx(size_type idx) const {
        if constexpr (self::params::checking == Checking::BOUNDS) this->_bound_check_idx(idx);
        return Index2D{this->_data[idx].i, this->_data[idx].j};
    }

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::SPARSE)
    [[nodiscard]] bool contains_index(size_type i, size_type j) const noexcept {
        return this->_search_ij(i, j) != this->size();
    }

    // --- Reductions ---
    // ------------------
    _utl_reqs(_supports_addition<value_type>::value)
    [[nodiscard]] value_type sum() const { return std::accumulate(this->cbegin(), this->cend(), value_type()); }

    _utl_reqs(_supports_multiplication<value_type>::value)
    [[nodiscard]] value_type product() const {
        return std::accumulate(this->cbegin(), this->cend(), value_type(), std::multiplies<value_type>());
    }

    _utl_reqs(_supports_comparison<value_type>::value)
    [[nodiscard]] value_type min() const { return *std::min_element(this->cbegin(), this->cend()); }

    _utl_reqs(_supports_comparison<value_type>::value)
    [[nodiscard]] value_type max() const { return *std::max_element(this->cbegin(), this->cend()); }

    // --- Predicate operations ---
    // ----------------------------
    template <class PredType, _enable_if_signature<PredType, bool(const_reference)> = true>
    [[nodiscard]] bool true_for_any(PredType predicate) const {
        for (size_type idx = 0; idx < this->size(); ++idx)
            if (predicate(this->operator[](idx), idx)) return true;
        return false;
    }

    template <class PredType, _enable_if_signature<PredType, bool(const_reference, size_type)> = true>
    [[nodiscard]] bool true_for_any(PredType predicate) const {
        for (size_type idx = 0; idx < this->size(); ++idx)
            if (predicate(this->operator[](idx), idx)) return true;
        return false;
    }

    template <class PredType, _enable_if_signature<PredType, bool(const_reference, size_type, size_type)> = true,
              _utl_require(dimension == Dimension::MATRIX)>
              [[nodiscard]] bool true_for_any(PredType predicate) const {
        // Loop over all 2D indices using 1D loop with idx->ij conversion
        // This is just as fast and ensures looping only over existing elements in non-dense matrices
        for (size_type idx = 0; idx < this->size(); ++idx) {
            const auto ij = this->get_ij_of_idx(idx);
            if (predicate(this->operator[](idx), ij.i, ij.j)) return true;
        }
        return false;
    }

    template <class PredType, _enable_if_signature<PredType, bool(const_reference)> = true>
    [[nodiscard]] bool true_for_all(PredType predicate) const {
        auto inversed_predicate = [&](const_reference e) -> bool { return !predicate(e); };
        return !this->true_for_any(inversed_predicate);
    }

    template <class PredType, _enable_if_signature<PredType, bool(const_reference, size_type)> = true>
    [[nodiscard]] bool true_for_all(PredType predicate) const {
        auto inversed_predicate = [&](const_reference e, size_type idx) -> bool { return !predicate(e, idx); };
        return !this->true_for_any(inversed_predicate);
    }

    template <class PredType, _enable_if_signature<PredType, bool(const_reference, size_type, size_type)> = true,
              _utl_require(dimension == Dimension::MATRIX)>
              [[nodiscard]] bool true_for_all(PredType predicate) const {
        // We can reuse .true_for_any() with inverted predicate due to following conjecture:
        // FOR_ALL (predicate)  ~  ! FOR_ANY (!predicate)
        auto inversed_predicate = [&](const_reference e, size_type i, size_type j) -> bool {
            return !predicate(e, i, j);
        };
        return !this->true_for_any(inversed_predicate);
    }

    // --- Const algorithms ---
    // ------------------------
    template <class FuncType, _enable_if_signature<FuncType, void(const_reference)> = true>
    const self& for_each(FuncType func, _for_each_tag = _for_each_tag::DUMMY) const {
        for (size_type idx = 0; idx < this->size(); ++idx) func(this->operator[](idx));
        return *this;
    }

    template <class FuncType, _enable_if_signature<FuncType, void(const_reference, size_type)> = true>
    const self& for_each(FuncType func, _for_each_idx_tag = _for_each_idx_tag::DUMMY) const {
        for (size_type idx = 0; idx < this->size(); ++idx) func(this->operator[](idx), idx);
        return *this;
    }

    template <class FuncType, _enable_if_signature<FuncType, void(const_reference, size_type, size_type)> = true,
              _utl_require(dimension == Dimension::MATRIX)>
              const self& for_each(FuncType func) const {
        // Loop over all 2D indices using 1D loop with idx->ij conversion.
        // This is just as fast and ensures looping only over existing elements in non-dense matrices.
        for (size_type idx = 0; idx < this->size(); ++idx) {
            const auto ij = this->get_ij_of_idx(idx);
            func(this->operator[](idx), ij.i, ij.j);
        }
        return *this;
    }

    // --- Mutating algorithms ---
    // ---------------------------
    template <class FuncType, _enable_if_signature<FuncType, void(reference)> = true,
              _utl_require(ownership != Ownership::CONST_VIEW)>
              self& for_each(FuncType func) {
        for (size_type idx = 0; idx < this->size(); ++idx) func(this->operator[](idx));
        return *this;
    }

    template <class FuncType, _enable_if_signature<FuncType, void(reference, size_type)> = true,
              _utl_require(ownership != Ownership::CONST_VIEW)>
              self& for_each(FuncType func) {
        for (size_type idx = 0; idx < this->size(); ++idx) func(this->operator[](idx), idx);
        return *this;
    }

    template <class FuncType, _enable_if_signature<FuncType, void(reference, size_type, size_type)> = true,
              _utl_require(dimension == Dimension::MATRIX)>
              self& for_each(FuncType func, _for_each_ij_tag = _for_each_ij_tag::DUMMY) {
        for (size_type idx = 0; idx < this->size(); ++idx) {
            const auto ij = this->get_ij_of_idx(idx);
            func(this->operator[](idx), ij.i, ij.j);
        }
        return *this;
    }

    template <class FuncType, _enable_if_signature<FuncType, value_type(const_reference)> = true,
              _utl_require(ownership != Ownership::CONST_VIEW)>
              self& transform(FuncType func) {
        const auto func_wrapper = [&](reference elem) { elem = func(elem); };
        return this->for_each(func_wrapper);
    }

    template <class FuncType, _enable_if_signature<FuncType, value_type(const_reference, size_type)> = true,
              _utl_require(dimension == Dimension::VECTOR && ownership != Ownership::CONST_VIEW)>
              self& transform(FuncType func) {
        const auto func_wrapper = [&](reference elem, size_type i) { elem = func(elem, i); };
        return this->for_each(func_wrapper);
    }

    template <class FuncType, _enable_if_signature<FuncType, value_type(const_reference, size_type, size_type)> = true,
              _utl_require(dimension == Dimension::MATRIX && ownership != Ownership::CONST_VIEW)>
              self& transform(FuncType func, _for_each_ij_tag = _for_each_ij_tag::DUMMY) {
        const auto func_wrapper = [&](reference elem, size_type i, size_type j) { elem = func(elem, i, j); };
        return this->for_each(func_wrapper);
    }

    _utl_reqs(ownership != Ownership::CONST_VIEW)
    self& fill(const_reference value) {
        for (size_type idx = 0; idx < this->size(); ++idx) this->operator[](idx) = value;
        return *this;
    }

    template <class FuncType, _enable_if_signature<FuncType, value_type()> = true,
              _utl_require(ownership != Ownership::CONST_VIEW)>
              self& fill(FuncType func) {
        const auto func_wrapper = [&](reference elem) { elem = func(); };
        return this->for_each(func_wrapper);
    }

    template <class FuncType, _enable_if_signature<FuncType, value_type(size_type)> = true,
              _utl_require(dimension == Dimension::VECTOR && ownership != Ownership::CONST_VIEW)>
              self& fill(FuncType func) {
        const auto func_wrapper = [&](reference elem, size_type i) { elem = func(i); };
        return this->for_each(func_wrapper);
    }

    template <class FuncType, _enable_if_signature<FuncType, value_type(size_type, size_type)> = true,
              _utl_require(dimension == Dimension::MATRIX && ownership != Ownership::CONST_VIEW)>
              self& fill(FuncType func) {
        const auto func_wrapper = [&](reference elem, size_type i, size_type j) { elem = func(i, j); };
        return this->for_each(func_wrapper);
    }

    template <typename Compare, _utl_require(ownership != Ownership::CONST_VIEW)>
              self& sort(Compare cmp) {
        std::sort(this->begin(), this->end(), cmp);
        return *this;
    }
    template <typename Compare, _utl_require(ownership != Ownership::CONST_VIEW)>
              self& stable_sort(Compare cmp) {
        std::stable_sort(this->begin(), this->end(), cmp);
        return *this;
    }

    _utl_reqs(ownership != Ownership::CONST_VIEW && _supports_comparison<value_type>::value)
    self& sort() {
        std::sort(this->begin(), this->end());
        return *this;
    }
    _utl_reqs(ownership != Ownership::CONST_VIEW && _supports_comparison<value_type>::value)
    self& stable_sort() {
        std::stable_sort(this->begin(), this->end());
        return *this;
    }

    // --- Sparse Subviews ---
    // -----------------------

    // - Const views -
private:
    using _cref_triplet_array =
        std::vector<SparseEntry2D<std::reference_wrapper<const value_type>>>; // NOTE: Generalize for 1D

public:
    using sparse_const_view_type = GenericTensor<value_type, self::params::dimension, Type::SPARSE,
                                                 Ownership::CONST_VIEW, self::params::checking, Layout::SPARSE>;

    template <typename UnaryPredicate, _enable_if_signature<UnaryPredicate, bool(const_reference)> = true>
    [[nodiscard]] sparse_const_view_type filter(UnaryPredicate predicate) const {
        const auto forwarded_predicate = [&](const_reference elem, size_type, size_type) -> bool {
            return predicate(elem);
        };
        return this->filter(forwarded_predicate);
        // NOTE: This would need its own implementation for a proper 1D support
    }

    template <typename UnaryPredicate, _enable_if_signature<UnaryPredicate, bool(const_reference, size_type)> = true>
    [[nodiscard]] sparse_const_view_type filter(UnaryPredicate predicate) const {
        const auto forwarded_predicate = [&](const_reference elem, size_type i, size_type j) -> bool {
            const size_type idx = this->get_idx_of_ij(i, j);
            return predicate(elem, idx);
        };
        return this->filter(forwarded_predicate);
        // NOTE: This would need its own implementation for a proper 1D support
    }

    template <typename UnaryPredicate,
              _enable_if_signature<UnaryPredicate, bool(const_reference, size_type, size_type)> = true,
              _utl_require(dimension == Dimension::MATRIX)>
              [[nodiscard]] sparse_const_view_type filter(UnaryPredicate predicate) const {
        // We can't preallocate triplets without scanning predicate through the whole matrix,
        // so we just push back entries into a vector and use to construct a sparse view
        _cref_triplet_array triplets;
        const auto          add_triplet_if_predicate = [&](const_reference elem, size_type i, size_type j) -> void {
            if (predicate(elem, i, j)) triplets.push_back({i, j, elem});
        };

        this->for_each(add_triplet_if_predicate);

        triplets.shrink_to_fit();
        return sparse_const_view_type(this->rows(), this->cols(), std::move(triplets));
    }

    _utl_reqs(dimension == Dimension::MATRIX)
    [[nodiscard]] sparse_const_view_type diagonal() const {
        // Sparse matrices have no better way of getting a diagonal than filtering (i ==j)
        if constexpr (self::params::type == Type::SPARSE) {
            return this->filter([](const_reference, size_type i, size_type j) { return i == j; });
        }
        // Non-sparce matrices can just iterate over diagonal directly
        else {
            const size_type     min_size = std::min(this->rows(), this->cols());
            _cref_triplet_array triplets;
            triplets.reserve(min_size);
            for (size_type k = 0; k < min_size; ++k) triplets.push_back({k, k, this->operator()(k, k)});
            return sparse_const_view_type(this->rows(), this->cols(), std::move(triplets));
        }
    }

    // - Mutable views -
private:
    using _ref_triplet_array =
        std::vector<SparseEntry2D<std::reference_wrapper<value_type>>>; // NOTE: Generalize for 1D

public:
    using sparse_view_type = GenericTensor<value_type, self::params::dimension, Type::SPARSE, Ownership::VIEW,
                                           self::params::checking, Layout::SPARSE>;

    template <typename UnaryPredicate, _utl_require(ownership != Ownership::CONST_VIEW),
                                                    _enable_if_signature<UnaryPredicate, bool(const_reference)> = true>
              [[nodiscard]] sparse_view_type filter(UnaryPredicate predicate) {
        const auto forwarded_predicate = [&](const_reference elem, size_type, size_type) -> bool {
            return predicate(elem);
        };
        return this->filter(forwarded_predicate);
        // NOTE: This would need its own implementation for a proper 1D support
    }

    template <typename UnaryPredicate,
              _utl_require(ownership != Ownership::CONST_VIEW),
                           _enable_if_signature<UnaryPredicate, bool(const_reference, size_type)> = true>
              [[nodiscard]] sparse_view_type filter(UnaryPredicate predicate) {
        const auto forwarded_predicate = [&](const_reference elem, size_type i, size_type j) -> bool {
            const size_type idx = this->get_idx_of_ij(i, j);
            return predicate(elem, idx);
        };
        return this->filter(forwarded_predicate);
        // NOTE: This would need its own implementation for a proper 1D support
    }

    template <typename UnaryPredicate,
              _enable_if_signature<UnaryPredicate, bool(const_reference, size_type, size_type)> = true,
              _utl_require(dimension == Dimension::MATRIX && ownership != Ownership::CONST_VIEW)>
              [[nodiscard]] sparse_view_type filter(UnaryPredicate predicate) {
        // This method implements actual filtering, others just forward predicates to it
        _ref_triplet_array triplets;
        // We can't preallocate triplets without scanning predicate through the whole matrix,
        // so we just push back entries into a vector and use to construct a sparse view
        const auto         add_triplet_if_predicate = [&](reference elem, size_type i, size_type j) -> void {
            if (predicate(elem, i, j)) triplets.push_back({i, j, elem});
        };

        this->for_each(add_triplet_if_predicate);

        triplets.shrink_to_fit();
        return sparse_view_type(this->rows(), this->cols(), std::move(triplets));
    }

    _utl_reqs(dimension == Dimension::MATRIX && ownership != Ownership::CONST_VIEW)
    [[nodiscard]] sparse_view_type diagonal() {
        /* Sparse matrices have no better way of getting a diagonal than filtering (i == j) */
        if constexpr (self::params::type == Type::SPARSE) {
            return this->filter([](const_reference, size_type i, size_type j) { return i == j; });
        } /* Non-sparce matrices can just iterate over diagonal directly */
        else {
            const size_type    min_size = std::min(this->rows(), this->cols());
            _ref_triplet_array triplets;
            triplets.reserve(min_size);
            for (size_type k = 0; k < min_size; ++k) triplets.push_back({k, k, this->operator()(k, k)});
            return sparse_view_type(this->rows(), this->cols(), std::move(triplets));
        }
    }

    // --- Block Subviews ---
    // ----------------------
public:
    // - Const views -
    using block_const_view_type =
        std::conditional_t<self::params::type == Type::SPARSE, sparse_const_view_type,
                           GenericTensor<value_type, self::params::dimension, Type::STRIDED, Ownership::CONST_VIEW,
                                         self::params::checking, self::params::layout>>;

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::SPARSE)
    [[nodiscard]] block_const_view_type block(size_type block_i, size_type block_j, size_type block_rows,
                                              size_type block_cols) const {
        // Sparse matrices have no better way of getting a block than filtering by { i, j }

        // Do the same thing as in .filter(), but shrink resulting view size to
        // { block_rows, block_cols } and shift indexation by { block_i, block_j }
        _cref_triplet_array triplets;

        const auto add_triplet_if_inside_block = [&](const_reference elem, size_type i, size_type j) -> void {
            if ((block_i <= i) && (i < block_i + block_rows) && (block_j <= j) && (j < block_j + block_cols))
                triplets.push_back({i - block_i, j - block_j, elem});
        };

        this->for_each(add_triplet_if_inside_block);

        triplets.shrink_to_fit();
        return block_const_view_type(block_rows, block_cols, std::move(triplets));
    }

    _utl_reqs(dimension == Dimension::MATRIX && type != Type::SPARSE)
    [[nodiscard]] block_const_view_type block(size_type block_i, size_type block_j, size_type block_rows,
                                              size_type block_cols) const {
        if constexpr (self::params::layout == Layout::RC) {
            const size_type row_stride = this->row_stride() + this->col_stride() * (this->cols() - block_cols);
            const size_type col_stride = this->col_stride();
            return block_const_view_type(block_rows, block_cols, row_stride, col_stride,
                                         &this->operator()(block_i, block_j));
        }
        if constexpr (self::params::layout == Layout::CR) {
            const size_type row_stride = this->row_stride();
            const size_type col_stride = this->col_stride() + this->row_stride() * (this->rows() - block_rows);
            return block_const_view_type(block_rows, block_cols, row_stride, col_stride,
                                         &this->operator()(block_i, block_j));
        }
        _unreachable();
    }

    _utl_reqs(dimension == Dimension::MATRIX)
    [[nodiscard]] block_const_view_type row(size_type i) const { return this->block(i, 0, 1, this->cols()); }

    _utl_reqs(dimension == Dimension::MATRIX)
    [[nodiscard]] block_const_view_type col(size_type j) const { return this->block(0, j, this->rows(), 1); }

    // - Mutable views -
    using block_view_type =
        std::conditional_t<self::params::type == Type::SPARSE, sparse_view_type,
                           GenericTensor<value_type, self::params::dimension, Type::STRIDED, Ownership::VIEW,
                                         self::params::checking, self::params::layout>>;

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::SPARSE && ownership != Ownership::CONST_VIEW)
    [[nodiscard]] block_view_type block(size_type block_i, size_type block_j, size_type block_rows,
                                        size_type block_cols) {
        // Sparse matrices have no better way of getting a block than filtering by { i, j }

        // Do the same thing as in .filter(), but shrink resulting view size to
        // { block_rows, block_cols } and shift indexation by { block_i, block_j }
        _ref_triplet_array triplets;

        const auto add_triplet_if_inside_block = [&](reference elem, size_type i, size_type j) -> void {
            if ((block_i <= i) && (i < block_i + block_rows) && (block_j <= j) && (j < block_j + block_cols))
                triplets.push_back({i - block_i, j - block_j, elem});
        };

        this->for_each(add_triplet_if_inside_block);

        triplets.shrink_to_fit();
        return block_view_type(block_rows, block_cols, std::move(triplets));
    }

    _utl_reqs(dimension == Dimension::MATRIX && type != Type::SPARSE && ownership != Ownership::CONST_VIEW)
    [[nodiscard]] block_view_type block(size_type block_i, size_type block_j, size_type block_rows,
                                        size_type block_cols) {
        if constexpr (self::params::layout == Layout::RC) {
            const size_type row_stride = this->row_stride() + this->col_stride() * (this->cols() - block_cols);
            const size_type col_stride = this->col_stride();
            return block_view_type(block_rows, block_cols, row_stride, col_stride, &this->operator()(block_i, block_j));
        }
        if constexpr (self::params::layout == Layout::CR) {
            const size_type row_stride = this->row_stride();
            const size_type col_stride = this->col_stride() + this->row_stride() * (this->rows() - block_rows);
            return block_view_type(block_rows, block_cols, row_stride, col_stride, &this->operator()(block_i, block_j));
        }
        _unreachable();
    }

    _utl_reqs(dimension == Dimension::MATRIX && ownership != Ownership::CONST_VIEW)
    [[nodiscard]] block_view_type row(size_type i) { return this->block(i, 0, 1, this->cols()); }

    _utl_reqs(dimension == Dimension::MATRIX && ownership != Ownership::CONST_VIEW)
    [[nodiscard]] block_view_type col(size_type j) { return this->block(0, j, this->rows(), 1); }

    // --- Sparse operations ---
    // -------------------------

private:
    using _triplet_t = _choose_based_on_ownership<_ownership, SparseEntry2D<value_type>,
                                                  SparseEntry2D<std::reference_wrapper<value_type>>,
                                                  SparseEntry2D<std::reference_wrapper<const value_type>>>;

public:
    using sparse_entry_type = _triplet_t;

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::SPARSE)
    self& insert_triplets(const std::vector<sparse_entry_type>& triplets) {
        // Bulk-insert triplets and sort by index
        const auto ordering = [](const sparse_entry_type& l, const sparse_entry_type& r) -> bool {
            return (l.i < r.i) && (l.j < r.j);
        };

        this->_data.insert(this->_data.end(), triplets.begin(), triplets.end());
        std::sort(this->_data.begin(), this->_data.end(), ordering);

        return *this;
    }

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::SPARSE)
    self& rewrite_triplets(std::vector<sparse_entry_type>&& triplets) {
        // Move-construct all triplets at once and sort by index
        const auto ordering = [](const sparse_entry_type& l, const sparse_entry_type& r) -> bool {
            return (l.i < r.i) && (l.j < r.j);
        };

        this->_data = std::move(triplets);
        std::sort(this->_data.begin(), this->_data.end(), ordering);

        return *this;
    }

    _utl_reqs(dimension == Dimension::MATRIX && type == Type::SPARSE)
    self& erase_triplets(std::vector<Index2D> indices) {
        // Erase triplets with {i, j} from 'indices' using the fact that both
        // 'indices' and triplets are sorted. We can scan through triplets once
        // while advancing 'cursor' when 'indices[cursor]' gets deleted, which
        // result in all necessary triplets being marked for erasure in order.
        std::sort(indices.begin(), indices.end(), _index_2d_sparse_ordering);
        std::size_t cursor = 0;

        const auto erase_condition = [&](const sparse_entry_type& triplet) -> bool {
            /* Stop erasing once all target indices are handled */
            if (cursor == indices.size()) return false;
            if (indices[cursor].i == triplet.i && indices[cursor].j == triplet.j) {
                ++cursor;
                return true;
            }
            return false;
        };

        const auto iter = std::remove_if(this->_data.begin(), this->_data.end(), erase_condition);
        this->_data.erase(iter, this->_data.end());

        // Re-sort triplets just in case
        const auto ordering = [](const sparse_entry_type& l, const sparse_entry_type& r) -> bool {
            return (l.i < r.i) && (l.j < r.j);
        };
        std::sort(this->_data.begin(), this->_data.end(), ordering);

        return *this;
    }

    // --- Constructors ---
    // --------------------

    // - Matrix -
public:
    // Rule of five:
    // copy-ctor       - [+] (deduced from copy-assignment)
    // move-ctor       - [+] (= default)
    // copy-assignment - [+] (custom for dense/strided, same as default for sparse)
    // move-assignment - [+] (= default)
    // destructor      - [+] (= default)

    // Move-ctor is default for all types
    GenericTensor(self&& other) noexcept = default;

    // Move-assignment is default for all types
    self& operator=(self&& other) noexcept = default;

    // Copy-ctor is deduced from assignment operator
    GenericTensor(const self& other) { *this = other; }

    // Copy-assignment
    self& operator=(const self& other) {
        // Note: copy-assignment operator CANNOT be templated, it has to be implemented with 'if constexpr'
        this->_rows = other.rows();
        this->_cols = other.cols();
        if constexpr (self::params::type == Type::DENSE) {
            this->_data = std::move(make_unique_ptr_array<value_type>(this->size()));
            std::copy(other.begin(), other.end(), this->begin());
        }
        if constexpr (self::params::type == Type::STRIDED) {
            this->_row_stride = other.row_stride();
            this->_col_stride = other.col_stride();
            this->_data       = std::move(make_unique_ptr_array<value_type>(this->size()));
            std::copy(other.begin(), other.end(), this->begin());
        }
        if constexpr (self::params::type == Type::SPARSE) { this->_data = other._data; }
        return *this;
    }

    // Default-ctor (containers)
    _utl_reqs(ownership == Ownership::CONTAINER)
    GenericTensor() noexcept {}

    // Default-ctor (views)
    _utl_reqs(ownership != Ownership::CONTAINER)
    GenericTensor() noexcept = delete;

    // Copy-assignment over the config boundaries
    // We can change checking config, copy from matrices with different layouts,
    // copy from views and even matrices of other types
    template <Type other_type, Ownership other_ownership, Checking other_checking, Layout other_layout,
              _utl_require(dimension == Dimension::MATRIX && type == Type::DENSE && ownership == Ownership::CONTAINER)>
              self& operator=(const GenericTensor<value_type, self::params::dimension, other_type, other_ownership,
                                                  other_checking, other_layout>& other) {
        this->_rows = other.rows();
        this->_cols = other.cols();
        this->_data = std::move(make_unique_ptr_array<value_type>(this->size()));
        this->fill(value_type());
        other.for_each([&](const value_type& element, size_type i, size_type j) { this->operator()(i, j) = element; });
        return *this;
        // copying from sparse to dense works, all elements that weren't in the sparse matrix remain default-initialized
    }

    template <Type other_type, Ownership other_ownership, Checking other_checking, Layout other_layout,
              _utl_require(dimension == Dimension::MATRIX && type == Type::STRIDED && ownership == Ownership::CONTAINER)>
              self& operator=(const GenericTensor<value_type, self::params::dimension, other_type, other_ownership,
                                                  other_checking, other_layout>& other) {
        this->_rows       = other.rows();
        this->_cols       = other.cols();
        this->_row_stride = other.row_stride();
        this->_col_strude = other.col_stride();
        this->_data       = std::move(make_unique_ptr_array<value_type>(this->size()));
        this->fill(value_type());
        // Not quite sure whether swapping strides when changing layouts like this is okay,
        // but it seems to be correct
        if constexpr (self::params::layout != other_layout) std::swap(this->_row_stride, this->_cols_stride);
        other.for_each([&](const value_type& element, size_type i, size_type j) { this->operator()(i, j) = element; });
        return *this;
        // copying from sparse to strided works, all elements that weren't in the sparse matrix remain
        // default-initialized
    }

    template <Type other_type, Ownership other_ownership, Checking other_checking, Layout other_layout,
              _utl_require(dimension == Dimension::MATRIX && type == Type::SPARSE && ownership == Ownership::CONTAINER)>
              self& operator=(const GenericTensor<value_type, self::params::dimension, other_type, other_ownership,
                                                  other_checking, other_layout>& other) {
        this->_rows = other.rows();
        this->_cols = other.cols();
        std::vector<sparse_entry_type> triplets;

        // Other sparse matrices can be trivially copied
        if constexpr (other_type == Type::SPARSE) {
            triplets.reserve(other.size());
            other.for_each([&](const value_type& elem, size_type i, size_type j) { triplets.push_back({i, j, elem}); });
        }
        // Non-sparse matrices are filtered by non-default-initialized-elements to construct a sparse subset
        else {
            other.for_each([&](const_reference elem, size_type i, size_type j) {
                if (elem != value_type()) triplets.push_back({i, j, elem});
            });
        }

        this->rewrite_triplets(std::move(triplets));
        return *this;
    }

    // Copy-ctor over the config boundaries (deduced from assignment over config boundaries)
    template <Type other_type, Ownership other_ownership, Checking other_checking, Layout other_layout,
              _utl_require(dimension == Dimension::MATRIX && ownership == Ownership::CONTAINER)>
              GenericTensor(const GenericTensor<value_type, self::params::dimension, other_type, other_ownership,
                                                other_checking, other_layout>& other) {
        *this = other;
    }

    // Move-assignment over config boundaries
    // Note: Unlike copying, we can't change layout, only checking config
    // Also 'other' can no longer be a view or have a different type
    template <Checking other_checking,
              _utl_require(dimension == Dimension::MATRIX && type == Type::DENSE && ownership == Ownership::CONTAINER)>
              self& operator=(GenericTensor<value_type, self::params::dimension, self::params::type,
                                            self::params::ownership, other_checking, self::params::layout>&& other) {
        this->_rows = other.rows();
        this->_cols = other.cols();
        this->_data = std::move(other._data);
        return *this;
    }

    template <Checking other_checking, _utl_require(dimension == Dimension::MATRIX && type == Type::STRIDED && ownership == Ownership::CONTAINER)>
              self& operator=(GenericTensor<value_type, self::params::dimension, self::params::type,
                                            self::params::ownership, other_checking, self::params::layout>&& other) {
        this->_rows       = other.rows();
        this->_cols       = other.cols();
        this->_row_stride = other.row_stride();
        this->_col_stride = other.col_stride();
        this->_data       = std::move(other._data);
        return *this;
    }

    template <Checking other_checking,
              _utl_require(dimension == Dimension::MATRIX && type == Type::SPARSE && ownership == Ownership::CONTAINER)>
              self& operator=(GenericTensor<value_type, self::params::dimension, self::params::type,
                                            self::params::ownership, other_checking, self::params::layout>&& other) {
        this->_rows = other.rows();
        this->_cols = other.cols();
        this->_data = std::move(other._data);
        return *this;
    }

    // Move-ctor over the config boundaries (deduced from move-assignment over config boundaries)
    template <Type other_type, Ownership other_ownership, Checking other_checking, Layout other_layout,
              _utl_require(dimension == Dimension::MATRIX && ownership == Ownership::CONTAINER)>
              GenericTensor(GenericTensor<value_type, self::params::dimension, other_type, other_ownership,
                                          other_checking, other_layout>&& other) {
        *this = std::move(other);
    }

    // Init-with-value
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::DENSE && ownership == Ownership::CONTAINER)
    explicit GenericTensor(size_type rows, size_type cols, const_reference value = value_type()) noexcept {
        this->_rows = rows;
        this->_cols = cols;
        this->_data = std::move(make_unique_ptr_array<value_type>(this->size()));
        this->fill(value);
    }

    // Init-with-lambda
    template <typename FuncType,
              _utl_require(dimension == Dimension::MATRIX && type == Type::DENSE && ownership == Ownership::CONTAINER) >
              explicit GenericTensor(size_type rows, size_type cols, FuncType init_func) {
        // .fill() already takes care of preventing improper values of 'FuncType', no need to do the check here
        this->_rows = rows;
        this->_cols = cols;
        this->_data = std::move(make_unique_ptr_array<value_type>(this->size()));
        this->fill(init_func);
    }

    // Init-with-ilist
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::DENSE && ownership == Ownership::CONTAINER)
    GenericTensor(std::initializer_list<std::initializer_list<value_type>> init) {
        this->_rows = init.size();
        this->_cols = (*init.begin()).size();
        this->_data = std::move(make_unique_ptr_array<value_type>(this->size()));

        // Check dimensions (throw if cols have different dimensions)
        for (auto row_it = init.begin(); row_it < init.end(); ++row_it)
            if (static_cast<size_type>((*row_it).end() - (*row_it).begin()) != this->cols())
                throw std::invalid_argument("Initializer list dimensions don't match.");

        // Copy elements
        for (size_type i = 0; i < this->rows(); ++i)
            for (size_type j = 0; j < this->cols(); ++j) this->operator()(i, j) = (init.begin()[i]).begin()[j];
    }

    // Init-with-data
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::DENSE && ownership == Ownership::CONTAINER)
    explicit GenericTensor(size_type rows, size_type cols, pointer data_ptr) noexcept {
        this->_rows = rows;
        this->_cols = cols;
        this->_data = std::move(decltype(this->_data)(data_ptr));
    }

    // - Matrix View -

    // Init-from-data
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::DENSE && ownership == Ownership::VIEW)
    explicit GenericTensor(size_type rows, size_type cols, pointer data_ptr) {
        this->_rows = rows;
        this->_cols = cols;
        this->_data = data_ptr;
    }

    // Init-from-tensor (any tensor of the same API type)
    template <Ownership other_ownership, Checking other_checking, Layout other_layout,
              _utl_require(dimension == Dimension::MATRIX && type == Type::DENSE && ownership == Ownership::VIEW)>
              GenericTensor(GenericTensor<value_type, self::params::dimension, self::params::type, other_ownership,
                                          other_checking, other_layout>& other) {
        this->_rows = other.rows();
        this->_cols = other.cols();
        this->_data = other.data();
    }

    // - Const Matrix View -

    // Init-from-data
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::DENSE && ownership == Ownership::CONST_VIEW)
    explicit GenericTensor(size_type rows, size_type cols, const_pointer data_ptr) {
        this->_rows = rows;
        this->_cols = cols;
        this->_data = data_ptr;
    }

    // Init-from-tensor (any tensor of the same API type)
    template <Ownership other_ownership, Checking other_checking, Layout other_layout,
              _utl_require(dimension == Dimension::MATRIX && type == Type::DENSE && ownership == Ownership::CONST_VIEW)>
              GenericTensor(const GenericTensor<value_type, self::params::dimension, self::params::type,
                                                other_ownership, other_checking, other_layout>& other) {
        this->_rows = other.rows();
        this->_cols = other.cols();
        this->_data = other.data();
    }

    // - Strided Matrix -

    // Init-with-value
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::STRIDED && ownership == Ownership::CONTAINER)
    explicit GenericTensor(size_type rows, size_type cols, size_type row_stride, size_type col_stride,
                           const_reference value = value_type()) noexcept {
        this->_rows       = rows;
        this->_cols       = cols;
        this->_row_stride = row_stride;
        this->_col_stride = col_stride;
        // Allocates size is NOT the same as .size() due to padding, see notes on '_total_allocated_size()'
        this->_data       = std::move(make_unique_ptr_array<value_type>(this->_total_allocated_size()));
        this->fill(value);
    }

    // Init-with-lambda
    template <typename FuncType, _utl_require(dimension == Dimension::MATRIX && type == Type::STRIDED && ownership == Ownership::CONTAINER) >
              explicit GenericTensor(size_type rows, size_type cols, size_type row_stride, size_type col_stride,
                                     FuncType init_func) {
        // .fill() already takes care of preventing improper values of 'FuncType', no need to do the check here
        this->_rows       = rows;
        this->_cols       = cols;
        this->_row_stride = row_stride;
        this->_col_stride = col_stride;
        // Allocates size is NOT the same as .size() due to padding, see notes on '_total_allocated_size()'
        this->_data       = std::move(make_unique_ptr_array<value_type>(this->_total_allocated_size()));
        this->fill(init_func);
    }

    // Init-with-ilist
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::STRIDED && ownership == Ownership::CONTAINER)
    GenericTensor(std::initializer_list<std::initializer_list<value_type>> init, size_type row_stride,
                  size_type col_stride) {
        this->_rows       = init.size();
        this->_cols       = (*init.begin()).size();
        this->_row_stride = row_stride;
        this->_col_stride = col_stride;
        // Allocates size is NOT the same as .size() due to padding, see notes on '_total_allocated_size()'
        this->_data       = std::move(make_unique_ptr_array<value_type>(this->_total_allocated_size()));

        // Check dimensions (throw if cols have different dimensions)
        for (auto row_it = init.begin(); row_it < init.end(); ++row_it)
            if ((*row_it).end() - (*row_it).begin() != this->_cols)
                throw std::invalid_argument("Initializer list dimensions don't match.");

        // Copy elements
        for (size_type i = 0; i < this->rows(); ++i)
            for (size_type j = 0; j < this->cols(); ++j) this->operator()(i, j) = (init.begin()[i]).begin()[j];
    }

    // Init-with-data
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::STRIDED && ownership == Ownership::CONTAINER)
    explicit GenericTensor(size_type rows, size_type cols, size_type row_stride, size_type col_stride,
                           pointer data_ptr) noexcept {
        this->_rows       = rows;
        this->_cols       = cols;
        this->_row_stride = row_stride;
        this->_col_stride = col_stride;
        this->_data       = std::move(decltype(this->_data)(data_ptr));
    }

    // - Strided Matrix View -

    // Init-from-data
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::STRIDED && ownership == Ownership::VIEW)
    explicit GenericTensor(size_type rows, size_type cols, size_type row_stride, size_type col_stride,
                           pointer data_ptr) {
        this->_rows       = rows;
        this->_cols       = cols;
        this->_row_stride = row_stride;
        this->_col_stride = col_stride;
        this->_data       = data_ptr;
    }

    // Init-from-tensor (any tensor of the same API type)
    template <Ownership other_ownership, Checking other_checking, Layout other_layout,
              _utl_require(dimension == Dimension::MATRIX && type == Type::STRIDED && ownership == Ownership::VIEW)>
              GenericTensor(GenericTensor<value_type, self::params::dimension, self::params::type, other_ownership,
                                          other_checking, other_layout>& other) {
        this->_rows       = other.rows();
        this->_cols       = other.cols();
        this->_row_stride = other.row_stride();
        this->_col_stride = other.col_stride();
        this->_data       = other.data();
    }

    // - Const Strided Matrix View -

    // Init-from-data
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::STRIDED && ownership == Ownership::CONST_VIEW)
    explicit GenericTensor(size_type rows, size_type cols, size_type row_stride, size_type col_stride,
                           const_pointer data_ptr) {
        this->_rows       = rows;
        this->_cols       = cols;
        this->_row_stride = row_stride;
        this->_col_stride = col_stride;
        this->_data       = data_ptr;
    }

    // Init-from-tensor (any tensor of the same API type)
    template <Ownership other_ownership, Checking other_checking, Layout other_layout,
              _utl_require(dimension == Dimension::MATRIX && type == Type::STRIDED && ownership == Ownership::CONST_VIEW)>
              GenericTensor(const GenericTensor<value_type, self::params::dimension, self::params::type,
                                                other_ownership, other_checking, other_layout>& other) {
        this->_rows       = other.rows();
        this->_cols       = other.cols();
        this->_row_stride = other.row_stride();
        this->_col_stride = other.col_stride();
        this->_data       = other.data();
    }

    // - Sparse Matrix / Sparse Matrix View / Sparse Matrix Const View -

    // Init-from-data (copy)
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::SPARSE)
    explicit GenericTensor(size_type rows, size_type cols, const std::vector<sparse_entry_type>& data) {
        this->_rows = rows;
        this->_cols = cols;
        this->insert_triplets(std::move(data));
    }

    // Init-from-data (move)
    _utl_reqs(dimension == Dimension::MATRIX && type == Type::SPARSE)
    explicit GenericTensor(size_type rows, size_type cols, std::vector<sparse_entry_type>&& data) {
        this->_rows = rows;
        this->_cols = cols;
        this->rewrite_triplets(std::move(data));
    }
};

// ===========================
// --- Predefined Typedefs ---
// ===========================

constexpr auto _default_checking        = Checking::NONE;
constexpr auto _default_layout_dense_2d = Layout::RC;

// - Dense 2D -
template <typename T, Checking checking = _default_checking, Layout layout = _default_layout_dense_2d>
using Matrix = GenericTensor<T, Dimension::MATRIX, Type::DENSE, Ownership::CONTAINER, checking, layout>;

template <typename T, Checking checking = _default_checking, Layout layout = _default_layout_dense_2d>
using MatrixView = GenericTensor<T, Dimension::MATRIX, Type::DENSE, Ownership::VIEW, checking, layout>;

template <typename T, Checking checking = _default_checking, Layout layout = _default_layout_dense_2d>
using ConstMatrixView = GenericTensor<T, Dimension::MATRIX, Type::DENSE, Ownership::CONST_VIEW, checking, layout>;

// - Strided 2D -
template <typename T, Checking checking = _default_checking, Layout layout = _default_layout_dense_2d>
using StridedMatrix = GenericTensor<T, Dimension::MATRIX, Type::STRIDED, Ownership::CONTAINER, checking, layout>;

template <typename T, Checking checking = _default_checking, Layout layout = _default_layout_dense_2d>
using StridedMatrixView = GenericTensor<T, Dimension::MATRIX, Type::STRIDED, Ownership::VIEW, checking, layout>;

template <typename T, Checking checking = _default_checking, Layout layout = _default_layout_dense_2d>
using ConstStridedMatrixView =
    GenericTensor<T, Dimension::MATRIX, Type::STRIDED, Ownership::CONST_VIEW, checking, layout>;

// - Sparse 2D -
template <typename T, Checking checking = _default_checking>
using SparseMatrix = GenericTensor<T, Dimension::MATRIX, Type::SPARSE, Ownership::CONTAINER, checking, Layout::SPARSE>;

template <typename T, Checking checking = _default_checking>
using SparseMatrixView = GenericTensor<T, Dimension::MATRIX, Type::SPARSE, Ownership::VIEW, checking, Layout::SPARSE>;

template <typename T, Checking checking = _default_checking>
using ConstSparseMatrixView =
    GenericTensor<T, Dimension::MATRIX, Type::SPARSE, Ownership::CONST_VIEW, checking, Layout::SPARSE>;

// ==================
// --- Formatters ---
// ==================

namespace format {

// TODO: Formats 'as_matrix', 'as_dictionary', 'as_json_array', 'as_raw_text' need 1D overloads

// - Human-readable formats -

constexpr std::size_t max_displayed_rows      = 70;
constexpr std::size_t max_displayed_cols      = 40;
constexpr std::size_t max_displayed_flat_size = 500;
constexpr auto        content_indent          = "  ";

template <class T, Dimension dimension, Type type, Ownership ownership, Checking checking, Layout layout>
[[nodiscard]] std::string
_stringify_metainfo(const GenericTensor<T, dimension, type, ownership, checking, layout>& tensor) {
    std::ostringstream ss;
    ss << "Tensor [size = " << tensor.size() << "] (" << tensor.rows() << " x " << tensor.cols() << "):\n";
    return ss.str();
}

template <class T, Dimension dimension, Type type, Ownership ownership, Checking checking, Layout layout>
[[nodiscard]] std::string _as_too_large(const GenericTensor<T, dimension, type, ownership, checking, layout>& tensor) {
    std::ostringstream ss;
    ss << _stringify_metainfo(tensor) << content_indent << "<hidden due to large size>\n";
    return ss.str();
}

template <class T>
[[nodiscard]] std::string _ss_stringify(const T& value) {
    std::ostringstream ss;
    ss.flags(std::ios::boolalpha);
    ss << value;
    return ss.str();
}

template <class T>
[[nodiscard]] std::string _ss_stringify_for_json(const T& value) {
    // Modification of '_ss_stringify()' that properly handles floats for JSON
    std::ostringstream ss;
    ss.flags(std::ios::boolalpha);

    if constexpr (std::is_floating_point_v<T>) {
        if (std::isfinite(value)) ss << value;
        else ss << "\"" << value << "\"";
    } else {
        ss << value;
    }

    return ss.str();
}

template <class T, Dimension dimension, Type type, Ownership ownership, Checking checking, Layout layout>
[[nodiscard]] std::string as_vector(const GenericTensor<T, dimension, type, ownership, checking, layout>& tensor) {
    if (tensor.size() > max_displayed_flat_size) return _as_too_large(tensor);

    std::ostringstream ss;
    ss << _stringify_metainfo(tensor);

    ss << content_indent << "{ ";
    for (std::size_t idx = 0; idx < tensor.size(); ++idx) ss << tensor[idx] << (idx + 1 < tensor.size() ? ", " : "");
    ss << " }\n";

    return ss.str();
}

template <class T, Type type, Ownership ownership, Checking checking, Layout layout>
[[nodiscard]] std::string
as_matrix(const GenericTensor<T, Dimension::MATRIX, type, ownership, checking, layout>& tensor) {
    if (tensor.rows() > max_displayed_rows || tensor.cols() > max_displayed_cols) return _as_too_large(tensor);

    // Take care of sparsity using 'fill-ctor + for_each()' - if present, missing elements will just stay "default"
    GenericTensor<std::string, Dimension::MATRIX, Type::DENSE, Ownership::CONTAINER, Checking::NONE, Layout::RC>
        strings(tensor.rows(), tensor.cols(), "-");
    tensor.for_each([&](const T& elem, std::size_t i, std::size_t j) { strings(i, j) = _ss_stringify(elem); });

    // Get appropriate widths for each column - we want matrix to format nicely
    std::vector<std::size_t> column_widths(strings.cols());
    for (std::size_t i = 0; i < strings.rows(); ++i)
        for (std::size_t j = 0; j < strings.cols(); ++j)
            column_widths[j] = std::max(column_widths[j], strings(i, j).size());

    // Output the formatted result
    std::ostringstream ss;
    ss << _stringify_metainfo(tensor);

    for (std::size_t i = 0; i < strings.rows(); ++i) {
        ss << content_indent << "[ ";
        for (std::size_t j = 0; j < strings.cols(); ++j)
            ss << std::setw(column_widths[j]) << strings(i, j) << (j + 1 < strings.cols() ? " " : "");
        ss << " ]\n";
    }

    return ss.str();
}

template <class T, Type type, Ownership ownership, Checking checking, Layout layout>
[[nodiscard]] std::string
as_dictionary(const GenericTensor<T, Dimension::MATRIX, type, ownership, checking, layout>& tensor) {
    if (tensor.size() > max_displayed_flat_size) return _as_too_large(tensor);

    std::ostringstream ss;
    ss << _stringify_metainfo(tensor);

    tensor.for_each([&](const T& elem, std::size_t i, std::size_t j) {
        ss << content_indent << "(" << i << ", " << j << ") = " << _ss_stringify(elem) << "\n";
    });

    return ss.str();
}

// - Export formats -

template <class T, Type type, Ownership ownership, Checking checking, Layout layout>
[[nodiscard]] std::string
as_raw_text(const GenericTensor<T, Dimension::MATRIX, type, ownership, checking, layout>& tensor) {
    // Take care of sparsity using 'fill-ctor + for_each()' - if present, missing elements will just stay "default"
    GenericTensor<std::string, Dimension::MATRIX, Type::DENSE, Ownership::CONTAINER, Checking::NONE, Layout::RC>
        strings(tensor.rows(), tensor.cols(), _ss_stringify(T()));
    tensor.for_each([&](const T& elem, std::size_t i, std::size_t j) { strings(i, j) = _ss_stringify(elem); });

    // Output the formatted result
    std::ostringstream ss;

    for (std::size_t i = 0; i < strings.rows(); ++i) {
        for (std::size_t j = 0; j < strings.cols(); ++j) ss << strings(i, j) << (j + 1 < strings.cols() ? " " : "");
        ss << " \n";
    }
    ss << "\n";

    return ss.str();
}

template <class T, Type type, Ownership ownership, Checking checking, Layout layout>
[[nodiscard]] std::string
as_json_array(const GenericTensor<T, Dimension::MATRIX, type, ownership, checking, layout>& tensor) {
    // Take care of sparsity using 'fill-ctor + for_each()' - if present, missing elements will just stay "default"
    GenericTensor<std::string, Dimension::MATRIX, Type::DENSE, Ownership::CONTAINER, Checking::NONE, Layout::RC>
        strings(tensor.rows(), tensor.cols(), _ss_stringify(T()));
    tensor.for_each([&](const T& elem, std::size_t i, std::size_t j) { strings(i, j) = _ss_stringify(elem); });

    // Get appropriate widths for each column - we want matrix to format nicely
    std::vector<std::size_t> column_widths(strings.cols());
    for (std::size_t i = 0; i < strings.rows(); ++i)
        for (std::size_t j = 0; j < strings.cols(); ++j)
            column_widths[j] = std::max(column_widths[j], strings(i, j).size());

    // Output the formatted result
    std::ostringstream ss;

    ss << "[\n";
    for (std::size_t i = 0; i < strings.rows(); ++i) {
        ss << "  [ ";
        for (std::size_t j = 0; j < strings.cols(); ++j)
            ss << std::setw(column_widths[j]) << strings(i, j) << (j + 1 < strings.cols() ? ", " : "");
        ss << " ]" << (i + 1 < strings.rows() ? "," : "") << " \n";
    }
    ss << "]\n";

    return ss.str();
}

} // namespace format

// Clear out internal macros
#undef _utl_define_operator_support_type_trait
#undef _utl_template_arg_defs
#undef _utl_template_arg_vals
#undef _utl_require
#undef _utl_reqs

} // namespace utl::mvl

#endif
#endif // module utl::mvl






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::progressbar
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/module_progressbar.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_PROGRESSBAR)
#ifndef UTLHEADERGUARD_PROGRESSBAR
#define UTLHEADERGUARD_PROGRESSBAR

// _______________________ INCLUDES _______________________

#include <algorithm> // fill_n
#include <chrono>    // chrono::steady_clock, chrono::time_point<>, chrono::duration_cast<>, chrono::seconds
#include <cmath>     // floor()
#include <cstddef>   // size_t
#include <iomanip>   // setprecision()
#include <ios>       // fixed
#include <iostream>  // cout
#include <iterator>  // ostreambuf_iterator<>
#include <ostream>   // ostream
#include <sstream>   // ostringstream
#include <string>    // string

// ____________________ DEVELOPER DOCS ____________________

// Simple progress bars for terminal applications. Rendered in ASCII on the main thread with manual updates
// for maximal compatibility. Perhaps can be extended with some fancier async options that display animations.
//
// # ::set_ostream() #
// Sets ostream used for progress bars.
//
// # ::Percentage #
// Proper progress bar, uses carriage return escape sequence (\r) to render new states in the same spot.
// Shows an estimate of remaining time.
//
// # ::Ruler #
// Primitive & lightweight progress bar, useful when terminal has no proper support for escape sequences.

// ____________________ IMPLEMENTATION ____________________

namespace utl::progressbar {

inline std::ostream* _output_stream = &std::cout;

inline void set_ostream(std::ostream& new_ostream) { _output_stream = &new_ostream; }

class Percentage {
private:
    char done_char;
    char not_done_char;
    bool show_time_estimate;

    std::size_t length_total;   // full   bar length
    std::size_t length_current; // filled bar length

    double last_update_percentage;
    double update_rate;

    using Clock     = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint timepoint_start; // used to estimate remaining time
    TimePoint timepoint_current;

    int previous_string_length; // used to properly return the carriage when dealing with changed string size

    void draw_progressbar(double percentage) {
        const auto displayed_percentage = this->update_rate * std::floor(percentage / this->update_rate);
        // floor percentage to a closest multiple of 'update_rate' for nicer display
        // since actual updates are only required to happen no more ofter than 'update_rate'
        // and don't have to correspond to exact multiples of it

        // Estimate remaining time (linearly) and format it min + sec
        const auto time_elapsed       = this->timepoint_current - this->timepoint_start;
        const auto estimate_full      = time_elapsed / percentage;
        const auto estimate_remaining = estimate_full - time_elapsed;

        const auto estimate_remaining_sec = std::chrono::duration_cast<std::chrono::seconds>(estimate_remaining);

        const auto displayed_min = (estimate_remaining_sec / 60ll).count();
        const auto displayed_sec = (estimate_remaining_sec % 60ll).count();

        const bool show_min  = (displayed_min != 0);
        const bool show_sec  = (displayed_sec != 0) && !show_min;
        const bool show_time = (estimate_remaining_sec.count() > 0);

        std::ostringstream ss;

        // Print bar
        ss << '[';
        std::fill_n(std::ostreambuf_iterator<char>(ss), this->length_current, this->done_char);
        std::fill_n(std::ostreambuf_iterator<char>(ss), this->length_total - this->length_current, this->not_done_char);
        ss << ']';

        // Print percentage
        ss << ' ' << std::fixed << std::setprecision(2) << 100. * displayed_percentage << '%';

        // Print time estimate
        if (this->show_time_estimate && show_time) {
            ss << " (remaining:";
            if (show_min) ss << ' ' << displayed_min << " min";
            if (show_sec) ss << ' ' << displayed_sec << " sec";
            ss << ')';
        }

        const std::string bar_string = ss.str();

        // Add spaces at the end to overwrite the previous string if it was longer that current
        const int current_string_length = static_cast<int>(bar_string.length());
        const int string_length_diff    = this->previous_string_length - current_string_length;

        if (string_length_diff > 0) { std::fill_n(std::ostreambuf_iterator<char>(ss), string_length_diff, ' '); }

        this->previous_string_length = current_string_length;

        // Return the carriage
        (*_output_stream) << ss.str(); // don't reuse 'bar_string', now 'ss' can also contain spaces at the end
        (*_output_stream) << '\r';
        (*_output_stream).flush();
        // '\r' returns cursor to the beginning of the line => most sensible consoles will
        // render render new lines over the last one. Otherwise every update produces a
        // bar on a new line, which looks worse but isn't critical for the purpose.
    }

public:
    Percentage(char done_char = '#', char not_done_char = '.', std::size_t bar_length = 30, double update_rate = 1e-2,
               bool show_time_estimate = true)
        : done_char(done_char), not_done_char(not_done_char), show_time_estimate(show_time_estimate),
          length_total(bar_length), length_current(0), last_update_percentage(0), update_rate(update_rate),
          timepoint_start(Clock::now()), previous_string_length(static_cast<int>(bar_length) + sizeof("[] 100.00%")) {}

    void start() {
        this->last_update_percentage = 0.;
        this->length_current         = 0;
        this->timepoint_start        = Clock::now();
        (*_output_stream) << '\n';
    }

    void set_progress(double percentage) {
        if (percentage - this->last_update_percentage <= this->update_rate) return;

        this->last_update_percentage = percentage;
        this->length_current         = static_cast<std::size_t>(percentage * static_cast<double>(this->length_total));
        this->timepoint_current      = Clock::now();
        this->draw_progressbar(percentage);
    }

    void finish() {
        this->last_update_percentage = 1.;
        this->length_current         = this->length_total;
        this->draw_progressbar(1.);
        (*_output_stream) << '\n';
    }
};

class Ruler {
private:
    char done_char;

    std::size_t length_total;
    std::size_t length_current;

public:
    Ruler(char done_char = '#') : done_char(done_char), length_total(51), length_current(0) {}

    void start() {
        this->length_current = 0;

        (*_output_stream) << '\n'
                          << " 0    10   20   30   40   50   60   70   80   90   100%\n"
                          << " |----|----|----|----|----|----|----|----|----|----|\n"
                          << ' ';
    }

    void set_progress(double percentage) {
        const std::size_t length_new = static_cast<std::size_t>(percentage * static_cast<double>(this->length_total));

        if (length_new > length_current) {
            const auto chars_to_add = length_new - this->length_current;
            std::fill_n(std::ostreambuf_iterator<char>(*_output_stream), chars_to_add, this->done_char);
        }

        this->length_current = length_new;
    }

    void finish() {
        if (this->length_total > this->length_current) {
            const auto chars_to_add = this->length_total - this->length_current;
            std::fill_n(std::ostreambuf_iterator<char>(*_output_stream), chars_to_add, this->done_char);
        }

        this->length_current = this->length_total;

        (*_output_stream) << '\n';
    }
};

} // namespace utl::progressbar

#endif
#endif // module utl::progressbar






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::random
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/module_random.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_RANDOM)
#ifndef UTLHEADERGUARD_RANDOM
#define UTLHEADERGUARD_RANDOM

// _______________________ INCLUDES _______________________

#include <cstdint>          // uint64_t
#include <ctime>            // time()
#include <initializer_list> // initializer_list<>
#include <limits>           // numeric_limits<>::digits
#include <limits>           // numeric_limits<>::min(), numeric_limits<>::max()
#include <random>           // random_device, std::uniform_int_distribution<>,
                            // std::uniform_real_distribution<>, generate_canonical<>

// ____________________ DEVELOPER DOCS ____________________

// Implements a proper modern PRNG engine, compatible with std <random>.
// Adds 'sensible std <random> wrappers' for people who aren't fond of writing
// 3 lines code just to get a simple rand value.
//
// # XorShift64StarGenerator #
// Random 'std::uint64_t' generator that satisfies uniform random number generator requirements
// from '<random>' (see https://en.cppreference.com/w/cpp/named_req/UniformRandomBitGenerator).
// Implementation "XorShift64* suggested by Marsaglia G. in 2003 "Journal of Statistical Software"
// (see https://www.jstatsoft.org/article/view/v008i14).
// Slightly faster than 'std::rand()', while providing higher quality random.
// Significantly faster than 'std::mt19937'.
// State consists of a single 'std::uint64_t', requires seed >= 1.
//
// # xorshift64star #
// Global instance of XorShift64StarGenerator.
//
// # ::seed(), ::seed_with_time(), ::seed_with_random_device() #
// Seeds random with value/current_time/random_device.
// Random device is a better source of entropy, however it's more expensive to initialize
// than just taking current time with <ctime>, in some cases a "worse by lightweigh" can
// be prefered.
//
// # ::rand_int(), ::rand_uint(), ::rand_float(), ::rand_double() #
// Random value in [min, max] range.
// Floats with no parameters assume range [0, 1].
//
// # ::rand_bool() #
// Randomly chooses 0 or 1.
//
// # ::rand_choise() #
// Randomly chooses a value from initializer list.
//
// # ::rand_linear_combination() #
// Produces "c A + (1-c) B" with random "0 < c < 1" assuming objects "A", "B" support arithmetic operations.
// Useful for vector and color operations.

// ____________________ IMPLEMENTATION ____________________

namespace utl::random {

// =========================
// --- Random Generators ---
// =========================

class XorShift64StarGenerator {
    // meets uniform random number generator requirements
    // (see https://en.cppreference.com/w/cpp/named_req/UniformRandomBitGenerator)
    //
    // implementation XorShift64* suggested by Marsaglia G. in 2003 "Journal of Statistical Software"
    // (see https://www.jstatsoft.org/article/view/v008i14)
public:
    using result_type = std::uint64_t;

private:
    result_type state;

public:
    XorShift64StarGenerator(result_type seed = 0) : state(seed + 1) {}

    [[nodiscard]] static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
    [[nodiscard]] static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }

    void seed(result_type seed) { this->state = seed + 1; } // enforce seed >= 1 by always adding +1 to uint

    result_type operator()() {
        this->state ^= this->state >> 12;
        this->state ^= this->state << 25;
        this->state ^= this->state >> 27;
        return this->state * 0x2545F4914F6CDD1DULL;
    }
};

inline XorShift64StarGenerator xorshift64star;

// ===============
// --- Seeding ---
// ===============

inline void seed(XorShift64StarGenerator::result_type random_seed) { xorshift64star.seed(random_seed); }

inline void seed_with_time() { utl::random::seed(static_cast<XorShift64StarGenerator::result_type>(std::time(NULL))); }

inline void seed_with_random_device() {
    std::random_device rd;
    utl::random::seed(static_cast<XorShift64StarGenerator::result_type>(rd()));
}

// ========================
// --- Random Functions ---
// ========================

// Note 1:
// Despite the intuitive judgement, benchmarks don't seem to indicate that creating
// 'std::uniform_..._distribution<>' on each call introduces any noticeble overhead
//
// sizeof(std::uniform_int_distribution<int>) == 8
// sizeof(std::uniform_real_distribution<double>) == 16

// Note 2:
// No '[[nodiscard]]' since random functions inherently can't be pure due to advancing the generator state.
// Discarding return values while not very sensible, can still be done for the sake of advancing state.
// Ideally we would want users to advance the state directly, but I'm not sure how to communicate that in
// '[[nodiscard]]' warnings.

inline int rand_int(int min, int max) {
    std::uniform_int_distribution<int> distr{min, max};
    return distr(xorshift64star);
}

inline int rand_uint(unsigned int min, unsigned int max) {
    std::uniform_int_distribution<unsigned int> distr{min, max};
    return distr(xorshift64star);
}

inline float rand_float() { return std::generate_canonical<float, std::numeric_limits<float>::digits>(xorshift64star); }

inline float rand_float(float min, float max) {
    std::uniform_real_distribution<float> distr{min, max};
    return distr(xorshift64star);
}

inline double rand_double() {
    return std::generate_canonical<double, std::numeric_limits<double>::digits>(xorshift64star);
}

inline double rand_double(double min, double max) {
    std::uniform_real_distribution<double> distr{min, max};
    return distr(xorshift64star);
}

inline bool rand_bool() { return static_cast<bool>(xorshift64star() % 2); }

template <class T>
const T& rand_choise(std::initializer_list<T> objects) {
    const int random_index = rand_int(0, static_cast<int>(objects.size()) - 1);
    return objects.begin()[random_index];
}

template <class T>
T rand_linear_combination(const T& A, const T& B) { // random linear combination of 2 colors/vectors/etc
    const auto coef = rand_double();
    return A * coef + B * (1. - coef);
}

} // namespace utl::random

#endif
#endif // module utl::random






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::shell
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/module_shell.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_SHELL)
#ifndef UTLHEADERGUARD_SHELL
#define UTLHEADERGUARD_SHELL

// _______________________ INCLUDES _______________________

#include <cstddef>       // size_t
#include <cstdlib>       // atexit(), system()
#include <filesystem>    // filesystem::remove(), filesystem::path, filesystem::exists()
#include <fstream>       // ofstream, ifstream
#include <sstream>       // ostringstream
#include <string>        // string
#include <string_view>   // string_view
#include <unordered_set> // unordered_set<>
#include <vector>        // vector<>

// ____________________ DEVELOPER DOCS ____________________

// Command line utils that allow simple creation of temporary files and command line
// calls with stdout and stderr piping (a task surprisingly untrivial in standard C++).
//
// # ::random_ascii_string() #
// Creates random ASCII string of given length.
// Uses chars in ['a', 'z'] range.
//
// # ::generate_temp_file() #
// Generates temporary .txt file with a random unique name, and returns it's filepath.
// Files generated during current runtime can be deleted with '::clear_temp_files()'.
// If '::clear_temp_files()' wasn't called manually, it gets called automatically upon exiting 'main()'.
// Uses relative path internally.
//
// # ::clear_temp_files() #
// Clears temporary files generated during current runtime.
//
// # ::erase_temp_file() #
// Clears a single temporary file with given filepath.
//
// # ::run_command() #
// Runs a command using the default system shell.
// Returns piped status (error code), stdout and stderr.
//
// # ::exe_path() #
// Parses executable path from argcv as std::string_view.
//
// # ::command_line_args() #
// Parses command line arguments from argcv as std::string_view.

// ____________________ IMPLEMENTATION ____________________

namespace utl::shell {

// =================================
// --- Temporary File Generation ---
// =================================

[[nodiscard]] inline std::string random_ascii_string(std::size_t length) {
    constexpr char min_char = 'a';
    constexpr char max_char = 'z';

    std::string result(length, '0');
    for (std::size_t i = 0; i < length; ++i)
        result[i] = static_cast<char>(min_char + rand() % (max_char - min_char + 1));
    return result;
}

inline std::unordered_set<std::string> _temp_files; // currently existing temp files
inline bool                            _temp_files_cleanup_registered = false;

inline void clear_temp_files() {
    for (const auto& file : _temp_files) std::filesystem::remove(file);
    _temp_files.clear();
}

inline void erase_temp_file(const std::string& file) {
    // we take 'file' as 'std::string&' instead of 'std::string_view' because it is
    // used to call '.erase()' on the map of 'std::string', which does not take string_view
    std::filesystem::remove(file);
    _temp_files.erase(file);
}

inline std::string generate_temp_file() {
    // No '[[nodiscard]]' since the function could still be used to generate files without
    // actually accessing them (through the returned path) in the same program.
    
    constexpr std::size_t MAX_ATTEMPTS = 500; // shouldn't realistically be encountered but still
    constexpr std::size_t NAME_LENGTH  = 30;

    // Register std::atexit() if not already registered
    if (!_temp_files_cleanup_registered) {
        const bool success             = (std::atexit(clear_temp_files) == 0);
        _temp_files_cleanup_registered = success;
    }

    // Try creating files until unique name is found
    for (std::size_t i = 0; i < MAX_ATTEMPTS; ++i) {
        const std::filesystem::path temp_path(random_ascii_string(NAME_LENGTH) + ".txt");

        if (std::filesystem::exists(temp_path)) continue;

        const std::ofstream temp_file(temp_path);

        if (temp_file.good()) {
            _temp_files.insert(temp_path.string());
            return temp_path.string();
        } else {
            return std::string();
        }
    }

    return std::string();
}

// ===================
// --- Shell Utils ---
// ===================

struct CommandResult {
    int         status; // aka error code
    std::string stdout_output;
    std::string stderr_output;
};

inline CommandResult run_command(const std::string& command) {
    // Note 1:
    // we take 'std::string&' instead of 'std::string_view' because there
    // has to be a guarantee that contained string is null-terminated

    // Note 2:
    // Creating temporary files doesn't seem to be ideal, but I'd yet to find
    // a way to pipe BOTH stdout and stderr directly into the program without
    // relying on platform-specific API like Unix forks and Windows processes

    // Note 3:
    // Usage of std::system() is often discouraged due to security reasons,
    // but it doesn't seem there is a portable way to do better (aka going
    // back to previous note about platform-specific APIs)

    const auto stdout_file = utl::shell::generate_temp_file();
    const auto stderr_file = utl::shell::generate_temp_file();

    // Redirect stdout and stderr of the command to temporary files
    std::ostringstream ss;
    ss << command.c_str() << " >" << stdout_file << " 2>" << stderr_file;
    const std::string modified_command = ss.str();

    // Call command
    const auto status = std::system(modified_command.c_str());

    // Read stdout and stderr from temp files and remove them
    std::ostringstream stdout_stream;
    std::ostringstream stderr_stream;
    stdout_stream << std::ifstream(stdout_file).rdbuf();
    stderr_stream << std::ifstream(stderr_file).rdbuf();
    utl::shell::erase_temp_file(stdout_file);
    utl::shell::erase_temp_file(stderr_file);

    // Return
    CommandResult result = {status, stdout_stream.str(), stderr_stream.str()};

    return result;
}

// =========================
// --- Argc/Argv parsing ---
// =========================

// This is just "C to C++ string conversion" for argc/argv
//
// Perhaps it could be expanded to proper parsing of standard "CLI options" format
// (like ordered/unordered flags prefixed with '--', shortcuts prefixed with '-' and etc.)

[[nodiscard]] inline std::string_view get_exe_path(char** argv) {
    // argc == 1 is a reasonable assumption since the only way to achieve such launch
    // is to run executable through a null-execv, most command-line programs assume
    // such scenario to be either impossible or an error on user side
    return std::string_view(argv[0]);
}

[[nodiscard]] inline std::vector<std::string_view> get_command_line_args(int argc, char** argv) {
    std::vector<std::string_view> arguments(argc - 1);
    for (std::size_t i = 0; i < arguments.size(); ++i) arguments.emplace_back(argv[i]);
    return arguments;
}

} // namespace utl::shell

#endif
#endif // module utl::shell






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::sleep
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/module_sleep.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_SLEEP)
#ifndef UTLHEADERGUARD_SLEEP
#define UTLHEADERGUARD_SLEEP

// _______________________ INCLUDES _______________________

#include <chrono>  // chrono::steady_clock, chrono::nanoseconds, chrono::duration_cast<>
#include <cmath>   // sqrt()
#include <cstdint> // int64_t
#include <thread>  // this_thread::sleep_for()

// ____________________ DEVELOPER DOCS ____________________

// Various implementation of sleep(), used for precise delays.
//
// # ::spinlock() #
// Best precision, uses CPU.
//
// # ::hybrid() #
// Recommended option, similar precision to spinlock with minimal CPU usage.
// Loops short system sleep while statistically estimating its error on the fly and once within error
// margin of the end time, finished with spinlock sleep (essentialy negating usual system sleep error).
//
// # ::system() #
// Worst precision, frees CPU.

// ____________________ IMPLEMENTATION ____________________

namespace utl::sleep {
    
// =============================
// --- Sleep Implementations ---
// =============================

using _clock     = std::chrono::steady_clock;
using _chrono_ns = std::chrono::nanoseconds;

inline void spinlock(double ms) {
    const long long ns              = static_cast<std::int64_t>(ms * 1e6);
    const auto      start_timepoint = _clock::now();

    volatile int i = 0; // volatile 'i' prevents standard-compliant compilers from optimizing away the loop
    while (std::chrono::duration_cast<_chrono_ns>(_clock::now() - start_timepoint).count() < ns) { ++i; }
}

inline void hybrid(double ms) {
    static double       estimate = 5e-3; // initial sleep_for() error estimate
    static double       mean     = estimate;
    static double       m2       = 0;
    static std::int64_t count    = 1;

    // We treat sleep_for(1 ms) as a random variate "1 ms + random_value()"
    while (ms > estimate) {
        const auto start = _clock::now();
        std::this_thread::sleep_for(_chrono_ns(static_cast<std::int64_t>(1e6)));
        const auto end = _clock::now();

        const double observed = std::chrono::duration_cast<_chrono_ns>(end - start).count() / 1e6;
        ms -= observed;

        ++count;

        // Welford's algorithm for mean and unbiased variance estimation
        const double delta = observed - mean;
        mean += delta / static_cast<double>(count);
        m2 += delta * (observed - mean); // intermediate values 'm2' reduce numerical instability
        const double variance = std::sqrt(m2 / static_cast<double>(count - 1));

        estimate = mean + variance; // set estimate 1 standard deviation above the mean
        // can be adjusted to make estimate more or less pessimistic
    }

    utl::sleep::spinlock(ms);
}

inline void system(double ms) { std::this_thread::sleep_for(_chrono_ns(static_cast<std::int64_t>(ms * 1e6))); }

} // namespace utl::sleep

#endif
#endif // module utl::sleep






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::stre
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/module_stre.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_STRE)
#ifndef UTLHEADERGUARD_STRE
#define UTLHEADERGUARD_STRE

// _______________________ INCLUDES _______________________

#include <cstddef>     // size_t
#include <iomanip>     // setfill(), setw()
#include <ostream>     // ostream
#include <sstream>     // ostringstream
#include <string>      // string
#include <string_view> // string_view
#include <tuple>       // tuple<>, get<>()
#include <type_traits> // false_type, true_type, void_t<>, is_convertible<>, enable_if_t<>
#include <utility>     // declval<>(), index_sequence<>

// ____________________ DEVELOPER DOCS ____________________

// String utility extensions, mainly a template ::to_str() method which works with all STL containers,
// including maps, sets and tuples with any level of mutual nesting.
//
// Also includes some expansions of <type_traits> header, since we need them anyways to implement a generic ::to_str().
//
// # ::is_printable<Type> #
// Integral constant, returns in "::value" whether Type can be printed through std::cout.
// Criteria: Existance of operator 'ANY_TYPE operator<<(std::ostream&, Type&)'
//
// # ::is_iterable_through<Type> #
// Integral constant, returns in "::value" whether Type can be iterated through.
// Criteria: Existance of .begin() and .end() with applicable operator()++
//
// # ::is_const_iterable_through<Type> #
// Integral constant, returns in "::value" whether Type can be const-iterated through.
// Criteria: Existance of .cbegin() and .cend() with applicable operator()++
//
// # ::is_tuple_like<Type> #
// Integral constant, returns in "::value" whether Type has a tuple-like structure.
// Tuple-like structure include std::tuple, std::pair, std::array, std::ranges::subrange (since C++20)
// Criteria: Existance of applicable std::get<0>() and std::tuple_size()
//
// # ::is_string<Type> #
// Integral constant, returns in "::value" whether Type is a char string.
// Criteria: Type can be decayed to std::string or a char* pointer
//
// # ::is_to_str_convertible<Type> #
// Integral constant, returns in "::value" whether Type can be converted to string through ::to_str().
// Criteria: Existance of a valid utl::stre::to_str() overload
//
// # ::to_str() #
// Converts any standard container or a custom container with necessary member functions to std::string.
// Works with tuples and tuple-like classes.
// Works with nested containers/tuples through recursive template instantiation, which
// resolves as long as types at the end of recursion have a valid operator<<() for ostreams.
//
// Not particularly fast, but it doesn't really have to be since this kind of thing is mostly
// useful for debugging and other human-readable prints.
//
// # ::InlineStream #
// Inline 'std::ostringstream' construction with implicit conversion to 'std::string'.
// Rather unperformant, but convenient for using stream formating during string construction.
// Example: std::string str = (stre::InlineStream() << "Value " << 3.14 << " is smaller than " << 6.28);
//
// In retrospective the usefulness of this seems rather dubious. Perhaps I'll deprecate it later.
//
// # ::repeat_symbol(), ::repeat_string() #
// Repeats character/string a given number of times.
//
// # ::pad_with_zeroes() #
// Pads given integer with zeroes untill a certain lenght.
// Useful when saving data in files like 'data_0001.txt', 'data_0002.txt', '...' so they get properly sorted.

// ____________________ IMPLEMENTATION ____________________

namespace utl::stre {

// ===================
// --- Type Traits ---
// ===================

template <typename Type, typename = void>
struct is_printable : std::false_type {};

template <typename Type>
struct is_printable<Type, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<Type>())>>
    : std::true_type {};

template <typename Type, typename = void, typename = void>
struct is_iterable_through : std::false_type {};

template <typename Type>
struct is_iterable_through<Type, std::void_t<decltype(++std::declval<Type>().begin())>,
                           std::void_t<decltype(std::declval<Type>().end())>> : std::true_type {};

template <typename Type, typename = void, typename = void>
struct is_const_iterable_through : std::false_type {};

template <typename Type>
struct is_const_iterable_through<Type, std::void_t<decltype(++std::declval<Type>().cbegin())>,
                                 std::void_t<decltype(std::declval<Type>().cend())>> : std::true_type {};

template <typename Type, typename = void, typename = void>
struct is_tuple_like : std::false_type {};

template <typename Type>
struct is_tuple_like<Type, std::void_t<decltype(std::get<0>(std::declval<Type>()))>,
                     std::void_t<decltype(std::tuple_size<Type>::value)>> : std::true_type {};

template <typename Type>
struct is_string : std::is_convertible<Type, std::string_view> {};

// ================
// --- to_str() ---
// ================

// --- Delimers ---
// ----------------

constexpr auto _array_delimer_l = "[ ";
constexpr auto _array_delimer_m = ", ";
constexpr auto _array_delimer_r = " ]";
constexpr auto _tuple_delimer_l = "< ";
constexpr auto _tuple_delimer_m = ", ";
constexpr auto _tuple_delimer_r = " >";

// --- Predeclarations ---
// -----------------------

// 'false_type' half of 'is_to_str_convertible' should be declared before 'to_str()' to resolve circular dependency
template <typename Type, typename = void>
struct is_to_str_convertible : std::false_type {};


// 'to_str(tuple)' should be predeclared to resolve circular dependency between 'to_str(container)' and 'to_str(tuple)'
template <template <typename... Params> class TupleLikeType, typename... Args,
          std::enable_if_t<!utl::stre::is_printable<TupleLikeType<Args...>>::value, bool>              = true,
          std::enable_if_t<!utl::stre::is_const_iterable_through<TupleLikeType<Args...>>::value, bool> = true,
          std::enable_if_t<utl::stre::is_tuple_like<TupleLikeType<Args...>>::value, bool>              = true>
[[nodiscard]] std::string to_str(const TupleLikeType<Args...>& tuple);

// --- Implementation ---
// ----------------------

// - to_str(printable) -
template <typename Type, std::enable_if_t<utl::stre::is_printable<Type>::value, bool> = true>
[[nodiscard]] std::string to_str(const Type& value) {
    std::ostringstream ss;

    ss << value;

    return ss.str();
}

// - to_str(container) -
template <typename ContainerType, std::enable_if_t<!utl::stre::is_printable<ContainerType>::value, bool> = true,
          std::enable_if_t<utl::stre::is_const_iterable_through<ContainerType>::value, bool>                  = true,
          std::enable_if_t<utl::stre::is_to_str_convertible<typename ContainerType::value_type>::value, bool> = true>
[[nodiscard]] std::string to_str(const ContainerType& container) {

    std::ostringstream ss;

    // Special case for empty containers
    if (container.cbegin() == container.cend()) {
        ss << _array_delimer_l << _array_delimer_r;
        return ss.str();
    }

    // Iterate throught the container 'looking forward' by one step
    // so we can know not to place the last delimer. Using -- or std::prev()
    // is not and option since we only require iterators to be forward-iterable
    ss << _array_delimer_l;

    auto it_next = container.cbegin(), it = it_next++;
    for (; it_next != container.cend(); ++it_next, ++it) ss << utl::stre::to_str(*it) << _array_delimer_m;

    ss << utl::stre::to_str(*it) << _array_delimer_r;

    return ss.str();
}

// - to_str(tuple) helpers -
template <typename TupleElemType>
[[nodiscard]] std::string _deduce_and_perform_string_conversion(const TupleElemType& elem) {
    std::ostringstream temp_ss;

    if constexpr (utl::stre::is_to_str_convertible<TupleElemType>::value) temp_ss << utl::stre::to_str(elem);
    else temp_ss << elem;

    return temp_ss.str();
}

template <typename TupleLikeType, std::size_t... Is>
void _print_tuple_fold(std::ostringstream& ss, const TupleLikeType& tuple, std::index_sequence<Is...>) {
    ((ss << (Is == 0 ? "" : _tuple_delimer_m) << _deduce_and_perform_string_conversion(std::get<Is>(tuple))), ...);
} // prints tuple to stream

// - to_str(tuple) -
template <template <typename... Params> class TupleLikeType, typename... Args,
          std::enable_if_t<!utl::stre::is_printable<TupleLikeType<Args...>>::value, bool>,
          std::enable_if_t<!utl::stre::is_const_iterable_through<TupleLikeType<Args...>>::value, bool>,
          std::enable_if_t<utl::stre::is_tuple_like<TupleLikeType<Args...>>::value, bool>>
[[nodiscard]] std::string to_str(const TupleLikeType<Args...>& tuple) {

    std::ostringstream ss;

    // Print tuple using C++17 variadic folding with index sequence
    ss << _tuple_delimer_l;
    _print_tuple_fold(ss, tuple, std::index_sequence_for<Args...>{});
    ss << _tuple_delimer_r;

    return ss.str();
}

// - is_to_str_convertible -
template <typename Type>
struct is_to_str_convertible<Type, std::void_t<decltype(utl::stre::to_str(std::declval<Type>()))>> : std::true_type {};

// ===========================
// --- Inline stringstream ---
// ===========================

class InlineStream {
public:
    template <typename Type>
    InlineStream& operator<<(const Type& arg) {
        this->ss << arg;
        return *this;
    }

    inline operator std::string() const { return this->ss.str(); }

private:
    std::ostringstream ss;
};

// ===================
// --- Misc. Utils ---
// ===================

[[nodiscard]] inline std::string repeat_symbol(char symbol, size_t repeats) { return std::string(repeats, symbol); }

[[nodiscard]] inline std::string repeat_string(std::string_view str, size_t repeats) {
    std::string res;
    res.reserve(str.size() * repeats);
    while (repeats--) res += str;
    return res;
}

template <typename IntegerType, std::enable_if_t<std::is_integral<IntegerType>::value, bool> = true>
[[nodiscard]] std::string pad_with_zeroes(IntegerType number, std::streamsize total_size = 10) {
    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(total_size) << number;
    return ss.str();
}

} // namespace utl::stre

#endif
#endif // module utl::stre






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::table
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/module_table.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_TABLE)
#ifndef UTLHEADERGUARD_TABLE
#define UTLHEADERGUARD_TABLE

// _______________________ INCLUDES _______________________

#include <cstddef>          // size_t
#include <initializer_list> // initializer_list<>
#include <iomanip>          // resetiosflags(), setw()
#include <ios>              // streamsize, ios_base::fmtflags, ios
#include <iostream>         // cout
#include <ostream>          // ostream
#include <string>           // string
#include <vector>           // vector<>

// ____________________ DEVELOPER DOCS ____________________

// Functions used to build and render simple ASCII table in console.
//
// # ::create() #
// Sets up table with given number of columns and their widths.
//
// # ::set_formats() #
// (optional) Sets up column formats for better display
//
// # ::set_ostream() #
// (optional) Select 'std::ostream' to which all output gets forwarded. By default 'std::cout' is selected.
//
// # ::NONE, ::FIXED(), ::DEFAULT(), ::SCIENTIFIC(), ::BOOL() #
// Format flags with following effects:
// > NONE          - Use default C++ formats
// > FIXED(N)      - Display floats in fixed form with N decimals, no argument assumes N = 3
// > DEFAULT(N)    - Display floats in default form with N decimals, no argument assumes N = 6
// > SCIENTIFIC(N) - Display floats in scientific form with N decimals, no argument assumes N = 3
// > BOOL          - Display booleans as text
//
// # ::cell() #
// Draws a single table cell, if multiple arguments are passed, draws each one in a new cell.
// Accepts any type with a defined "<<" ostream operator.
//
// # ::hline() #
// Draws horizontal line in a table. Similar to LaTeX '\hline'.

// ____________________ IMPLEMENTATION ____________________

namespace utl::table {

// =====================
// --- Column Format ---
// =====================

using uint       = std::streamsize;
using _ios_flags = std::ios_base::fmtflags;

struct ColumnFormat {
    _ios_flags flags;
    uint       precision;
};

struct _Column {
    uint         width;
    ColumnFormat col_format;
};

// --- Predefined Formats ---
// --------------------------

constexpr ColumnFormat NONE = {std::ios::showpoint, 6};

constexpr ColumnFormat FIXED(uint decimals = 3) { return {std::ios::fixed, decimals}; }
constexpr ColumnFormat DEFAULT(uint decimals = 6) { return {std::ios::showpoint, decimals}; }
constexpr ColumnFormat SCIENTIFIC(uint decimals = 3) { return {std::ios::scientific, decimals}; }

constexpr ColumnFormat BOOL = {std::ios::boolalpha, 3};

// --- Internal Table State ---
// ----------------------------

inline std::vector<_Column> _columns;
inline std::size_t          _current_column = 0;
inline std::ostream*        _output_stream  = &std::cout;

// ===================
// --- Table Setup ---
// ===================

inline void create(std::initializer_list<uint>&& widths) {
    _columns.resize(widths.size());
    for (std::size_t i = 0; i < _columns.size(); ++i) {
        _columns[i].width      = widths.begin()[i];
        _columns[i].col_format = DEFAULT();
    }
}

inline void set_formats(std::initializer_list<ColumnFormat>&& formats) {
    for (std::size_t i = 0; i < _columns.size(); ++i) _columns[i].col_format = formats.begin()[i];
}

inline void set_ostream(std::ostream& new_ostream) { _output_stream = &new_ostream; }

// =======================
// --- Table Rendering ---
// =======================

inline void cell(){};

template <typename T, typename... Types>
void cell(T value, const Types... other_values) {
    const std::string left_cline      = (_current_column == 0) ? "|" : "";
    const std::string right_cline     = (_current_column == _columns.size() - 1) ? "|\n" : "|";
    const _ios_flags  format          = _columns[_current_column].col_format.flags;
    const uint        float_precision = _columns[_current_column].col_format.precision;

    // Save old stream state
    std::ios old_state(nullptr);
    old_state.copyfmt(*_output_stream);

    // Set table formatting
    (*_output_stream) << std::resetiosflags((*_output_stream).flags());
    (*_output_stream).flags(format);
    (*_output_stream).precision(float_precision);

    // Print
    (*_output_stream) << left_cline << std::setw(_columns[_current_column].width) << value << right_cline;

    // Return old stream state
    (*_output_stream).copyfmt(old_state);

    // Advance column counter
    _current_column = (_current_column == _columns.size() - 1) ? 0 : _current_column + 1;

    cell(other_values...);
}

inline void hline() {
    (*_output_stream) << "|";
    for (const auto& col : _columns) (*_output_stream) << std::string(static_cast<std::size_t>(col.width), '-') << "|";
    (*_output_stream) << "\n";
}

} // namespace utl::table

#endif
#endif // module utl::table






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::timer
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/module_timer.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_TIMER)
#ifndef UTLHEADERGUARD_TIMER
#define UTLHEADERGUARD_TIMER

// _______________________ INCLUDES _______________________

#include <array>   // array<>
#include <chrono>  // chrono::steady_clock, chrono::nanoseconds, chrono::duration_cast<>
#include <ctime>   // time, time_t, tm, strftime()
#include <string>  // string, to_string()
#include <utility> // forward<>()

// ____________________ DEVELOPER DOCS ____________________

// Global-state timer with built-in formatting. Functions for local date and time.
//
// Uses SFINAE to resolve platform-specific calls to local time (localtime_s() on Windows,
// localtime_r() on Linux), the same can be done with macros. The fact that there seems to
// be no portable way of getting local time before C++20 (which adds "Calendar" part of <chrono>)
// is rather bizzare, but not unmanageable.
//
// # ::start() #
// Starts the timer.
// Note: If start() wasn't called system will use uninitialized value as a start.
//
// # ::elapsed_ms(), ::elapsed_sec(), ::elapsed_min(), ::elapsed_hours() #
// Elapsed time as double.
//
// # ::elapsed_string_ms(), ::elapsed_string_sec(), ::elapsed_string_min(), ::elapsed_string_hours() #
// Elapsed time as std::string.
//
// # ::elapsed_string:fullform() #
// Elapsed time in format "%H hours %M min %S sec %MS ms".
//
// # ::datetime_string() #
// Current local date and time in format "%y-%m-%d %H:%M:%S".
//
// # ::datetime_string_id() #
// Current local date and time in format "%y-%m-%d-%H-%M-%S".
// Less readable that usual format, but can be used in filenames which prohibit ":" usage.

// ____________________ IMPLEMENTATION ____________________

namespace utl::timer {

// =================
// --- Internals ---
// =================

using _clock     = std::chrono::steady_clock;
using _chrono_ns = std::chrono::nanoseconds;

constexpr double _ns_in_ms = 1e6;

constexpr long long _ms_in_sec  = 1000;
constexpr long long _ms_in_min  = 60 * _ms_in_sec;
constexpr long long _ms_in_hour = 60 * _ms_in_min;

inline _clock::time_point _start_timepoint;

[[nodiscard]] inline double _elapsed_time_as_ms() {
    const auto elapsed = std::chrono::duration_cast<_chrono_ns>(_clock::now() - _start_timepoint).count();
    return static_cast<double>(elapsed) / _ns_in_ms;
}

inline void start() { _start_timepoint = _clock::now(); }

// ==============================
// --- Elapsed Time Functions ---
// ==============================

// --- Elapsed Time as 'double' ---
// --------------------------------

[[nodiscard]] inline double elapsed_ms() { return _elapsed_time_as_ms(); }
[[nodiscard]] inline double elapsed_sec() { return _elapsed_time_as_ms() / static_cast<double>(_ms_in_sec); }
[[nodiscard]] inline double elapsed_min() { return _elapsed_time_as_ms() / static_cast<double>(_ms_in_min); }
[[nodiscard]] inline double elapsed_hours() { return _elapsed_time_as_ms() / static_cast<double>(_ms_in_hour); }

// --- Elapsed Time as 'std::string' ---
// -------------------------------------

[[nodiscard]] inline std::string elapsed_string_ms() { return std::to_string(elapsed_ms()) + " ms"; }
[[nodiscard]] inline std::string elapsed_string_sec() { return std::to_string(elapsed_sec()) + " sec"; }
[[nodiscard]] inline std::string elapsed_string_min() { return std::to_string(elapsed_min()) + " min"; }
[[nodiscard]] inline std::string elapsed_string_hours() { return std::to_string(elapsed_hours()) + " hours"; }

[[nodiscard]] inline std::string elapsed_string_fullform() {
    long long unaccounted_ms = static_cast<long long>(_elapsed_time_as_ms());

    long long ms    = 0;
    long long min   = 0;
    long long sec   = 0;
    long long hours = 0;

    if (unaccounted_ms > _ms_in_hour) {
        hours += unaccounted_ms / _ms_in_hour;
        unaccounted_ms -= hours * _ms_in_hour;
    }

    if (unaccounted_ms > _ms_in_min) {
        min += unaccounted_ms / _ms_in_min;
        unaccounted_ms -= min * _ms_in_min;
    }

    if (unaccounted_ms > _ms_in_sec) {
        sec += unaccounted_ms / _ms_in_sec;
        unaccounted_ms -= sec * _ms_in_sec;
    }

    ms = unaccounted_ms;

    return std::to_string(hours) + " hours " + std::to_string(min) + " min " + std::to_string(sec) + " sec " +
           std::to_string(ms) + " ms ";
}

// ============================
// --- Local Time Functions ---
// ============================

// - SFINAE to select localtime_s() or localtime_r() -
template <typename Arg_tm, typename Arg_time_t>
auto _available_localtime_impl(Arg_tm time_moment, Arg_time_t timer)
    -> decltype(localtime_s(std::forward<Arg_tm>(time_moment), std::forward<Arg_time_t>(timer))) {
    return localtime_s(std::forward<Arg_tm>(time_moment), std::forward<Arg_time_t>(timer));
}

template <typename Arg_tm, typename Arg_time_t>
auto _available_localtime_impl(Arg_tm time_moment, Arg_time_t timer)
    -> decltype(localtime_r(std::forward<Arg_time_t>(timer), std::forward<Arg_tm>(time_moment))) {
    return localtime_r(std::forward<Arg_time_t>(timer), std::forward<Arg_tm>(time_moment));
}

// - Implementation -
[[nodiscard]] inline std::string _datetime_string_with_format(const char* format) {
    std::time_t timer = std::time(nullptr);
    std::tm     time_moment{};

    // Call localtime_s() or localtime_r() depending on which one is present
    _available_localtime_impl(&time_moment, &timer);

    // // Macro version, can be used instead of SFINAE resolution
    // // Get localtime safely (if possible)
    // #if defined(__unix__)
    // localtime_r(&timer, &time_moment);
    // #elif defined(_MSC_VER)
    // localtime_s(&time_moment, &timer);
    // #else
    // // static std::mutex mtx; // mutex can be used to make thread-safe version but add another dependency
    // // std::lock_guard<std::mutex> lock(mtx);
    // time_moment = *std::localtime(&timer);
    // #endif

    // Convert time to C-string
    std::array<char, 100> mbstr;
    std::strftime(mbstr.data(), mbstr.size(), format, &time_moment);

    return std::string(mbstr.data());
}

[[nodiscard]] inline std::string datetime_string() { return _datetime_string_with_format("%Y-%m-%d %H:%M:%S"); }

[[nodiscard]] inline std::string datetime_string_id() { return _datetime_string_with_format("%Y-%m-%d-%H-%M-%S"); }

} // namespace utl::timer

#endif
#endif // module utl::timer






// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DmitriBogdanov/prototyping_utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Module:        utl::voidstream
// Documentation: https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/module_voidstream.md
// Source repo:   https://github.com/DmitriBogdanov/prototyping_utils
//
// This project is licensed under the MIT License
//
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#if !defined(UTL_PICK_MODULES) || defined(UTLMODULE_VOIDSTREAM)
#ifndef UTLHEADERGUARD_VOIDSTREAM
#define UTLHEADERGUARD_VOIDSTREAM

// _______________________ INCLUDES _______________________

#include <ostream>   // ostream
#include <streambuf> // streambuf

// ____________________ DEVELOPER DOCS ____________________

// TODO:
// Expand this module into a proper "streams" module. Being as small as it currently is
// it has rather little reason for existence. Combined with what is probably a rather
// unperformant way of discaring '::vout' inputs, it's quite justified for a rewrite.
// Streams that could be implemented:
//    - Multisink Stream (forwards inputs to multiple std::ostream's)
//    - Appending Stream (not sure if it's even implementable or useful)
//
// "voidstream" that functions like std::ostream with no output.
// Can be passed to interfaces that use streams to silence their output.
//
// # ::vstreambuf #
// Stream buffer that overflows with no output, usage example:
//   > std::ofstream output_stream(&vstreambuf);
//   > output_stream << VALUE; // produces nothing
//
// # ::vout #
// Output stream that produces no output, usage example:
//   > vout << VALUE; // produces nothing

// ____________________ IMPLEMENTATION ____________________

namespace utl::voidstream {

// ===================
// --- Void Stream ---
// ===================

class VoidStreamBuf : public std::streambuf {
public:
    inline int overflow(int c) { return c; }
};

class VoidStream : public std::ostream {
public:
    inline VoidStream() : std::ostream(&buffer) {}

private:
    VoidStreamBuf buffer;
};

inline VoidStreamBuf vstreambuf;
inline VoidStream    vout;

} // namespace utl::voidstream

#endif
#endif // module utl::voidstream






