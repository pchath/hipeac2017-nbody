
typedef struct { float x, y, z, vx, vy, vz; } Body;

void randomizeBodies(float *data, int n);
void bodyForce(Body *p, float dt, int n);
