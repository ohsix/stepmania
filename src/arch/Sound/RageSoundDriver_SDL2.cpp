#include "global.h"
#include "RageSoundDriver_SDL2.h"

#include "RageTimer.h"
#include "RageLog.h"
#include "RageSound.h"
#include "RageUtil.h"
#include "RageSoundManager.h"
#include "PrefsManager.h"

/* only rough concern for other compilers rn */
#ifdef __FUNCSIG__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

/* not using LOG / RageLog for now*/
#define HERE SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, __PRETTY_FUNCTION__)

#define US "RSD_SDL2"

#define HEHE

REGISTER_SOUND_DRIVER_CLASS( SDL2 );

static void dumpAudioSpec(SDL_AudioSpec* s)
{
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "freq:\t%d\n", s->freq);
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "format:\t%d\n", s->format);
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "channels:\t%d\n", s->channels);
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "silence:\t%d\n", s->silence);
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "samples:\t%d\n", s->samples);
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "size:\t%d\n", s->size);
}

RString RageSoundDriver_SDL2::Init()
{
	HERE;
	
	if(SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, US "audio init failed: %s\n", SDL_GetError());
		return SDL_GetError();
	}

	SDL_AudioSpec have;

	dev = SDL_OpenAudioDevice(NULL, 0, &spec, &have, 0);

	if(dev == 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, US "OpenAudioDevice failed: %s\n", SDL_GetError());
		return SDL_GetError();
	}

#if defined(HEHE)
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, US "want:\n");
	dumpAudioSpec(&spec);
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, US "have:\n");
	dumpAudioSpec(&have);
#endif

	buf = (int16_t *)SDL_malloc(spec.size);

	StartDecodeThread();

	SDL_PauseAudioDevice(dev, 0);

	return RString();
}

RageSoundDriver_SDL2::RageSoundDriver_SDL2()
{
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE); /* belongs elsewere */
	HERE;

	SDL_memset(&spec, 0, sizeof(spec));

	spec.freq = PREFSMAN->m_iSoundPreferredSampleRate;

	if(spec.freq == 0)
		spec.freq = 48000;

	spec.format = AUDIO_S16;
	spec.channels = 2;
	spec.samples = 512;

	spec.userdata = (void*)this;
	spec.callback = RageSoundDriver_SDL2::Callback;
	buf = NULL;

	cb = 0;

	dev = 0;
	last_pos = 0;
}

RageSoundDriver_SDL2::~RageSoundDriver_SDL2()
{
	HERE;

	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "audio cb called %" PRId64 " times\n", cb);

	SDL_free(buf);
	SDL_CloseAudioDevice(dev);
}

#if 0
void RageSoundDriver_SDL2::Update()
{
	//this->Mix(buf, spec.samples, last_pos, GetPosition());

	last_pos += spec.samples;

	RageSoundDriver::Update();
}
#endif

int64_t RageSoundDriver_SDL2::GetPosition() const
{
	Uint32 sz = SDL_GetQueuedAudioSize(dev);

	return last_pos - (sz / sizeof(int16_t) * spec.channels);
}

float RageSoundDriver_SDL2::GetPlayLatency() const
{
	return spec.samples * (1.0f / spec.freq);
}

int RageSoundDriver_SDL2::GetSampleRate() const
{
	return spec.freq;
}

void RageSoundDriver_SDL2::Render(Uint8* stream, int len){
	size_t frames = len / sizeof(int16_t);
	int64_t last_frame = last_pos + frames / 2;

	if(cb <= 5)
	{
		// print some stuff on the first few render calls
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Render(%08X, %d)\n", stream, len);
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "frames %d, last_frame = %" PRId64 "\n", frames, last_frame);
		
	}

	this->Mix((int16_t*)stream, last_frame - last_pos, last_pos, last_frame);

	last_pos = last_frame;

	cb++;
}

void RageSoundDriver_SDL2::Callback(void* userdata, Uint8* stream, int len)
{
	((RageSoundDriver_SDL2*)userdata)->Render(stream, len);
}

#undef HERE
#undef US

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
