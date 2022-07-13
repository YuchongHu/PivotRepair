PivotRepair
====

This repository includes source code of `PivotRepair` prototype system described in our paper accepted by ICDCS'22.

Installation
----

We developed the system on Ubuntu 16.04. So the following instructions on preparing development enironment are also based on Ubuntu 16.04.

### Common

Users can use `apt` to install the required tools and libraries.

 - gcc & g++
 - make & cmake
 - nasm
 - libtool & autoconf
 - git
 - python3

```bash
$  sudo apt update
$  sudo apt install gcc g++ make cmake nasm libtool autoconf git python3
```

### Intel®-storage-acceleration-library (ISA-L)

`PivotRepair` uses [ISA-L](https://github.com/intel/isa-l)(Intel(R) Intelligent Storage Acceleration Library) to perform encoding operations.

```bash
$  git clone https://github.com/intel/isa-l.git
$  cd isa-l
$  ./autogen.sh
$  ./configure; make; sudo make install
```

### Sockpp

`PivotRepair` uses [Sockpp](https://github.com/fpagliughi/sockpp) to implement socket communication.

```bash
$  git clone https://github.com/fpagliughi/sockpp.git
$  cd sockpp
$  mkdir build ; cd build
$  cmake ..
$  make
$  sudo make install
$  sudo ldconfig
```

### Wondershaper

`PivotRepair` needs [Wondershaper](https://github.com/magnific0/wondershaper) to limit the speed of network and complete the experiments.

```bash
$  git clone git://github.com/magnific0/wondershaper.git
$  cd wondershaper
$  sudo make install
```

### SSH

`PivotRepair` contains a quick start of the experiment, which requires the master node to control the other nodes using SSH.
Users can use following instructions to generate the ssh key on master and complete a key-free access from the master to the other nodes. `$username` and `$ip_address` should be replaced with yours.
Note that `$username` of all the nodes should be the same.

```bash
$  sudo apt install openssh-client openssh-server
$  ssh-keygen
$  ssh-copy-id -i ~/.ssh/id_rsa.pub $username@$ip_address
$  ssh $username@$ip_address
```

Run the experiment
----

### Compile

After preparing the development environment as shown in the Installation part, users can compile `PivotRepair` via make.

```bash
$  make
```

### Configuration

Before running the experiments, users should customize the configurations manually by modifying file `./scripts/config.py`.
The items that can be modified has comments above it.

### Data

The bandwidth data can also be modified, which is initially contained in the file `./config/bandwidths.txt`.

### Run

First, output the config file.

```bash
$  python3 scripts/config.py
```

Then, Run the experiments

```bash
$  python3 scripts/start.py
```

Users can also run the experiments in the background like this.

```bash
$  nohup python3 scripts/start.py > output.log 2>&1 &
```

### Results

After running, the results would be initially stored in the file `./files/results.txt`.
Each line is the results of each time, (n, k), algorithm and bandwidth. The construction of the lines would be (times, nks, algorithms, bandwidths).
The four numbers in each line represent the overall bandwidth, algorithm computing time (us), repair time (us), and task number (always be 1 in this experiment), respectively.

## Publication

Qiaori Yao, Yuchong Hu*, Xinyuan Tu, Patrick P. C. Lee, Dan Feng, Xia Zhu, Xiaoyang Zhang, Zhen Yao and Wenjia Wei. **"PivotRepair: Fast Pipelined Repair for Erasure-Coded Hot Storage"** Proceedings of the 42nd IEEE International Conference on Distributed Computing Systems (ICDCS 2022)(CCF B)(accepted)

## Contact

Please email to Yuchong Hu ([yuchonghu@hust.edu.cn](mailto:yuchonghu@hust.edu.cn)) if you have any questions.