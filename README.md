# cmcol -- a music collection file creator

cmcol is a small command-line program for creating a XML file containing basic information about your music collection.

## Prerequisites

* glibc 2.1 or newer
* g++ or clang++
* LibTag
* libxml2

## Installing

Generally just *make* run in the source directory should be sufficient to build the binary, but as include path or compiler are hardcoded in the Makefile (for the time being), you may need to do some changes.

## Usage

```
cmcol [OPTION...] INPUT_DIR

  -o, --output[=FILE]       Output to FILE instead of standard output
  -?, --help                Give this help list
  --usage                   Give a short usage message
  -V, --version             Print program version

```

## Authors

* **Josef Vitu**


## License

This project is licensed under the GPLv3 License - see the LICENSE file for details.

