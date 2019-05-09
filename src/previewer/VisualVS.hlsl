

cbuffer cbTrans : register(b0)
{
    float4x4 wvp;
}



struct VertexIn
{
	float3 pos :POSITION;
	float3 color :COLOR;
};

struct VertexOut
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
};

VertexOut main(VertexIn vin ) 
{
    VertexOut vout;
    vout.posH = mul(float4(vin.pos, 1.f), wvp);
    vout.color = float4(vin.color, 1.f);
    return vout;

}