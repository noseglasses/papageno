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

#ifndef PPG_NOTE_DETAIL_H
#define PPG_NOTE_DETAIL_H

#include "ppg_note.h"
#include "detail/ppg_token_detail.h"

typedef struct {
	
	PPG_Token__ super;
	 
	PPG_Input input;
	
	bool active;
	 
} PPG_Note;

PPG_Note *ppg_note_new(PPG_Note *note);

PPG_Note *ppg_note_alloc(void);

#endif
