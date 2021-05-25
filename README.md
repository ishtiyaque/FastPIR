# FastPIR

FastPIR is a computational private information retrieval (CPIR) library which
is specialized to reduce response generation time by the PIR server.
Details of FastPIR can be found in our [paper](https://eprint.iacr.org/2021/044.pdf).
Please note that FastPIR is a research prototype and should not be used in production system.


## Dependencies

FastPIR depends on [Microsoft SEAL version 3.5.6](https://github.com/microsoft/SEAL/tree/3.5.6). For optimal 
performance SEAL should be compiled with Clang++. 

## Compiling FastPIR

After installing SEAL, go to FastPIR clone directory and run the following commands:

	cd src
	cmake .
	make
	cd ..
	
This should produce a binary file ``bin/fastpir``.
To compile FastPIR with parameters other than the default ones, take a look at ``src/bfvparams.h`` and modify the 
intended parameter values.

## Running FastPIR

To run FastPIR from its clone directory, simply run:

	bin/fastpir -n <number of elements> -s <element size in bytes>
 


