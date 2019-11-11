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

std::vector<krpc::services::SpaceCenter::Node> generatedNodes;


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
    generatedNodes.push_back(node);
}

double connection::changePeriapsis(double finalPeriapsis, double burnRadius)
{
    bool apoBurn = false;
    if ( std::abs(burnRadius - vessel.orbit().apoapsis()) < 5.0) {
        burnRadius = vessel.orbit().apoapsis();
        apoBurn = true;
    }
    double mu = vessel.orbit().body().gravitational_parameter();
    double a = vessel.orbit().semi_major_axis();
    double b = vessel.orbit().semi_minor_axis();
    
    //double v1 = std::sqrt(mu / a) * b / burnRadius;
    //double v2 = std::sqrt(mu / a) * finalPeriapsis / burnRadius;
    //double v1 = std::sqrt(mu * ((2.0 / burnRadius) - (1.0 / b)));
    //double v2 = (std::sqrt(mu * ((2.0 / burnRadius) - (1.0 / finalPeriapsis))) + v1) / 2.0;
    
    
    
    double delta_v = std::sqrt((mu / burnRadius)) * ( (std::sqrt((2.0 * finalPeriapsis)/ (burnRadius + finalPeriapsis))) - 1.0 );
    std::cerr << "delta v is " << delta_v << std::endl;
    
    if (apoBurn) {
        auto node = vessel.control().add_node(
            sc->ut() + vessel.orbit().time_to_apoapsis(), delta_v);
        generatedNodes.push_back(node);
        
        return sc->ut() + vessel.orbit().time_to_apoapsis();
    } else {
        double e = vessel.orbit().eccentricity();
        double tFinal = std::sqrt(std::pow(a, 3.0) / mu) * (2.0 * std::atan(std::sqrt((burnRadius - a * (1 - e)) / (a * (1 + e) - burnRadius))) - std::sqrt(std::pow(e, 2.0) - std::pow(1.0 - burnRadius / a, 2.0) ));
        double tCurrent = std::sqrt(std::pow(a, 3.0) / mu) * (2.0 * std::atan(std::sqrt((shipTelemetry_height - a * (1 - e)) / (a * (1 + e) - shipTelemetry_height))) - std::sqrt(std::pow(e, 2.0) - std::pow(1.0 - shipTelemetry_height / a, 2.0) ));
        std::cerr << "Tfinal is " << tFinal << " and tcurrent is " << tCurrent << std::endl;
        auto node = vessel.control().add_node(
            sc->ut() + (tFinal - tCurrent), delta_v);
        generatedNodes.push_back(node);
        return sc->ut() + (tFinal - tCurrent);
    }
}


double connection::calculateNodeBurnTime() {
    // Calculate burn time (using rocket equation)
    if (generatedNodes.size() == 0) {
        return 0;
    }
    double delta_v = generatedNodes.at(0).delta_v();
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

std::vector<std::pair<krpc::services::SpaceCenter::Part, int>> parachuteList;

bool connection::prepareParachuteList()
{
    parachuteList.clear();
    std::vector<krpc::services::SpaceCenter::Part> parts = vessel.parts().all();
    for (int i = 0; i < parts.size(); i++) {
        if (parts.at(i).name() == "radialDrogue") {
            parachuteList.push_back(std::make_pair(parts.at(i), 1));
        } else if (parts.at(i).name() == "parachuteRadial") {
            parachuteList.push_back(std::make_pair(parts.at(i), 2));
        } else if (parts.at(i).name() == "parachuteDrogue") {
            parachuteList.push_back(std::make_pair(parts.at(i), 0));
        } else if (parts.at(i).name() == "parachuteSingle") {
            parachuteList.push_back(std::make_pair(parts.at(i), 2));
        } else if (parts.at(i).name() == "parachuteLarge") {
            parachuteList.push_back(std::make_pair(parts.at(i), 2));
        }
    }
    return (parachuteList.size() > 0);
}


bool connection::deployParachutesIfSafe()
{
    const double safeVelo0 = 550;
    const double safeVelo1 = 424;
    const double safeVelo2 = 264;
    if (shipTelemetry_velocity < safeVelo0) {
        for (auto i : parachuteList) {
            if (i.second <= 0) {
                i.second = 3;
                i.first.parachute().deploy();
            }
        }
    }
    if (shipTelemetry_velocity < safeVelo1) {
        for (auto i : parachuteList) {
            if (i.second <= 1) {
                i.second = 3;
                i.first.parachute().deploy();
            }
        }
    }
    if (shipTelemetry_velocity < safeVelo2) {
        for (auto i : parachuteList) {
            if (i.second <= 2) {
                i.second = 3;
                i.first.parachute().deploy();
            }
        }
        return true;
    }
    return false;
}

void connection::setWarpRate(int rate)
{
    sc->set_rails_warp_factor(rate);
}


void connection::warpTo(double time) {
    sc->warp_to(time);
}

double connection::getRemainingNodeBurn() {
    if (generatedNodes.size() == 0) {
        return 0.0;
    }
    auto node = generatedNodes.at(0);
    auto remainingBurn = node.remaining_burn_vector();
    return shine_math::magnitude(remainingBurn);
}

void connection::clearAllNodes()
{
    generatedNodes.clear();
    vessel.control().remove_nodes();
}


void connection::deleteNode(int nodeNum)
{
    if (generatedNodes.size() <= nodeNum) {
        return;
    }
    generatedNodes.at(nodeNum).remove();
}

// Basically self destruct
void connection::stageAll()
{
    while (vessel.control().current_stage() > 0) {
        vessel.control().activate_next_stage();
    }
}




