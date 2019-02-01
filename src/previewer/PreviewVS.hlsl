cbuffer cbTrans : register(b0)
{
    float4x4 proj;
}

struct VertexIn
{
    float2 pos : POSITION;
    float2 tex : TEXCOORD;
    float4 color : COLOR;
    
};

struct VertexOut
{
    float4 posH : SV_Position;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;
    vout.posH = mul(float4(vin.pos.xy, 0.f, 1.f), proj);
    vout.color = vin.color;
    vout.tex = vin.tex;

    return vout;
}