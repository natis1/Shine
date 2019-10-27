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

#include <krpc.hpp>
#include <krpc/client.hpp>
#include <krpc/services/krpc.hpp>
#include <krpc/services/space_center.hpp>


class connection {
public:
    connection();
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

    double shipTelemetry_timeToApoapsis;
    double shipTelemetry_apoapsisAltitude;

    // Relative to planet
    double shipTelemetry_phi;
    double shipTelemetry_theta;

    double getUniversalTime();

    std::map<std::string, std::tuple<double, double>> shipTelemetry_resources;

    void getShipTelemetry();
    void getShipAngles();


    krpc::Client *getKrpcConnection();
    void resetTelemetry();
    bool testConnection();

    void aimVessel(double theta, double phi);
    void setAutopilot(bool enable);
    void setVesselThrust(double thrust);
    void circularize();
    double calculateNodeBurnTime();
    void pointTowardsNode();
    void warpTo(double time);
    double getRemainingNodeBurn();
    void deleteNode(int nodeNum);


    bool didReset = true;
    
private:
    
    krpc::services::SpaceCenter::Vessel vessel;
    krpc::services::SpaceCenter *sc;
    krpc::Client krpcConnection;
    //krpc::Stream<double> ut;
};


#endif //SHINE_CONNECTION_H
