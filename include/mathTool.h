#ifndef MATHTOOL_H
#define MATHTOOL_H

#include <stdbool.h>

#define M_PI 3.14159265358979323846
#define DEG2RAD(deg) ((deg) * M_PI / 180.0)
#define RAD2DEG(rad) ((rad) * 180.0 / M_PI)
#define TOLERANCE_DEGREES 5.0  // Angle de tolérance en degrés

typedef struct {
    double x, y, z;
} Vec3;

typedef struct 
{
   double x,y 
}Vec2;


// Fonctions
Vec3 normalize(Vec3 v);
double dot(Vec3 a, Vec3 b);
Vec3 getDirectionVector(double yawDeg, double pitchDeg);
bool isLookingAt(Vec3 playerPos, double yawDeg, double pitchDeg, Vec3 targetPos);
Vec2 getYawPitch(Vec3 playerPos, Vec3 targetPos);

#endif