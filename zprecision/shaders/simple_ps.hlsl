// Simple pixel shader

struct VS2PS {
	float4 pos : SV_POSITION;
};

float4 main(VS2PS input) : SV_TARGET
{
	return float4(0.0, 0.5, 0.5, 1.0);
}
