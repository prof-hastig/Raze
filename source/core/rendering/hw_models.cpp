// 
//---------------------------------------------------------------------------
//
// Copyright(C) 2005-2016 Christoph Oelckers
// All rights reserved.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/
//
//--------------------------------------------------------------------------
//
/*
** gl_models.cpp
**
** hardware renderer model handling code
**
**/

#include "filesystem.h"
#include "i_time.h"
#include "cmdlib.h"
#include "hw_material.h"
#include "hwrenderer/data/buffers.h"
#include "flatvertices.h"
#include "hw_renderstate.h"
#include "hw_models.h"

//CVAR(Bool, gl_light_models, true, CVAR_ARCHIVE)

VSMatrix FHWModelRenderer::GetViewToWorldMatrix()
{
	VSMatrix objectToWorldMatrix(0);
	//di->VPUniforms.mViewMatrix.inverseMatrix(objectToWorldMatrix);
	return objectToWorldMatrix;
}

void FHWModelRenderer::BeginDrawModel(FRenderStyle style, FSpriteModelFrame *smf, const VSMatrix &objectToWorldMatrix, bool mirrored)
{
	state.SetDepthFunc(DF_LEqual);
	state.EnableTexture(true);
	state.SetCulling(mirrored ? Cull_CCW : Cull_CW);

	state.mModelMatrix = objectToWorldMatrix;
	state.EnableModelMatrix(true);
}

void FHWModelRenderer::EndDrawModel(FRenderStyle style, FSpriteModelFrame *smf)
{
	state.EnableModelMatrix(false);
	state.SetDepthFunc(DF_Less);
	state.SetCulling(Cull_None);
}

void FHWModelRenderer::BeginDrawHUDModel(FRenderStyle style, const VSMatrix &objectToWorldMatrix, bool mirrored, FSpriteModelFrame*)
{
	state.SetDepthFunc(DF_LEqual);
	state.SetCulling(mirrored ? Cull_CCW : Cull_CW);
	state.mModelMatrix = objectToWorldMatrix;
	state.EnableModelMatrix(true);
}

void FHWModelRenderer::EndDrawHUDModel(FRenderStyle style, FSpriteModelFrame*)
{
	state.EnableModelMatrix(false);

	state.SetDepthFunc(DF_Less);
	state.SetCulling(Cull_None);
}

IModelVertexBuffer *FHWModelRenderer::CreateVertexBuffer(bool needindex, bool singleframe)
{
	return new FModelVertexBuffer(needindex, singleframe);
}

void FHWModelRenderer::SetInterpolation(double inter)
{
	state.SetInterpolationFactor((float)inter);
}

void FHWModelRenderer::SetMaterial(FGameTexture *skin, bool clampNoFilter, FTranslationID translation)
{
	state.SetMaterial(skin, UF_Skin, 0, clampNoFilter ? CLAMP_NOFILTER : CLAMP_NONE, translation.index(), -1);
	state.SetLightIndex(modellightindex);
}

void FHWModelRenderer::DrawArrays(int start, int count)
{
	state.Draw(DT_Triangles, start, count);
}

void FHWModelRenderer::DrawElements(int numIndices, size_t offset)
{
	state.DrawIndexed(DT_Triangles, int(offset / sizeof(unsigned int)), numIndices);
}

//===========================================================================
//
//
//
//===========================================================================

int FHWModelRenderer::SetupFrame(FModel* model, unsigned int frame1, unsigned int frame2, unsigned int size, const TArray<VSMatrix>& bones, int boneStartIndex)
{
	auto mdbuff = static_cast<FModelVertexBuffer*>(model->GetVertexBuffer(GetType()));
	state.SetVertexBuffer(mdbuff->vertexBuffer(), frame1, frame2);
	if (mdbuff->indexBuffer()) state.SetIndexBuffer(mdbuff->indexBuffer());
	return 0;
}

