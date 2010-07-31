/*
-------------------------------------------------------------------------------
    This file is part of OgreKit.
    http://gamekit.googlecode.com/

    Copyright (c) 2006-2010 Nestor Silveira & Charlie C.

    Contributor(s): none yet.
-------------------------------------------------------------------------------
  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------
*/
#ifndef _gkSound_h_
#define _gkSound_h_

#include "gkCommon.h"
#include "gkSource.h"

#include "gkSoundUtil.h"
#include "gkMathUtils.h"

class gkSoundStream;
class gkBuffer;
class gkStreamer;


// ----------------------------------------------------------------------------
class gkSound
{
private:
	typedef utArray<gkSource *> Sources;


	const gkString      m_name;
	gkSoundStream       *m_stream;
	Sources             m_sources;

public:

	gkSound(const gkString &name);
	virtual ~gkSound();


	GK_INLINE const gkString    &getName(void)          {return m_name;}
	GK_INLINE gkSoundStream     *getStream(void)        {return m_stream;}

	gkSource    *createSource(void);
	void        destroySource(gkSource *);

	void        stopPlayback(void);


	// Load sound block from file, then dumps it into memory
	bool loadToMemory(const char *file);

	// Load sound from file, then streams
	// file data to a playback buffer.
	bool load(const char *file);


	// Load sound from memory, then streams
	// memory data to a playback buffer.
	bool load(void *handle, UTsize len);
};

#endif//_gkSound_h_
