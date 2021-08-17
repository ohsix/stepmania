#include "global.h"
#include "LowLevelWindow_SDL2.h"

#include "RageLog.h" /* native logging */
#include "RageDisplay_OGL_Helpers.h" /* RenderTarget */

#include "RageDisplay.h"
#include "DisplaySpec.h"

/* only rough concern for other compilers rn */
#ifdef __FUNCSIG__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

/* not using LOG / RageLog for now*/
#define HERE SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, __PRETTY_FUNCTION__)

#define US "LLW_SDL2"

#define HEHE

#if 0
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

	const ActualVideoModeParams GetActualVideoModeParams() const { return CurrentParams; }

	bool SupportsRenderToTexture() const;
	RenderTarget *CreateRenderTarget();

	bool SupportsFullscreenBorderlessWindow() const;

	bool SupportsThreadedRendering();
	void BeginConcurrentRenderingMainThread();
	void EndConcurrentRenderingMainThread();
	void BeginConcurrentRendering();
	void EndConcurrentRendering();

private:
	ActualVideoModeParams CurrentParams;
};
#endif

LowLevelWindow_SDL2::LowLevelWindow_SDL2()
{
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE); /* belongs elsewere */
	HERE;
}

LowLevelWindow_SDL2::~LowLevelWindow_SDL2()
{
	HERE;
}

void *LowLevelWindow_SDL2::GetProcAddress(RString s)
{
	HERE;
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, US "::GetProcAddress(\"%s\");\n", s);

	return NULL;
}

RString LowLevelWindow_SDL2::TryVideoMode(const VideoModeParams& p, bool& bNewDeviceOut)
{
	HERE;

	bNewDeviceOut = false;

	return "fun times";
}

void LowLevelWindow_SDL2::GetDisplaySpecs(DisplaySpecs& out) const
{
	HERE;
	out.clear();

	DisplayMode spoofMode = { 640U, 480U, 30.0 };
	DisplaySpec spoofSpec("LowLevelWindow_SDL2", "LowLevelWindow_SDL2", spoofMode);

	out.insert(spoofSpec);
}

void LowLevelWindow_SDL2::LogDebugInformation() const
{
	HERE;

	LOG->Info(US "::LogDebugInformation native logging");
}

bool LowLevelWindow_SDL2::IsSoftwareRenderer(RString &)
{
	HERE;

	/* SDL could actually support a fast software mode with just a blitter */

	/* legitimately not caring about software gl even if we arrive here
 	 * with a sw context. from testing sw gl was fast enough regardless */
	return false;
}

void LowLevelWindow_SDL2::SwapBuffers()
{
	/* HERE; */

	/* SDL_GL_SwapWindow
     * SDL_GL_SetSwapInterval
     */
}

void LowLevelWindow_SDL2::Update()
{
	/* HERE; */
}

const ActualVideoModeParams LowLevelWindow_SDL2::GetActualVideoModeParams() const
{
	ActualVideoModeParams dontcare;
	return dontcare;
}

bool LowLevelWindow_SDL2::SupportsRenderToTexture() const
{
	HERE;

	/* want all the render to texture requests */
	return true;
}

RenderTarget* LowLevelWindow_SDL2::CreateRenderTarget()
{
	HERE;

	return nullptr;
}

bool LowLevelWindow_SDL2::SupportsFullscreenBorderlessWindow() const
{
	HERE;

	return true;
}

bool LowLevelWindow_SDL2::SupportsThreadedRendering()
{
	HERE;

	/* want all the threaded rendering requests */
	return true;
}

void LowLevelWindow_SDL2::BeginConcurrentRenderingMainThread()
{
	HERE;
}

void LowLevelWindow_SDL2::EndConcurrentRenderingMainThread()
{
	HERE;
}

void LowLevelWindow_SDL2::BeginConcurrentRendering()
{
	HERE;
}

void LowLevelWindow_SDL2::EndConcurrentRendering()
{
	HERE;
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
