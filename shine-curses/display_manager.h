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

#ifndef SHINE_DISPLAY_MANAGER_H
#define SHINE_DISPLAY_MANAGER_H

#include "ncurses.h"

class display_manager {
public:
    enum Module { ksp, other };
    display_manager(Module m);
    connection *kerbalConnection;
    void startDisplay();

private:
    struct displayTelemetry {
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

    void kspTelemetry();
    void drawKspTelemetry();
    int getUserInput();

    void drawTelemetry(struct displayTelemetry *t);
    Module loadedMod;



};


#endif //SHINE_DISPLAY_MANAGER_H
