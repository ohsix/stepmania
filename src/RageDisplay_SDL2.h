/* RageDisplay_SDL2 - SDL2 base renderer */

#ifndef RAGE_DISPLAY_SDL2_H
#define RAGE_DISPLAY_SDL2_H

#include "RageDisplay.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <SDL2/SDL_image.h>

class RageDisplay_SDL2: public RageDisplay
{
public: /* non-optional and pure virtual methods */
	RString Init(const VideoModeParams& p, bool bAllowUnacceleratedRenderer) override;

	RageDisplay_SDL2();
	~RageDisplay_SDL2();

	RString GetApiDescription() const override;
	void GetDisplaySpecs(DisplaySpecs&) const override;
	ActualVideoModeParams GetActualVideoModeParams() const override;

	RString TryVideoMode(const VideoModeParams& p, bool& bNewDeviceOut) override;

	const RagePixelFormatDesc* GetPixelFormatDesc(RagePixelFormat) const override;
	int GetMaxTextureSize() const override;
	int GetNumTextureUnits() override;
	bool SupportsTextureFormat(RagePixelFormat, bool realtime = false) override;
	bool SupportsPerVertexMatrixScale() override;

	void ClearAllTextures() override;
	uintptr_t CreateTexture(RagePixelFormat, RageSurface*, bool bGenerateMipMaps) override;
	void UpdateTexture(uintptr_t iTexHandle, RageSurface*, int xoffset, int yofffset, int width, int height) override;
	void DeleteTexture(uintptr_t iTexHandle) override;

	void SetSphereEnvironmentMapping(TextureUnit, bool) override;
	void SetTexture(TextureUnit, uintptr_t) override;
	void SetTextureMode(TextureUnit, TextureMode) override;
	void SetTextureWrapping(TextureUnit, bool) override;
	void SetTextureFiltering(TextureUnit, bool) override;

	bool IsZTestEnabled() const override;
	bool IsZWriteEnabled() const override;
	void SetZWrite(bool) override;
	void SetZTestMode(ZTestMode) override;
	void SetZBias(float) override;
	void ClearZBuffer() override;

	void SetCullMode(CullMode mode) override;
	void SetAlphaTest(bool) override;
	void SetBlendMode(BlendMode mode) override;
	void SetCelShaded(int stage) override;

	void SetLighting(bool) override;
	void SetLightOff(int index) override;
	void SetLightDirectional(int index, const RageColor& ambient, const RageColor& diffuse, const RageColor& specular, const RageVector3& direction) override;
	void SetMaterial(const RageColor& emissive, const RageColor& ambient, const RageColor& diffuse, const RageColor& specular, float shininess) override;

	RageCompiledGeometry* CreateCompiledGeometry() override;
	void DeleteCompiledGeometry(RageCompiledGeometry*) override;

	RageSurface* CreateScreenshot() override;

protected:
	void DrawQuadsInternal(const RageSpriteVertex v[], int iNumVerts) override;
	void DrawQuadStripInternal(const RageSpriteVertex v[], int iNumVerts) override;
	void DrawFanInternal(const RageSpriteVertex v[], int iNumVerts) override;
	void DrawStripInternal(const RageSpriteVertex v[], int iNumVerts) override;
	void DrawTrianglesInternal(const RageSpriteVertex v[], int iNumVerts) override;
	void DrawCompiledGeometryInternal(const RageCompiledGeometry* p, int iMeshIndex) override;
	void DrawSymmetricQuadStripInternal(const RageSpriteVertex v[], int iNumVerts) override;

public: /* non pure virtual, optional methods */
	bool BeginFrame() override;
	void EndFrame() override;

private: /* our stuff */
	SDL_Window* w;
	SDL_GLContext gl;
	VideoModeParams activeMode;
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
