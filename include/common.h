/*
 * This file is part of libkern.
 *
 * libkern is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libkern is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libkern.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMMON_H_
#define COMMON_H_

/* logical/comparison operators */
#define ne  !=
#define eq  ==
#define gt  >
#define lt  <
#define ge  >=
#define le  <=
#define and &&
#define or  ||
#define not !

/* bitwise operators */
#define bnot ~
#define band &
#define bor  |
#define bxor ^

/* arithmetic operators */
#define mod   %

/* control constructs */
#define repeat    do
#define until(p)  while(not (p))
#define loop      while(1)
#define unless(p) if(not (p))
#define nop

/* functional constructs */
#define lambda(return_type, function_body) \
    ({ \
        return_type __fn__ function_body \
        __fn__; \
    })

#endif // COMMON_H_
