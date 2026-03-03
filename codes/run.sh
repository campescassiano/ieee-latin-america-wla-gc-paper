#!/bin/bash

# Default values
N_BATCHES=10
N_BLOCKS=100000
TEST_BINARY=""

function compile_test_binary() {
    local TEST_BINARY=${1:-"test_binary"}
    if ! gcc -g src/debug.c src/equation.c src/random.c src/ssd.c -Iinclude -pthread -o "$TEST_BINARY"; then
    #if [[ "$?" -ne 0 ]]; then
      echo "Error while compiling code"
      exit 1
    fi

    echo "generated binary: $TEST_BINARY"
}

function usage() {
    echo "Usage: $0 --n_batches <value> --n_blocks <value>"
}

function parse_args() {
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --n_batches)    N_BATCHES="$2"; shift 2 ;;
            --n_blocks)     N_BLOCKS="$2"; shift 2 ;;
            --test_binary)  TEST_BINARY="$2"; shift 2;;
            --help|-h)      usage;  exit 1 ;;
            *)              echo "Unknown argument: $1"; usage; exit 1 ;;
        esac
    done

    if [[ -z "$N_BATCHES" || -z "$N_BLOCKS" ]]; then
        echo "Error: both --n_batches and --n_blocks are required."
        usage
        exit 1
    fi

    if [[ -z "$TEST_BINARY" ]]; then
        echo "Error: --test_binary must not be empty."
        usage
        exit 1
    fi

    if [[ "$N_BATCHES" -eq 0 || "$N_BLOCKS" -eq 0 ]]; then
        echo "Error: both --n_batches and --n_blocks must not be zero."
        usage
        exit 1
    fi


}

function print_params() {
    echo -e "Configured parameters\n"
    echo "n_batches = $N_BATCHES"
    echo "n_blocks  = $N_BLOCKS"
    echo ""
}

parse_args "$@"
print_params
compile_test_binary "$TEST_BINARY"


PERCENTILES=(5 10 15 20 25 30 35 40 45 50 55 60 65 70 75 80 85 90 95 100)

OUTPUT_FILE="logs-${N_BLOCKS}-blocks$(date '+%Y-%m-%d_%H%M%S').txt"

start_time=$(date +%s)
echo "--------------------------------------------------------" >> "$OUTPUT_FILE"
echo "%, #batches, #blks, #pgs/blk, baseline, baseline wearout, wla, wla wearout, improvement, % wearout" >> "$OUTPUT_FILE"
for percentile in "${PERCENTILES[@]}"; do
  echo "./$TEST_BINARY --n_batches $N_BATCHES --num_blocks=$N_BLOCKS --percentile_valid=$percentile"
  output=$("./$TEST_BINARY" --n_batches "$N_BATCHES" --num_blocks "$N_BLOCKS" --percentile_valid "$percentile")
  echo "$output" >> "$OUTPUT_FILE"
  echo "$output"
done
end_time=$(date +%s)
elapsed_time=$((end_time - start_time))

echo -e "\nTotal execution time: $elapsed_time seconds"
echo -e "\nTotal execution time: $elapsed_time seconds" >> "$OUTPUT_FILE"
