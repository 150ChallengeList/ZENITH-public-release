#ifndef BASESHADER_HPP
#define BASESHADER_HPP

#include "DX11ShaderManager.hpp"
#include <d3dcompiler.h>
#include <d3d11.h>

struct DirectXShaderData {
    ID3D11Device* d3d11Device = nullptr;
    ID3D11Texture2D* d3d11BackBuffer = nullptr;
    ID3D11DeviceContext* d3d11DeviceContext = nullptr;
    ID3D11RenderTargetView* d3d11RenderTargetView = nullptr;
};

class DirectXResourceManager {
private:
    std::unordered_map<std::string, class ShaderResource*> resources;

public:

    /// <summary>
    /// Gets the singleton instance of the object
    /// </summary>
    /// <returns>pointer to the object</returns>
    inline static DirectXResourceManager* getInstance() {
        static DirectXResourceManager resourceManager;
        return &resourceManager;
    }

    DirectXShaderData* shaderData;

    /// <summary>
    /// Will add the ptr to the resources given the raw ptr
    /// </summary>
    /// <typeparam name="T"> -> resource type</typeparam>
    /// <param name="name"> -> resource name</param>
    /// <param name="resource"> -> resource ptr</param>
    /// <returns></returns>
    template<typename T>
    void addResourcePtr(const std::string& name, T* resource) {
        this->resources[name] = reinterpret_cast<ShaderResource*>(resource);
    }


    /// <summary>
    /// Will construct a new object given the resource constructor data
    /// </summary>
    /// <typeparam name="T"> -> resource type</typeparam>
    /// <typeparam name="...Args"> -> resource constructor data type</typeparam>
    /// <param name="name"> -> resource name</param>
    /// <param name="...args"> -> resource constructor data</param>
    /// <returns>Pointer of the initialized object</returns>
    template<typename T, typename... Args>
    T* addResource(const std::string& name, Args&&... args) {
        T* rawResource = new T(std::forward<Args>(args)...);
        this->resources[name] = reinterpret_cast<ShaderResource*>(rawResource);;
        return rawResource;
    }

    /// <summary>
    /// Given the name of the resource will return the resourcePtr
    /// </summary>
    /// <typeparam name="T"> -> resource type</typeparam>
    /// <param name="name"> -> resource name</param>
    /// <param name="resource"> -> resource ptr</param>
    /// <param name="forceFind"> -> if it should be retrieved everytime</param>
    /// <returns>Wheter the operation was successful</returns>
    template<typename T>
    bool getResource(const std::string& name, T** resource, bool forceFind = false) {

        if (*resource != nullptr && !forceFind)
            return true;

        const auto& foundResource = this->resources.find(name);
        if (foundResource == this->resources.end())
            return false;

        *resource = reinterpret_cast<T*>(foundResource->second);

        return true;
    }

};

class ShaderResource {
public:
    virtual void bind(UINT slot) {};
};

template<typename T>
class ConstantBuffer : public ShaderResource {
private:
    DirectXShaderData* shaderData;
    D3D11_BUFFER_DESC bufferDesc;
    T data;
public:
    ID3D11Buffer* buffer;

    ConstantBuffer(D3D11_BUFFER_DESC initDesc) {
        this->shaderData = DirectXResourceManager::getInstance()->shaderData;
        this->bufferDesc = initDesc;
        this->shaderData->d3d11Device->CreateBuffer(&this->bufferDesc, nullptr, &this->buffer);
    }

    void update(const T& newData) {
        this->data = newData;
        this->shaderData->d3d11DeviceContext->UpdateSubresource(this->buffer, 0, nullptr, &this->data, 0, 0);
    }


    void bind(UINT slot) override {
        this->shaderData->d3d11DeviceContext->PSSetConstantBuffers(slot, 1, &this->buffer);
    }
};

template<typename T>
class SamplerState : public ShaderResource {
private:
    DirectXShaderData* shaderData;
    D3D11_SAMPLER_DESC samplerDesc;
public:
    ID3D11SamplerState* samplerState;

    SamplerState(const D3D11_SAMPLER_DESC& initDesc) :
        shaderData(DirectXResourceManager::getInstance()->shaderData),
        samplerDesc(initDesc),
        samplerState(nullptr) {
        this->shaderData->d3d11Device->CreateSamplerState(&this->samplerDesc, &this->samplerState);
    }

    void bind(UINT slot) override {
        this->shaderData->d3d11DeviceContext->PSSetSamplers(slot, 1, &this->samplerState);
    }
};

template<typename T>
class RenderTargetView : public ShaderResource {
private:
    DirectXShaderData* shaderData;
public:
    ID3D11RenderTargetView* renderTargetView;

    RenderTargetView(ID3D11RenderTargetView* inPtr) :
        shaderData(DirectXResourceManager::getInstance()->shaderData),
        renderTargetView(nullptr) {
        this->renderTargetView = inPtr;
    }
    RenderTargetView(ID3D11Texture2D* inTexture) :
        shaderData(DirectXResourceManager::getInstance()->shaderData),
        renderTargetView(nullptr) {
        this->shaderData->d3d11Device->CreateRenderTargetView(inTexture, nullptr, &this->renderTargetView);
    }

    void bind() {
        this->shaderData->d3d11DeviceContext->OMSetRenderTargets(1, &this->renderTargetView, nullptr);
    }
};

template<typename T>
class Texture2D : public ShaderResource {
private:
    DirectXShaderData* shaderData;
public:
    ID3D11Texture2D* texture;

    Texture2D(ID3D11Texture2D* inPtr) :
        shaderData(DirectXResourceManager::getInstance()->shaderData),
        texture(inPtr) {
    }
    Texture2D(const D3D11_TEXTURE2D_DESC& initDesc) :
        shaderData(DirectXResourceManager::getInstance()->shaderData),
        texture(nullptr) {
        this->shaderData->d3d11Device->CreateTexture2D(&initDesc, nullptr, &this->texture);
    }

};

template<typename T, typename X>
class ShaderResourceView : public ShaderResource {
private:
    DirectXShaderData* shaderData;
public:
    ID3D11ShaderResourceView* shaderResourceView;

    ShaderResourceView(ID3D11ShaderResourceView* inPtr) :
        shaderData(DirectXResourceManager::getInstance()->shaderData),
        shaderResourceView(inPtr) {
    }
    ShaderResourceView(const T& initDesc, X inTexture) :
        shaderData(DirectXResourceManager::getInstance()->shaderData),
        shaderResourceView(nullptr) {
        this->shaderData->d3d11Device->CreateShaderResourceView(inTexture, &initDesc, &this->shaderResourceView);
    }

};

enum DirectXShaderType {
    TYPE_NOT_SET,
    TYPE_VERTEX,
    TYPE_PIXEL
};

class DirectXShader {
public:

    DirectXResourceManager* resourceManager;
    DirectXShaderData* shaderData;

    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;

    ID3DBlob* shaderBlob = nullptr;

	const char* code;

    /// <summary>
    /// The user HAS to overwrite this function to set shader code
    /// </summary>
    /// <returns>Whether the shader setup was successful</returns>
    virtual bool init() { return false; }

    /// <summary>
    /// The render function that will be called every frame
    /// </summary>
    virtual void render() {};

    /// <summary>
    /// Binds the pixel/vertex shader automatically to the rendering pipeline
    /// </summary>
    void bind() {
        if (this->vertexShader)
            this->shaderData->d3d11DeviceContext->VSSetShader(this->vertexShader, nullptr, 0);
        if (this->pixelShader)
            this->shaderData->d3d11DeviceContext->PSSetShader(this->pixelShader, nullptr, 0);
    }

    bool compile(DirectXShaderType shaderType, const char* entryPoint) {
        
        ID3DBlob* errorBlob = nullptr;
        HRESULT result;
        switch (shaderType) {
            case TYPE_VERTEX: {
                result = D3DCompile(this->code, strlen(this->code), nullptr, nullptr, nullptr, entryPoint, "vs_5_0", 0, 0, &this->shaderBlob, &errorBlob);
                if (FAILED(result)) {
                    if (!errorBlob)
                        return false;
                    MessageBoxA(NULL, reinterpret_cast<const char*>(errorBlob->GetBufferPointer()), "DirectXShader::compile", NULL);
                    errorBlob->Release();
                    return true;
                }
                result = this->shaderData->d3d11Device->CreateVertexShader(this->shaderBlob->GetBufferPointer(), this->shaderBlob->GetBufferSize(), nullptr, &this->vertexShader);
                if (FAILED(result))
                    return false;
                break;
            }
            case TYPE_PIXEL: {
                result = D3DCompile(this->code, strlen(this->code), nullptr, nullptr, nullptr, entryPoint, "ps_5_0", 0, 0, &this->shaderBlob, &errorBlob);
                if (FAILED(result)) {
                    if (!errorBlob)
                        return false;
                    MessageBoxA(NULL, reinterpret_cast<const char*>(errorBlob->GetBufferPointer()), "DirectXShader::compile", NULL);
                    errorBlob->Release();
                    return true;
                }
                result = this->shaderData->d3d11Device->CreatePixelShader(this->shaderBlob->GetBufferPointer(), this->shaderBlob->GetBufferSize(), nullptr, &this->pixelShader);
                if (FAILED(result))
                    return false;
                break;
            }
        }

        return true;
    };
};

#endif BASESHADER_HPP