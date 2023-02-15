# Parallel NS-3 simulation examples

### A simple tutorial on running NS-3 in parallel, including simulation of FatTree, GEANT, and Abilene topologies.

### Environment Setup
#### Dependency Libraries Installation
Our simulation examples are based on the NS-3 simulator, which requires some dependency libraries.

- [Python == 2.7.X](https://www.python.org/)
- [GCC-5 G++-5](https://gcc.gnu.org/)
- [MPI](https://www.open-mpi.org/)


#### Compile Simulator 
```
CC='gcc-5' CXX='g++-5' ./waf configure --enable-examples --enable-mpi
```

**Note that the MPI component must be installed if you want to run the simulation in parallel**

### How To Run
You need to specify the topology and traffic files you want to run in the configuration file before running the simulation. The whole project consists of three components:
- Source code: `/scratch/distributed-third`
- Input/output files: `mix/geanet_topo.txt`(topology input) `mix/geanet_flow.txt`(flow characteristics input) `parallel_distributed/trace_result_parallel2_geanet.tr` `parallel_distributed/fct_parallel2_geanet.txt` `parallel_distributed/pfc_parallel2_geanet.txt`(result output)
- configuration file: `/parallel_distributed/distributed_config.txt`

#### Run The Simulation In A Single Process
**Modify the configuration file**
```
vim /parallel_distributed/distributed_config.txt

...

## lines 4-9 of this file
TOPOLOGY_FILE mix/fattree4_topo.txt
FLOW_FILE mix/fattree4_flow.txt
TRACE_FILE mix/trace.txt
TRACE_OUTPUT_FILE parallel_distributed/trace_result_parallel1_fattree4.tr
FCT_OUTPUT_FILE parallel_distributed/fct_parallel1_fattree4.txt
PFC_OUTPUT_FILE parallel_distributed/pfc_parallel1_fattree4.txt

...

```
**Run the Simulation**
```
  ./waf --run 'scratch/distributed-third parallel_distributed/distributed_config.txt'
```


#### Run With Multiple Processes
Use `mpirun` to run the program parallelly. For example, Run FatTree-4 with 2 processes:
```
mpirun -np 2 ./waf --run 'scratch/distributed-third parallel_distributed/distributed_config.txt'
```

#### Run With Multiple machines
Ensure that programs on different machines have the same path. Parallel simulation can then be started on a primary server. Based on the above command, add IP addresses of all machines after `-host`. For example, run FatTree-4 on two machines:
```
mpirun -np 2 -hosts 12.12.12.101,12.12.12.102(IP addresses of your machines) ./waf --run 'scratch/distributed-third parallel_distributed/distributed_config.txt'
```


