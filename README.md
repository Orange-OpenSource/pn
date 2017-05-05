# `pn` -- a libphonenumber command-line wrapper

`pn` is a tool that allows the command line user/programmer to operate on phone
numbers (get validity information, reformat them, or extract numbers from a
text snippet), using
[libphonenumber](https://github.com/googlei18n/libphonenumber).

# How to build `pn`?

## Prerequisites

For **Debian 9** (Stretch), **Ubuntu 16.10**, **Ubuntu 17.04**:

```
$ sudo apt install cmake build-essential libphonenumber-dev libicu-dev
```

For **Ubuntu 16.04**:

```
$ sudo apt install cmake build-essential libphonenumber-dev libgeocoding-dev libicu-dev
```

Other platforms:

To build `pn` you need a C++ compiler, [CMake](https://cmake.org/), Google's
[libphonenumber](https://github.com/googlei18n/libphonenumber) (including the
geocoding part) binaries and dev headers, [ICU](http://site.icu-project.org/)
binaries and dev headers.

## Build
```
$ cd build
$ cmake ..
$ make
```
## Install

```
$ sudo make install
```

# Features

See the man page for more details.

Check the validity of a number:

```
$ pn valid -v +33123456789
valid number
(SUCCESS)
$ pn valid -v '(202) 555-0110'
invalid country code
(ERROR)
$ pn valid -v -c US '(202) 555-0110'
valid number
(SUCCESS)

```

Re-format a phone number:

```
$ pn format "+1 20 2 555    01 10"
+12025550110
$ pn format -c FR "0123456789"
+33123456789
$ pn format -c FR -f nat "0123456789"
01 23 45 67 89
$ pn format -f int "+12025550110"
+1 202-555-0110
```

Find valid numbers inside a free text input:

```
$ pn find -f nat "2017/04/20: You have 2 messsages, call +1-202-555-0110 to listen to them."
(202) 555-0110
$ pn find -c FR "rappelle-moi au 01 23 4 56789 ou au 06 78 90 12 34 après 20h00"
+33123456789
+33678901234
```

Get some informations about a number:

```
$ pn info "+33 2 96 48 46 98"
country code: 33 (FR)
number type: fixed line
location: Lannion
possible short number: false
valid short number: false
emergency number: false

$ pn info -c FR 112
country code: 33 (FR)
number type: unknown
location: 
possible short number: true
valid short number: true
emergency number: true
```

