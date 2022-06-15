#include "DrawTriangle.h"

void DrawTriangle::Initialize(HINSTANCE hInstance, int width, int height)
{
}

void DrawTriangle::Destroy()
{
}

void DrawTriangle::InitTriangle()
{
	VERTEX vertices[]
	{
		{ 0.0f, 0.5f, 0.0f, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ 0.45f, -0.5f, 0.0f, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ -0.45f, -0.5f, 0.0f, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	CD3D11_BUFFER_DESC bd(
		sizeof(VERTEX) * 3,// �츮�� ������� �迭�� �� ����Ʈ �ΰ�?
		D3D11_BIND_VERTEX_BUFFER, // bind flag ��� ����� ������ ������ �˷��ִ� ��
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
}

void DrawTriangle::Render()
{
}
