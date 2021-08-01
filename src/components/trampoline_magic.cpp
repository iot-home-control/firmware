// SPDX-License-Identifier: GPL-3.0-or-later
/*
    Copyright (C) 2021  The Home Control Authors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "trampoline_magic.h"
/*
ISR*const * tramps=get_trampolines(foo::make_index_sequence<NUM_ISR_TRAMPS>{});

std::function<void()> isr_trampolines[NUM_ISR_TRAMPS];

int allocate_trampoline(const std::function<void()> func)
{
    for(int index=0; index<NUM_ISR_TRAMPS; index++)
    {
        if(!isr_trampolines[index])
        {
            isr_trampolines[index]=func;
            return index;
        }
    }
    return -1;
}
*/
