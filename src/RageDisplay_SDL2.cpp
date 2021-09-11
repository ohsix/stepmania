#include "global.h"

#include "RageDisplay.h"
#include "RageDisplay_SDL2.h"

#include "RageFileManager.h"
#include "RageSurface.h"

#include "DisplaySpec.h"
#include "PrefsManager.h"

#include "arch/ArchHooks/ArchHooks.h"

#include <cstdint>
#include <execinfo.h>

/* only rough concern for other compilers rn */
#ifdef __FUNCSIG__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

static void bt()
{
	void* frames[128];
	char** symbols;
	int entries;

	entries = backtrace(frames, 128);
	symbols = backtrace_symbols(frames, entries);

	if(symbols != NULL)
	{
		entries -= 4; /* remove libc/loader */

		for(int i = 2; i <= entries; i++) /* skip the ones we know about */
			SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "%s", symbols[i]);
	}
	else
	{
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "bt() failed");
		return;
	}

	free(symbols);
}

/* used for timestamps */
static double pc_freq = SDL_GetPerformanceFrequency();
static uint64_t pc_init = SDL_GetPerformanceCounter();

/* not using LOG / RageLog for now*/
#define HERE SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "[%f] %s", (SDL_GetPerformanceCounter() - pc_init)/pc_freq, __PRETTY_FUNCTION__)

#define US "RD_SDL2"

#define HEHE

/* since SDL2 is also its' own LowLevelWindow in stepmania terms, we don't use one as a wrapper */

/* RGB555 with the 5551 layout instead of 1555 */
uint32_t SDL_PIXELFORMAT_RGBX5551 = SDL_DEFINE_PIXELFORMAT(SDL_PIXELTYPE_PACKED16, SDL_PACKEDORDER_RGBX, SDL_PACKEDLAYOUT_5551, 15, 2);

uint32_t sdl_pfmap[NUM_RagePixelFormat] =
{
	SDL_PIXELFORMAT_RGBA8888,	/* R8G8B8A8 */
	SDL_PIXELFORMAT_BGRA8888,	/* B8R8G8A8 */
	SDL_PIXELFORMAT_RGBA4444,	/* R4G4B4A4 */
	SDL_PIXELFORMAT_RGBA5551,	/* R5G5B5A1 */
	SDL_PIXELFORMAT_RGBX5551,	/* R5G5B5X1 */
	SDL_PIXELFORMAT_RGB888,		/* R8G8B8 */
	SDL_PIXELFORMAT_INDEX8,		/* Index w/ color table */
	SDL_PIXELFORMAT_BGR888,		/* B8G8R8 */
	SDL_PIXELFORMAT_ARGB1555,	/* A1R5G5B5 */
	SDL_PIXELFORMAT_RGB555		/* X1R5G5B5 */
};

RageDisplay::RagePixelFormatDesc pf_desc[NUM_RagePixelFormat];

static void init_pfmap()
{
	for(int i = 0; i < NUM_RagePixelFormat; i++)
	{
		(void)SDL_PixelFormatEnumToMasks(sdl_pfmap[i], &pf_desc[i].bpp,
			&pf_desc[i].masks[0], &pf_desc[i].masks[1],	&pf_desc[i].masks[2], &pf_desc[i].masks[3]);
	}
}


RString RageDisplay_SDL2::Init(const VideoModeParams& p, bool bAllowUnacceleratedRenderer)
{
	HERE;
	bt();

	SDL_SetHint(SDL_HINT_EVENT_LOGGING, "1"); // log all but mouce & finger

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, US "audio init failed: %s\n", SDL_GetError());
		return SDL_GetError();
	}

	init_pfmap(); /* i refuse an ugly table when sdl already has good pixel format handling */

	/* the SDL_OPENGL_LIBRARY environment variable can specify a library at any path */
	if(SDL_GL_LoadLibrary("libGLESv2.so") == 0)
	{
		/* local libEGL can come from ANGLE (or Regal, or OSMesa, or gl4es */
		SDL_GL_UnloadLibrary(); /* for now we just want to know it's there */
	}

	VideoModeParams adjusted = p;	

	/* this can mean a speed hit and should probably be gated by something */
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	/* OpenGL ES 2.0 */
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	/* non-negotiable */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	/* directly override vsync */
	adjusted.vsync = true;
	
	activeMode = adjusted;

	bool reloadTex = false;
	return SetVideoMode(adjusted, reloadTex);
}

RageDisplay_SDL2::RageDisplay_SDL2()
{
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE); /* belongs elsewere */
	HERE;
}

RageDisplay_SDL2::~RageDisplay_SDL2()
{
	HERE;
}

RString RageDisplay_SDL2::GetApiDescription() const
{
	HERE;
	bt();

	return "SDL2";
}

void RageDisplay_SDL2::GetDisplaySpecs(DisplaySpecs& out) const
{
	HERE;
	bt();

	/* not using LowLevelWindow, SDL can do it all */
	out.clear();
	DisplayMode nullMode = { 640U, 480U, 30.0 };
	DisplaySpec nullSpec("SDL2 Placeholder", "foo", nullMode);
	out.insert(nullSpec);
}

ActualVideoModeParams RageDisplay_SDL2::GetActualVideoModeParams() const
{
	HERE;
	bt();
	return activeMode;
}

/* map the cursor position to a display index */
static int mouse_on_display()
{
	int mx, my;
	int displays = SDL_GetNumVideoDisplays();

	if(displays == 1)
		return 0;

	SDL_GetGlobalMouseState(&mx, &my);

	while(displays-- > 0)
	{
		SDL_Rect db;

		SDL_GetDisplayBounds(displays, &db);

		if(mx >= db.x && mx <= db.x + db.w && my >= db.y && my <= db.y + db.h)
			return displays;
	}

	return 0;
}

/* scary terry says we can run but we can't hide
 * RageDisplay.h says don't override SetVideoMode but we totally can
 * the fallback to different bpp and safe video mode isn't a problem for us
 * for now we play along and do the dumb upcall to RageDisplay
 */
RString RageDisplay_SDL2::TryVideoMode(const VideoModeParams& p, bool& bNewDeviceOut)
{
	HERE;

	/* don't even try 565 */
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	
	/* we like the display with the cursor on it */
	int di = mouse_on_display();
	w = SDL_CreateWindow(p.sWindowTitle, SDL_WINDOWPOS_UNDEFINED_DISPLAY(di), SDL_WINDOWPOS_UNDEFINED_DISPLAY(di), p.width, p.height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

	if(w == NULL)
		return "SDL_CreateWindow failed";

	gl = SDL_GL_CreateContext(w);

	if(gl == NULL)
		return "SDL_GL_CreateContext failed";

	/* high res textures being disabled slows down startup */
	PREFSMAN->m_HighResolutionTextures.Set(HighResolutionTextures_ForceOn);
	PREFSMAN->m_iMaxTextureResolution.Set(GetMaxTextureSize());
	PREFSMAN->m_bForceMipMaps.Set(false);

	/* icon could be in a RageFile zip but failure is ok */
	SDL_Surface* icon = IMG_Load(FILEMAN->ResolvePath(p.sIconFile));

	if(icon != NULL)
	{
		SDL_SetWindowIcon(w, icon);
		SDL_FreeSurface(icon);
	}

	glViewport(0, 0, p.width, p.height);

	/* could draw something nice before showing the window
	 * the black pause during initial texture load is buns */
	SDL_ShowWindow(w);

	return RString();
}

const RageDisplay::RagePixelFormatDesc* RageDisplay_SDL2::GetPixelFormatDesc(RagePixelFormat pf) const
{
	/* silly
 	 * this indirection is for individual drivers to offer a pixel format layout for
 	 * the 10 named RagePixelFormat(s). CreateSurface needs bpp & mask, not and index
 	 * so they need GetPixelFormatDesc to tell them what they are
 	 */

	return &pf_desc[pf];
}

int RageDisplay_SDL2::GetMaxTextureSize() const
{
	/* RageBitmapTexture::Create is essentially the only caller, it is very silly */
	static GLint sz;

	if(sz == 0)
	{
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &sz);
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "GL_MAX_TEXTURE_SIZE: %d", sz);
	}

	return sz;
}

int RageDisplay_SDL2::GetNumTextureUnits()
{
	HERE;

	/* only one caller: Model.cpp
	 * mesh is drawn 4 times with multitexture, 5 times without */
	return 4;
}

bool RageDisplay_SDL2::SupportsTextureFormat(RagePixelFormat, bool realtime)
{
	HERE;
	return true;
}

bool RageDisplay_SDL2::SupportsPerVertexMatrixScale()
{
	HERE;

	/* one caller: RageModelGeometry::LoadMilkshapeAscii
	 * it's the criteria for running MergeMeshes at the end
	 * since we're going to use vertex programs it's always yes */
	return true;
}

void RageDisplay_SDL2::ClearAllTextures()
{
	HERE;
}

uintptr_t RageDisplay_SDL2::CreateTexture(RagePixelFormat pf, RageSurface* s, bool bGenerateMipMaps)
{
	HERE;
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "0x%016" PRIXPTR ": w: %d, h: %d, mips %d", (uintptr_t)s, s->w, s->h, bGenerateMipMaps);
	bt();

	return (uintptr_t)s;
}

void RageDisplay_SDL2::UpdateTexture(uintptr_t iTexHandle, RageSurface* s, int xoffset, int yofffset, int width, int height)
{
	HERE;
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "tex: 0x%016" PRIXPTR " 0x%016" PRIXPTR ": w: %d, h: %d", iTexHandle, (uintptr_t)s, s->w, s->h);
}

void RageDisplay_SDL2::DeleteTexture(uintptr_t iTexHandle)
{
	HERE;
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "tex: 0x%016" PRIXPTR, iTexHandle);
}

void RageDisplay_SDL2::SetSphereEnvironmentMapping(TextureUnit, bool)
{
	HERE;
	/* only one caller: Model.cpp */
}

void RageDisplay_SDL2::SetTexture(TextureUnit tu, uintptr_t iTexHandle)
{
	HERE;
	bt();
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "tu: %d tex: 0x%016" PRIXPTR, tu, iTexHandle);
}

void RageDisplay_SDL2::SetTextureMode(TextureUnit tu, TextureMode tm)
{
	HERE;
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "tu: %d mode: %d", tu, tm);
}

void RageDisplay_SDL2::SetTextureWrapping(TextureUnit tu, bool wrap)
{
	HERE;
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "tu: %d wrap: %d", tu, wrap);
}

void RageDisplay_SDL2::SetTextureFiltering(TextureUnit tu, bool filter)
{
	HERE;
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "tu: %d filter: %d", tu, filter);
}

bool RageDisplay_SDL2::IsZTestEnabled() const
{
	HERE;
	/* nobody even calls this */
	return false;
}

bool RageDisplay_SDL2::IsZWriteEnabled() const
{
	HERE;
	/* nobody calls this either but the old non-working GLES driver thinks a depth clear doesn't work with it disabled */
	return false;
}

void RageDisplay_SDL2::SetZWrite(bool)
{
	HERE;
}

void RageDisplay_SDL2::SetZTestMode(ZTestMode)
{
	HERE;
}

void RageDisplay_SDL2::SetZBias(float)
{
	HERE;
}

void RageDisplay_SDL2::ClearZBuffer()
{
	HERE;
	/* fix the todo in NoteDisplay::DrawTapsInRange make the fast note rendering pref obsolete */
}

void RageDisplay_SDL2::SetCullMode(CullMode mode)
{
	HERE;
}

void RageDisplay_SDL2::SetAlphaTest(bool t)
{
	HERE;

	static bool seen;

	/* this is set to true every frame and isn't reachable via lua
	 * we only want to hear if someone enabled it at least once and ever disabled it
	 */

	if(t == true && seen == false)
	{
		seen = true;
	}

	if(seen == true && t == false)
	{
		/* should probably just assert */
		SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "BUG: find out who disabled the alpha test");
	}
}

void RageDisplay_SDL2::SetBlendMode(BlendMode mode)
{
	HERE;

	/* additive blending is used by Model and NoteDisplay for second texture passes (glow) */
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "blend: %d", mode);
}

void RageDisplay_SDL2::SetCelShaded(int stage)
{
	HERE;
	/* character shading, Model, BeginnerHelper and DancingCharacters.cpp behind pref */
}

void RageDisplay_SDL2::SetLighting(bool)
{
	HERE;
}

void RageDisplay_SDL2::SetLightOff(int index)
{
	HERE;
}

void RageDisplay_SDL2::SetLightDirectional(int index, const RageColor& ambient, const RageColor& diffuse, const RageColor& specular, const RageVector3& direction)\
{
	HERE;
}

void RageDisplay_SDL2::SetMaterial(const RageColor& emissive, const RageColor& ambient, const RageColor& diffuse, const RageColor& specular, float shininess)
{
	HERE;
}

RageCompiledGeometry* RageDisplay_SDL2::CreateCompiledGeometry()
{
	HERE;
	return nullptr;
}

void RageDisplay_SDL2::DeleteCompiledGeometry(RageCompiledGeometry*)
{
	HERE;
}

RageSurface* RageDisplay_SDL2::CreateScreenshot()
{
	HERE;
	return nullptr;
}

/* Draw*Internal gets a pointer v that's sufficient to track usage
 * this issues the proper draw call of everything we've prepared
 */
static void real_issue_draw()
{
}

void RageDisplay_SDL2::DrawQuadsInternal(const RageSpriteVertex v[], int iNumVerts)
{
	HERE;
	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "rv 0x%08X, n %d", v, iNumVerts);
}

void RageDisplay_SDL2::DrawQuadStripInternal(const RageSpriteVertex v[], int iNumVerts)
{
	HERE;
}

void RageDisplay_SDL2::DrawFanInternal(const RageSpriteVertex v[], int iNumVerts)
{
	HERE;
}

void RageDisplay_SDL2::DrawStripInternal(const RageSpriteVertex v[], int iNumVerts)
{
	HERE;
}

void RageDisplay_SDL2::DrawTrianglesInternal(const RageSpriteVertex v[], int iNumVerts)
{
	HERE;
}

void RageDisplay_SDL2::DrawCompiledGeometryInternal(const RageCompiledGeometry* p, int iMeshIndex)
{
	HERE;
}

void RageDisplay_SDL2::DrawSymmetricQuadStripInternal(const RageSpriteVertex v[], int iNumVerts)
{
	HERE;
}

static void service_events()
{
	/* pump the SDL event loop
	 * places things need to go:
	 * input -> InputHandler_SDL (when it exists)
	 * focused / quit / clipboard update -> ArchUtils_SDL (when it exists)
	 */

	SDL_Event e;

	while(SDL_PollEvent(&e))
	{
		switch(e.type)
		{
			case SDL_QUIT:
				HOOKS->SetUserQuit();
				break;

			case SDL_WINDOWEVENT:
				break;

			case SDL_KEYDOWN:
			case SDL_KEYUP:
				/* wire up enough of this to hit enter
				 * renderer doesn't have to work completely
				 * to start making it send other events
				 * with the keyboard
				 */
				break;

			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				break;

			case SDL_TEXTEDITING:
			case SDL_KEYMAPCHANGED:
				break;

			case SDL_AUDIODEVICEADDED:
			case SDL_AUDIODEVICEREMOVED:
				break;

			default:
				SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "unhandled SDL_Event: 0x%08x", e.type);
		}
	}

}

static void doodle_begin()
{
	/* place for us to scribble stuff */
}

bool RageDisplay_SDL2::BeginFrame()
{
	HERE;

	/* SDL has fewer events than win32 but i'm deeming this acceptable
	 * because the D3D driver does it, even though LowLevelDriver_Win32
	 * has a GraphicsWindow with a message loop
	 * - ohsix
	 */

	service_events();

	glClearColor(0, 1, 0, 0); /* does it matter if we clear the color buffer? it gets clobbered */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* used for startup time testing */
	//HOOKS->SetUserQuit();

	doodle_begin();

	/* makes redundant state changes and always returns true */
	RageDisplay::BeginFrame();
	
	/* we can return false here to skip a frame */
	return true;
}

static void doodle_end()
{
}

void RageDisplay_SDL2::EndFrame()
{
	HERE;

	doodle_end();

	/* present the magesty that is  a frame */
	SDL_GL_SwapWindow(w);

	RageDisplay::EndFrame();
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
