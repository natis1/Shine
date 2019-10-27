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

#include "autopilot.h"
#include "../../kerbal/connection.h"
#include "../../kerbal/math.h"
#include <math.h>
#include <stdio.h>


double burnTime;


void autopilot::takeoff() {

    if (autopilotPhase == 0) {
        double h = c->shipTelemetry_altitude;
        double angle = shine_math::idealAimAngle(h, alpha, beta, 0);
        c->aimVessel(M_PI_2, angle);
        std::cerr << "Aimed vessel at " << angle << " because height is " << h << std::endl;

        if (c->shipTelemetry_apoapsisAltitude >= targetHeight) {
            c->setVesselThrust(0.0);
            autopilotPhase = 1;
        }
    } else if (autopilotPhase == 1) {
        if (c->shipTelemetry_altitude < 70500) {
            return;
        } else {
            autopilotPhase = 2;
            c->circularize();
            burnTime = c->calculateNodeBurnTime();
            c->pointTowardsNode();
        }
    } else if (autopilotPhase == 2) {
        double burnUT = c->getUniversalTime() + c->shipTelemetry_timeToApoapsis - (burnTime / 2.0);
        c->warpTo(burnUT - 3.0);
        autopilotPhase = 3;
    } else if (autopilotPhase == 3) {
        double burnUT = c->getUniversalTime() + c->shipTelemetry_timeToApoapsis - (burnTime / 2.0);
        if (burnUT < c->getUniversalTime()) {
            autopilotPhase = 4;
            c->setVesselThrust(1.0);
        }
    } else if (autopilotPhase == 4) {
        c->pointTowardsNode();
        double remainingBurnAmt = c->getRemainingNodeBurn();
        std::cerr << "Remaining burn amount " << remainingBurnAmt << std::endl;

        if (remainingBurnAmt <= 100.0) {
            c->setVesselThrust(remainingBurnAmt / 100.0);
        } if (remainingBurnAmt <= 5.0) {
            autopilotPhase = 5;
        }

    } else {
        double remainingBurnAmt = c->getRemainingNodeBurn();
        if (remainingBurnAmt > 2.0) {
            c->deleteNode(0);
            return;
        } else {
            c->setVesselThrust(0.0);
            autopilotPhase = -1;
        }
    }

}

autopilot::autopilot(connection *conn) {
    this->c = conn;
    autopilotPhase = 0;
    targetHeight = 0;
    alpha = 0;
    beta = 0;
}
