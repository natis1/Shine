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

#include <stdexcept>
#include "module_menu.h"
#include "../kerbal/connection.h"
#include "display_manager.h"
#include <curses.h>

const int MENU_DISPLAY_HEIGHT = 5;
const int CENTER = 29;

module_menu::module_menu() {
    initDisplay();
    drawMenu();



    while (true) {
        switch (getch()) {
            case 'q':
            case 'Q':
                exit(0);
            case 'k':
            case 'K':
                clear();
                move(0, 0);
                printw("Attempting connection... Please accept in KSP.");
                refresh();
                connection *c = new connection;
                bool b = c->tryConnection() == 0;

                if (!b) {
                    move(MENU_DISPLAY_HEIGHT, 0);
                    printw("Error connecting to kRPC. Is the kRPC mod installed and KSP running?");
                    getch();
                } else {
                    clear();
                    refresh();
                    display_manager *dm = new display_manager(display_manager::ksp);
                    dm->kerbalConnection = c;
                    dm->startDisplay();
                }
                break;
        }
        drawMenu();
    }


}




int module_menu::initDisplay()
{
    initscr();
    cbreak();
    noecho();
    start_color();
    return 0;
}

void module_menu::drawMenu() {
    clear();
    move(MENU_DISPLAY_HEIGHT, 35);
    printw("Shine Modules:");
    move(MENU_DISPLAY_HEIGHT + 4, CENTER);
    printw("[K]erbal space program");
    move(MENU_DISPLAY_HEIGHT + 5, CENTER);
    printw("[Q]uit Shine.");
    move(0, 0);
}
