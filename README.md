# README

Here are all the code resoures and experiments performed while writting the
research paper:
"Word-Line-Aware Garbage Collector for QLC-based NAND Flash Memories"

It is organized as follows:

- codes/experiments: contains the executed experiments with different block sizes.
- codes/include: C-header source files of the code developed to emulate an SSD and its FTL with Vanilla and WLA-GC modes
- codes/src: C-language source files developed to emulate a SSD and its FTL with Vanilla and WLA-GC modes

The file `run.sh` automates everything for you, that means, it compiles and run new batches of experiments.

Please observe that in our machine, to run 1 million blocks we took about 90 hours of execution.
The source code launches only two threads to run each one of the algorithms.
So, to recude this time, one option would be to split into more threads to deal with chunck of SSD blocks.
