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
		sizeof(VERTEX) * 3,// 우리가 만들려는 배열이 몇 바이트 인가?
		D3D11_BIND_VERTEX_BUFFER, // bind flag 어디에 사용할 것인지 정보를 알려주는 것
		D3D11_USAGE_DYNAMIC,		// usage 사용 방법 어떻게 사용할 것인지 ( GPU 읽기, CPU 쓰기 ) => 왜 이렇게 하냐면 CPU와 GPU가 서로 연결이 안 되어 있기 때문에 각 부분에 할 것을 지정해 줘야한다.
		D3D11_CPU_ACCESS_WRITE		// CPU  Acccess : CPU가 값을 저장할 수 있다.
	);

	mspDevice->CreateBuffer(&bd, nullptr, mspVertexBuffer.ReleaseAndGetAddressOf());

	// 위 처럼 해도 동기화가 해결 안되기 때문에 CPU 저장, GPU가 나중에 읽어감
	// CPU가 Buffer에 전송을 마칠 때까지 기다려줘야 함.
	// Map(Lock) - CPU가 저장을 시작 : GPU는 접근 금지
	// UnMap(Unlock) - CPU가 저장을 마치면 : GPU는 접근 가능
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
	// Shader 준비
		// HLSL -> 컴파일 -> 기계어 (Binary Data)
	// IA(Input Assembly)

	// Shader compile

	Microsoft::WRL::ComPtr<ID3DBlob> spVS;
	Microsoft::WRL::ComPtr<ID3DBlob> spPS;

	D3DCompileFromFile(
		L"VertexShader.hlsl",		// Shader File 이름
		nullptr,							// 메크로 데이터 지정
		nullptr,							// 헤더 파일 포인터
		"main",								// Shader의 entry point
		"vs_4_0_level_9_3",					// HLSL 프로그램 언어 버전
		0,									// Compile option
		0,									// Compile option
		spVS.GetAddressOf(),				// 생성된 세이더 binary Code
		nullptr								// 컴파일 에러 메시지
	);

	D3DCompileFromFile(
		L"PixelShader.hlsl",		// Shader File 이름
		nullptr,							// 메크로 데이터 지정
		nullptr,							// 헤더 파일 포인터
		"main",								// Shader의 entry point
		"ps_4_0_level_9_3",					// HLSL 프로그램 언어 버전
		0,									// Compile option
		0,									// Compile option
		spPS.GetAddressOf(),				// 생성된 세이더 binary Code
		nullptr								// 컴파일 에러 메시지
	);

	mspDevice->CreateVertexShader(
		spVS->GetBufferPointer(),
		spVS->GetBufferSize(),
		nullptr,									// Linker 정보
		mspVertexShader.ReleaseAndGetAddressOf()
	);
	mspDevice->CreatePixelShader(
		spPS->GetBufferPointer(),
		spPS->GetBufferSize(),
		nullptr,
		mspPixelShader.ReleaseAndGetAddressOf()
	);

	// 조립하기
	mspDeviceContext->VSSetShader(mspVertexShader.Get(), nullptr, 0);
	mspDeviceContext->PSSetShader(mspPixelShader.Get(), nullptr, 0);
}

void DrawTriangle::Render()
{
}
