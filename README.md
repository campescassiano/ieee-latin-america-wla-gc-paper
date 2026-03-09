# Word-Line-Aware Garbage Collector for QLC-based NAND Flash Memories

**Manuscript ID:** IEEE LATAM

**Submission-ID:** 10214

**Authors:**
- Cassiano Silva de Campes<sup>1,2</sup> (cassiano.campes@csl.skku.edu, scampes@edu.unisinos.br)
- Rodrigo Marques de Figueiredo<sup>1</sup> (marquesf@unisinos.br)
- Sandro José Rigo<sup>1</sup> (rigo@unisinos.br)

**Affiliations**:

- <sup>1</sup> Universidade do Vale do Rio dos Sinos - UNISINOS, São Leopoldo, Rio Grande do Sul, Brazil

- <sup>2</sup> Sungkyunkwan University - SKKU, Swon, South Korea


## Disclaimer

All the codes and scripts here mentioned were developed on the referred machine presented on Table II of our paper running **Linux Ubuntu 22.04 LTS**.

## 🔍 Analytical Modeling Experiments

For **Equation 1**, **Equation 2**, and **Equation 3**, we implemented the analytical modeling using Python language to determine the experienced latencies based on the variation of valid pages within a block. The results are presented on **Figure 5** of the paper.

To run the python code, follow this:

```py
python3.10 codes/analytical_model.py
```

### Requirements

- python3.x
- numpy



## 👩‍💻 Experimental Modeling Experiments

Results presented on Figure 6 are gathered form the `experiments/logs-1M.txt` file. That logs are the experimental results of the execution of our NAND Flash Memory emulator running with 1 million blocks.
The execution shows the results for the Baseline (Vanilla GC) and the WLA-GC (proposed work), respectively.

The emulator was written in C language and we developed a helper script (`run.sh`) that simplifies the usage of the emulator.

It compiles the source files, generate the emulator binary, and executes the experiments according to the command line arguments passed to the `run.sh` script, that is configurable (you can refer to `./run.sh --help` to see the helper of that script).

So, the script executes `n_batches` of `n_blocks` for every valid page percentile (5%, 10%, 15%, ... 100%) and logs the execution statistics for that percentile (%), `n_batches` (#batches), `n_blocks` (#blks), number of pages per block (#pgs/blk), baseline statistics (baseline), proposed work statistics (wla), and the percentile improvement of WLA-GC over baseline (improvement).

To emulate the 1 million blocks in our hardware, it took:

```
Total execution time: 325251 seconds
```

Which is about 90 hours execution time.

That is because our emulator runs in a single thread per algorithm. To reduce this execution time, we would need to improve the multi-threading capacity of the emulator.

### How to execute

```bash
./codes/run.sh --n_batches 10 --n_blocks 1000000
```

FInally, use the `normalize.py` script to normalize the results of the execution.

For that, you need to copy the output result and paste inside the `normalize.py` file as indicated:

```py
python3.10 ./codes/normalize.py
```

### Requirements

- python3.x
- numpy
- pandas
- gcc compiler

## Summarized relationship of figures, tables and codes/scripts

| Script  | Related Figure(s) / Equations  | Description  |
|---------|---|---|
| `codes/analytical_model.py` |  Eq. 1, Eq. 2, Eq. 3, Fig. 5 | Implementation of the analytical modeling using Python language. The script uses the parameters specified in Table I. |
| `codes/run.sh` | Fig. 6 | Experimental results from the emulated NAND Flash memory. The related C code are compiled automatically by this script as depicted in [experimental modeling experiments](#-experimental-modeling-experiments) |
| `codes/normalize.py` | Fig. 6 | Script used to normalize the data and better present it using GNU Plot on LaTex.  |