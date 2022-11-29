# sst-dbg: Structural Simulation Toolkit (SST) Debugging Environment

## License
See the include LICENSE file.

## Getting Started

### Prerequisites
* C++14 Compiler (LLVM/Clang and GCC are tested)
* CMake 2.6+
* SST 12.0.1+ (other versions may work, but are not tested)

### Building

Generally speaking, the sst-dbg infrastructure is entirely 
self contained.  It does not currently depend upon an active SST 
installation to build from source.  It does, however, require that 
the `sst-config` tool is in the current path for installation.  Generally
speaking, sst-dbg is built using the following steps:

1. Clone the sst-dbg repository.  There are currently no git dependencies
2. Create a ``build`` directory within the sst-dbg source tree
3. Change directories to ``build``
4. Execute cmake to generate the target-specific makefiles
5. Execute the build
6. Install the build

```
$> git clone git@github.com:tactcomplabs/sst-dbg.git
$> cd sst-dbg
$> mkdir build
$> cd build
$> cmake ../
$> make -j
$> make install
```

Following a successful installation, the `SSTDebug.h` header file is installed 
to `SST\_ROOT/include/sst/dbg` directory.  All of the sst-dbg binaries are installed 
to the `SST\_ROOT/bin` directory.


### Build options
* -DBUILD\_ALL\_TESTING=ON : builds the test suite (use `make test` to run the test suite)
* -DSSTDBG\_ASCII=ON : Enables ASCII output (default is JSON)

## Using sst-dbg

### Integrating sst-dbg into an SST Component

### Executing sst-dbg
```
$> sst-dbg -i 10 -- sst basicTest.py
```
