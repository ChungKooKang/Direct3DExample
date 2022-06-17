Texture2D shaderTexture;	// texture�� ������ �� �ִ� ����
SamplerState Sampler;		// texture�� ������ �� texture sampling�� ��Ģ ���ϱ�



struct PIn
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PIn input) : SV_TARGET
{
	float4 pix = shaderTexture.Sample(Sampler, input.tex);

	return pix;
}