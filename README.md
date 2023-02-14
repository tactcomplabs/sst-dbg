# sst-dbg: Structural Simulation Toolkit (SST) Debugging Environment

## License
See the include LICENSE file.

## Getting Started

### Prerequisites
* C++17 Compiler (LLVM/Clang and GCC are tested; gcc 8+ is preferred)
* CMake 3.14+
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
to `SST_ROOT/include/sst/dbg` directory.  All of the sst-dbg binaries are installed 
to the `SST_ROOT/bin` directory.


### Build options
* -DBUILD\_ALL\_TESTING=ON : builds the test suite (use `make test` to run the test suite)
* -DSSTDBG\_ASCII=ON : Enables ASCII output (default is JSON)
* -DSSTDBG\_MPI=ON : Enables MPI support in the test harness

## Using sst-dbg

### Integrating sst-dbg into an SST Component

The *sst-dbg* environment resides withiin the `printStatus` virtual method of a 
component or subcomponent.  When a debug dump request is made from the `sst-dbg` console 
application, the sst-core infrastructure makes forces each primary component to 
call the `printStatus` method.  Note that it DOES NOT call `printStatus` for 
subcomponents.  If a primary component requires that subcomponents also output 
debug data, then the `printStatus` function must explicitly call the `printStatus` 
method for each constituent subcomponent.

The process by which to add *sst-dbg* support consists of three steps.  First, 
we need to add support to our component's header file by including the `SSTDebug.h` 
header and adding `SSTDebug` object.  We also need to add a `printStatus` method
to the component class.  This is similar to the following:

```
#ifndef _BASIC_COMPONENT_H_
#define _BASIC_COMPONENT_H_

#include <sst/core/sst_config.h>
#include <sst/core/component.h>
#include <sst/core/subcomponent.h>
#include <sst/core/event.h>
#include <sst/core/link.h>
#include <sst/core/rng/marsaglia.h>
#include <vector>
#include <string>

#include <sst/dbg/SSTDebug.h>

namespace SST {
namespace basicComponent {
...

class basicClock : public SST::Component
{
public:

  // Register the component with the SST element library
  SST_ELI_REGISTER_COMPONENT(
    basicClock,                             // Component class
    "basicComponent",                       // Component library
    "basicClock",                           // Component name
    SST_ELI_ELEMENT_VERSION(1,0,0),         // Version of the component
    "basicClock: simple clocked component", // Description of the component
    COMPONENT_CATEGORY_UNCATEGORIZED        // Component category
  )

  // Document the parameters that this component accepts
  // { "parameter_name", "description", "default value or NULL if required" }
  SST_ELI_DOCUMENT_PARAMS(
    { "clockFreq",  "Frequency of period (with units) of the clock", "1GHz" },
    { "clockTicks", "Number of clock ticks to execute",              "500"  }
  )

  // [Optional] Document the ports: we do not define any
  SST_ELI_DOCUMENT_PORTS()

  // Document the statisitcs: we do not define any
  SST_ELI_DOCUMENT_STATISTICS(
    {"EvenCycleCounter", "Counts even numbered cycles", "unitless", 1},
    {"OddCycleCounter",  "Counts odd numbered cycles",  "unitless", 2},
    {"HundredCycleCounter", "Counts every 100th cycle", "unitless", 3}
  )

  // Document the subcomponents
  SST_ELI_DOCUMENT_SUBCOMPONENT_SLOTS(
    {"msg", "Message Interface", "SST::basicComponent::basicMsg"}
  )

  // Class members

  // Constructor: Components receive a unique ID and the set of parameters
  //              that were assigned in the simulation configuration script
  basicClock(SST::ComponentId_t id, SST::Params& params);

  // Destructor
  ~basicClock();

  // Debug print status
  void printStatus(Output& out) override;

private:

  // Clock handler
  bool clock(SST::Cycle_t cycle);

  // Params
  SST::Output* out;       // SST Output object for printing, messaging, etc
  std::string clockFreq;  // Clock frequency
  Cycle_t cycleCount;     // Cycle counter
  Cycle_t printInterval;  // Interval at which to print to the console

  SST::RNG::MarsagliaRNG* rng;  // Random number generator

  unsigned long long REG[32];

  // Subcomponents
  basicSubcomponent *msg; // basicSubcomponent

  // Statistics
  Statistic<uint64_t>* EvenCycles;    // Even cycle statistics counter
  Statistic<uint64_t>* OddCycles;     // Odd cycle statistics counter
  Statistic<uint32_t>* HundredCycles; // Hundred cycle statistics counter

  // Debug
#ifdef ENABLE_SSTDBG
  SSTDebug *Dbg;
#endif

};  // class basicClocks
}   // namespace basicComponent
}   // namespace SST

#endif
```

The next stage will be to modify the C++ implementation file.  This requires us to 
initialize our `SSTDebug` object in the constructor and destroy it in the destructor.
The process of doing so requires a name for the component, often derived from the 
`getName()` SST function and a path to where the output files are placed.  This is 
analogous to the following:

```
// basicClock constructor
// - Read the parameters
// - Register the clock handler
basicClock::basicClock(ComponentId_t id, Params& params)
  : Component(id) {

  // Create a new SST output object
  out = new Output("", 1, 0, Output::STDOUT);

  // Retrieve the paramaters from the simulation script
  clockFreq  = params.find<std::string>("clockFreq", "1GHz");
  cycleCount = params.find<Cycle_t>("clockTicks", "500");

  // Tell the simulation not to end until we signal completion
  registerAsPrimaryComponent();
  primaryComponentDoNotEndSim();

  // Register the clock handler
  registerClock(clockFreq, new Clock::Handler<basicClock>(this, &basicClock::clock));

  out->output("Registering clock with frequency=%s\n", clockFreq.c_str());

  // Register statistics
  EvenCycles = registerStatistic<uint64_t>("EvenCycleCounter");
  OddCycles  = registerStatistic<uint64_t>("OddCycleCounter");
  HundredCycles = registerStatistic<uint32_t>("HundredCycleCounter");

  // Load the subcomponent
  msg = loadUserSubComponent<basicSubcomponent>("msg");

  // Initialize the RNG
  rng = new SST::RNG::MarsagliaRNG(11, 272727);

  // This component prints the clock cycles & time every so often so calculate a print interval
  // based on simulation time
  printInterval = cycleCount / 10;
  if (printInterval < 1)
    printInterval = 1;

#ifdef ENABLE_SSTDBG
  Dbg = new SSTDebug(getName(),"./");
#endif
}

// basicClock destructor
basicClock::~basicClock(){
  delete out;
  delete rng;
#ifdef ENABLE_SSTDBG
  delete Dbg;
#endif
}
```

Finally, we need to craft the `printStatus` method which performs the actual debug 
dumps.  The `dump` method requires that the user specify the current simulation 
cycle, often using the `getCurrentSimCycle()` SST function.  Following that, users 
can specify any number of (variadic) *name*, *value* pairs.  The SSTDebug infrastructure 
provides a convenient macro called `DARG` that does this for you.  Within the `printStatus` 
method we can optionally also call the `printStatus` method for all the attached subcomponents.

```
void basicClock::printStatus(Output& out){
#ifdef ENABLE_SSTDBG
  if( !Dbg->dump(getCurrentSimCycle(),
                  DARG(cycleCount),
                  DARG(printInterval)) ){
    out.output("!!!!!!!!!!!!!!!!!!!!!! DEBUG DUMP FAILED !!!!!!!!!!!!!!!!!!!!!!\n");
  }

  // optional, call the subcomponent's printStatus
  subComp->printStatus(out);
#endif
}
```

### Building sst-dbg components with MPI support

If you seek to reduce the number of files generated during a given parallel simulation, 
we highly suggest that you enable MPI functionality in SST-Dbg.  This switches the base 
library to generate a single file per MPI rank per dump iteration.  These files will 
be generated in the form: `SSTDbg.RANK.CLOCK.json/csv`.

Enabling this functionality requires that the user build the SST component with 
the MPI compiler wrapper (`mpicxx`) and enable the SSTDBG_MPI preprocessor macro.  This 
mimics the following:

```
mpic++ -O2  -std=c++1y -D__STDC_FORMAT_MACROS -fPIC -DHAVE_CONFIG_H -I/scratch/sst/sst-12.1.0/include -I/scratch/sst/sst-12.1.0/elements/include -DENABLE_SSTDBG -DSSTDBG_MPI -DDEBUG -g -I./ -c myComponent.cc
```

### Executing sst-dbg
```
$> sst-dbg -i 10 -- sst basicTest.py
$> sst-dbg -i 60 -- mpirun sst basicTest.py
$> sst-dbg -i 10 -- sst --model-options="--node 10" basicTest.py
```

### Contributing
Please submit all pull requests to the `devel` branch.  All PRs will be tested 
for functionality before they will be merged.
