// Simple vertex shader

struct vertex {
	float4 position : POSITION;
};

struct VS2PS {
	float4 pos : SV_POSITION;
};

VS2PS main(vertex inp)
{
	VS2PS output;
	output.pos = inp.position;
	return output;
}
