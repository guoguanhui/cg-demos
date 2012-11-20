#include <Graphics/SpriteBatch.h>
#include <Graphics/Renderable.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/Material.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Texture.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/Font.h>
#include <Graphics/Camera.h>
#include <Graphics/RenderOperation.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneObject.h>
#include <Scene/SceneNode.h>
#include <Resource/ResourceManager.h>
#include <Math/MathUtil.h>
#include <Core/Context.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>


namespace {

using namespace RcEngine;

template<typename Real>
static void Rotate(Vector<Real,2>& point, const Vector<Real,2>& origin, Real rotation)
{
	Real cosAngle = cos(rotation);
	Real sinAngle = sin(rotation);

	Real newX, newY;

	if (origin.X() == Real(0) && origin.Y() == Real(0))
	{
		newX = point.X() * cosAngle - point.Y() * sinAngle;
		newY = point.X() * sinAngle + point.Y() * cosAngle;

	}
	else
	{
		point -= origin;
		newX = point.X() * cosAngle - point.Y() * sinAngle + origin.X();
		newY = point.X() * sinAngle + point.Y() * cosAngle + origin.Y();
	}	

	point = Vector<Real,2>(newX, newY);
}

}

namespace RcEngine {

struct SpriteVertex
{
	Vector3f Position;
	Vector2f TexCoord;
	ColorRGBA Color;
};

class SpriteEntity : public Renderable, public SceneObject
{
public:
	SpriteEntity(const shared_ptr<Texture>& texture, const shared_ptr<Material>& mat)
		: SceneObject("Sprite"), mDirty(true)
	{
		RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

		mType = SOT_Sprite;
		mRenderable = true;

		mSpriteTexture = texture;
		mSpriteMaterial = mat;

		mWindowSizeParam = mSpriteMaterial->GetCustomParameter("WindowSize");

		mSpriteMaterial->SetTexture("SpriteTexture", texture);


		mRenderOperation = std::make_shared<RenderOperation>();
		mRenderOperation->BaseVertexLocation = 0;
		mRenderOperation->StartIndexLocation = 0;
		mRenderOperation->StartVertexLocation = 0;
		mRenderOperation->PrimitiveType = PT_Triangle_List;

		// create index buffer 
		mIndexBuffer = factory.CreateIndexBuffer(BU_Dynamic, EAH_CPU_Write | EAH_GPU_Read, NULL);

		// create vertex buffer
		mVertexBuffer = factory.CreateVertexBuffer(BU_Dynamic, EAH_CPU_Write | EAH_GPU_Read, NULL);

		// bind vertex stream
		VertexElement elements[3] = 
		{
			VertexElement(0, VEF_Float3, VEU_Position, 0),
			VertexElement(sizeof(Vector3f), VEF_Float2, VEU_TextureCoordinate, 0),
			VertexElement(sizeof(Vector3f) + sizeof(Vector2f), VEF_Float4, VEU_Color, 0)
		};

		mRenderOperation->BindVertexStream(mVertexBuffer, factory.CreateVertexDeclaration(elements, 3));
		mRenderOperation->BindIndexStream(mIndexBuffer, IBT_Bit16);

		// add to scene graph
		SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
		SceneNode* sceneNode = sceneMan.GetRootSceneNode();
		sceneNode->AttachObject(this);
	}

	~SpriteEntity()
	{
		Context::GetSingleton().GetSceneManager().GetRootSceneNode()->DetachOject(this);
	}

	const shared_ptr<Material>& GetMaterial() const		{ return mSpriteMaterial; }
	EffectTechnique* GetTechnique() const	{ return mSpriteMaterial->GetCurrentTechnique(); }

	uint32_t GetWorldTransformsCount() const 
	{
		// no world transform
		return 0;
	}

	void GetWorldTransforms(Matrix4f* xform) const
	{

	}

	const shared_ptr<RenderOperation>& GetRenderOperation() const
	{
		return mRenderOperation;
	}

	void OnRenderBegin()
	{
		Renderable::OnRenderBegin();

		auto frameBuffer = Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer();
		auto w = frameBuffer->GetWidth();
		auto h = frameBuffer->GetHeight();
		mWindowSizeParam->SetValue(Vector2f(float(frameBuffer->GetWidth()), float(frameBuffer->GetHeight())));

	}

	void OnUpdateRenderQueue( RenderQueue* renderQueue, Camera* cam, RenderOrder order )
	{
		UpdateGeometryBuffers();

		RenderQueueItem item;
		item.Renderable = this;
		item.Type = SOT_Sprite;

		// ignore render order, only handle state change order
		item.SortKey = (float)mSpriteMaterial->GetEffect()->GetResourceHandle();

		renderQueue->AddToQueue(item, RenderQueue::BucketGui);
	}

	void SetProjectionMatrix(const Matrix4f& mat)
	{
		mSpriteMaterial->GetCustomParameter("ProjMat")->SetValue(mat);
	}

	void UpdateGeometryBuffers()
	{
		if (mDirty)
		{
			if (mVertices.size() && mInidces.size())
			{
				uint32_t vbSize = sizeof(SpriteVertex) * mVertices.size();
				mVertexBuffer->ResizeBuffer(vbSize);

				uint8_t* vbData = (uint8_t*)mVertexBuffer->Map(0, vbSize, BA_Read_Write);
				memcpy(vbData, (uint8_t*)&mVertices[0], vbSize);
				mVertexBuffer->UnMap();

				uint32_t ibSize = sizeof(uint16_t) * mInidces.size();
				mIndexBuffer->ResizeBuffer(ibSize);

				uint8_t* ibData = (uint8_t*)mIndexBuffer->Map(0, ibSize, BA_Read_Write);
				memcpy(ibData, (uint8_t*)&mInidces[0], ibSize);
				mIndexBuffer->UnMap();
			}

			mDirty = false;
		}
	}

	bool Empty() const 
	{
		return mInidces.empty();
	}

	void ClearAll()
	{
		mVertices.resize(0);
		mInidces.resize(0);
	}

	vector<SpriteVertex>& GetVertices()
	{
		mDirty = true;
		return mVertices;
	}

	vector<uint16_t>& GetIndices()
	{
		mDirty = true;
		return mInidces;
	}

private:
	shared_ptr<Texture> mSpriteTexture;
	shared_ptr<Material> mSpriteMaterial;
	shared_ptr<RenderOperation> mRenderOperation;
	vector<SpriteVertex> mVertices;
	vector<uint16_t> mInidces;

	shared_ptr<GraphicsBuffer> mVertexBuffer;
	shared_ptr<GraphicsBuffer> mIndexBuffer;

	EffectParameter* mWindowSizeParam;

	bool mDirty;
};

//////////////////////////////////////////////////////////////////////////
SpriteBatch::SpriteBatch()
{
	mSpriteMaterial = std::static_pointer_cast<Material>(
		ResourceManager::GetSingleton().GetResourceByName(ResourceTypes::Material, "Sprite.material.xml", "General"));

	mSpriteMaterial->Load();
}	

SpriteBatch::~SpriteBatch()
{

}

void SpriteBatch::Begin( )
{
	for (auto iter = mBatches.begin(); iter != mBatches.end(); ++iter)
	{
		iter->second->ClearAll();
	}
	
}

void SpriteBatch::End()
{

}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const IntRect& dest, IntRect* src, const ColorRGBA& color, float rotAngle /*= 0*/, const Vector2f& origin /*= Vector2f::Zero()*/, float layerDepth /*= 0.0f*/ )
{
	if (color.A() <= 0)
		return;

	uint32_t texWidth = texture->GetWidth(0);
	uint32_t texHeight = texture->GetHeight(0);

	IntRect srcRect = src ? (*src) : IntRect(0, 0, texWidth, texHeight);

	SpriteEntity* spriteEntity = nullptr;
	if (mBatches.find(texture) == mBatches.end())
	{
		spriteEntity = new SpriteEntity(texture, std::static_pointer_cast<Material>(mSpriteMaterial->Clone()));
		mBatches[texture] = spriteEntity;
	}
	else
	{
		spriteEntity = static_cast<SpriteEntity* >(mBatches[texture]);
	}
	assert(spriteEntity);

	Vector2f topLeft = Vector2f((float)dest.X, (float)dest.Y);
	Vector2f topRight = Vector2f(float(dest.X + dest.Width), (float)dest.Y);
	Vector2f bottomLeft = Vector2f((float)dest.X, float(dest.Y + dest.Height));
	Vector2f bottomRight = Vector2f(float(dest.X + dest.Width), float(dest.Y + dest.Height));

	Vector2f pivotPoint(origin);
	pivotPoint.X() *= dest.Width;
	pivotPoint.Y() *= dest.Height;
	pivotPoint.X() += dest.X;
	pivotPoint.Y() += dest.Y;
	
	Rotate(topLeft, pivotPoint, rotAngle);
	Rotate(topRight, pivotPoint, rotAngle);
	Rotate(bottomLeft, pivotPoint, rotAngle);
	Rotate(bottomRight, pivotPoint, rotAngle);


	// Calculate the points on the texture
	float u1 = (float)srcRect.X / (float)texWidth;
	float v1 = 1.0f - (float)srcRect.Y / (float)texHeight;
	float u2 = u1 + (float)srcRect.Width / texWidth;
	float v2 = v1 - (float)srcRect.Height / texHeight;

	vector<SpriteVertex>& vertices = spriteEntity->GetVertices();
	vector<uint16_t>& indices = spriteEntity->GetIndices();

	uint16_t lastIndex = static_cast<uint16_t>(vertices.size());

	SpriteVertex spriteVertex;
	vertices.reserve(vertices.size() + 4);
	
	spriteVertex.Position = Vector3f(topLeft.X(), topLeft.Y(), layerDepth);
	spriteVertex.TexCoord = Vector2f(u1, v1);
	spriteVertex.Color = color;
	vertices.push_back(spriteVertex);

	spriteVertex.Position = Vector3f(bottomLeft.X(), bottomLeft.Y(), layerDepth);
	spriteVertex.TexCoord = Vector2f(u1, v2);
	spriteVertex.Color = color;
	vertices.push_back(spriteVertex);

	spriteVertex.Position = Vector3f(bottomRight.X(), bottomRight.Y(), layerDepth);
	spriteVertex.TexCoord = Vector2f(u2, v2);
	spriteVertex.Color = color;
	vertices.push_back(spriteVertex);

	spriteVertex.Position = Vector3f(topRight.X(), topRight.Y(), layerDepth);
	spriteVertex.TexCoord = Vector2f(u2, v1);
	spriteVertex.Color = color;
	vertices.push_back(spriteVertex);
	
	indices.reserve(indices.size() + 6);
	indices.push_back(lastIndex + 0);
	indices.push_back(lastIndex + 1);
	indices.push_back(lastIndex + 2);
	indices.push_back(lastIndex + 2);
	indices.push_back(lastIndex + 3);
	indices.push_back(lastIndex + 0);
}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const Vector2f& position, const ColorRGBA& color )
{
	Draw(texture, position, nullptr, color);
}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const Vector2f& position, IntRect* sourceRectangle, const ColorRGBA& color )
{
	IntRect destination = IntRect((int32_t)position.X(), (int)position.Y(), texture->GetWidth(0), texture->GetHeight(0));
	Draw(texture, destination, sourceRectangle, color, 0.0f, Vector2f::Zero(), 0.0f);
}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const IntRect& destinationRectangle, const ColorRGBA& color )
{
	Draw(texture, destinationRectangle, NULL, color, 0.0f, Vector2f::Zero(), 0.0f);
}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const IntRect& destinationRectangle, IntRect* sourceRectangle, const ColorRGBA& color )
{
	Draw(texture, destinationRectangle, sourceRectangle, color, 0.0f, Vector2f::Zero(), 0.0f);
}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const Vector2f& position, IntRect* sourceRectangle, const ColorRGBA& color, float rotation, const Vector2f& origin, float scale, float layerDepth )
{
	int32_t width, height;
	if (sourceRectangle)
	{
		width = (int32_t)(sourceRectangle->Width * scale);
		height = (int32_t)(sourceRectangle->Height * scale);
	}
	else
	{
		width = (int32_t)(texture->GetWidth(0) * scale);
		height = (int32_t)(texture->GetHeight(0) * scale);
	}

    IntRect destination = IntRect((int)position.X(), (int)position.Y(), width, height);
	Draw(texture, destination, sourceRectangle, color, rotation, Vector2f::Zero(), 0.0f);
}

void SpriteBatch::Draw( const shared_ptr<Texture>& texture, const Vector2f& position, IntRect* sourceRectangle, const ColorRGBA& color, float rotation, const Vector2f& origin, const Vector2f& scale, float layerDepth )
{
	int32_t width, height;
	if (sourceRectangle)
	{
		width = (int32_t)(sourceRectangle->Width * scale.X());
		height = (int32_t)(sourceRectangle->Height * scale.Y());
	}
	else
	{
		width = (int32_t)(texture->GetWidth(0) * scale.X());
		height = (int32_t)(texture->GetHeight(0) * scale.Y());
	}

	IntRect destination = IntRect((int)position.X(), (int)position.Y(), width, height);
	Draw(texture, destination, sourceRectangle, color, rotation, Vector2f::Zero(), 0.0f);
}

void SpriteBatch::Flush()
{
	//SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	//SceneNode* sceneNode = sceneMan.GetRootSceneNode();

	// update parameter
	//Window* mainWindow =  Context::GetSingleton().GetApplication().GetMainWindow();
	//mProjectionMatrix = CreateOrthographicLH(float(mainWindow->GetWidth()), float(mainWindow->GetHeight()), float(-1), float(1));
	//Context::GetSingleton().GetRenderDevice().AdjustProjectionMatrix(mProjectionMatrix);

	for (auto iter = mBatches.begin(); iter != mBatches.end(); ++iter)
	{
		if (!iter->second->Empty())
		{
			//iter->second->SetProjectionMatrix(mProjectionMatrix);
		}	
		else
		{
			iter->second->SetVisible(false);
		}
	}
}

}