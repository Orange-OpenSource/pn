# `gawkpn`: a libphonenumber binding for GNU Awk

This library adds some functions to GNU Awk for phone number manipulation.

## Example

Given this awk program:

```awk
@load "gawkpn"

BEGIN {
	FS=";"
	OFS=";"

	# set the phone number output format
	pn_set_format("e164")

	# input numbers are considered as being dialed from this country
	pn_set_country("FR")
}

# only keep lines whose 2nd field is a valid phone number
pn_valid($2) {
	# fetch informations about the number
	pn_info($2, infos)

	# display first field, e164-formated number, location informations
	print $1, pn_format($2), infos["location"]
}
```

Filtering this data file:

```
foo;02 96 48 46 98
bar;00 12 34 56 78
baz;0123456789
fizz;01.55.48.70.02
buzz;00 1 202 555 0110
```

through the program given above, gives the following output:

```
foo;+33296484698;Lannion
baz;+33123456789;France
fizz;+33155487002;France
buzz;+12025550110;Washington D.C.
```

The line starting with `bar` is evicted because `00 12 34 56 78` is not a valid
number if dialled from a French telephone.

## Functions

### `pn_set_format(FORMAT)`

Sets the phone number output format. **FORMAT** must be one of **"e164"**,
**"int"** (international), **"nat"** (national), or **"teluri"** (RFC3966 tel
URI). The default format is **"e164"**.

### `pn_set_country(COUNTRY-CODE)`

The phone number(s) given as input to the following functions will be
considered as being dialed from the country specified by the 2 uppercase
letters **COUNTRY-CODE** (e.g.  **"FR"**, **"US"**, ...). If this option is not
set, gawkpn will consider the numbers are in the international format.

### `pn_valid(NUMBER)`

Tests whether the phone number given as NUMBER (string) is a valid number or
short number. Returns **0** if the number is invalid, **1** if it's a valid
number, **2** if it's a valis short number. 

### `pn_format(NUMBER)`

Formats the given **NUMBER** using the formating set with `pn_set_format()`, or
E164 if none was set.

### `pn_info(NUMBER, ARRAY)`

Gets informations about the given **NUMBER**, and returns them in the given
**ARRAY** with the following keys:

- **"prefix"** (number): the international prefix
- **"country"** (string): the 2-uppercase-letter country code
- **"type"** (number): the telephone number type
	- **0**: fixed line
	- **1**: mobile
	- **2**: fixed line or mobile (in some regions (e.g. the USA), it is
	  impossible to distinguish between fixed-line and mobile numbers by
	  looking at the phone number itself)
	- **3**: toll-free
	- **4**: premium rate
	- **5**: [shared cost](http://en.wikipedia.org/wiki/Shared_Cost_Service)
	- **6**: VoIP
	- **7**: [personal number](http://en.wikipedia.org/wiki/Personal_Numbers)
	- **8**: pager
	- **9**: universal access number or company number
	- **10**: voicemail access number
	- **11**: unknown
- **"location"** (string): the approximate location of the given number (using
  offline geocoding)
- **"possible\_short\_number"** (number): 1 if the number is a possible short
  number, 0 otherwise
- **"valid\_short\_number"** (number): 1 if the number is a valid short number,
  0 otherwise
- **"emergency\_number"** (number): 1 if the number is an emergency number, 0
  otherwise

### `pn_find(TEXT, ARRAY)`

Finds phone numbers in the given **TEXT**, and returns the numbers that were
found as **keys** of the given **ARRAY**.

### `pn_dialout(E164NUMBER, [COUNTRY-CODE])`

Formats the given **E164NUMBER** phone number (e164 format) and returns the
dialing string for out-of-country dialing purposes. If **COUNTRY-CODE** is
provided, it will specify the origin country for out-of-country dialing,
otherwise the country given in the last call to `pn_set_country(COUNTRY-CODE)`
will be used.

## Building

If GNU Awk is installed on your system and its extension development header was
found (`gawkapi.h`), building the `pn` project in `../build` will compile the GNU Awk extension library as **gawkpn.so** (see README.md in the
parent directory for prerequisites and build instructions). The `make install` rule will try to locate your awk extensions path, and will install this extension in it automatically.

If you want to install it manually, either copy it in your system's GNU Awk extensions
folder (e.g.: `/usr/lib/x86_64-linux-gnu/gawk/` on an amd64 Debian 8 install),
or set the **AWKLIBPATH** environment variable to include the folder where
**gawkpn.so** lies before running your awk program:

```
$ AWKLIBPATH=/folder/containing/the/lib gawk '@load "gawkpn"; ...'
```
