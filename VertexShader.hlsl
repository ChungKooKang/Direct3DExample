// Buffer가 추가됨
// Constant Buffer 이게 우리가 쓸 것! constant인 이유는 CPU의 data를 바꾸는 것은 아님! 연산만 해 주면 됨!
cbuffer MatrixBuffer		// 이건 namespace같은 용도로 쓰이는 것.
{
	matrix worldMatrix;
};

// Texture Buffer
struct VOut
{
	float4 position : SV_POSITION; // SV_POSITON : 시스템에 흘러다니는 위치 정보라는 말
	float2 tex : TEXCOORD0;
};

VOut main(float4 pos : POSITION, float2 tex : TEXCOORD0)
{
	VOut output;

	//output.position = mul(pos, worldMatrix);
	output.position = pos;
	output.tex = tex;

	return output;
}

// 언어들 종류 : 쓰는 진영
// HLSL : DirectX
// CG : nVidia
// GLSL : OpenGL

// Unity Shader
// Unreal Shader