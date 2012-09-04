#include <Graphics/RenderFactory.h>
#include <Graphics/Material.h>
#include <Graphics/VertexDeclaration.h>
#include <IO/FileStream.h>
#include <Core/Exception.h>

namespace RcEngine {


RenderFactory::RenderFactory(void)
{
}


RenderFactory::~RenderFactory(void)
{

}

shared_ptr<Material> RenderFactory::CreateMaterialFromFile( const String& matName, const String& path )
{
	MaterialMapIter find = mMaterialPool.find(matName);
	if ( find == mMaterialPool.end())
	{
		FileStream file;
		if (!file.Open(path, FILE_READ))
		{
			ENGINE_EXCEPT(Exception::ERR_FILE_NOT_FOUND, 
				"Error: " + path + " not exits!", "RenderFactory::CreateMaterialFromFile");
		}
		mMaterialPool[matName] = Material::LoadFrom(file);

		//shared_ptr<Material> m = mMaterialPool[matName]->Clone();
	}		
	return mMaterialPool[matName];
}

shared_ptr<VertexDeclaration> RenderFactory::CreateVertexDeclaration( VertexElement* elems, uint32_t count )
{
	return std::make_shared<VertexDeclaration>(elems, count);
}

shared_ptr<VertexDeclaration> RenderFactory::CreateVertexDeclaration( const std::vector<VertexElement>& elems )
{
	return std::make_shared<VertexDeclaration>(elems);
}



} // Namespace RcEngine
