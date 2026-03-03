#include <stdint.h>

#define MAX_RAND_NUMBER   1000 ///< Maximum random number generator range

uint64_t init_random(void);
uint64_t get_random(void);
bool random_get_bool(int percentile);

