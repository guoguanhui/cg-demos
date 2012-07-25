#pragma once

#include "MainApp/Application.h"
#include "Graphics/VertexDeclaration.h"
#include "Graphics/SimpleGeometry.h"

#pragma comment(lib, "RcEngine.lib")

using namespace RcEngine::Render;

class TestApp : public RcEngine::Application
{
public:
	TestApp(void);
	~TestApp(void);

protected:
	void Initialize();
	void LoadContent();
	void UnloadContent();
	void Render();
	void Update(float deltaTime);

private:
	shared_ptr<Effect> mEffect;
	EffectTechnique* mTechnique;
	shared_ptr<GraphicsBuffer> mVertexBuffer;
	shared_ptr<GraphicsBuffer> mIndexBuffer;
	shared_ptr<VertexDeclaration> mVertexDecl;
	shared_ptr<Texture> mTexture;
	shared_ptr<SimpleBox> mBox;
	shared_ptr<SimpleTexturedQuad> mQuad;
	shared_ptr<Mesh> mDwarf;
	shared_ptr<Material> mDwarfMaterial;
};
