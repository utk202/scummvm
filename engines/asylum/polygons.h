/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#ifndef ASYLUM_POLYGONS_H_
#define ASYLUM_POLYGONS_H_

#include "common/array.h"
#include "common/rect.h"
#include "common/stream.h"

#define MAX_POLYGONS 200

namespace Asylum {

typedef struct PolyDefinitions {
	int32		  numPoints;
	Common::Point *points;
	Common::Rect  boundingRect;

	/**
	 * Check if the x/y coordinates exist within
	 * the current polygon definition
	 *
	 * (was pointInPoly())
	 */
	bool contains(int16 x, int16 y);

} PolyDefinitions;

class Polygons {
public:
	Polygons(Common::SeekableReadStream *stream);
	virtual ~Polygons();

	int32 size;
	int32 numEntries;

	Common::Array<PolyDefinitions> entries;

private:

	void load(Common::SeekableReadStream *stream);

}; // end of class Polygons

} // end of namespace Asylum

#endif