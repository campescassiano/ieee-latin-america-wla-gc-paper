# README

Here are all the code resoures and experiments performed while writting the
research paper.

**Title:**
- Word-Line-Aware Garbage Collector for QLC-based NAND Flash Memories

**Authors:**
- Cassiano Silva de Campes (cassiano.campes@csl.skku.edu, scampes@edu.unisinos.br)
- Rodrigo Marques de Figueiredo (marquesf@unisinos.br)
- Sandro José Rigo (rigo@unisinos.br)

This research paper was accepted to be published on:
- Journal IEEE Latin America Transactions

**Submission-ID:** 10214

It is organized as follows:

- codes/experiments: contains the executed experiments with different block sizes.
- codes/include: C-header source files of the code developed to emulate an SSD and its FTL with Vanilla and WLA-GC modes
- codes/src: C-language source files developed to emulate a SSD and its FTL with Vanilla and WLA-GC modes

The file `run.sh` automates everything for you, that means, it compiles and run new batches of experiments.

Please observe that in our machine, to run 1 million blocks we took about 90 hours of execution.
The source code launches only two threads to run each one of the algorithms.
So, to recude this time, one option would be to split into more threads to deal with chunck of SSD blocks.
