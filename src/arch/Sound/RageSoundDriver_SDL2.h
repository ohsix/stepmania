#ifndef RAGE_SOUND_SDL2
#define RAGE_SOUND_SDL2

#include "RageSoundDriver.h"
#include "RageThreads.h"

#include <SDL2/SDL.h>

class RageSoundDriver_SDL2: public RageSoundDriver
{
public:
	RString Init();
	RageSoundDriver_SDL2();
	~RageSoundDriver_SDL2();

//	void Update();
	int64_t GetPosition() const;
	float GetPlayLatency() const;
	int GetSampleRate() const;

	void Render(Uint8* stream, int len);

private:
	static void Callback(void* userdata, Uint8* stream, int len);
	SDL_AudioDeviceID dev;
	SDL_AudioSpec spec;
	int16_t* buf;
	int last_pos;

	int64_t cb;
};

#endif

/*
 * (c) 2021 Josh Hill
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
 * INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
