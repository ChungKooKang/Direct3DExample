#pragma once
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "D3DFramwork.h"

#pragma comment (lib, "d3dcompiler.lib")

class DrawTriangle : public D3DFramwork
{
	struct VERTEX
	{
		FLOAT X, Y, Z;
		// FLOAT Color[4];
		FLOAT U, V;
	};
	
	struct MatrixBuffer
	{
		DirectX::XMMATRIX world;	//registerø° ¿÷¿Ω
	};

	Microsoft::WRL::ComPtr<ID3D11InputLayout>		mspInputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			mspVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		mspVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		mspPixelShader;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>			 mspTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mspTextureView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		 mspSamplerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState>		 mspBlendState;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			 mspConstantBuffer;

	float mX, mY;
	float mRotationZ;
	DirectX::XMMATRIX mWorld;

public :
	void Initialize(HINSTANCE hInstance, int width = 800, int height = 600) override;
	void Destroy() override;

private :
	void InitTriangle();
	void InitPipeline();

	HRESULT CreateTextureFromBMP();

protected :
	void Update(float delta) override;
	void Render() override;
};

