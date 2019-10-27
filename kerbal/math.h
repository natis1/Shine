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

#ifndef SHINE_MATH_H
#define SHINE_MATH_H

#include <krpc.hpp>


namespace shine_math {

    double idealAimAngle(double currentHeight, double alpha, double beta, int planet = 0);

    double angleBetween(std::tuple<double, double, double> vec1, std::tuple<double, double, double> vec2);
    double dotProduct(std::tuple<double, double, double> vec1, std::tuple<double, double, double> vec2);
    double magnitude(std::tuple<double, double, double> vec);
    std::tuple<double, double, double> normalize(std::tuple<double, double, double> vec);
}


#endif //SHINE_MATH_H
