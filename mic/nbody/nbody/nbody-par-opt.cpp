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

void bodyForce_seq_opt(BodySystem* bodies, float dt, int n, int tileSize) {

	int nTiles = n / tileSize;
#pragma omp parallel for schedule(static) num_threads(4)
	for (int tile = 0; tile < nTiles; tile += 1) {
		BodySystem p = bodies[tile];
		for (int i = 0; i < n; i++) {
			float Fx = 0.0f; float Fy = 0.0f; float Fz = 0.0f;
#pragma vector aligned
			for (int j = tile; j < tile + tileSize; j++) {
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

	BodySystem p = bodies[0];
	for (int i = 0; i < n; i++) {
		for (int tile = 1; tile < nTiles; tile++) {
			BodySystem q = bodies[tile];
			p.vx[i] += q.vx[i];
			p.vy[i] += q.vy[i];
			p.vz[i] += q.vz[i];
		}
	}
}

int main(const int argc, const char** argv) {

	int nBodies = 32768;
	if (argc > 1) nBodies = atoi(argv[1]);

	int tileSize = 512;
	if (tileSize > nBodies) tileSize = nBodies;
	int nTiles = nBodies / tileSize;

	const float dt = 0.01f; // time step
	const int nIters = 20;  // simulation iterations

	int offset = nBodies;
	BodySystem* bodies = (BodySystem*)mkl_malloc(sizeof(BodySystem)*nTiles, 64);
	for (int i = 0; i < nTiles; i++) {
		int bytes = nBodies*sizeof(BodySystem);
		float* buf = (float*)mkl_malloc(bytes, 64);
		randomizeBodies_seq_opt(buf, 6 * nBodies); // Init pos / vel data
		bodies[i].x = buf + 0 * offset; bodies[i].y = buf + 1 * offset; bodies[i].z = buf + 2 * offset;
		bodies[i].vx = buf + 3 * offset; bodies[i].vy = buf + 4 * offset; bodies[i].vz = buf + 5 * offset;
	}


	double totalTime = 0.0;

	for (int iter = 1; iter <= nIters; iter++) {
		StartTimer();

		bodyForce_seq_opt(bodies, dt, nBodies, tileSize); // compute interbody forces
		BodySystem p = bodies[0];

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
	for (int i = 0; i < nTiles; i++) {
		mkl_free(bodies[i].x);
	}
}