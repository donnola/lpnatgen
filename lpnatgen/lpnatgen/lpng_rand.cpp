#include "lpng_rand.h"


void fast_lpng_rand(int seed)
{
  lpng_seed = seed;
}

int fast_lpng_rand()
{
  lpng_seed = (214013 * lpng_seed + 2531011);
  return (lpng_seed >> 16) & 0x7FFF;
}

int fast_lpng_rand(int a, int b)
{
  int r = fast_lpng_rand();
  return r % (b - a) + a;
}

int get_lpng_seed()
{
  return lpng_seed;
}