# cmcol -- a music collection file creator

cmcol is a small command-line program for creating a XML file containing basic information about your music collection.

## Prerequisites

* cmake
* glibc (or compatible)
* g++ or clang++
* Boost.Program\_options
* LibTag
* libxml2

## Installing

Get the code:

```
git clone https://github.com/JosefVitu/cmcol.git && cd cmcol
```

Create a separate build directory and compile:
```
mkdir build && cd build
cmake ..
make
```

Optionally install:

```
sudo make install
```

### Uninstalling

Delete the installed binary *cmcol* manually, or run the following command from the build directory:

```
sudo make uninstall
```

## Usage

```
  -i [ --input ] arg    Input directory (current directory if not specified)
  -o [ --output ] arg   Output file (standard output if not specified)
  -? [ --help ]         Print this help message
  -V [ --version ]      Print program version
```

## Authors

* **Josef Vitu**

## License

This project is licensed under the GPLv3 License - see the LICENSE file for details.

