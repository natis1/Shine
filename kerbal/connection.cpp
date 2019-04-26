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

#include "connection.h"
#include <krpc.hpp>
#include <krpc/services/krpc.hpp>
#include <krpc/services/space_center.hpp>
#include <iostream>
#include <ctime>
#include <iomanip>


std::string NAME() {
    return "Husk Intelligence 0.0.1";
}

// TODO: Make configurable
const char* ADDR = "127.0.0.1";
const uint RPC_PORT = 50000;
const uint STREAM_PORT = 50001;



int connection::tryConnection() {
    try {
        krpcConnection = krpc::connect(NAME(), ADDR, RPC_PORT, STREAM_PORT);
        krpc::services::KRPC krpc(&krpcConnection);
        std::cout << krpc.get_status().version() << std::endl;
    } catch (const std::system_error& e) {
        std::cerr << "Unable to connect to krpc server." << std::endl;
        return 1;
    }

    return 0;
}



void connection::getShipTelemetry() {

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    shipTelemetry_realTime = oss.str();

        krpc::services::SpaceCenter sc(&krpcConnection);
        auto vessel = sc.active_vessel();
        auto orbit = vessel.orbit();

        //auto flight = vessel.flight();

        shipTelemetry_gForce = -1;
        shipTelemetry_altitude = -1;
        shipTelemetry_longitude = -1;
        shipTelemetry_latitude = -1;

        shipTelemetry_name = vessel.name();
        shipTelemetry_time = vessel.met();
        shipTelemetry_stageNum = -1;
        shipTelemetry_totalStages = -1;

        shipTelemetry_velocity = orbit.speed();
        shipTelemetry_apoapsis = orbit.apoapsis();
        shipTelemetry_periapsis = orbit.periapsis();
        shipTelemetry_eccentricity = orbit.eccentricity();
        shipTelemetry_inclination = orbit.inclination();

}

void connection::resetTelemetry() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    shipTelemetry_name = "No vessel loaded. Please wait";
    std::string s = oss.str();
    shipTelemetry_realTime = s.c_str();
    shipTelemetry_gForce = -1;
    shipTelemetry_altitude = -1;
    shipTelemetry_longitude = -1;
    shipTelemetry_latitude = -1;

    shipTelemetry_time = -1;
    shipTelemetry_stageNum = -1;
    shipTelemetry_totalStages = -1;

    shipTelemetry_velocity = -1;
    shipTelemetry_apoapsis = -1;
    shipTelemetry_periapsis = -1;
    shipTelemetry_eccentricity = -1;
    shipTelemetry_inclination = -1;
}


