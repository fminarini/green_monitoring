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

Quick Start
====================

## System pre-requisites
To build the library, your machine should have:

    - g++ supporting c++17 standard
    - cmake (min.required version 3.20)
    - pkg-config
    - git

package installation varies across different distros. As an example, if you use Aptitude:

```
sudo apt-get install -y build-essential
sudo apt-get install -y cmake
sudo apt-get install -y pkg-config
sudo apt-get install -y git
```

### Library pre-requisites
The library relies on only one external library, which is provided here:
`https://github.com/ToruNiina/toml11` (MIT LICENSED)

we suggest cloning the repo and build the software with cmake. 
```
git clone https://github.com/ToruNiina/toml11
cd toml11
mkdir build
cd build
cmake ..
make all
make install
```

### Download KIG 
In order to download KIG, please enter the following commands on your terminal.
```
mkdir <dir_name>
cd <dir_name>
git init

git clone "<LINK>"
```

### Test download and Install KIG 
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

### Install KIG library
Once the build has been tested, installing KIG only requires these two commands:
```
sudo make install
sudo ldconfig
```

CONTAINERIZED SOLUTION
=========================

If you don't want (or you're not allowed) to install the library, there is a containerized solution publicly available on DockerHub.

In order to run KIG from the containerized solution, these are the commands to run it "as an executable":

### EXECUTABLE-LIKE CONTAINER ###
```
docker run -dit --name <container_name> --pid host --mount type=bind,source="$(pwd)"/<folder_with_toml_file>,target=/conf <IMAGE_NAME> bash
docker exec -it <container_name> ./KIG_ex $(pgrep -f "<monitored_activity>" | awk 'ORS=" "')
```
The LaTeX report, if required, will be created in the home of kig_user.


### INTERACTIVE MODE ###
You can also use the container interactively:
```
docker run -it --name <container_name> --pid host --mount type=bind,source="$(pwd)"/<folder_with_toml_file>,target=/conf <IMAGE_NAME> bash
```
your user is "kig_user" and your home is "/home/kig_user". There you will find MWE.cpp (and its compiled executable ./KIG_ex). 
To monitor a process, launch:

```
./KIG_ex $(pgrep -f "<monitored_activity>" | awk 'ORS=" "')
```
The LaTeX report, if required, will be created in the home of kig_user.


## Bibliography ##

[1] Jones, N. "How to stop data centres from gobbling up the world's electricity", Nature, 2018.

[2] Lannelongue, L. et al. "Green Algorithms: Quantifying the Carbon Footprint of Computation", Advanced Science, 2021.

[3] Wright, L.A. et al. "Carbon footprinting: towards a universally accepted definition, Carbon Management, 2014.
