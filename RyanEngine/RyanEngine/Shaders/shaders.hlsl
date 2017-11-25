cbuffer transformBuffer : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PSInput vsmain(float3 position : POSITION, float4 color : COLOR )
{
	PSInput result;

	result.position = mul ( projectionMatrix, mul ( viewMatrix, mul ( worldMatrix, float4(position, 1.0) ) ) );
	result.color = color;

	return result;
}


float4 psmain(PSInput input) : SV_TARGET
{
	return input.color;
}



//PSInput vsmain ( uint vid : SV_VertexID )
//{
//	PSInput result;
//
//	switch ( vid )
//	{
//	case 0:
//		result.position = float4(0, 0.25, 0, 1); break;
//	case 1:
//		result.position = float4(0.25, -0.25, 0, 1); break;
//	case 2:
//		result.position = float4(-0.25, -0.25, 0, 1); break;
//	default:
//		result.position = float4(0, 0, 0, 1); break;
//	}
//
//	result.color = float(1,1,1,1);
//
//	return result;
//}

