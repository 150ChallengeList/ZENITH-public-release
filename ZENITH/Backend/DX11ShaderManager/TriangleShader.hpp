#ifndef TRIANGLESHADER_HPP
#define TRIANGLESHADER_HPP

#include "BaseShader.hpp"

class TriangleShader : public DirectXShader {
private:
public:

	bool init() override {
		this->code = R"(
#ifndef BASE_FULLSCREEN_TRIANGLE_HLSL
#define BASE_FULLSCREEN_TRIANGLE_HLSL

struct VS_OUTPUT {
    float4 position  : SV_Position;
    float2 uv  		 : TEXCOORD0;
};

#if !defined( TRIANGLE_NO_UV )
#if defined( TRIANGLE_INVERT_UV_Y )
static const float2 uvs[3] = {
    { 0.0, 0.0 },
    { 2.0, 0.0 },
    { 0.0, 2.0 },
};
#else
static const float2 uvs[3] = {
    { 0.0,  1.0 },
    { 2.0,  1.0 },
    { 0.0, -1.0 },
};
#endif
#endif

static const float4 positions[3] = {
    { -1.0, -1.0,  1.0,  1.0 },
    {  3.0, -1.0,  1.0,  1.0 },
    { -1.0,  3.0,  1.0,  1.0 },
};

VS_OUTPUT main( in uint vertexId : SV_VertexID ) {
    VS_OUTPUT output;
    output.position = positions[ vertexId ];
#if !defined( TRIANGLE_NO_UV )
    output.uv = uvs[ vertexId ];
#endif
    return output;
};

#endif // BASE_FULLSCREEN_TRIANGLE_HLSL
)";

		return this->compile(DirectXShaderType::TYPE_VERTEX, "main");
	}

};

#endif TRIANGLESHADER_HPP