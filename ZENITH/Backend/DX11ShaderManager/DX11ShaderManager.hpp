#ifndef DX11BLUR_HPP
#define DX11BLUR_HPP

#include <ClassManager/BaseClass.hpp>
#include "BaseShader.hpp"
#include <unordered_map>
#include <d3dcompiler.h>
#include <filesystem>
#include <fstream>
#include <d3d11.h>

//resourceManager.bindResourceInput<sampler>("blurredSampledGameTexture", register1);
//resourceManager.bindResourceInput<constbuffer>("constbuffer", register2);
//
//resourceManager.bindResourceOutput<renderTarget>("sometarget", 1);
//
//shaderClass.render()->set stock rendertarget

class DirectXShaderManager : public BaseClass {
private:
    DirectXShaderData shaderData;
public:
    std::vector<DirectXShader*> shaderPtrs;
    std::unordered_map<std::string, DirectXShader*> shaderInstances;

    /// <summary>
    /// Registers a new shader object into the DirectXShaderManager
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="name"> -> internal shader name</param>
    template <typename T>
    void addShader(const std::string& name) {
        DirectXShader* shader = new T;
        shader->shaderData = &this->shaderData;
        shader->resourceManager = DirectXResourceManager::getInstance();
        
        // Add shader code to include handler
        this->shaderPtrs.push_back(shader);
        this->shaderInstances[name] = shader;
    }

    /// <summary>
    /// Upon providing existing shader name, gives back the shaderPtr
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="shaderName"> -> internal shader name</param>
    /// <param name="shader"> -> outPtr for the shader</param>
    /// <param name="forceFind"> -> if it should be retrieved everytime</param>
    /// <returns>Wheter the operation was successful</returns>
    template <typename T>
    bool getShader(const std::string& shaderName, T** shader, bool forceFind = false) {
        
        if (*shader != nullptr && !forceFind)
            return true;

        const auto& foundShader = this->shaderInstances.find(shaderName);
        if (foundShader == this->shaderInstances.end())
            return false;

        *shader = reinterpret_cast<T*>(foundShader->second);

        return true;
    }

    bool init(void* state) override;
    void renderHud(void* state) override;
    void deInit(void* state) override;
};

#endif DX11BLUR_HPP