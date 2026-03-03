#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

#include "random.h"
#include "ssd.h"

int N_BATCHES = 100;
uint64_t NUM_BLOCKS = 10000;
int PERCENTILE_VALID = 20;

typedef struct params_t {
  uint64_t wearout;
  block_t *blocks;
  uint64_t latency;
} params_t;

void parse_arguments(int argc, char **argv)
{
  for (int i = 0; i < argc; i++)
    {
      if (!strcmp("--n_batches", argv[i]))
          N_BATCHES = strtoul(argv[++i], NULL, 10);
      else if (!strcmp("--num_blocks", argv[i]))
          NUM_BLOCKS = strtoul(argv[++i], NULL, 10);
      else if (!strcmp("--percentile_valid", argv[i]))
          PERCENTILE_VALID = strtoul(argv[++i], NULL, 10);
    }
}

void set_wl_wearout(block_t *block, wl_t *wl, uint64_t wearout)
{
  if (block == NULL || wl == NULL)
    return;

  wl->wearout += wearout;
  block->wearout += wearout;
}

void init_blocks(block_t *blocks, int n_blocks)
{
  if (blocks == NULL)
    return;

  for (int i = 0; i < n_blocks; i++)
    {
      blocks[i].n_blocks = n_blocks;
      for (int j = 0; j < NUM_WL_PER_BLOCK; j++)
        {
          for (int k = 0; k < WL_WIDTH; k++)
            {
              for (int l = 0; l < CELL_TYPE; l++)
                {
                  blocks[i].wl[j].page[k][l] = random_get_bool(PERCENTILE_VALID);

                  blocks[i].invalid += blocks[i].wl[j].page[k][l] ? 1 : 0;
                  blocks[i].weight += blocks[i].wl[j].page[k][l] ? cell_read_weight[l] : 0;
                }
            }
        }
    }
}

void* run_gc_vanilla(void *args)
{
  params_t *params = (params_t*) args;
  block_t *blocks = params->blocks;
  uint64_t latency = 0;
  uint32_t n_selected = 0;
  uint32_t should_select = blocks->n_blocks/4;
  block_t *selected = calloc(blocks->n_blocks/4, sizeof(*selected));
  if (selected == NULL)
    {
      printf("Error allocating memory at %s:%d\n", __func__, __LINE__);
      return 0;
    }

  for (int i = 0; i < blocks->n_blocks && n_selected < should_select; i++)
    {
    if (random_get_bool(50))
      {
        memcpy(&selected[n_selected], &blocks[i], sizeof(block_t));
        n_selected += 1;
      }
    }

  for (int i = 0; i < should_select; i++)
    {
      for (int j = 0; j < NUM_WL_PER_BLOCK; j++)
        {
          bool invalid_pages = 0;

          for (int k = 0; k < WL_WIDTH; k++)
            {
              for (int l = 0; l < CELL_TYPE; l++)
                {
                  if (blocks[i].wl[j].page[k][l])
                    {
                      params->latency += cell_read_weight[l];
                      params->latency += CELL_WRITE_WEIGHT;
                      invalid_pages |= (1 << j);
                    }
                }
            }

          /* if we have any invalid page on this WL, we then go check the wearout */
          if (invalid_pages)
            {
              for (int m = CELL_TYPE; m >= 0; m--)
                {
                  /* we find the higher page level, that causes the most wearout.
                   * once we find it, we stop checking the other pages on the same
                   * WL because they would be erased already by the hight level
                   * of wearout */
                  if (invalid_pages & (1 << m))
                    {
                      set_wl_wearout(&blocks[i], &blocks[i].wl[j], wearout_values[m]);
                      params->wearout += wearout_values[m];
                      break;
                    }
                }
            }
        }
      params->latency += BLOCK_ERASE;
    }

  free(selected);

  return NULL;
}

void* run_gc_wla(void *args)
{
  params_t *params = (params_t*) args;
  uint32_t n_selected = 0;
  block_t *blocks = params->blocks;
  uint32_t should_select = blocks->n_blocks/4;
  block_t *selected = calloc(blocks->n_blocks/4, sizeof(*selected));
  if (selected == NULL)
    {
      printf("Error allocating memory at %s:%d\n", __func__, __LINE__);
      return 0;
    }

  block_t *temp = &blocks[0];

  while (n_selected < should_select)
    {
      for (int i = 0; i < blocks->n_blocks && n_selected < should_select; i++)
        {
          if (blocks[i].selected == false)
            {
              if (blocks[i].weight < temp->weight)
                {
                  temp = &blocks[i];
                }
            }
        }

      memcpy(&selected[n_selected++], temp, sizeof(block_t));
      temp->selected = true;
    }

  for (int i = 0; i < n_selected; i++)
    {
      for (int j = 0; j < NUM_WL_PER_BLOCK; j++)
        {
          bool invalid_pages = 0;

          for (int k = 0; k < WL_WIDTH; k++)
            {
              for (int l = 0; l < CELL_TYPE; l++)
                {
                  if (selected[i].wl[j].page[k][l])
                    {
                      params->latency += cell_read_weight[l];
                      params->latency += CELL_WRITE_WEIGHT;
                      invalid_pages |= ( 1 << j);
                    }
                }
            }

          /* if we have any invalid page on this WL, we then go check the wearout */
          if (invalid_pages)
            {
              for (int m = CELL_TYPE; m >= 0; m--)
                {
                  /* we find the higher page level, that causes the most wearout.
                   * once we find it, we stop checking the other pages on the same
                   * WL because they would be erased already by the hight level
                   * of wearout */
                  if (invalid_pages & (1 << m))
                    {
                      set_wl_wearout(&blocks[i], &blocks[i].wl[j], wearout_values[m]);
                      params->wearout += wearout_values[m];
                      break;
                    }
                }
            }
        }
      params->latency += BLOCK_ERASE;
    }

  free(selected);

  return NULL;
}

int main(int argc, char **argv)
{
    int batch = 0;
    uint64_t vanilla = 0;
    uint64_t wla = 0;

    pthread_t t1, t2;
    params_t params[2] = {{.blocks=NULL,.latency=0},{.blocks=NULL,.latency=0}};

    cpu_set_t cpu1, cpu2;

    parse_arguments(argc, argv);
    printf("%d, ", PERCENTILE_VALID);
    printf("%" PRIu64 ", ", (unsigned long int)N_BATCHES);
    printf("%" PRIu64 ", ", (unsigned long int)NUM_BLOCKS);
    printf("%" PRIu64 ", ", (unsigned long int)NUM_PAGES_PER_BLOCK);

    while (batch++ < N_BATCHES)
      {
        block_t *blocks = calloc(NUM_BLOCKS, sizeof(*blocks));

        if (blocks == NULL)
        {
          printf("Could not allocate memory\n");
          return -1;
        }
        init_blocks(&blocks[0], NUM_BLOCKS);
        params[0].blocks = params[1].blocks = blocks;

        pthread_create(&t1, NULL, run_gc_vanilla, (void*) &params[0]);
        pthread_create(&t2, NULL, run_gc_wla, (void*) &params[1]);

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);

        vanilla += params[0].latency;
        wla += params[1].latency;

        free(blocks);
      }

    vanilla = vanilla / N_BATCHES;
    wla = wla / N_BATCHES;
    params[0].wearout = params[0].wearout / N_BATCHES;
    params[1].wearout = params[1].wearout / N_BATCHES;

    printf("%" PRIu64 ", ", vanilla);
    printf("%" PRIu64 ", ", params[0].wearout);
    printf("%" PRIu64 ", ", wla);
    printf("%" PRIu64 ", ", params[1].wearout);
    printf("%.3f, ", (double)(100 - (double)(100 * wla) / vanilla));
    printf("%.3f\n", (double)(100 - (double)(100 * params[1].wearout) / params[0].wearout));

    return 0;
}
