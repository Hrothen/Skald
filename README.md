# Skald: C++ Entity-Component framework

Skald is a fairly bare bones implementation of an Entity-Component framework, created in my spare time for fun. The design of Skald was inspired by Richard Lord's excellent [Ash Framework.](https://github.com/richardlord/Ash)

## Overview

Objects in Skald are either Entities, Components, or Systems. Entities are used internally and are not interfaced with by the user, an entity is just a unique id and a set of index values used to access components. The user creates component classes, which store only data and must be POD, and System classes which loop across entities and manipulate their components.

## Motivation

Entity-Component frameworks tend to use heavily object-oriented design patterns which work well in languages that optimize for heavy use of references like Java and C#. When using a language like C++ the OO design will still be pretty fast, but we can make use of language features and compiler optimizations to do even better.
In particular Skald aims to meld the typical Entity-Component design with the data-driven design which loops across arrays of contiguous data, achieved by using template polymorphism instead of regular inheritance. The main idea is that a programmer wanting the ease of use of an Entity-Component framework can get that, and then have the ability to drop to very low-level code for portions that must be optimized.

## Installation

The following are required to use Skald:

- A reasonably new compiler such as clang3.1 or gcc4.7, technically the latest visual c++ compiler should work, but their standard library hasn't been updated yet and I haven't tested it.
To build the tests you will additionally need:

- [CMake](http://cmake.org/)
- [SVN](http://subversion.apache.org/)
- [GTest](http://code.google.com/p/googletest/) Which you can install via the build script.

To actually build the tests do:

```c++
cd vendor
./checkout-gtest.sh
cd ..
mkdir build && cd build
cmake .. && make
```