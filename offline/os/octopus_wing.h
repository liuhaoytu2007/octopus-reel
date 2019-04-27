/*
Octopus-ReEL - Realtime Encephalography Laboratory Network
   Copyright (C) 2007 Barkin Ilhan

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If no:t, see <https://www.gnu.org/licenses/>.

 Contact info:
 E-Mail:  barkin@unrlabs.org
 Website: http://icon.unrlabs.org/staff/barkin/
 Repo:    https://github.com/4e0n/
*/

#ifndef OCTOPUS_WING_H
#define OCTOPUS_WING_H

#include <QColor>
#include "octopus_vec3.h"

typedef struct _Vertex { Vec3 r;   } Vertex;
typedef struct _Edge   { int v[2]; } Edge;
typedef struct _Face   { int v[3]; } Face;
typedef struct _Wing   { int f[2]; } Wing;
typedef struct _SNeigh { int v[3]; } SNeigh;

#endif
