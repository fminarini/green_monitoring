About 
==============================
<p>Greenhouse gas (GHG) emissions have been recognized as accelerators of
the Global climate change phenomenon and several human activities take
part in it. In particular, the contribution of the computing sector is
significant and deemed to grow[1]. While on one side unprecedented results
have been reached thanks to the increasing computational power available,
on the other, scientific research might become, in the near future,
energetically unsustainable. This might be seen as a consequence of the
general neglection of the burst of eager requirements/algorithms to carry
it on.</p>
<p>In order to avoid underestimating the footprint of Computing, users (as
well as data centers) should be able to keep track and analyze the burden
associated to their operations, thus making said footprint a viable
performance indicator. Equivalently, Reporting such data should be also
promoted, in the interest of transparent sharing of computing results and
impacts.[2]</p>
<p>We introduce KIG, an open tool written in C++, that allows users
and data centers to easily keep track, analyze and report the energy
requirements and carbon footprint in gCO2e[3] of their computing tasks. Such
tool should help shedding some light on the often not-so-trivial trade-
off between performance and environmental Footprint. By gathering
detailed data, such tool should also trigger meta-analyses on the
behaviour of algorithms as well as computing infrastructures with a view
to better leveraging said resources.</p>

# Quick Start
## System pre-requisites ##
To build the library, your machine should have:

    - g++ supporting c++17 standard
    - cmake (min.required version 3.20)

To check if your machine satisfies the requirements, you can launch 
```
g++ --version
cmake --version
```
in a terminal window.

In case you're not meeting the requirements, consult your distro manual to find
out how to set your environment properly.

### Library pre-requisites ###
The library relies on only one external library which is provided here:
`https://github.com/ToruNiina/toml11`

we suggest cloning the repo and build the software with cmake. 
```
git clone <REPO>
cd <CLONED_REPO_FOLDER>
mkdir build
cd build
cmake ..
make install
```

## Download KIG ##
In order to download KIG, please enter the following commands on your terminal.
```
mkdir <dir_name>
cd <dir_name>
git init

git clone "<LINK>"
```

## Test download and Install KIG ##
Given the previous step was successfully completed, before installing the
library it is advisable to test the build.

```
cd <KIG>
mkdir build
cd build
cmake ..
make all
```
It should bring 0 warnings and 0 errors.

## Install KIG library ##
Once the build has been tested, installing KIG only requires these two commands:
```
sudo make install
sudo ldconfig
```

## M.W.E. ##

A small program `mwe.cpp` has been uploaded in this repo to show a minimal working example of the library.
The user can adopt it "as is" or customize its few parameters.
Each function will be documented in an external Doc page.
