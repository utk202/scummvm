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

#ifndef ASYLUM_GRAPHICS_H
#define ASYLUM_GRAPHICS_H

#include "asylum/shared.h"

#include "graphics/surface.h"

#include "common/array.h"

namespace Asylum {

class AsylumEngine;

struct GraphicFrame {
	int32 size;
	int32 offset;

	int16 x;
	int16 y;
	Graphics::Surface surface;

	uint16 getWidth() { return surface.w; }
	uint16 getHeight() { return surface.h; }

	Common::Rect getRect() {
		return Common::Rect(x, y, x + getWidth(), y + getHeight());
	}
};

// Graphic resources can be sprites or images, with multiple frames
class GraphicResource {
public:

	GraphicResource(AsylumEngine *engine) {}
	GraphicResource(AsylumEngine *engine, ResourceId id);
	~GraphicResource();

	void load(ResourceId id);

	/**
	 * Copies an animation frame to the target buffer
	 */
	void copyFrameToDest(byte *dest, uint32 frame);

	/**
	 * Copies a sprite to the target buffer, with transparency
	 */
	void copySpriteToDest(byte *dest, uint32 frame);

	GraphicFrame *getFrame(uint32 frame);
	ResourceId    getResourceId() { return _resourceId; }
	int32         getFlags()      { return _flags; }
	int32         getFlags2()     { return _flags2; }
	uint32        count()         { return _frames.size(); }

	// Helper functions
	static uint32 getFrameCount(AsylumEngine *engine, ResourceId id);
	static Common::Rect getFrameRect(AsylumEngine *engine, ResourceId id, uint32 index);

private:
	AsylumEngine *_vm;

	Common::Array <GraphicFrame> _frames;

	ResourceId _resourceId;
	int32 _flags;
	int32 _flags2;

	void init(byte *data, int32 size);
	void clear();
};

} // end of namespace Asylum

#endif