//
// Created by eli on 12/17/18.
//

#ifndef SHINE_GLOBALS_H
#define SHINE_GLOBALS_H

#include <string>

const int VERSION_INT = 1;
const std::string VERSION_STR = "0.0.1";

struct shipTelemetry {
public:
    // vessel data
    std::string name;
    double time;
    int stageNum;
    int totalStages;

    // flight data
    float gForce;
    double altitude;
    double latitude;
    double longitude;

    // orbit data
    double velocity; // With respect to the current ref frame.
    double apoapsis;
    double periapsis;
    double eccentricity;
    double inclination;


};


#endif //SHINE_GLOBALS_H
