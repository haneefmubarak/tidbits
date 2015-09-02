#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <stdio.h>

// flags: -xc -O[n] -ftree-vectorize -std=gnu11 -fverbose-asm -march=haswell -mfpmath=both

void l (const float *restrict in,
		const float *restrict *restrict mul,
		float *restrict out,
		uint16_t m,
		uint16_t n) {
	int x, y;
	for (x = 0; x < n; x++) {
		out[x] = 0;

		for (y = 0; y < m; y++) {
			out[x] += in[y] * mul[x][y];
		}
	}
}

typedef float v8fp __attribute__ ((vector_size (32)));

void vl (const float *restrict in,
		const float *restrict *restrict mul,
		float *restrict out,
		uint16_t m,
		uint16_t n) {
	int i = n / 8;
	int j = n % 8;

	int x, y, z;
	for (x = 0; x < i; x += 8) {
		v8fp vo;
		for (y = 0; y < 8; y++)
			vo[y] = 0;

		for (y = 0; y < m; y++) {
			v8fp vi, vm;
			for (z = 0; z < 8; z++) {
				vi[z] = in[y];
				vm[z] = mul[x + z][y];
			}

			vo += vi * vm;
		}

		for (y = 0; y < 8; y++)
			out[x + y] = vo[y];
	}

	return l (in, mul + j, out + j, m, j);
}
