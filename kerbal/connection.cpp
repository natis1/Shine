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
#include <iostream>
#include <ctime>
#include <iomanip>
#include "math.h"
#include <cmath>


std::string NAME() {
    return "Husk Intelligence 0.0.1";
}

// TODO: Make configurable
const char* ADDR = "127.0.0.1";
const uint RPC_PORT = 50000;
const uint STREAM_PORT = 50001;



connection::connection() {

}

int connection::tryConnection() {
    try {
        krpcConnection = krpc::connect(NAME(), ADDR, RPC_PORT, STREAM_PORT);
        sc = new krpc::services::SpaceCenter(&krpcConnection);
        vessel = sc->active_vessel();
    } catch (krpc::RPCError) {
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

    auto orbit = vessel.orbit();
    //auto flight = vessel.flight();

    shipTelemetry_gForce = -1;
    shipTelemetry_longitude = -1;
    shipTelemetry_latitude = -1;

    shipTelemetry_name = vessel.name();
    shipTelemetry_time = vessel.met();
    shipTelemetry_stageNum = -1;
    shipTelemetry_totalStages = -1;

    shipTelemetry_radius = orbit.radius();
    shipTelemetry_velocity = orbit.speed();
    shipTelemetry_apoapsis = orbit.apoapsis();
    shipTelemetry_periapsis = orbit.periapsis();
    shipTelemetry_eccentricity = orbit.eccentricity();
    shipTelemetry_inclination = orbit.inclination();
    shipTelemetry_timeToApoapsis = orbit.time_to_apoapsis();

    shipTelemetry_height = shine_math::magnitude(vessel.position(orbit.body().reference_frame()));

    shipTelemetry_apoapsisAltitude = orbit.apoapsis_altitude();
    shipTelemetry_altitude = shipTelemetry_height - (shipTelemetry_apoapsis - shipTelemetry_apoapsisAltitude);

    vessel.position(vessel.orbit().body().reference_frame());

    std::tuple<double, double, double> bodyDirectionVec = vessel.direction(vessel.surface_reference_frame());

    auto res = vessel.resources();

    if (didReset) {
        for (ulong i = 0; i < res.all().size(); i++) {
            std::string s = res.all().at(i).name();
            if ( res.has_resource(s) ) {
                shipTelemetry_resources.insert(std::pair<std::string, std::tuple<double, double>>(s,
                                                                                                  std::make_tuple(res.amount(s), res.max(s))) );
            }
        }

        didReset = false;
    } else {
        for (auto const& [key, val] : shipTelemetry_resources) {
            shipTelemetry_resources[key] = std::make_tuple(res.amount(key), res.max(key));
        }
    }



    //std::tuple<double, double, double> surfaceDirectionVec = vessel.direction(vessel.surface_reference_frame());

    // x = r cos (phi). r is 1 so acos(y) =  theta
    shipTelemetry_phi = acos(std::get<0>(bodyDirectionVec));

    // z = sin (theta) * cos (phi). acos ( z / sin(theta) ) = phi
    shipTelemetry_theta = acos(std::get<1>(bodyDirectionVec) / sin(shipTelemetry_phi));

    shipTelemetry_theta *= 180 / M_PIl;
    shipTelemetry_phi *= 180 / M_PIl;



    auto ref =  vessel.surface_velocity_reference_frame();

}

void connection::resetTelemetry() {
    didReset = true;
    shipTelemetry_resources.clear();
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    shipTelemetry_name = "No vessel loaded. Please wait";
    shipTelemetry_realTime = oss.str();
    shipTelemetry_gForce = -1;
    shipTelemetry_altitude = -1;
    shipTelemetry_height = -1;
    shipTelemetry_longitude = -1;
    shipTelemetry_latitude = -1;

    shipTelemetry_time = -1;
    shipTelemetry_stageNum = -1;
    shipTelemetry_totalStages = -1;

    shipTelemetry_radius = -1;
    shipTelemetry_velocity = -1;
    shipTelemetry_apoapsis = -1;
    shipTelemetry_periapsis = -1;
    shipTelemetry_eccentricity = -1;
    shipTelemetry_inclination = -1;
    shipTelemetry_timeToApoapsis = -1;
    shipTelemetry_apoapsisAltitude = -1;


    shipTelemetry_theta = -1;
    shipTelemetry_phi = -1;
}

bool connection::testConnection() {
    try {
        krpc::services::SpaceCenter sc(&krpcConnection);
        sc.reputation();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void connection::aimVessel(double theta, double phi) {
    if (theta < 0.0) {
        theta = (2.0 * M_PI) + theta;
    }
    double t_actual = theta * 180 / M_PI;
    double p_actual = 90.0 - (phi * 180.0 / M_PI);
    vessel.auto_pilot().target_pitch_and_heading(p_actual, t_actual);
}

void connection::setAutopilot(bool enable) {
    if (enable) {
        vessel.auto_pilot().engage();
    } else {
        vessel.auto_pilot().disengage();
    }

}

void connection::setVesselThrust(double thrust) {
    vessel.control().set_throttle(thrust);
}

void connection::circularize() {
    double mu = vessel.orbit().body().gravitational_parameter();
    double r = vessel.orbit().apoapsis();
    double a1 = vessel.orbit().semi_major_axis();
    double a2 = r;
    double v1 = std::sqrt(mu * ((2.0 / r) - (1.0 / a1)));
    double v2 = std::sqrt(mu * ((2.0 / r) - (1.0 / a2)));
    double delta_v = v2 - v1;
    auto node = vessel.control().add_node(
        sc->ut() + vessel.orbit().time_to_apoapsis(), delta_v);
}

double connection::calculateNodeBurnTime() {
    // Calculate burn time (using rocket equation)
    if (vessel.control().nodes().size() == 0) {
        return 0;
    }
    double delta_v = vessel.control().nodes().at(0).delta_v();
    double F = vessel.available_thrust();
    double Isp = vessel.specific_impulse() * 9.82;
    double m0 = vessel.mass();
    double m1 = m0 / std::exp(delta_v / Isp);
    double flowRate = F / Isp;
    return (m0 - m1) / flowRate;
}

double connection::getUniversalTime() {
    return sc->ut();
}

void connection::pointTowardsNode() {
    if (vessel.control().nodes().size() == 0) {
        return;
    }
//     auto a = vessel.control().nodes().at(0).remaining_burn_vector(vessel.surface_reference_frame());
//     auto a0 = shine_math::normalize(a);
//     std::cerr << "node direction is " << std::get<0>(a0) << ", " << std::get<1>(a0) << ", " << std::get<2>(a0)<< std::endl;
// 
//     x = r cos (phi). r is 1 so acos(y) =  theta
//     double phi = acos(std::get<0>(a0));
//     double theta = atan2(std::get<1>(a0), std::get<2>(a0));
//     std::cerr << "phi is " << phi << " and theta is " << theta << std::endl;
//     aimVessel(theta, phi);
    
    vessel.auto_pilot().set_reference_frame(vessel.control().nodes().at(0).reference_frame());
    vessel.auto_pilot().set_target_direction(std::tuple<double, double, double>(0.0, 1.0, 0.0));
    vessel.auto_pilot().wait();

}

void connection::warpTo(double time) {
    sc->warp_to(time);
}

double connection::getRemainingNodeBurn() {
    if (vessel.control().nodes().size() == 0) {
        return 0.0;
    }
    auto node = vessel.control().nodes().at(0);
    auto remainingBurn = node.remaining_burn_vector();
    return shine_math::magnitude(remainingBurn);
}

void connection::deleteNode(int nodeNum)
{
    if (vessel.control().nodes().size() < nodeNum) {
        return;
    }
    vessel.control().nodes().at(nodeNum).remove();
}



