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
fundamental building blocks like databases, file handling, application updates, registry settings, and other general helper utilities
specially designed to make building robust, sophisticated applications a breeze. Furthermore, it is designed to
bind together all the other liblec libraries like [lecui](https://github.com/alecmus/lecui) and
[lecnet](https://github.com/alecmus/lecnet).

### Classes
The following classes have been added to the library
Class Name            | Description                            | Interface
--------------------- | -------------------------------------- | ------------------------------------------------
database::connection  | Database connection                    | [#include <liblec/leccore/database.h>](https://github.com/alecmus/leccore/blob/master/database.h)
file                  | File handling helper                   | [#include <liblec/leccore/file.h>](https://github.com/alecmus/leccore/blob/master/file.h)
pc_info               | PC hardware information                | [#include <liblec/leccore/pc_info.h>](https://github.com/alecmus/leccore/blob/master/pc_info.h)
hash                  | Cryptographic hash                     | [#include <liblec/leccore/hash.h>](https://github.com/alecmus/leccore/blob/master/hash.h)
encode::base32        | Base32 character encoding              | [#include <liblec/leccore/encode.h>](https://github.com/alecmus/leccore/blob/master/encode.h)
encode::base64        | Base64 character encoding              | [#include <liblec/leccore/encode.h>](https://github.com/alecmus/leccore/blob/master/encode.h)
app_version_info      | Application version information        | [#include <liblec/leccore/app_version_info.h>](https://github.com/alecmus/leccore/blob/master/app_version_info.h)
registry              | Registry reading and writing           | [#include <liblec/leccore/registry.h>](https://github.com/alecmus/leccore/blob/master/registry.h)
encrypt               | Encryption                             | [#include <liblec/leccore/encrypt.h>](https://github.com/alecmus/leccore/blob/master/encrypt.h)
settings              | Application settings                   | [#include <liblec/leccore/settings.h>](https://github.com/alecmus/leccore/blob/master/settings.h)
check_update          | Checking for app updates               | [#include <liblec/leccore/web_update.h>](https://github.com/alecmus/leccore/blob/master/web_update.h)
download_update       | Downloading app updates                | [#include <liblec/leccore/web_update.h>](https://github.com/alecmus/leccore/blob/master/web_update.h)
zip                   | Zipping to a zip archive               | [#include <liblec/leccore/zip.h>](https://github.com/alecmus/leccore/blob/master/zip.h)
unzip                 | Unzipping a zip archive                | [#include <liblec/leccore/zip.h>](https://github.com/alecmus/leccore/blob/master/zip.h)
user_folder           | Getting the path to known user folders | [#include <liblec/leccore/system.h>](https://github.com/alecmus/leccore/blob/master/system.h)
commandline_arguments | Parsing command line arguments         | [#include <liblec/leccore/system.h>](https://github.com/alecmus/leccore/blob/master/system.h)
shell                 | Shell helper class                     | [#include <liblec/leccore/system.h>](https://github.com/alecmus/leccore/blob/master/system.h)

### Usage Examples
The library is used in the [pc_info](https://github.com/alecmus/pc_info) app.

### Prebuilt Binaries
Prebuilt binaries of the library can be found under
[releases](https://github.com/alecmus/leccore/releases).

### Compile-time Dependencies
The library has the following compile-time dependencies:
1. boost C++ libraries
* assumes boost 1.72 prebuilt binaries are in C:\local\libs\boost_1_72_0
* download prebuilt boost binaries [here](https://sourceforge.net/projects/boost/files/boost-binaries/)
* For example, the binaries for boost 1.72 are [here](https://sourceforge.net/projects/boost/files/boost-binaries/1.72.0/)
* For Microsoft Visual Studio 2019 you can download the file boost_1_72_0-msvc-14.2-64.exe (64 bit)
and boost_1_72_0-msvc-14.2-32.exe (32 bit).
2. sqlcipher
* assumes sqlcipher prebuilt binaries are in C:\local\libs\sqlcipher
* you can use the pre-compiled files [here](https://github.com/alecmus/files/tree/master/sqlcipher)
3. crypto++
* assumes crypto++ prebuilt static library files are in C:\local\libs\cryptopp
* you can use the pre-compiled files [here](https://github.com/alecmus/files/tree/master/cryptopp)
4. poco
* assumes poco prebuild static library files are in C:\local\libs\poco
* you can use the pre-compiled files [here](https://github.com/alecmus/files/tree/master/poco)

If the boost, sqlcipher and/or crypto++ libraries are installed elsewhere you will need to change the Microsoft Visual Studio project
properties under Properties - C/C++ - General - Additional Include Directories and also under
Properties - Linker - General - Additional Library Directories.

### Run-time Dependencies
1. sqlcipher
2. openssl

You can use the pre-compiled files for both from [here](https://github.com/alecmus/files).

### Building
Create a folder '\liblec' and clone the repository into it such that it resides in 'liblec\leccore'. Open the Microsoft Visual Studio Solution file liblec\leccore\leccore.sln. Select Build -> Batch Build, then select the desired configurations of the given four:
1. Debug x86
2. Relese x86 (32 bit Release Build)
3. Debug x64
4. Release x64 (64 bit Release Build)

Build.

Three folders will be created in the \liblec directory, namely bin, lib and include. Below is a description of these subdirectories.

1. bin - contains the binary files. The following files will be created:

File             | Description
---------------- | ------------------------------------
leccore32.dll    | 32 bit release build
leccore64.dll    | 64 bit release build
leccore32d.dll   | 32 bit debug build
leccore64d.dll   | 64 bit debug build

2. lib - contains the static library files that accompany the dlls. The files are named after the respective dlls.
3. include - contains the include files

### Linking to the Library

#### Microsoft Visual Studio
Open your project's properties and for All Configurations and All Platforms set the following:
1. C/C++ -> General -> Additional Include Directories -> Edit -> New Line ... add \liblec\include
2. Linker -> General -> Additional Library Directories -> Edit -> New Line ... add \liblec\lib
3. Debugging -> Environment -> Edit ... add PATH=\liblec\bin;PATH%

Now you can use the required functions by calling #include <liblec/leccore/...>

Build.

### Deploying your Application
If it's the 32 bit build you will need to deploy it with leccore32.dll, sqlcipher32.dll and libeay32.dll in the same folder. If it's a
64 bit build use leccore64.dll, sqlcipher64.dll and libeay64.dll.
