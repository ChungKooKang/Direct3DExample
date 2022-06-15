struct VOout
{
	float4 position : SV_POSITION; // SV_POSITON : 시스템에 흘러다니는 위치 정보라는 말
	float4 color : COLOR;
};

VOut main(float4 pos : POSITION, float4 color : COLOR)
{
	Vout output;

	output.position = pos;
	output.color = color;

	return output;
}

// 언어들 종류 : 쓰는 진영
// HLSL : DirectX
// CG : nVidia
// GLSL : OpenGL

// Unity Shader
// Unreal Shader