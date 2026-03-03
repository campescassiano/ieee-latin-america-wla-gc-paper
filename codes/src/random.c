#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "random.h"

static bool rand_initialized = false;

// Function to get a random seed using RDTSCP
uint64_t init_random() {
    uint64_t aux;
    uint64_t seed;

    // RDTSCP returns the TSC value in EDX:EAX and the processor ID in ECX
    __asm__ volatile ("rdtscp" : "=a" (seed), "=d" (aux) :: "%ecx");
    seed = (aux << 32) | seed;

    srand(seed);

    rand_initialized = true;

    return seed;
}

uint64_t get_random(void)
{
  if (rand_initialized == false)
    init_random();

  return rand();

}

bool random_get_bool(int percentile)
{
  uint64_t val = get_random() % MAX_RAND_NUMBER;

  if (val < (uint64_t)(percentile * MAX_RAND_NUMBER / 100))
    return 1;
  else
    return 0;
}
