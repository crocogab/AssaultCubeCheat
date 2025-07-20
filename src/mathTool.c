#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "mathTool.h"





// Normalise un vecteur
Vec3 normalize(Vec3 v) {
    double length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0) return (Vec3){0, 0, 0};
    return (Vec3){v.x / length, v.y / length, v.z / length};
}

// Produit scalaire
double dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 getDirectionVector(double yawDeg, double pitchDeg) {
    Vec3 direction;
    
    
    double yawRad = DEG2RAD(yawDeg);
    double pitchRad = DEG2RAD(pitchDeg);
    
    
    direction.x = cos(pitchRad) * sin(yawRad);     // Composante X (droite/gauche)
    direction.y = -cos(pitchRad) * cos(yawRad);     // Composante Y (avant/arrière)
    direction.z = sin(pitchRad);                  // Composante 
    
    
    return normalize(direction);
}

/*Renvoie un Vec2 constitué du yaw et du pitch afin de viser exactement depuis playerPos la position targetPos*/
Vec2 getYawPitch(Vec3 playerPos, Vec3 targetPos){
    // Calculer le vecteur de direction vers la cible
    Vec3 direction = {
        targetPos.x - playerPos.x,
        targetPos.y - playerPos.y,
        targetPos.z - playerPos.z
    };
    
    // Calculer la distance horizontale (dans le plan XY)
    double horizontalDistance = sqrt(direction.x * direction.x + direction.y * direction.y);
    
    // Calculer le yaw (rotation horizontale) en degrés
    double yaw = atan2(direction.x, -direction.y) * 180.0 / M_PI;
    
    // Calculer le pitch (rotation verticale) en degrés
    double pitch = atan2(direction.z, horizontalDistance) * 180.0 / M_PI;
    
    // Normaliser le yaw entre -180 et 180 degrés
    while (yaw > 180.0) yaw -= 360.0;
    while (yaw < -180.0) yaw += 360.0;
    
    // Le pitch est généralement limité entre -90 et +90 degrés
    if (pitch > 90.0) pitch = 90.0;
    if (pitch < -90.0) pitch = -90.0;
    
    Vec2 result = {yaw, pitch};
    return result;
}

// Vérifie si le joueur regarde vers une cible
bool isLookingAt(Vec3 playerPos, double yawDeg, double pitchDeg, Vec3 targetPos) {
    Vec3 dirToTarget = normalize((Vec3){
        targetPos.x - playerPos.x,
        targetPos.y - playerPos.y,
        targetPos.z - playerPos.z
    });

    Vec3 viewDir = getDirectionVector(yawDeg, pitchDeg);

    double cosAngle = dot(dirToTarget, viewDir);
    double angleDeg = acos(cosAngle) * 180.0 / M_PI;
    
    return angleDeg <= TOLERANCE_DEGREES;
}
