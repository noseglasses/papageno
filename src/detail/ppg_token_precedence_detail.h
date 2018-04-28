/* Copyright 2017 noseglasses <shinynoseglasses@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PPG_TOKEN_PRECEDENCE_DETAIL_H
#define PPG_TOKEN_PRECEDENCE_DETAIL_H

enum PPG_Token_Precedence {
   
   PPG_Token_Precedence_None = 0,
   
   PPG_Token_Precedence_Cluster,
   PPG_Token_Precedence_Note,
   PPG_Token_Precedence_Explicit_Note,
      // A note that expects either an activation or deactivation
   PPG_Token_Precedence_Sequence,
   PPG_Token_Precedence_Chord
};

#endif
