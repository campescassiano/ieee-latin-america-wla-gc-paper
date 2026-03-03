#include "ssd.h"
#include <stdbool.h>
#include <stdio.h>

void print_blocks(block_t *blocks)
{
  if (blocks == NULL)
    return;

  for (int a = 0; a < blocks->n_blocks; a++)
    {
      printf("BLOCK[%d] --> invalid: %d, weight: %d\n", a, blocks[a].invalid, blocks[a].weight);
      continue;
      for (int i = 0; i < NUM_WL_PER_BLOCK; i++)
        {
          printf("wl[%d]=\t\t", i);
          for (int j = 0; j < CELL_TYPE; j++)
            {
              if (!j)
                printf("|");
              else
                printf("\t\t|");

              for (int k = 0; k < WL_WIDTH; k++)
                printf("%d", blocks[a].wl[i].page[j][k]);
              printf("|\n");
            }
          printf("\n");
        }
    }
}
