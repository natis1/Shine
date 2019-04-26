/*
 * Created by Konthi.
 *
 * Copyright (C) 2018 Solaris Aerospace
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SHINE_CONNECTION_H
#define SHINE_CONNECTION_H

#include <krpc/client.hpp>

class connection {
public:
    int tryConnection();

    // vessel data
    std::string shipTelemetry_name;
    std::string shipTelemetry_realTime;
    double shipTelemetry_time;
    int shipTelemetry_stageNum;
    int shipTelemetry_totalStages;

    // flight data
    float shipTelemetry_gForce;
    double shipTelemetry_altitude;
    double shipTelemetry_height;
    double shipTelemetry_radius;
    double shipTelemetry_latitude;
    double shipTelemetry_longitude;

    // orbit data
    double shipTelemetry_velocity; // With respect to the current ref frame.
    double shipTelemetry_apoapsis;
    double shipTelemetry_periapsis;
    double shipTelemetry_eccentricity;
    double shipTelemetry_inclination;

    // Relative to planet
    double shipTelemetry_phi;
    double shipTelemetry_theta;

    std::map<std::string, std::tuple<double, double>> shipTelemetry_resources;

    void getShipTelemetry();
    void getShipAngles();


    krpc::Client krpcConnection;
    void resetTelemetry();
    bool testConnection();

    bool didReset = true;
};


#endif //SHINE_CONNECTION_H
