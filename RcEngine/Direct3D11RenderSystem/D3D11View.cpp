#include "D3D11View.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Texture.h"
#include "D3D11Buffer.h"
#include "D3D11Device.h"

namespace RcEngine {

D3D11BufferSRV::D3D11BufferSRV( const shared_ptr<RHBuffer>& buffer )
	: RHBufferSRV(buffer),
	  ShaderResourceViewD3D11(nullptr)
{

}

D3D11BufferSRV::~D3D11BufferSRV()
{
	SAFE_RELEASE(ShaderResourceViewD3D11);
}

D3D11StructuredBufferSRV::D3D11StructuredBufferSRV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount )
	: D3D11BufferSRV(buffer)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;

	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = elementCount;

	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;
	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(bufferD3D11, &desc, &ShaderResourceViewD3D11);
}

//////////////////////////////////////////////////////////////////////////
D3D11TextureBufferSRV::D3D11TextureBufferSRV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format )
	: D3D11BufferSRV(buffer)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;

	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Format = D3D11Mapping::Mapping(format);
	desc.Buffer.ElementOffset = 0;
	desc.Buffer.ElementWidth = elementCount;
	
	//assert(PixelFormatUtils::GetNumElemBytes(format) * elementCount == buffer->GetBufferSize());

	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;
	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(bufferD3D11, &desc, &ShaderResourceViewD3D11);
}

//////////////////////////////////////////////////////////////////////////
D3D11TextureSRV::D3D11TextureSRV( const shared_ptr<RHTexture>& texture )
	: RHTextureSRV(texture),
	  ShaderResourceViewD3D11(nullptr)
{

}

D3D11TextureSRV::~D3D11TextureSRV()
{
	SAFE_RELEASE(ShaderResourceViewD3D11);
}


D3D11Texture1DSRV::D3D11Texture1DSRV( const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
	: D3D11TextureSRV(texture)
{
	assert(mipLevels > 0 && arraySize > 0);

	uint32_t createFlags = texture->GetCreateFlags();
	assert(createFlags & TexCreate_ShaderResource);

	D3D11Texture1D* textureD3D11 = static_cast_checked<D3D11Texture1D*>(texture.get());

	D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;
	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());

	if (textureD3D11->GetTextureArraySize() <= 1)
	{	
		viewDesc.Texture1D.MostDetailedMip = mostDetailedMip;
		viewDesc.Texture1D.MipLevels = mipLevels;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;	
	}
	else
	{
		viewDesc.Texture1DArray.FirstArraySlice = firstArraySlice;
		viewDesc.Texture1DArray.ArraySize = arraySize;
		viewDesc.Texture1DArray.MostDetailedMip = mostDetailedMip;
		viewDesc.Texture1DArray.MipLevels = mipLevels;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
	}

	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, &ShaderResourceViewD3D11);
}


D3D11Texture2DSRV::D3D11Texture2DSRV( const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
	: D3D11TextureSRV(texture)
{
	assert(mipLevels > 0 && arraySize > 0);

	uint32_t createFlags = texture->GetCreateFlags();
	assert(createFlags & TexCreate_ShaderResource);

	D3D11Texture2D* textureD3D11 = static_cast_checked<D3D11Texture2D*>(texture.get());

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());

	if (textureD3D11->GetTextureArraySize() <= 1)
	{
		if (textureD3D11->GetSampleCount() <= 1)
		{
			viewDesc.Texture2D.MostDetailedMip = mostDetailedMip;
			viewDesc.Texture2D.MipLevels = mipLevels;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		}
		else
		{
			assert(createFlags & TexCreate_RenderTarget);
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		}
	}
	else
	{
		if (textureD3D11->GetSampleCount() <= 1)
		{
			viewDesc.Texture2DArray.FirstArraySlice = firstArraySlice;
			viewDesc.Texture2DArray.ArraySize = arraySize;
			viewDesc.Texture2DArray.MostDetailedMip = mostDetailedMip;
			viewDesc.Texture2DArray.MipLevels = mipLevels;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		}
		else
		{
			viewDesc.Texture2DMSArray.ArraySize = firstArraySlice;
			viewDesc.Texture2DMSArray.FirstArraySlice = arraySize;
			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
		}
	}

	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, &ShaderResourceViewD3D11);
}

D3D11Texture3DSRV::D3D11Texture3DSRV( const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels )
: D3D11TextureSRV(texture)
{
	assert(mipLevels > 0);

	uint32_t createFlags = texture->GetCreateFlags();
	assert(createFlags & TexCreate_ShaderResource);

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
	viewDesc.Texture3D.MostDetailedMip = mostDetailedMip;
	viewDesc.Texture3D.MipLevels = mipLevels;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;

	D3D11Texture3D* textureD3D11 = static_cast_checked<D3D11Texture3D*>(texture.get());
	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, &ShaderResourceViewD3D11);
}

D3D11TextureCubeSRV::D3D11TextureCubeSRV( const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize )
	: D3D11TextureSRV(texture)
{
	assert(mipLevels > 0 && arraySize > 0);

	uint32_t createFlags = texture->GetCreateFlags();
	assert(createFlags & TexCreate_ShaderResource);

	D3D11TextureCube* textureD3D11 = static_cast_checked<D3D11TextureCube*>(texture.get());

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());

	if (textureD3D11->GetTextureArraySize() <= 1)
	{
		viewDesc.TextureCube.MostDetailedMip = mostDetailedMip;
		viewDesc.TextureCube.MipLevels = mipLevels;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	}
	else
	{
		// Need test
		assert(false);
		viewDesc.TextureCubeArray.MostDetailedMip = mostDetailedMip;
		viewDesc.TextureCubeArray.MipLevels = mipLevels;
		viewDesc.TextureCubeArray.First2DArrayFace = firstArraySlice;
		viewDesc.TextureCubeArray.NumCubes = arraySize;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
	}

	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, &ShaderResourceViewD3D11);
}

//bool CreateTexture1DSRV( const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV )
//{
//	assert(mipLevels > 0 && arraySize > 0);
//
//	uint32_t createFlags = texture->GetCreateFlags();
//	assert(createFlags & TexCreate_ShaderResource);
//
//	D3D11Texture1D* textureD3D11 = static_cast_checked<D3D11Texture1D*>(texture.get());
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC  viewDesc;
//	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
//
//	if (textureD3D11->GetTextureArraySize() <= 1)
//	{	
//		viewDesc.Texture1D.MostDetailedMip = mostDetailedMip;
//		viewDesc.Texture1D.MipLevels = mipLevels;
//		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;	
//	}
//	else
//	{
//		viewDesc.Texture1DArray.FirstArraySlice = firstArraySlice;
//		viewDesc.Texture1DArray.ArraySize = arraySize;
//		viewDesc.Texture1DArray.MostDetailedMip = mostDetailedMip;
//		viewDesc.Texture1DArray.MipLevels = mipLevels;
//		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
//	}
//
//	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
//	
//	return SUCCEEDED(hr);
//}
//
//bool CreateTexture2DSRV( const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV )
//{
//	assert(mipLevels > 0 && arraySize > 0);
//
//	uint32_t createFlags = texture->GetCreateFlags();
//	assert(createFlags & TexCreate_ShaderResource);
//
//	D3D11Texture2D* textureD3D11 = static_cast_checked<D3D11Texture2D*>(texture.get());
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
//	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
//
//	if (textureD3D11->GetTextureArraySize() <= 1)
//	{
//		if (textureD3D11->GetSampleCount() <= 1)
//		{
//			viewDesc.Texture2D.MostDetailedMip = mostDetailedMip;
//			viewDesc.Texture2D.MipLevels = mipLevels;
//			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//		}
//		else
//		{
//			assert(createFlags & TexCreate_RenderTarget);
//			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
//		}
//	}
//	else
//	{
//		if (textureD3D11->GetSampleCount() <= 1)
//		{
//			viewDesc.Texture2DArray.FirstArraySlice = firstArraySlice;
//			viewDesc.Texture2DArray.ArraySize = arraySize;
//			viewDesc.Texture2DArray.MostDetailedMip = mostDetailedMip;
//			viewDesc.Texture2DArray.MipLevels = mipLevels;
//			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
//		}
//		else
//		{
//			viewDesc.Texture2DMSArray.ArraySize = firstArraySlice;
//			viewDesc.Texture2DMSArray.FirstArraySlice = arraySize;
//			viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
//		}
//	}
//
//	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
//	return SUCCEEDED(hr);
//}
//
//bool CreateTexture3DSRV( const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV )
//{
//	assert(mipLevels > 0);
//
//	uint32_t createFlags = texture->GetCreateFlags();
//	assert(createFlags & TexCreate_ShaderResource);
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
//	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
//	viewDesc.Texture3D.MostDetailedMip = mostDetailedMip;
//	viewDesc.Texture3D.MipLevels = mipLevels;
//	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
//
//	D3D11Texture3D* textureD3D11 = static_cast_checked<D3D11Texture3D*>(texture.get());
//	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
//	return SUCCEEDED(hr);
//}
//
//bool CreateTextureCubeSRV( const shared_ptr<RHTexture>& texture, uint32_t mostDetailedMip, uint32_t mipLevels, uint32_t firstArraySlice, uint32_t arraySize, ID3D11ShaderResourceView** pTextureSRV )
//{
//	assert(mipLevels > 0 && arraySize > 0);
//
//	uint32_t createFlags = texture->GetCreateFlags();
//	assert(createFlags & TexCreate_ShaderResource);
//
//	D3D11TextureCube* textureD3D11 = static_cast_checked<D3D11TextureCube*>(texture.get());
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
//	viewDesc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
//
//	if (textureD3D11->GetTextureArraySize() <= 1)
//	{
//		viewDesc.TextureCube.MostDetailedMip = mostDetailedMip;
//		viewDesc.TextureCube.MipLevels = mipLevels;
//		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
//	}
//	else
//	{
//		// Need test
//		assert(false);
//		viewDesc.TextureCubeArray.MostDetailedMip = mostDetailedMip;
//		viewDesc.TextureCubeArray.MipLevels = mipLevels;
//		viewDesc.TextureCubeArray.First2DArrayFace = firstArraySlice;
//		viewDesc.TextureCubeArray.NumCubes = arraySize;
//		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
//	}
//
//	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateShaderResourceView(textureD3D11->TextureD3D11, &viewDesc, pTextureSRV);
//	return SUCCEEDED(hr);
//}

//////////////////////////////////////////////////////////////////////////
D3D11BufferUAV::D3D11BufferUAV( const shared_ptr<RHBuffer>& buffer )
	: RHBufferUAV(buffer),
	UnorderedAccessViewD3D11(nullptr)
{

}

D3D11BufferUAV::~D3D11BufferUAV()
{
	SAFE_RELEASE(UnorderedAccessViewD3D11);
}


D3D11TextureBufferUAV::D3D11TextureBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount, PixelFormat format )
	: D3D11BufferUAV(buffer)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;

	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Format = D3D11Mapping::Mapping(format);
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = elementCount;
	desc.Buffer.Flags = 0;

	// Not supported
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW ;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;
	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateUnorderedAccessView(bufferD3D11, &desc, &UnorderedAccessViewD3D11);
}


D3D11StructuredBufferUAV::D3D11StructuredBufferUAV( const shared_ptr<RHBuffer>& buffer, uint32_t elementCount )
	: D3D11BufferUAV(buffer)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;

	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = elementCount;
	desc.Buffer.Flags = 0;

	// Not supported
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW ;
	//desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	ID3D11Buffer* bufferD3D11 = (static_cast<D3D11Buffer*>(buffer.get()))->BufferD3D11;
	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateUnorderedAccessView(bufferD3D11, &desc, &UnorderedAccessViewD3D11);
}

//////////////////////////////////////////////////////////////////////////
D3D11TextureUAV::D3D11TextureUAV( const shared_ptr<RHTexture>& texture )
	: RHTextureUAV(texture),
	UnorderedAccessViewD3D11(nullptr)
{

}

D3D11TextureUAV::~D3D11TextureUAV()
{
	SAFE_RELEASE(UnorderedAccessViewD3D11);
}

D3D11Texture2DUAV::D3D11Texture2DUAV( const shared_ptr<RHTexture>& texture, uint32_t level )
	: D3D11TextureUAV(texture)
{
	D3D11Texture2D* textureD3D11 = (static_cast<D3D11Texture2D*>(texture.get()));

	uint32_t createFlags = textureD3D11->GetCreateFlags();

	assert(textureD3D11->GetTextureArraySize() <= 1);
	assert(createFlags & TexCreate_UAV);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;

	desc.Format = D3D11Mapping::Mapping(texture->GetTextureFormat());
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = level;

	HRESULT hr = gD3D11Device->GetDeviceD3D11()->CreateUnorderedAccessView(textureD3D11->TextureD3D11, &desc, &UnorderedAccessViewD3D11);
}





}