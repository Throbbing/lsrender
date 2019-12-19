

struct VertexOut
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
};
SamplerState sam;
Texture2D frontTex;


float4 main(VertexOut pin) : SV_TARGET
{
    return pin.color * frontTex.SampleLevel(sam, pin.tex, 0.f);
}