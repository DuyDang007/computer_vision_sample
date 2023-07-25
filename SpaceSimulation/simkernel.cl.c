__kernel void calc_sim(__global double* posXin, __global double* posYin, __global double* posZin,
                        __global double* velXin, __global double* velYin, __global double* velZin, __global double* massin, __global double* massout,
                        __global double* posXout, __global double* posYout, __global double* posZout,
                        __global double* velXout, __global double* velYout, __global double* velZout,
                        __global bool* alive, __global double* temperature)//, __global double* radius)
{
    __constant double gravCoef = 0.5;
    __constant double interval = 0.1;
    size_t totalObj;
    totalObj = get_global_size(0);

    size_t id;
    id = get_global_id(0);

    double sqrDistance;
    double distance;
    double accX, accY, accZ; // Next frame acceleration
    double forceX, forceY, forceZ, force;
    forceX = 0.0;
    forceY = 0.0;
    forceZ = 0.0;

    size_t i;
    for(i = 0; i < totalObj; i++)
    {
        // Skip if the current object is on calculation or mass is zezo, or object is not alive
        if (i == id || massin[i] == 0.0 || alive[i] == 0) continue;

        sqrDistance = pow(posXin[i] - posXin[id], 2) + pow(posYin[i] - posYin[id], 2) +pow(posZin[i] - posZin[id], 2);
        if (sqrDistance == 0.0)
            continue;

        // Calculate force magnitude
        force = gravCoef * massin[i] * massin[id] / sqrDistance;

        // Force by each axis
        distance = sqrt(sqrDistance);
        forceX += force * (posXin[i] - posXin[id]) / distance;
        forceY += force * (posYin[i] - posYin[id]) / distance;
        forceZ += force * (posZin[i] - posZin[id]) / distance;
        // If distance is small, merge 2 objects

        if (distance < 100 && id < i)
        {
            printf("!");
            // Calculate collision force
            velXin[id] = (massin[id] * velXin[id] + massin[i] * velXin[i]) / (massin[id] + massin[i]);
            velYin[id] = (massin[id] * velYin[id] + massin[i] * velYin[i]) / (massin[id] + massin[i]);
            velZin[id] = (massin[id] * velZin[id] + massin[i] * velZin[i]) / (massin[id] + massin[i]);
            massout[id] = massin[id] + massin[i];
            temperature[id] += massout[id] / 1000000.0; // Just a silly value to show that temperature is higher on heavy objects
            massout[i] = 0.0;
            massin[i] = 0.0;
            alive[i] = 0;
        }
    }
    //printf("%f  %f  %f\n", forceX, forceY, forceZ);

    // Calculate acceleration
    accX = forceX / massin[id];
    accY = forceY / massin[id];
    accZ = forceZ / massin[id];

    // Calculate next interval velocity and position
    velXout[id] = velXin[id] + accX * interval;
    velYout[id] = velYin[id] + accY * interval;
    velZout[id] = velZin[id] + accZ * interval;

    posXout[id] = posXin[id] + velXin[id] * interval + 0.5 * accX * interval * interval;
    posYout[id] = posYin[id] + velYin[id] * interval + 0.5 * accY * interval * interval;
    posZout[id] = posZin[id] + velZin[id] * interval + 0.5 * accZ * interval * interval;
}