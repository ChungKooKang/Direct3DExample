#include <vector>
#include<fstream>
#include "DrawTriangle.h"

void DrawTriangle::Initialize(HINSTANCE hInstance, int width, int height)
{
	D3DFramwork::Initialize(hInstance, width, height);

	InitPipeline();
	InitTriangle();

	CreateTextureFromBMP();
}

void DrawTriangle::Destroy()
{
	mspTextureView.Reset();
	mspTexture.Reset();
	mspVertexBuffer.Reset();
	mspInputLayout.Reset();
	mspPixelShader.Reset();
	mspVertexShader.Reset();

	D3DFramwork::Destroy();
}

void DrawTriangle::InitTriangle()
{
	VERTEX vertices[]
	{
		{ -0.45f, 0.5f, 0.0f, 0.0f, 0.0f },
		{ 0.45f, 0.5f, 0.0f, 1.0f, 0.0f },
		{ -0.45f, -0.5f, 0.0f, 0.0f, 1.0f },
		{ 0.45f, -0.5f, 0.0f, 1.0f, 1.0f }
	};

	CD3D11_BUFFER_DESC bd(
		sizeof(VERTEX) * 4,// �츮�� ������� �迭�� �� ����Ʈ �ΰ�?
		D3D11_BIND_VERTEX_BUFFER,	// bind flag ��� ����� ������ ������ �˷��ִ� ��
		D3D11_USAGE_DYNAMIC,		// usage ��� ��� ��� ����� ������ ( GPU �б�, CPU ���� ) => �� �̷��� �ϳĸ� CPU�� GPU�� ���� ������ �� �Ǿ� �ֱ� ������ �� �κп� �� ���� ������ ����Ѵ�.
		D3D11_CPU_ACCESS_WRITE		// CPU  Acccess : CPU�� ���� ������ �� �ִ�.
	);

	mspDevice->CreateBuffer(&bd, nullptr, mspVertexBuffer.ReleaseAndGetAddressOf());

	// �� ó�� �ص� ����ȭ�� �ذ� �ȵǱ� ������ CPU ����, GPU�� ���߿� �о
	// CPU�� Buffer�� ������ ��ĥ ������ ��ٷ���� ��.
	// Map(Lock) - CPU�� ������ ���� : GPU�� ���� ����
	// UnMap(Unlock) - CPU�� ������ ��ġ�� : GPU�� ���� ����
	D3D11_MAPPED_SUBRESOURCE ms;
	mspDeviceContext->Map(
		mspVertexBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&ms
	);

	memcpy(ms.pData, vertices, sizeof(vertices));

	mspDeviceContext->Unmap(mspVertexBuffer.Get(), 0);
	

}

void DrawTriangle::InitPipeline()
{
	// Shader �غ�
		// HLSL -> ������ -> ���� (Binary Data)
	// IA(Input Assembly)

	// Shader compile

	Microsoft::WRL::ComPtr<ID3DBlob> spVS;
	Microsoft::WRL::ComPtr<ID3DBlob> spPS;

	D3DCompileFromFile(
		L"VertexShader.hlsl",		// Shader File �̸�
		nullptr,							// ��ũ�� ������ ����
		nullptr,							// ��� ���� ������
		"main",								// Shader�� entry point
		"vs_4_0_level_9_3",					// HLSL ���α׷� ��� ����
		0,									// Compile option
		0,									// Compile option
		spVS.GetAddressOf(),				// ������ ���̴� binary Code
		nullptr								// ������ ���� �޽���
	);

	D3DCompileFromFile(
		L"PixelShader.hlsl",		// Shader File �̸�
		nullptr,							// ��ũ�� ������ ����
		nullptr,							// ��� ���� ������
		"main",								// Shader�� entry point
		"ps_4_0_level_9_3",					// HLSL ���α׷� ��� ����
		0,									// Compile option
		0,									// Compile option
		spPS.GetAddressOf(),				// ������ ���̴� binary Code
		nullptr								// ������ ���� �޽���
	);

	mspDevice->CreateVertexShader(
		spVS->GetBufferPointer(),
		spVS->GetBufferSize(),
		nullptr,									// Linker ����
		mspVertexShader.ReleaseAndGetAddressOf()
	);
	mspDevice->CreatePixelShader(
		spPS->GetBufferPointer(),
		spPS->GetBufferSize(),
		nullptr,
		mspPixelShader.ReleaseAndGetAddressOf()
	);

	// �����ϱ�
	mspDeviceContext->VSSetShader(mspVertexShader.Get(), nullptr, 0);
	mspDeviceContext->PSSetShader(mspPixelShader.Get(), nullptr, 0);

	//Input Assembler Stage ����
	D3D11_INPUT_ELEMENT_DESC ied[]{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	mspDevice->CreateInputLayout(ied, 2, spVS->GetBufferPointer(), spVS->GetBufferSize(), mspInputLayout.ReleaseAndGetAddressOf());

	mspDeviceContext->IASetInputLayout(mspInputLayout.Get());
}

HRESULT DrawTriangle::CreateTextureFromBMP()
{
	// 1. ���� ����
	std::ifstream file;
	file.open("Data/32.bmp", std::ios::binary);

	BITMAPFILEHEADER bmh;
	BITMAPINFOHEADER bmi;

	// 2. BITMAPFILEHEADER �б�
	file.read(reinterpret_cast<char*>(&bmh), sizeof(BITMAPFILEHEADER));
	// 3. BITMAPINFOGHEADER �б�
	file.read(reinterpret_cast<char*>(&bmi), sizeof(BITMAPINFOHEADER));
	if (bmh.bfType != 0x4D42)
	{
		return E_FAIL;
	}
	if (bmi.biBitCount != 32)
	{
		return E_FAIL;
	}

	std::vector<char> pPixels(bmi.biSizeImage);
	// 4. �ȼ��� �ǳʶٱ�
	file.seekg(bmh.bfOffBits);
	// 5. ��Ʈ�� �б�
	int pitch = bmi.biWidth * (bmi.biBitCount / 8);
	for (int y = bmi.biHeight - 1; y >= 0; --y)
	{
		file.read(&pPixels[y * pitch], pitch);
	}

	file.close();

	return S_OK;
}

void DrawTriangle::Render()
{
	UINT offset = 0; // ������ �󸶳� ������ �ִ°�?
	UINT stride = sizeof(VERTEX); // �ϳ��� ũ�Ⱑ ���ΰ�?

	mspDeviceContext->IASetVertexBuffers(0, 1, mspVertexBuffer.GetAddressOf(), &stride, &offset);

	//primitive type �����ϱ�

	mspDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	mspDeviceContext->Draw(4, 0);
}
