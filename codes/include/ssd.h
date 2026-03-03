#ifndef __SSD_H__
#define __SSD_H__

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Cell types supported
 */
typedef enum cell_type_t {
    SLC = 1,  ///< Single-Level Cell
    MLC,      ///< Multi-Level Cell
    TLC,      ///< Triple-Level Cell
    QLC       ///< Quad-Level Cell
} cell_type_t;

#define WL_WIDTH            4   ///< Wordline width
#define CELL_TYPE           QLC ///< Cell type
#define NUM_PAGES_PER_BLOCK 4096 ///< Logical number of pages per block
                                  //
#define NUM_WL_PER_BLOCK    (NUM_PAGES_PER_BLOCK/WL_WIDTH/CELL_TYPE)  ///< Wordlines per block (hardware level)
#define BLOCK_ERASE         9
#define CELL_WRITE_WEIGHT   2

/**
 * @brief Indicates the possible wearout of the page position in a WL
 */
static const uint64_t wearout_values[CELL_TYPE] = { 1, 2, 3, 4 };

typedef struct wl_t {
    bool page[WL_WIDTH][CELL_TYPE];
    uint64_t wearout;
} wl_t;

typedef struct block_t {
    wl_t wl[NUM_WL_PER_BLOCK];
    int invalid;
    int weight;
    bool selected;
    int n_blocks;
    uint64_t wearout;
} block_t;

typedef enum cell_read_weight_e {
  RD_WEIGHT_SLC = 1,
  RD_WEIGHT_MLC,
  RD_WEIGHT_TLC,
  RD_WEIGHT_QLC
} cell_read_weight_e;

extern int cell_read_weight[];

#endif // __SSD_H__
