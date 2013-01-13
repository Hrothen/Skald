# Skald: C++ Entity-Component system

Skald is a fairly bare bones implementation of an Entity-Component framework, created in my spare time for fun. The design of Skald was inspired by Richard Lord's excellent [Ash Framework.](https://github.com/richardlord/Ash)

## Overview

Objects in Skald are either Entities, Components, or Systems. Entities are used internally and are not interfaced with by the user, an entity is just a unique id and a set of index values used to access components. The user creates component classes, which store only data and must be POD, and System classes which loop across entities and manipulate their components.

## Installation

The following are required to use Skald:

-A reasonably new compiler such as clang3.1 or gcc4.7, technically the latest visual c++ compiler should work, but their standard library hasn't been updated yet and I haven't tested it.
-[Boost](http://boost.org) `1.48.0` or higher. Boost Concepts is used to enforce POD requirements on components. If you don't want to link against boost you can alternately remove the concept check and include directive in VectorTuple.hpp.

To build the tests you will additionally need:

-[CMake](http://cmake.org/)
-[SVN](http://subversion.apache.org/)
-[GTest](http://code.google.com/p/googletest/) Which you can install via the build script.

To actually build the tests do:

```bash
cd vendor
./checkout-gtest.sh
cd ..
mkdir build && cd build
cmake .. && make
```