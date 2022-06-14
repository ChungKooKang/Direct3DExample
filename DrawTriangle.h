#pragma once
#include <d3dcompiler.h>

#include "D3DFramwork.h"

#pragma comment (lib, "d3dcompiler.lib")

class DrawTriangle : public D3DFramwork
{
	struct VERTEX
	{
		FLOAT X, Y, Z;
		FLOAT Color[4];
	};

	Microsoft::WRL::ComPtr<ID3D11InputLayout>		mspInputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			mspVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		mspVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		mspPixelShader;

public :
	void Initialize(HINSTANCE hInstance, int width = 800, int height = 600) override;
	void Destroy() override;

private :
	void InitTriangle();
	void InitPipeline();

protected :
	void Render() override;
};

