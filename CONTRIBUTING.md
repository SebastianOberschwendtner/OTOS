# Welcome!

There are some general rules to keep the code nice and tidy:
- [Naming Convention](#naming-convention)
  - [Defines](#defines)
  - [Variables](#variables)
  - [Functions](#functions)
- [Comments](#comments)
  - [File Header](#file-header)
  - [Function Header](#function-header)
- [Commits](#commits)
- [Release Management](#release-management)
- [License](#license)

# Naming Convention
There are some simple rules on how to name things.

## Variables
Variables are **all_lowercase**.
For integral types, use the types which explicitly specify the number of bits, for example:

```cpp
uin32_t some_value
```

## Constexpr
When defining constant use `constexpr`!
You can use the constants the same way as variables so the same naming rules apply:

Example:
```cpp
constexpr uint8_t new_constant = value; // optional comment.
```

Try to group all constants at the top of the file.

## Functions
Functions follow the same rules as [variables](#variables).

> When writing in *C++* use namespaces wherever appropriate.

## Classes
When you use classes, use *CapitalizedWords* as the class names. Also use namespaces wherever applicable.

- **Properties** should use *small_case*.
- **Methods** should use *small_case*.

# Comments
We use the *doxygen* documentation flags in the headers, to make them more readable. Although we do not export the documentation as *html*, the flags
are useful, because *VSCode* recognizes them, and renders them when hovering over the function name.

In general try to code like the person who is reading your code knows where you live. :wink:

When using *inline comments* make them **comment blocks**:
```cpp
/* I am an inline comment */
do_something();
```

## File Header
The file header gives a short description of what the file does. Also the [license](#license) information is added here.

Example:
```c
/**
 ==============================================================================
 * @file    filename.c
 * @author  name
 * @version vx.x.x
 * @date    dd-Month-yyyy
 * @brief   Description
 ==============================================================================
 */
 ```
## Function Header
A function header gives a short **description**, defines the **inputs** and the **return value**. Addtitional **details** are optional:

```c
/**
 * @brief Description
 * @param input1 Description of input, if any.
 * @param input2 Description of input, if any.
 * @return Description of th return value, if any.
 */
 ```
 Possible `@details` are:
 - *interrupt handler*
 - *inline function*
 - ...
 
 You can also use additional comments:
 - `@todo`: Note what has to be done
 - `@deprecated`: Why this function is no longer used.

 ## File Sections
 Try to separate *sections within one file. Sections can be anything which combines the same content, for example function declarations or variable declaration can be sections.
 Mark them according to:
 ```cpp
 /* === Section Name === */
 -> Code here.
 ```
 
# Commits
- Commit messages should be clear and concise 
- Use **English** as language.
- Use the `#` to close issues. 
- Please use *present* tense!

Symbols for commits:
- :art: `:art:` Improvements in the *User Experience*
- :bug: `:bug:` Bugfix
- :racehorse: `:racehorse:` Performance Improvement
- :wrench: `:wrench:` Minor Improvement or Fix
- :mag: `:mag:` Made Code More Readable
- :8ball: `:8ball:` Add or Change Unittests
- :memo: `:memo:` Improved Documentation
- :nut_and_bolt: `:nut_and_bolt:` Hardware Uploads and Fixes
- :tada: `:tada:` Celebrate New **Major** Releases!

# Release Management
*Tags* are used for the release management. The tag number is:
- **v***Major.Minor.Bugfix*
- Document every change in the [changelog](CHANGELOG.md)

> A release with release notes is only published on **minor** changes. Bugfixes are just pushed as tags.

# License

The following should be included in every file header:
> Adjust the name of the author according to who created the file.
```c
/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2024 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
 ```
