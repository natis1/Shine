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

#include <cmath>
#include "math.h"

using namespace shine_math;

double shine_math::angleBetween(std::tuple<double, double, double> vec1, std::tuple<double, double, double> vec2) {
    double dotVec = dotProduct(vec1, vec2);
    double netMagnitude = magnitude(vec1) * magnitude(vec2);
    return acos(dotVec / netMagnitude);
}

double shine_math::dotProduct(std::tuple<double, double, double> vec1, std::tuple<double, double, double> vec2) {
    return ( std::get<0>(vec1) * std::get<0>(vec2) + std::get<1>(vec1) * std::get<1>(vec2) + std::get<2>(vec1) * std::get<2>(vec2));
}

double shine_math::magnitude(std::tuple<double, double, double> vec) {
    return (sqrt( pow(std::get<0>(vec), 2.0) + pow(std::get<1>(vec), 2.0) + pow(std::get<2>(vec), 2.0)));
}
