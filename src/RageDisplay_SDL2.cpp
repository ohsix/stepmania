#include "global.h"

#include "RageDisplay.h"
#include "RageDisplay_SDL2.h"

#include "DisplaySpec.h"

/* only rough concern for other compilers rn */
#ifdef __FUNCSIG__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

/* not using LOG / RageLog for now*/
#define HERE SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, __PRETTY_FUNCTION__)

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

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, US "audio init failed: %s\n", SDL_GetError());
		return SDL_GetError();
	}

	init_pfmap(); /* i refuse an ugly table when sdl already has good pixel format handling */

	SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "doggie:\t%d\n", 54);

	bool nope = false;
	return SetVideoMode(p, nope);
}

RageDisplay_SDL2::RageDisplay_SDL2()
{
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE); /* belongs elsewere */
	HERE;

	/* judging by the names they all want answers before anything */
	vidMode =
		VideoModeParams(
			true, /* windowed */
			"display id?",
			640U, /* width */
			480U, /* height */
			32U, /* bpp */
			60U, /* hz */
			false, /* vsync */
			false, /* interlaced */
			false, /* smooth lines */
			false, /* trilinear filtering */
			false, /* anisotropic filtering */
			false, /* is full screen borderless */
			"window title",
			"icon file",
			false, /* paletted textures */
			640.0f/480.0f); /* aspect ratio */

}

RageDisplay_SDL2::~RageDisplay_SDL2()
{
	HERE;
}

RString RageDisplay_SDL2::GetApiDescription() const
{
	HERE;

	return "SDL2";
}

void RageDisplay_SDL2::GetDisplaySpecs(DisplaySpecs& out) const
{
	/* not using LowLevelWindow, SDL can do it all */
	out.clear();
	DisplayMode nullMode = { 640U, 480U, 30.0 };
	DisplaySpec nullSpec("SDL2 Placeholder", "foo", nullMode);
	out.insert(nullSpec);
}

ActualVideoModeParams RageDisplay_SDL2::GetActualVideoModeParams() const
{
	return vidMode;
}

RString RageDisplay_SDL2::TryVideoMode(const VideoModeParams& p, bool& bNewDeviceOut)
{
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
	HERE;
	return 4096;
}

int RageDisplay_SDL2::GetNumTextureUnits()
{
	HERE;
	return 4;
}

bool RageDisplay_SDL2::SupportsTextureFormat(RagePixelFormat, bool realtime)
{
	HERE;
	return false;
}

bool RageDisplay_SDL2::SupportsPerVertexMatrixScale()
{
	HERE;
	return false;
}

void RageDisplay_SDL2::ClearAllTextures()
{
	HERE;
}

uintptr_t RageDisplay_SDL2::CreateTexture(RagePixelFormat, RageSurface*, bool bGenerateMipMaps)
{
	HERE;
}

void RageDisplay_SDL2::UpdateTexture(uintptr_t iTexHandle, RageSurface*, int xoffset, int yofffset, int width, int height)
{
	HERE;
}

void RageDisplay_SDL2::DeleteTexture(uintptr_t iTexHandle)
{
	HERE;
}

void RageDisplay_SDL2::SetSphereEnvironmentMapping(TextureUnit, bool)
{
	HERE;
}

void RageDisplay_SDL2::SetTexture(TextureUnit, uintptr_t)
{
	HERE;
}

void RageDisplay_SDL2::SetTextureMode(TextureUnit, TextureMode)
{
	HERE;
}

void RageDisplay_SDL2::SetTextureWrapping(TextureUnit, bool)
{
	HERE;
}

void RageDisplay_SDL2::SetTextureFiltering(TextureUnit, bool)
{
	HERE;
}

bool RageDisplay_SDL2::IsZTestEnabled() const
{
	HERE;
	return false;
}

bool RageDisplay_SDL2::IsZWriteEnabled() const
{
	HERE;
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
}

void RageDisplay_SDL2::SetCullMode(CullMode mode)
{
	HERE;
}

void RageDisplay_SDL2::SetAlphaTest(bool)
{
	HERE;
}

void RageDisplay_SDL2::SetBlendMode(BlendMode mode)
{
	HERE;
}

void RageDisplay_SDL2::SetCelShaded(int stage)
{
	HERE;
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

/* specular *and* shininess? */
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

void RageDisplay_SDL2::DrawQuadsInternal(const RageSpriteVertex v[], int iNumVerts)
{
	HERE;
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
