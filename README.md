# leccore
liblec core library

<p>
  <img alt="Lines of code" src="https://img.shields.io/tokei/lines/github/alecmus/leccore">
  <img alt="GitHub issues" src="https://img.shields.io/github/issues-raw/alecmus/leccore">
  <img alt="GitHub closed issues" src="https://img.shields.io/github/issues-closed-raw/alecmus/leccore">
</p>

#### Release Info
<p>
  <img alt="GitHub release (latest by date)" src="https://img.shields.io/github/v/release/alecmus/leccore">
  <img alt="GitHub Release Date" src="https://img.shields.io/github/release-date/alecmus/leccore">
  <img alt="GitHub all releases" src="https://img.shields.io/github/downloads/alecmus/leccore/total">
</p>

#### Commit Info
<p>
  <img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/alecmus/leccore">
  <img alt="GitHub commit activity" src="https://img.shields.io/github/commit-activity/y/alecmus/leccore">
</p>

### About the Library
The leccore is designed for the rapid development of modern, efficient and easy to maintain C++ applications.
It is part of the [liblec libraries](https://github.com/alecmus/liblec). It is built to contain the most
fundamental building blocks like databases, file handling, registry settings, and other general helper utilities
specially designed to make building robust, sophisticated applications a breeze. Furthermore, it is designed to
bind together all the other liblec libraries like [lecui](https://github.com/alecmus/lecui) and
[lecnet](https://github.com/alecmus/lecnet).

### Classes
The following classes have been added to the library
Class Name           | Description                      | Interface
-------------------- | -------------------------------- | ------------------------------------------------
database::connection | Database connection              | [#include <liblec/leccore/database.h>](https://github.com/alecmus/leccore/blob/master/database.h)
file                 | File handling helper             | [#include <liblec/leccore/file.h>](https://github.com/alecmus/leccore/blob/master/file.h)
pc_info              | PC hardware information          | [#include <liblec/leccore/pc_info.h>](https://github.com/alecmus/leccore/blob/master/pc_info.h)
hash                 | Cryptographic hash               | [#include <liblec/leccore/hash.h>](https://github.com/alecmus/leccore/blob/master/hash.h)
encode::base32       | Base32 character encoding        | [#include <liblec/leccore/encode.h>](https://github.com/alecmus/leccore/blob/master/encode.h)
encode::base64       | Base64 character encoding        | [#include <liblec/leccore/encode.h>](https://github.com/alecmus/leccore/blob/master/encode.h)
app_version_info     | Application version information  | [#include <liblec/leccore/app_version_info.h>](https://github.com/alecmus/leccore/blob/master/app_version_info.h)
registry             | Registry reading and writing     | [#include <liblec/leccore/registry.h>](https://github.com/alecmus/leccore/blob/master/registry.h)
encrypt              | Encryption                       | [#include <liblec/leccore/encrypt.h>](https://github.com/alecmus/leccore/blob/master/encrypt.h)
settings             | Application settings             | [#include <liblec/leccore/settings.h>](https://github.com/alecmus/leccore/blob/master/settings.h)


### Compile-time Dependencies
1. sqlcipher - you can use the pre-compiled files [here](https://github.com/alecmus/files)
2. crypto++

### Run-time Dependencies
1. sqlcipher
2. openssl

You can use the pre-compiled files for both from [here](https://github.com/alecmus/files).
