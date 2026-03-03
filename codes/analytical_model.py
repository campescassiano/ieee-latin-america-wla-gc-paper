import numpy as np

# This python is to calculate the best-case scenario of wla-gc against vanilla.
# That  means, when the valid pages follows an increasing order starting
# from the fast-pages (SLC type), spread over the WL width, then go to the
# next row (MLC type), spread over the WL width, and so on.
# While on the vanilla we take the worst-case where its validity pages follows
# an decreasing order starting from the slow-page (QLC type), spread over the WL
# width, then go to the next upper row (TLC type), spread over the WL width,
# and so on.

t_program = 20
t_erase = 60
t_read = np.array([1, 2, 2.5, 3])   # read latencies
t_write = np.array([2, 3, 3.5, 4])  # write latencies

N_ROWS = 4  # each row indicates cell type: SLC, MLC, TLC, QLC
N_COLS = 4  # represent the WL width (in pages)

def calc_latency(N_valid):
    count_valid = np.sum(N_valid)
    return np.sum(np.matmul(t_read, N_valid)) + t_program * count_valid + t_erase

def fill_matrix(row_order):
    """
    Here we do an incremental fill of the 4x4 matrix that represents the cell
    types and the WL width. It increments one cell of the matrix at time.
    It follows the given row_order passed, example: [3,2,1,0] for the QLC type;
    or [0,1,2,3] for SLC type.

    Return the list with 16 values of the proper latencies.
    """
    N_valid = np.zeros((N_ROWS, N_COLS), dtype=int)
    latencies = []
    for row in row_order:
        for col in range(N_COLS):
            N_valid[row, col] = 1
            latencies.append(calc_latency(N_valid))
    return latencies


vanilla = fill_matrix(row_order=[3, 2, 1, 0])
wla_gc = fill_matrix(row_order=[0, 1, 2, 3])

intervals = range(1, 17)
diff = [v - w for v, w in zip(vanilla, wla_gc)]

print(f"{'Step, Vanilla, WLA-GC, diff'}")
for i in intervals:
    print(f"{i}, {vanilla[i-1]}, {wla_gc[i-1]}, {diff[i-1]}")
