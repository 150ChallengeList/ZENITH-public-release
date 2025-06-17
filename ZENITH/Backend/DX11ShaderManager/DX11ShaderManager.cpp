#include "DX11ShaderManager.hpp"
#include <imgui.h>

struct DirectXHookData {
	ID3D11Device* d3d11Device = nullptr;
	IDXGISwapChain* dxgiSwapChain = nullptr;
	ID3D11Texture2D* d3d11BackBuffer = nullptr;
	ID3D11DeviceContext* d3d11DeviceContext = nullptr;
	ID3D11RenderTargetView* d3d11RenderTargetView = nullptr;
};

DirectXHookData hookData;

DirectXHookData getData() {
    return hookData;
}

bool DirectXShaderManager::init(void* state) {

    DirectXResourceManager* resourceManager = DirectXResourceManager::getInstance();

    // Save hook data from directXHook
    DirectXHookData hookData = getData();
    this->shaderData.d3d11Device = hookData.d3d11Device;
    this->shaderData.d3d11BackBuffer = hookData.d3d11BackBuffer;
    this->shaderData.d3d11DeviceContext = hookData.d3d11DeviceContext;
    this->shaderData.d3d11RenderTargetView = hookData.d3d11RenderTargetView;

    // Set resourceManager data
    resourceManager->shaderData = &this->shaderData;
    
    // Create a main sampler state
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    resourceManager->addResource<SamplerState<D3D11_SAMPLER_DESC>>("mainSampler", samplerDesc);

    resourceManager->addResource<RenderTargetView<ID3D11RenderTargetView*>>("mainRenderTargetView", this->shaderData.d3d11RenderTargetView);

    for (const auto& [shaderName, shader] : this->shaderInstances) {
        if (!shader->init())
            return false;
        resourceManager->addResourcePtr<DirectXShader>(shaderName, shader);
    }
    
    // Create a clean texture and make it the "gameTexture"
    // d3d11BackBuffer is the "gameTexture"
    // We grab its info to make a texture with the same resolution etc
    //this->shaderData.d3d11BackBuffer->GetDesc(&this->shaderData.backBufferDesc);
    //this->shaderData.backBufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    //
    //// We then create our own texture to have the game content
    //result = this->shaderData.d3d11Device->CreateTexture2D(&this->shaderData.backBufferDesc, nullptr, &this->shaderData.gameTexture);
    //if (result != S_OK)
    //    return false;
    //
    //// We then link a shaderResourceView to that cleanGameTexture
    //D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc = {};
    //resourceViewDesc.Format = this->shaderData.backBufferDesc.Format;
    //resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    //resourceViewDesc.Texture2D.MipLevels = -1;
    //resourceViewDesc.Texture2D.MostDetailedMip = 0;
    //result = this->shaderData.d3d11Device->CreateShaderResourceView(this->shaderData.gameTexture, &resourceViewDesc, &this->shaderData.gameResourceView);
    //if (result != S_OK)
    //    return false;

    return true;
}

void DirectXShaderManager::renderHud(void* state) {

    //this->shaderData.d3d11DeviceContext->CopyResource(this->shaderData.gameTexture, this->shaderData.d3d11BackBuffer);
    
    for (const auto& shaderPtr : this->shaderPtrs)
        shaderPtr->render();

}

void DirectXShaderManager::deInit(void* state) {
    //if (this->shaderData.d3d11SamplerState) this->shaderData.d3d11SamplerState->Release();
    //if (this->shaderData.gameTexture) this->shaderData.gameTexture->Release();
    //if (this->shaderData.gameResourceView) this->shaderData.gameResourceView->Release();
}