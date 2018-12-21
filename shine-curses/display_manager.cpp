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

const int UPDATETIME = 100000;
const int KEYREADTIME = 10000;
const int ERRORFRAMES = 50;

const int TABLENGTH = 5;



display_manager::display_manager(display_manager::Module m) {
    loadedMod = m;
}

void display_manager::startDisplay() {

    userInput = "";

    // 1 Normal
    // 2 Good (Based on protium crystal color. See )
    // 3 Alright/neutral
    // 4 Warning
    // 5 Danger
    // 6 DANGER!
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_RED, COLOR_BLACK);
    init_pair(6, COLOR_BLACK, COLOR_RED);

    // 7 Radiant
    // 8 User Input
    init_pair(7, COLOR_BLACK, COLOR_YELLOW);
    init_pair(8, COLOR_GREEN, COLOR_BLACK);

    nodelay(stdscr, true);

    if (loadedMod == ksp) {
        kspTelemetry();
    }
}


void display_manager::kspTelemetry() {
    int sleepTime = 0;
    while (true) {
        while (sleepTime < UPDATETIME) {
            sleepTime += UPDATETIME;
        }
        sleepTime = 0;


        drawKspTelemetry();

        if (getUserInput() == 1) {
            break;
        }
        getDrawUserInput();

        usleep(UPDATETIME);

    }
}

void display_manager::getDrawUserInput() {
    char c;
    do {
        c = getch();
        if (c == KEY_RESIZE) {

        } else if (c == KEY_ENTER || c == 10) {

            int errCode = parseUserInput(userInput);
            if (errCode != 0) {
                errDisplayFrames = ERRORFRAMES;
            }

            if (errCode == 1) {
                move(LINES - 2, 0);
                attron(COLOR_PAIR(6));
                printw("Command not found: ");
                if (userInput.size() > COLS - 19) {
                    userInput = userInput.substr(0, COLS - 19);
                }
                attron(COLOR_PAIR(5));
                printw(userInput.c_str());
            }

            userInput = "";
        } else if (c == 127) {
            if (userInput.size() > 0) {
                userInput = userInput.substr(0, userInput.size() - 1);
            }

        } else if (c != ERR) {
            userInput = userInput + c;
        }

    } while (c != ERR);

    if (errDisplayFrames > 0) {
        errDisplayFrames--;
        if (errDisplayFrames == 0) {
            move(LINES - 2, 0);
            clrtoeol();
        }
    }

    attron(COLOR_PAIR(1));
    move(LINES - 1, 0);
    printw("> ");
    attron(COLOR_PAIR(8));
    printw(userInput.c_str());
    attron(COLOR_PAIR(1));
    clrtoeol();

    refresh();

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

    clearStatsLines();

    move(2, 0);
    printw("Vessel: ");
    printw(t->name.c_str());
    move(3, 0);

    printw("Mission Time: ");
    printw(std::to_string(t->time).c_str());

    move(4, 0);
    printw("Orbit velocity: ");
    printw(std::to_string(t->velocity).c_str());

}

int display_manager::getUserInput() {
    return 0;
}

void display_manager::drawDataElement(std::string dataType, std::string processedValue, int intensity) {


}

int display_manager::parseUserInput(std::string UI) {
    if (UI == "ERROR") {
        return 1;
    }

    return 0;
}

void display_manager::clearStatsLines() {
    for (int i = 2; i < 8; i++) {
        if (i < LINES - 2) {
            move(i, 0);
            clrtoeol();
        }
    }
}
