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

#ifndef SHINE_AUTOPILOT_H
#define SHINE_AUTOPILOT_H

#include "../../kerbal/connection.h"

#define AUTOPILOT_COMPLETE -1

class autopilot {
public:
    autopilot(connection *conn);
    void takeoff();
    void landAnywhere();
    // Returns false if landing is impossible.
    bool landAtCoordinates(double theta, double tau);
    double alpha;
    double beta;
    double targetHeight;

    int autopilotPhase;



private:
    connection *c;


};


#endif //SHINE_AUTOPILOT_H
