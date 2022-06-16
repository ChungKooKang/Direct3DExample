Texture2D shaderTexture;	// texture�� ������ �� �ִ� ����
SamplerState Sampler		// texture�� ������ �� texture sampling�� ��Ģ ���ϱ�
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};		


struct PIn
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PIn input) : SV_TARGET
{
	return shaderTexture.Sample(Sampler, input.tex);
}