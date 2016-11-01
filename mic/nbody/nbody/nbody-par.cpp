#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "mkl.h"

#define SOFTENING 1e-9f

typedef struct { float *x, *y, *z, *vx, *vy, *vz; } BodySystem;

void randomizeBodies_seq_opt(float *data, int n) {
	for (int i = 0; i < n; i++) {
		data[i] = 2.0f * (rand() / (float)RAND_MAX) - 1.0f;
	}
}

void bodyForce_seq_opt(BodySystem p, float dt, int n, int tileSize) {

#pragma omp parallel for schedule(static) num_threads(4)
	for (int tile = 0; tile < n; tile += tileSize) {
		int to = tile + tileSize;
		if (to > n) to = n;
		for (int i = 0; i < n; i++) {
			float Fx = 0.0f; float Fy = 0.0f; float Fz = 0.0f;
#pragma vector aligned
			for (int j = tile; j < to; j++) {
				float dy = p.y[j] - p.y[i];
				float dz = p.z[j] - p.z[i];
				float dx = p.x[j] - p.x[i];
				float distSqr = dx*dx + dy*dy + dz*dz + SOFTENING;
				float invDist = 1.0f / sqrtf(distSqr);
				float invDist3 = invDist * invDist * invDist;
				Fx += dx * invDist3; Fy += dy * invDist3; Fz += dz * invDist3;
			}

			p.vx[i] += dt*Fx; p.vy[i] += dt*Fy; p.vz[i] += dt*Fz;
		}
	}
}

int main(const int argc, const char** argv) {

	int nBodies = 32768;
	if (argc > 1) nBodies = atoi(argv[1]);

	int tileSize = 2048;
	if (tileSize > nBodies) tileSize = nBodies;

	const float dt = 0.01f; // time step
	const int nIters = 20;  // simulation iterations

	int bytes = nBodies*sizeof(BodySystem);
	float *buf = (float*)mkl_malloc(bytes, 64);
	BodySystem p;
	p.x = buf + 0 * nBodies; p.y = buf + 1 * nBodies; p.z = buf + 2 * nBodies;
	p.vx = buf + 3 * nBodies; p.vy = buf + 4 * nBodies; p.vz = buf + 5 * nBodies;

	randomizeBodies_seq_opt(buf, 6 * nBodies); // Init pos / vel data

	double totalTime = 0.0;

	for (int iter = 1; iter <= nIters; iter++) {
		StartTimer();

		bodyForce_seq_opt(p, dt, nBodies, tileSize); // compute interbody forces

#pragma ivdep
#pragma vector aligned
		for (int i = 0; i < nBodies; i++) { // integrate position
			p.x[i] += p.vx[i] * dt;
			p.y[i] += p.vy[i] * dt;
			p.z[i] += p.vz[i] * dt;
		}

		const double tElapsed = GetTimer() / 1000.0;
		if (iter > 1) { // First iter is warm up
			totalTime += tElapsed;
		}
#ifndef SHMOO
		printf("Iteration %d: %.3f seconds\n", iter, tElapsed);
#endif
	}
	double avgTime = totalTime / (double)(nIters - 1);

	//#ifdef SHMOO
	//	printf("%d, %0.3f\n", nBodies, 1e-9 * nBodies * nBodies / avgTime);
	//#else
	//	printf("Average rate for iterations 2 through %d: %.3f +- %.3f steps per second.\n",nIters, rate);
	//	printf("%d Bodies: average %0.3f Billion Interactions / second\n", nBodies, 1e-9 * nBodies * nBodies / avgTime);
	//#endif
	printf("%d Bodies: average %0.3f Billion Interactions / second\n", nBodies, 1e-9 * nBodies * nBodies / avgTime);
	mkl_free(buf);
}