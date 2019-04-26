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

namespace  display_manager {
    enum Module { ksp, other };

    void startDisplay();

    static int telemetryType;

    static void sigabrtHandler(int sigabrt);

    void kspTelemetry();
    void drawKspVesselTelemetry();

    void drawTelemetry();
    void drawDataElement(const std::string& dataType, const std::string& processedValue, uint intensity = 1);
    void getDrawUserInput();
    void clearStatsLines();
    int getNextTab(int y, int x, int msgLength);
    void chronoSleep(int uSecs);

    std::string parseUserInput(std::string UI);

    static void setInfoLine(const std::string& line);
    
    void printHelpMenu();
    void pausePrintHelpMenu();
    void resizeSignal(int signal);


    void loadKSP(Module m, connection *c);

    void *kspLoop(void *kerbalConnection);
}


#endif //SHINE_DISPLAY_MANAGER_H
