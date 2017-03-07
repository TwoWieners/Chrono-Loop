
cbuffer GSMatrix : register(b0)
{
	matrix view;
	matrix proj;
}
cbuffer Model : register(b1)
{
	matrix model;
}

struct GSParticle
{
	float4 pos : SV_POSITION;
	float size : SIZE;
	float3 pad : USELESS;
};


GSParticle main(GSParticle input)
{
	GSParticle output;

	float4 pos = input.pos;
	
	//pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, proj);

	output.pos = pos;
	output.size = input.size;

	return output;
}