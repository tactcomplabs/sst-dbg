# sst-dbg

build instructions
```
$> git clone git@github.com:tactcomplabs/sst-dbg.git
$> cd sst-dbg
$> module load sst/12.0.1
$> mkdir build
$> cd build
$> cmake -DBUILD_ALL_TESTING=ON ../
$> make -j
$> make test
```

## Build options
* -DBUILD\_ALL\_TESTING=ON : builds the test suite
* -DSSTDBG\_ASCII=ON : Enables ASCII output (default is JSON)
