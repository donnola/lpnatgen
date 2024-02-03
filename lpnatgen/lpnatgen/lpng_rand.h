#pragma once

static unsigned int lpng_seed;

// Used to seed the generator.           
void fast_lpng_rand(int seed);

// Compute a pseudorandom integer.
// Output value in range [0, 32767]
unsigned int fast_lpng_rand();

unsigned int get_lpng_seed();