/* Copyright 2017 Florian Fleissner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#ifndef PPG_LAYER_H
#define PPG_LAYER_H

/** @file */

#ifndef PPG_LAYER_TYPE 

/** This macro enables to define the layer type from outside the
 * compile process, e.g. from a build system
 */
#define PPG_LAYER_TYPE int8_t
#endif

/** The data type that is used to reference layers
 */
typedef PPG_LAYER_TYPE PPG_Layer;

#endif
