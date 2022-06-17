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
	float4 pix = shaderTexture.Sample(Sampler, input.tex);

	float gray = (pix[0] + pix[1] + pix[2]) / 3;
	if (gray > 0.5)
	{
		gray = 1.0f;
	}
	else
	{
		gray = 0.0f;
	}

	pix[0] = pix[1] = pix[2] = gray;

	return pix;
}