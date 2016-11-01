#include "quadtree.h"
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "mkl.h"

void computeGradient(float* Y, int nbodies, float* force, float theta, float* vel, float dt)
{
	int D = 2; // Two dimensions since QuadTree
	QuadTree* tree = new QuadTree(Y, nbodies);
	float sum_Q = .0;

	for (int body_i = 0; body_i < nbodies; body_i++) {
		float* f = force + body_i * D;
		f[0] = f[1] = 0;
		sum_Q += tree->computeNonEdgeForces(body_i, theta, f);
	}

#pragma vector aligned
	for (int body_i = 0; body_i < nbodies * D; body_i++) {
		vel[body_i] += (force[body_i] / sum_Q) * dt ;
	}

	delete tree;
}

void randomizeBodies(float *data, int n) {
	for (int i = 0; i < n; i++) {
		data[i] = 2.0f * (rand() / (float)RAND_MAX) - 1.0f;
	}
}

int main(const int argc, const char** argv) {
	int nBodies = 32768;
	float theta = 0.2;
	if (argc > 1) nBodies = atoi(argv[1]);

	const float dt = 0.01f; // time step
	const int nIters = 20;  // simulation iterations
	const int D = 2;

	int bytes = nBodies * sizeof(float) * D;
	float *pos_buf = (float*)mkl_malloc(bytes, 64);
	float *vel_buf = (float*)mkl_malloc(bytes, 64);

	randomizeBodies(pos_buf, D * nBodies); // Init pos / vel data
	randomizeBodies(vel_buf, D * nBodies);

	double totalTime = 0.0;
	float* force = (float*)mkl_malloc(nBodies * D * sizeof(float), 64);

	for (int iter = 1; iter <= nIters; iter++) {
		StartTimer();
		computeGradient(pos_buf, nBodies, force, theta, vel_buf, dt);

#pragma vector aligned
		for (int i = 0; i < nBodies * D; i++) { // integrate position
			pos_buf[i] += vel_buf[i] * dt;
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
	mkl_free(force);
	mkl_free(pos_buf);
	mkl_free(vel_buf);
	return 0;
}