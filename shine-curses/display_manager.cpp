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

#include "../kerbal/connection.h"
#include "display_manager.h"
#include <unistd.h>

display_manager::display_manager(display_manager::Module m) {
    loadedMod = m;
}

void display_manager::startDisplay() {
    if (loadedMod == ksp) {
        kspTelemetry();
    }
}


void display_manager::kspTelemetry() {
    while (true) {
        usleep(50000);
        drawKspTelemetry();

        if (getUserInput() == 1) {
            break;
        }
    }
}

void display_manager::drawKspTelemetry() {
    shipTelemetry *t = kerbalConnection->getShipTelemetry();
    displayTelemetry *d = new displayTelemetry {
        // vessel data
        t->name,
        t->time,
        t->stageNum,
        t->totalStages,

        // flight data
        t->gForce,
        t->altitude,
        t->latitude,
        t->longitude,

        // orbit info
        t->velocity,
        t->apoapsis,
        t->periapsis,
        t->eccentricity,
        t->inclination
    };
    drawTelemetry(d);


}

void display_manager::drawTelemetry(struct displayTelemetry *t) {
    clear();
    move(2, 0);
    printw("Vessel: ");
    printw(t->name.c_str());
    move(3, 0);

    printw("Mission Time: ");
    printw(std::to_string(t->time).c_str());

    move(4, 0);
    printw("Orbit velocity: ");
    printw(std::to_string(t->velocity).c_str());

    refresh();

}

int display_manager::getUserInput() {
    return 0;
}
