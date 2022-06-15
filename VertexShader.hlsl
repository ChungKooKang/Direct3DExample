struct VOut
{
	float4 position : SV_POSITION; // SV_POSITON : �ý��ۿ� �귯�ٴϴ� ��ġ ������� ��
	float4 color : COLOR;
};

VOut main(float4 pos : POSITION, float4 color : COLOR)
{
	VOut output;

	output.position = pos;
	output.color = color;

	return output;
}

// ���� ���� : ���� ����
// HLSL : DirectX
// CG : nVidia
// GLSL : OpenGL

// Unity Shader
// Unreal Shader