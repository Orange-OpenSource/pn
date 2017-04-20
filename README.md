# `pn` -- a libphonenumber command-line wrapper

`pn` is a tool that allows the command line user/programmer to operate on phone
numbers (currently reformat them, or extract numbers from a text snippet),
using [libphonenumber](https://github.com/googlei18n/libphonenumber).

# Features

## `format`

### Example


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

### Usage


```
pn format [options] NUMBER

Options:

-c COUNTRY_CODE  assumes NUMBER is a number of the given country
   (by default, pn assumes NUMBER is in an international format)
-f FORMAT        output format, one of:
                 "e164"   E164 (default)
                 "nat"    national formating
                 "int"    international formating
                 "teluri" tel URI (e.g. "tel:+123456789")

Returns:

On success: the formatted number on stdout and a success return code
On failure: an error message on stderr and a failure return code

```

## `find`

### Example


```
$ pn find -f nat "2017/04/20: You have 2 messsages, call +1-202-555-0110 to listen to them."
(202) 555-0110
$ pn find -c FR "rappelle-moi au 01 23 4 56789 ou au 06 78 90 12 34 après 20h00"
+33123456789
+33678901234
```

## More to come

Stay tuned

### Usage

```
pn find [options] TEXT

Options:

-c COUNTRY_CODE  assumes TEXT contains numbers of the given country
   (by default, pn assumes numbers are in an international format)
-f FORMAT        output format, one of:
                 "e164"   E164 (default)
                 "nat"    national formating
                 "int"    international formating
                 "teluri" tel URI (e.g. "tel:+123456789")
-l LENINECY      leniency when finding potential phone numbers in text segments,
                 one of "possible", "valid", "strict" or "exact"
   (see https://javadoc.io/doc/com.googlecode.libphonenumber/libphonenumber/8.4.1)

Returns:

On success: the formatted numbers on stdout (one per line) and a success return code
On failure: an error message on stderr and a failure return code

```

# Building

prerequisites: `cmake`, `libphonenumber` lib and dev headers


```
$ cd build
$ cmake ..
$ make
```
