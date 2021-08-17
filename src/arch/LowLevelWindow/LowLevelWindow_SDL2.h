#ifndef LOW_LEVEL_WINDOW_SDL2_H
#define LOW_LEVEL_WINDOW_SDL2_H

#include "RageDisplay.h" // VideoModeParams
#include "LowLevelWindow.h"

#include <SDL2/SDL.h>

class LowLevelWindow_SDL2 : public LowLevelWindow
{
public:
	LowLevelWindow_SDL2();
	~LowLevelWindow_SDL2();

	void *GetProcAddress(RString s);

	RString TryVideoMode(const VideoModeParams &p, bool &bNewDeviceOut);
	void GetDisplaySpecs(DisplaySpecs &out) const;

	void LogDebugInformation() const;
	bool IsSoftwareRenderer( RString &sError );

	void SwapBuffers();
	void Update();

	const ActualVideoModeParams GetActualVideoModeParams() const;

	bool SupportsRenderToTexture() const;
	RenderTarget *CreateRenderTarget();

	bool SupportsFullscreenBorderlessWindow() const;

	bool SupportsThreadedRendering();
	void BeginConcurrentRenderingMainThread();
	void EndConcurrentRenderingMainThread();
	void BeginConcurrentRendering();
	void EndConcurrentRendering();

private:
	int foo;
};

#ifdef ARCH_LOW_LEVEL_WINDOW
#error "More than one LowLevelWindow selected!"
#endif
#define ARCH_LOW_LEVEL_WINDOW LowLevelWindow_SDL2

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
