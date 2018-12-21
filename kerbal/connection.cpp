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
#include <chrono>



const std::string NAME = "Husk Intelligence " + VERSION_STR;

// TODO: Make configurable
const std::string ADDR = "127.0.0.1";
const uint RPC_PORT = 50000;
const uint STREAM_PORT = 50001;



int connection::tryConnection() {
    try {
        krpcConnection = krpc::connect(NAME, ADDR, RPC_PORT, STREAM_PORT);
        krpc::services::KRPC krpc(&krpcConnection);
        std::cout << krpc.get_status().version() << std::endl;
    } catch (std::system_error e) {
        std::cerr << "Unable to connect to krpc server." << std::endl;
        return 1;
    }

    return 0;
}

std::chrono::time_point lastTelemetryRead = std::chrono::high_resolution_clock::now();
shipTelemetry *lastShipTelemetry = new shipTelemetry;
const double MINUPDATEINTERVAL = 0.25;

struct shipTelemetry* connection::getShipTelemetry() {

    // Stop too often telemetry reads crashing the bot by getting errors in krpc.
    std::chrono::time_point now = std::chrono::high_resolution_clock::now();
    if ( std::chrono::duration<double> (now - lastTelemetryRead).count() < MINUPDATEINTERVAL) {
        return lastShipTelemetry;
    }


    krpc::services::SpaceCenter sc(&krpcConnection);
    lastShipTelemetry = new shipTelemetry;
    auto vessel = sc.active_vessel();
    auto orbit = vessel.orbit();
    //auto flight = vessel.flight();

    lastShipTelemetry->gForce = -1;
    lastShipTelemetry->altitude = -1;
    lastShipTelemetry->longitude = -1;
    lastShipTelemetry->latitude = -1;

    lastShipTelemetry->name = vessel.name();
    lastShipTelemetry->time = vessel.met();
    lastShipTelemetry->stageNum = -1;
    lastShipTelemetry->totalStages = -1;

    lastShipTelemetry->velocity = orbit.speed();
    lastShipTelemetry->apoapsis = orbit.apoapsis();
    lastShipTelemetry->periapsis = orbit.periapsis();
    lastShipTelemetry->eccentricity = orbit.eccentricity();
    lastShipTelemetry->inclination = orbit.inclination();
    lastTelemetryRead = std::chrono::high_resolution_clock::now();

    return lastShipTelemetry;
}


