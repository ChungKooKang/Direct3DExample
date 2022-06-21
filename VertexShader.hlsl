// Buffer�� �߰���
// Constant Buffer �̰� �츮�� �� ��! constant�� ������ CPU�� data�� �ٲٴ� ���� �ƴ�! ���길 �� �ָ� ��!

// Texture Buffer
struct VOut
{
	float4 position : SV_POSITION; // SV_POSITON : �ý��ۿ� �귯�ٴϴ� ��ġ ������� ��
	float2 tex : TEXCOORD0;
};

VOut main(float4 pos : POSITION, float2 tex : TEXCOORD0)
{
	VOut output;

	output.position = pos;
	output.tex = tex;

	return output;
}

// ���� ���� : ���� ����
// HLSL : DirectX
// CG : nVidia
// GLSL : OpenGL

// Unity Shader
// Unreal Shader