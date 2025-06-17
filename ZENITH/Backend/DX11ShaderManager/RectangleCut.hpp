#ifndef RECTANGLECUT_HPP
#define RECTANGLECUT_HPP

#include "BaseShader.hpp"

class RectangleCut : public DirectXShader {
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
)";
        return this->compile(DirectXShaderType::TYPE_PIXEL, "main");
    }

    void render() override {

    }

};

#endif RECTANGLECUT_HPP