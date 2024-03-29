

[![Language](https://img.shields.io/badge/C++-std=17-blue.svg?style=flat&logo=cplusplus)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/DmitriBogdanov/prototyping_utils/blob/master/LICENSE.md)

# Prototyping utils

This is a collection of various utilities that aim to provide a set of concise "helpers" that allow prototying with minimal boilerplate. Most of the following modules were created during my work in gamedev and university projects.

## Modules

> [**utl::math**](https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/math.md)

Math-related utilities.

> [**utl::progressbar**](https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/progressbar.md)

Configurable progress bars for console appls.

> [**utl::random**](https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/random.md)

Convenient random functions.

> [**utl::shell**](https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/shell.md)

Command-line related utilities.

> [**utl::sleep**](https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/sleep.md)

Precise sleep implementations.

> [**utl::stre**](https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/stre.md)

String conversion extensions.

> [**utl::table**](https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/table.md)

Table rendering tools.

> [**utl::timer**](https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/timer.md)

Timing methods.

> [**utl::voidstream**](https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/voidstream.md)

`std::ostream` API silencing method.

> [**UTL_MACROS**](https://github.com/DmitriBogdanov/prototyping_utils/blob/master/docs/MACROS.md)

Macros for automatic code generation, logging and general convenience

## Design goals

Implementation of this library sets following design goals:

* **Header only**. Adding library to the project should be as simple as adding a header.
* **Minimal boilerplate code**. Methods must be concise and require minimal prerequisites.
* **Platform agnostic**. Implementation should be based around C++ standard and avoid any OS-specific behaviour.
* **Toggleable modules**. Every module should be independent and reside in a separate namespace. Modules together with their STL dependencies should be toggleable through a `#define`.

Secondary design goals also include:

* **Modern C++ style**. Usage of modern C++ idioms is prefered.
* **Usage examples**. Documentation should include usage examples for all major methods.

## Work in progress

* Coordinate transformations in **utl::math**;
* More type traits in **utl::math**;
* `shell::run_command_async()` method;
* `stre::to_str()` implementation for `std::stack` and `std::queue`;
* Colored text module or color support for log macros that does not involve including '<Windows.h>'.

## Requirements

* Requires C++17 support;
* Some modules require POSIX-compliant system (Linux) or Windows.


## License

This project is licensed under the MIT License - see the [LICENSE.md](https://github.com/DmitriBogdanov/prototyping_utils/blob/master/LICENSE.md) for details
