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
#include <sstream>

const int UPDATETIME = 100000;
const int KEYREADTIME = 10000;
const int ERRORFRAMES = 50;

const int TABLENGTH = 10;

int display_manager::telemetryType = 0;


display_manager::display_manager(display_manager::Module m) {
    loadedMod = m;
}

void display_manager::startDisplay() {

    userInput = "";
    telemetryType = 0;

    signal(SIGABRT, display_manager::sigabrtHandler);

    // 1 Normal
    // 2 Good (Based on protium crystal color. See Half-Rose's drawings)
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

    printHelpMenu();
    move(0, 0);

    nodelay(stdscr, true);

    if (loadedMod == ksp) {
        kspTelemetry();
    }
}


void display_manager::kspTelemetry() {
    while (true) {

        if (telemetryType == 1) {

            try {
                drawKspVesselTelemetry();
            } catch (std::exception e) {
                move(LINES - 2, 0);
                printw(e.what());
                telemetryType = 0;
            }

        }

        if (telemetryType == -1) {
            break;
        }
        getDrawUserInput();

        usleep(UPDATETIME);

        // chronoSleep(UPDATETIME);

    }
}

void display_manager::getDrawUserInput() {
    char c;
    do {
        c = getch();
        if (c == KEY_RESIZE) {

        } else if (c == KEY_ENTER || c == 10) {

            std::string errorCode = parseUserInput(userInput);
            if (errorCode != "") {
                errDisplayFrames = ERRORFRAMES;
                move(LINES - 2, 0);
                attron(COLOR_PAIR(6));
                printw(errorCode.c_str());
                if (userInput.size() > COLS - errorCode.length()) {
                    userInput = userInput.substr(0, COLS - errorCode.length());
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

void display_manager::drawKspVesselTelemetry() {
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

    move(1, 0);
    drawDataElement("Vessel:", t->name, 3);
    drawDataElement("Mission Time:", std::to_string(t->time), 3);
    drawDataElement("Orbit velocity:", std::to_string(t->velocity), 2);


}

void display_manager::drawDataElement(std::string dataType, std::string processedValue, int intensity) {
    int y, x;
    getyx(stdscr, y, x);
    int nextTab = getNextTab(y, x);
    if (nextTab == 0) {
        move( (y + 1), 0);
    } else {
        move(y, nextTab);
    }

    printw(dataType.c_str());
    printw("  ");
    attron(COLOR_PAIR(intensity));
    printw(processedValue.c_str());
    attron(COLOR_PAIR(1));

}

std::string display_manager::parseUserInput(std::string UI) {
    std::string buffer;

    /*
     * Wingshade keyboards do not have lowercase characters on account of their language not distinguishing between
     * cases. Although the more common moth language does have case sensitivity, we must account for users who are
     * using devices without this functionality. Thus ALL commands must be made to work without case sensitivity.
     *
     * So that's why all strings are first converted to lowercase. The original case is preserved when printing
     * the error tho, so the user can see exactly what they entered.
     */
    std::transform(UI.begin(), UI.end(), UI.begin(), ::tolower);

    std::stringstream stream(UI);

    std::vector<std::string> tokens;

    while (stream >> buffer) {
        tokens.push_back(buffer);
    }

    // Safety
    if (tokens.size() == 0) {
        return "";
    }

    if (tokens.at(0) == "help") {
        pausePrintHelpMenu();
    }

    if (tokens.at(0) == "vessel") {
        if (tokens.size() < 2) {
            return "Usage: vessel [connect/disconnect/list]";
        } else {
            if (tokens.at(1) == "connect" && telemetryType == 0) {
                telemetryType = 1;
                return "";
            } else if (tokens.at(1) == "disconnect") {
                telemetryType = 0;
                return "";
            } else if (tokens.at(1) == "list") {
                return "not implemented yet: ";
            } else {
                return "Usage: vessel [connect/disconnect/list]";
            }

        }
    }



    if (tokens.at(0) == "error") {
        return "Intentional error created: ";
    }

    return "";
}

void display_manager::clearStatsLines() {
    for (int i = 2; i < 8; i++) {
        if (i < LINES - 2) {
            move(i, 0);
            clrtoeol();
        }
    }
}

// If on new line stay there. Otherwise go forward by up to 2 tabs.
int display_manager::getNextTab(int y, int x) {
    // Only happens when starting out, go to next page.
    if (x == 0) {
        return 0;
    }

    // Current x value is too large so go to next page
    if ( (x + (3 * TABLENGTH + 5)) >= COLS) {
        return 0;
    }
    int c = x % TABLENGTH;
    if (c == 0) {
        return x + 20;
    } else {
        return x + 10 + (10 - c);
    }
}

void display_manager::chronoSleep(int uSecs) {
    double secs = (double) uSecs / 1000000.0;
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::time_point end = start;
    do {
        usleep(uSecs);
        end = std::chrono::high_resolution_clock::now();
    }
    while (std::chrono::duration<double> (start - end).count() < secs);

}

void display_manager::sigabrtHandler(int sigabrt) {
    if (telemetryType > 0) {
        telemetryType = 0;
    } else {
        telemetryType = -1;
    }
}

void display_manager::printHelpMenu() {
    move (3, 0);
    drawDataElement("help - ", "Display this menu", 7);
    drawDataElement("vessel connect - ", "Connect to the current vessel", 7);
    drawDataElement("vessel disconnect - ", "Disconnect from a vessel", 7);
    drawDataElement("vessel list - ", "List all vessels", 7);
    drawDataElement("error - ", "Intentionally create error.", 7);
    refresh();

}

void display_manager::pausePrintHelpMenu() {
    nodelay(stdscr, false);
    move (0, 0);
    printw("Help Menu, press any key to return.");
    printHelpMenu();
    move(0, 0);
    getch();
    move(0, 0);
    clrtoeol();
    clearStatsLines();
    nodelay(stdscr, true);
}
