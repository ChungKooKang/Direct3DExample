Texture2D shaderTexture;	// texture를 저장할 수 있는 공간
SamplerState Sampler;		// texture를 입혀줄 때 texture sampling의 규칙 정하기



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