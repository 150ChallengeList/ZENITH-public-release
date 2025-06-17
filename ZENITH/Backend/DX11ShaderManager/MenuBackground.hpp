#ifndef MENUBACKGROUND_HPP
#define MENUBACKGROUND_HPP

#include "BaseShader.hpp"

class MenuBackground : public DirectXShader {
private:
public:

    struct bufferData {
        float resolution[2] = { 0.0f, 0.0f };
        float curTime = 0.0f;
        float padding;
    };
    bufferData constBufferData;

	bool init() override {
		this->code = R"(
cbuffer Constants : register(b10) {
    float2 resolution;
    float curTime;
    float padding;
}

float Hash(float2 p) {
    return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453123);
}

float Noise(float2 p) {
    float2 i = floor(p);
    float2 f = frac(p);
    float2 u = f * f * (3.0 - 2.0 * f);

    return lerp(
        lerp(Hash(i + float2(0.0, 0.0)), Hash(i + float2(1.0, 0.0)), u.x),
        lerp(Hash(i + float2(0.0, 1.0)), Hash(i + float2(1.0, 1.0)), u.x),
        u.y
    );
}

float4 main(float4 position : SV_POSITION, float2 texCoord : TEXCOORD0) : SV_TARGET {
    float2 uv = texCoord;
    
    float n = 0.6;
    float2 p = uv * 1.9;
    
    for (int i = 0; i < 6; ++i) {
        n += (1.8 / (i + 1)) * Noise(p + curTime * 0.1);
        p *= 1.7;
    }
    
    float lines = frac(n * 9);
    float contour = smoothstep(0.15, 0.09, lines); // thicker lines
    
    float3 base = float3(0.05, 0.05, 0.1);
    float3 glow = float3(0.1586, 0.1569, 0.2627);
    float3 color = lerp(base, glow, contour);
    
    return float4(color, 1.0);
}
)";

        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.ByteWidth = sizeof(bufferData);
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        this->resourceManager->addResource<ConstantBuffer<bufferData>>("menuBackgroundBuffer", bufferDesc);

        D3D11_TEXTURE2D_DESC backbufferDesc;
        this->shaderData->d3d11BackBuffer->GetDesc(&backbufferDesc);
        backbufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        Texture2D<D3D11_TEXTURE2D_DESC>* menuBackgroundTexture = this->resourceManager->addResource<Texture2D<D3D11_TEXTURE2D_DESC>>("menuBackgroundTexture", backbufferDesc);
        
        D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
        resourceViewDesc.Format = backbufferDesc.Format;
        resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        resourceViewDesc.Texture2D.MipLevels = -1;
        resourceViewDesc.Texture2D.MostDetailedMip = 0;
        this->resourceManager->addResource<ShaderResourceView<D3D11_SHADER_RESOURCE_VIEW_DESC, ID3D11Texture2D*>>("menuBackgroundResourceView", resourceViewDesc, menuBackgroundTexture->texture);
        
        this->resourceManager->addResource<RenderTargetView<ID3D11Texture2D*>>("menuBackgroundRenderTargetView", menuBackgroundTexture->texture);

		return this->compile(DirectXShaderType::TYPE_PIXEL, "main");
	}

	void render() override {

		static DirectXShader* triangleShader = nullptr;
		if (!this->resourceManager->getResource<DirectXShader>("triangleShader", &triangleShader))
			return;
        
        static ConstantBuffer<bufferData>* constantBuffer = nullptr;
        if (!this->resourceManager->getResource<ConstantBuffer<bufferData>>("menuBackgroundBuffer", &constantBuffer))
            return;

        static RenderTargetView<ID3D11Texture2D*>* renderTargetView = nullptr;
        if (!this->resourceManager->getResource<RenderTargetView<ID3D11Texture2D*>>("menuBackgroundRenderTargetView", &renderTargetView))
            return;
        
        ImGuiIO* io = &ImGui::GetIO();
        this->constBufferData.resolution[0] = io->DisplaySize.x;
        this->constBufferData.resolution[1] = io->DisplaySize.y;
        this->constBufferData.curTime = static_cast<float>(ImGui::GetTime());
        constantBuffer->update(this->constBufferData); // Update constantBuffer
        constantBuffer->bind(10); // Bind constantBuffer in register 10
        
        this->bind(); // Bind this pixelshader
        triangleShader->bind(); // Bind the triangle vertexshader
        
        renderTargetView->bind(); // Bind the renderTargetView
		
        this->shaderData->d3d11DeviceContext->Draw(4, 0);
	}
};

#endif MENUBACKGROUND_HPP