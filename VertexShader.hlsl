struct VOut
{
	float4 position : SV_POSITION; // SV_POSITON : 시스템에 흘러다니는 위치 정보라는 말
	float2 tex : TEXCOORD0;
};

VOut main(float4 pos : POSITION, float2 tex : TEXCOORD0)
{
	VOut output;

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