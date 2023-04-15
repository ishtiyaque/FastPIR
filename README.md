# FastPIR

FastPIR is a computational private information retrieval (CPIR) library which
is specialized to reduce response generation time by the PIR server.
Details of FastPIR can be found in our [paper](https://www.usenix.org/conference/osdi21/presentation/ahmad).
Please note that FastPIR is a research prototype and should not be used in production system.


## Dependencies

FastPIR depends on [Microsoft SEAL version 4.1.1](https://github.com/microsoft/SEAL/tree/4.1.1). For optimal
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
 

## Citing FastPIR

To cite FastPIR in an academic paper, please use the following BibTeX entry:

	@inproceedings{ahmad2021addra,
	  title={Addra: Metadata-private voice communication over fully untrusted infrastructure},
	  author={Ahmad, Ishtiyaque and Yang, Yuntian and Agrawal, Divyakant and El Abbadi, Amr and Gupta, Trinabh},
	  booktitle={15th {USENIX} Symposium on Operating Systems Design and Implementation ({OSDI} 21)},
	  pages = {313--329},
	  year={2021}
	  month = jul,
	}
