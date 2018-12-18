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

struct shipTelemetry* connection::getShipTelemetry() {

    krpc::services::SpaceCenter sc(&krpcConnection);

    shipTelemetry *st = new shipTelemetry;
    auto vessel = sc.active_vessel();
    auto orbit = vessel.orbit();
    //auto flight = vessel.flight();

    st->gForce = -1;
    st->altitude = -1;
    st->longitude = -1;
    st->latitude = -1;

    st->name = vessel.name();
    st->time = vessel.met();
    st->stageNum = -1;
    st->totalStages = -1;

    st->velocity = orbit.speed();
    st->apoapsis = orbit.apoapsis();
    st->periapsis = orbit.periapsis();
    st->eccentricity = orbit.eccentricity();
    st->inclination = orbit.inclination();

    return st;
}


