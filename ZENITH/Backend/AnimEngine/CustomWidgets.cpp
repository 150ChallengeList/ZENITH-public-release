#include "CustomWidgets.hpp"
#include <imgui_internal.h>
#include "AnimEngine.hpp"
#include <unordered_set>
#include <vector>
#include <imgui.h>

void ImGui::PushGlobalAlpha(float alpha) {
    for (ImGuiCol col = ImGuiCol_Text; col < ImGuiCol_COUNT; col++) {
        ImVec4 newCol = ImGui::GetStyleColorVec4(col);
        
        if (newCol.w != 0.0f)
            newCol.w = alpha;

        ImGui::PushStyleColor(col, newCol);
    }

    ImGui::accentBackground = ImGui::rgbaToImVec4(22, 22, 34, static_cast<uint8_t>(alpha * 255));
    ImGui::accentGrayDark = ImGui::rgbaToImVec4(42, 42, 58, static_cast<uint8_t>(alpha * 255));
    ImGui::accentGrayMedium1 = ImGui::rgbaToImVec4(30, 30, 45, static_cast<uint8_t>(alpha * 255));
    ImGui::accentGrayMedium2 = ImGui::rgbaToImVec4(36, 36, 52, static_cast<uint8_t>(alpha * 255));
    ImGui::accentGrayMedium3 = ImGui::rgbaToImVec4(60, 60, 86, static_cast<uint8_t>(alpha * 255));
    ImGui::accentGrayBright = ImGui::rgbaToImVec4(108, 114, 147, static_cast<uint8_t>(alpha * 255));
    ImGui::accentPurpleDark = ImGui::rgbaToImVec4(54, 41, 92, static_cast<uint8_t>(alpha * 255));
    ImGui::accentPurpleMedium1 = ImGui::rgbaToImVec4(40, 36, 63, static_cast<uint8_t>(alpha * 255));
    ImGui::accentPurpleMedium2 = ImGui::rgbaToImVec4(45, 41, 68, static_cast<uint8_t>(alpha * 255));
    ImGui::accentPurpleMedium3 = ImGui::rgbaToImVec4(106, 78, 170, static_cast<uint8_t>(alpha * 255));
    ImGui::accentPurpleBright = ImGui::rgbaToImVec4(144, 106, 231, static_cast<uint8_t>(alpha * 255));
    ImGui::accentIconBackground = ImGui::rgbaToImVec4(108, 73, 190, static_cast<uint8_t>(alpha * 255));
    ImGui::textColorLight = ImGui::rgbaToImVec4(255, 255, 255, static_cast<uint8_t>(alpha * 255));
    ImGui::textColorMedium = ImGui::rgbaToImVec4(108, 114, 147, static_cast<uint8_t>(alpha * 255));
    ImGui::textColorDark = ImGui::rgbaToImVec4(79, 93, 109, static_cast<uint8_t>(alpha * 255));

}
void ImGui::PopGlobalAlpha() {
    ImGui::PopStyleColor(ImGuiCol_COUNT);

    ImGui::accentBackground = ImGui::rgbaToImVec4(22, 22, 34, 255);
    ImGui::accentGrayDark = ImGui::rgbaToImVec4(42, 42, 58, 255);
    ImGui::accentGrayMedium1 = ImGui::rgbaToImVec4(30, 30, 45, 255);
    ImGui::accentGrayMedium2 = ImGui::rgbaToImVec4(36, 36, 52, 255);
    ImGui::accentGrayMedium3 = ImGui::rgbaToImVec4(60, 60, 86, 255);
    ImGui::accentGrayBright = ImGui::rgbaToImVec4(108, 114, 147, 255);
    ImGui::accentPurpleDark = ImGui::rgbaToImVec4(54, 41, 92, 255);
    ImGui::accentPurpleMedium1 = ImGui::rgbaToImVec4(40, 36, 63, 255);
    ImGui::accentPurpleMedium2 = ImGui::rgbaToImVec4(45, 41, 68, 255);
    ImGui::accentPurpleMedium3 = ImGui::rgbaToImVec4(106, 78, 170, 255);
    ImGui::accentPurpleBright = ImGui::rgbaToImVec4(144, 106, 231, 255);
    ImGui::accentIconBackground = ImGui::rgbaToImVec4(108, 73, 190, 255);
    ImGui::textColorLight = ImGui::rgbaToImVec4(255, 255, 255, 255);
    ImGui::textColorMedium = ImGui::rgbaToImVec4(108, 114, 147, 255);
    ImGui::textColorDark = ImGui::rgbaToImVec4(79, 93, 109, 255);
}

bool ImGui::Switch(const char* label, bool* state, ImVec2 sizeArg) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    const ImVec2 sizeAvailable = GetContentRegionAvail();
    ImVec2 size = CalcItemSize(sizeArg, sizeAvailable.x, sizeAvailable.y);

    const ImRect fullbb(pos, pos + size);
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(fullbb, id))
        return false;

    // The switch will be 35x21
    const ImRect clickbb(
        { pos.x + size.x - 35.0f, pos.y + size.y / 2.0f - 21.0f / 2.0f },
        { pos.x + size.x, pos.y + size.y / 2.0f + 21.0f / 2.0f }
    );

    bool hovered, held;
    bool pressed = ButtonBehavior(clickbb, id, &hovered, &held, ImGuiButtonFlags_MouseButtonLeft);
    if (pressed)
        *state = !*state;

    static AnimEngine* animEngine = nullptr;
    if (!classManager.getClass<AnimEngine>("AnimEngine", &animEngine))
        return false;
    static WidgetController* controller = nullptr;
    if (!animEngine->getController(id, &controller, true))
        return false;

    controller->updateState(hovered, *state);

    const ImColor textCol = controller->processColor(ImGui::textColorDark, ImGui::textColorLight, controller->tClick);
    const ImVec2 labelSize = CalcTextSize(label, NULL, true);
    const ImVec2 textPos = { pos.x, pos.y + size.y / 2.0f - labelSize.y / 2.0f };
    window->DrawList->AddText(textPos, textCol, label);

    const ImColor switchFillColor = controller->processColor(
        ImGui::accentGrayDark,
        ImGui::accentPurpleDark,
        controller->tClick
    );
    window->DrawList->AddRectFilled(clickbb.Min, clickbb.Max, switchFillColor, 20.0f);

    ImVec2 circlePos = {
        controller->processFloat(clickbb.Min.x + 10.0f, clickbb.Max.x - 11.0f, controller->tClick),
        clickbb.GetCenter().y
    };
    float circleSize = controller->processFloat(6.0f, 7.0f, controller->tClick);
    const ImColor circleColor = controller->processColor(
        ImGui::accentGrayBright,
        ImGui::accentPurpleBright,
        controller->tClick
    );
    window->DrawList->AddCircleFilled(circlePos, circleSize, circleColor);

    return pressed;
}

bool ImGui::SliderFlt(const char* label, float* state, float min, float max, const char* fmt, ImVec2 sizeArg) {
    return SliderWrapper(label, state, ImGuiDataType_Float, &min, &max, sizeArg, fmt);
}
bool ImGui::SliderInt(const char* label, int* state, int min, int max, const char* fmt, ImVec2 sizeArg) {
    return SliderWrapper(label, state, ImGuiDataType_S32, &min, &max, sizeArg, fmt);
}
bool ImGui::SliderDouble(const char* label, double* state, double min, double max, const char* fmt, ImVec2 sizeArg) {
    return SliderWrapper(label, state, ImGuiDataType_Double, &min, &max, sizeArg, fmt);
}
bool ImGui::SliderWrapper(const char* label, void* state, ImGuiDataType dataType, const void* min, const void* max, ImVec2 sizeArg, const char* fmt) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 labelSize = CalcTextSize(label, NULL, true);
    const ImVec2 sizeAvailable = GetContentRegionAvail();

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(sizeArg, sizeAvailable.x, sizeAvailable.y);

    // Label centered in top half
    // Slider centered in bottom half
    const float sliderHeight = 10.0f;

    const ImRect fullbb(pos, pos + size);
    const ImRect sliderbb = {
        ImVec2(pos.x, pos.y + size.y - size.y / 4.0f - sliderHeight / 2.0f),
        ImVec2(pos.x + size.x, pos.y + size.y - size.y / 4.0f + sliderHeight / 2.0f)
    };
    const ImRect clickbb = {
        ImVec2(pos.x, pos.y + size.y - size.y / 4.0f - sliderHeight),
        ImVec2(pos.x + size.x, pos.y + size.y - size.y / 4.0f + sliderHeight)
    };

    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(fullbb, id, &sliderbb))
        return false;

    // We have hovered
    const bool hovered = ItemHoverable(clickbb, id, g.LastItemData.ItemFlags);
    bool isActive = false;

    if (hovered) {
        SetActiveID(id, window);
        SetFocusID(id, window);
    }

    ImRect grabbb;
    const bool valueChanged = SliderBehavior(clickbb, id, dataType, state, min, max, fmt, ImGuiSliderFlags_None, &grabbb);
    if (valueChanged)
        MarkItemEdited(id);

    static AnimEngine* animEngine = nullptr;
    if (!classManager.getClass<AnimEngine>("AnimEngine", &animEngine))
        return false;
    static WidgetController* controller = nullptr;
    if (!animEngine->getController(id, &controller, true))
        return false;
    controller->updateState(hovered || g.ActiveId == id, g.ActiveId == id);

    // Center in top half
    ImVec2 textPos = {
        pos.x,
        pos.y + size.y / 4.0f - labelSize.y / 2.0f
    };
    const ImColor textCol = controller->processColor(ImGui::textColorDark, ImGui::textColorLight, controller->tHover);
    window->DrawList->AddText(textPos, textCol, label);

    // Display the value
    float sliderRatio = 0.0f;
    if (fmt) {
        char valueText[50] = { 0 };
        switch (dataType) {
        case ImGuiDataType_S32: {
            sprintf_s(valueText, fmt, *reinterpret_cast<int*>(state));

            int curState = *reinterpret_cast<int*>(state);
            const int minState = *reinterpret_cast<const int*>(min);
            const int maxState = *reinterpret_cast<const int*>(max);

            sliderRatio = static_cast<float>(curState - minState) / static_cast<float>(maxState);

            break;
        }
        case ImGuiDataType_Float: {
            sprintf_s(valueText, fmt, *reinterpret_cast<float*>(state));

            float curState = *reinterpret_cast<float*>(state);
            const float minState = *reinterpret_cast<const float*>(min);
            const float maxState = *reinterpret_cast<const float*>(max);

            sliderRatio = (curState - minState) / maxState;

            break;
        }
        case ImGuiDataType_Double: {
            sprintf_s(valueText, fmt, *reinterpret_cast<double*>(state));

            double curState = *reinterpret_cast<double*>(state);
            const double minState = *reinterpret_cast<const double*>(min);
            const double maxState = *reinterpret_cast<const double*>(max);

            sliderRatio = static_cast<float>(curState - minState) / static_cast<float>(maxState);

            break;
        }
        }

        ImVec2 valtext = CalcTextSize(valueText, NULL, true);
        ImVec2 valTextPos = {
            pos.x + size.x - valtext.x,
            pos.y + size.y / 4.0f - labelSize.y / 2.0f
        };
        window->DrawList->AddText(valTextPos, textCol, valueText);
    }

    // Gray background
    window->DrawList->AddRectFilled(
        sliderbb.Min,
        sliderbb.Max,
        ImGui::accentGrayDark,
        10.0f
    );

    // Ratio should have a min of 10
    // This makes the foregroud-state always rounded

    float minX = (sliderbb.Min.x + sliderbb.GetHeight()) * (1.0f - sliderRatio) + sliderbb.Max.x * sliderRatio;

    // Purple foregroud
    window->DrawList->AddRectFilled(
        sliderbb.Min,
        { minX, sliderbb.Max.y },
        ImGui::accentPurpleBright,
        10.0f
    );

    return false;
}

bool ImGui::ListCfgs(const char* label, std::vector<Config>* configs, Config** selected) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    const ImVec2 sizeAvailable = GetContentRegionAvail();
    ImVec2 size = CalcItemSize(ImVec2(0.0f, 0.0f), sizeAvailable.x, sizeAvailable.y);

    ImRect fullbb(pos, pos + size);

    const float barSize = 60.0f;
    const float borderSize = 1.0f;

    ImVec2 topSize(size.x, barSize);
    ImVec2 topbbPos = pos;
    ImRect topbb(topbbPos, topbbPos + topSize);

    ImVec2 centerbbPos(topbb.Min.x, topbb.Max.y + style.FramePadding.y);
    ImVec2 centerSize(size.x, size.y - barSize * 2 - style.FramePadding.y * 2);
    ImRect centerbb(centerbbPos, centerbbPos + centerSize);

    ImVec2 bottombbPos(centerbb.Min.x, centerbb.Max.y + style.FramePadding.y);
    ImVec2 bottomSize(size.x, barSize);
    ImRect bottombb(bottombbPos, bottombbPos + bottomSize);

    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(fullbb, id))
        return false;

    // Background
    window->DrawList->AddRectFilled(fullbb.Min, fullbb.Max, ImGui::accentPurpleMedium0, 10.0f);

    // Top bar
    window->DrawList->AddRectFilled(topbb.Min, topbb.Max, ImGui::accentPurpleMedium0, 10.0f, ImDrawFlags_RoundCornersTop);
    window->DrawList->AddLine(ImVec2(topbb.Min.x, topbb.Max.y), topbb.Max, ImGui::accentGrayDark, 1.0f);
    
    // Draw label into top bar
    ImVec2 labelSize = ImGui::CalcTextSize(label);
    ImVec2 labelPos = topbb.Min + ImVec2(style.WindowPadding.x, barSize / 2.0f - labelSize.y / 2.0f);
    window->DrawList->AddText(labelPos, ImGui::textColorLight, label);
    
    window->DrawList->AddRectFilled(bottombb.Min, bottombb.Max, ImGui::accentPurpleMedium0, 10.0f, ImDrawFlags_RoundCornersBottom);
    window->DrawList->AddLine(bottombb.Min, ImVec2(bottombb.Max.x, bottombb.Min.y), ImGui::accentGrayDark, 1.0f);
    
    ImGui::SetCursorScreenPos(centerbbPos);
    bool returnValue = false;
    ImGui::BeginChild("##ConfigButtons", centerSize, ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar);
    {
        ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, 0.0f);
        for (auto& config : *configs) {
            bool isSelected = &config == *selected;
            if (ImGui::ConfigButton(config, &isSelected))
                returnValue = true; // Config has been updated

            if (!isSelected)
                continue;

            *selected = &config;
        }
        ImGui::PopStyleVar(); // ImGuiStyleVar_ItemSpacing
    }
    ImGui::EndChild();

    return returnValue;
}
bool ImGui::ShowCfgInfo(const char* label, std::vector<Config>* configs, Config* config) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    const ImVec2 sizeAvailable = GetContentRegionAvail();
    ImVec2 size = CalcItemSize(ImVec2(430.0f, 0.0f), sizeAvailable.x, sizeAvailable.y);

    ImRect fullbb(pos, pos + size);

    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(fullbb, id))
        return false;

    // Background
    window->DrawList->AddRectFilled(fullbb.Min, fullbb.Max, ImGui::accentPurpleMedium0, 10.0f);

    return false;
}
bool ImGui::ConfigButton(const Config& config, bool* state) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(config.name.c_str());

    ImVec2 pos = window->DC.CursorPos;
    const ImVec2 sizeAvailable = GetContentRegionAvail();
    ImVec2 size = CalcItemSize(ImVec2(0.0f, 80.0f), sizeAvailable.x, sizeAvailable.y);

    ImRect fullbb(pos, pos + size);
    ImRect highlightbb(pos, pos + ImVec2(3.0f, size.y));

    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(fullbb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(fullbb, id, &hovered, &held, ImGuiButtonFlags_MouseButtonLeft);
    if (pressed)
        *state = !*state;

    static AnimEngine* animEngine = nullptr;
    if (!classManager.getClass<AnimEngine>("AnimEngine", &animEngine))
        return false;
    static WidgetController* controller = nullptr;
    if (!animEngine->getController(id, &controller, true))
        return false;
    controller->updateState(hovered, *state);

    // Background
    ImColor backgroundColor = controller->processColor(ImGui::accentPurpleMedium0, ImGui::accentPurpleMedium1, ImGui::accentPurpleMedium2, controller->tHover, controller->tClick);
    window->DrawList->AddRectFilled(fullbb.Min, fullbb.Max, backgroundColor);

    ImColor highlightColor = controller->processColor(ImColor(0.0f, 0.0f, 0.0f, 0.0f), ImGui::accentPurpleBright, controller->tClick);
    window->DrawList->AddRectFilled(highlightbb.Min, highlightbb.Max, highlightColor);

    // Name should be in the top half
    ImVec2 labelSize = ImGui::CalcTextSize(config.name.c_str());
    ImVec2 descSize = ImGui::CalcTextSize(config.description.c_str());

    ImVec2 labelPos = fullbb.Min + ImVec2(style.WindowPadding.x, size.y / 2.0f - (labelSize.y + descSize.y + style.FramePadding.y) / 2.0f);
    ImColor labelCol = controller->processColor(ImGui::textColorLight, ImGui::accentPurpleBright, controller->tClick);
    window->DrawList->AddText(labelPos, labelCol, config.name.c_str());

    // Description in the bottom half
    ImVec2 descPos = labelPos + ImVec2(0.0f, labelSize.y + style.FramePadding.y);
    window->DrawList->AddText(descPos, ImGui::textColorDark, config.description.c_str());
    
    return pressed;
}

//ImVec2 ImGui::BezierCurve::getRelPosAtT(float t) {
//    float t0 = 0.0f;
//    float t1 = 0.5f;
//    float t2 = 1.0f;
//
//    float l0 = (t - t1) * (t - t2) / ((t0 - t1) * (t0 - t2));
//    float l1 = (t - t0) * (t - t2) / ((t1 - t0) * (t1 - t2));
//    float l2 = (t - t0) * (t - t1) / ((t2 - t0) * (t2 - t1));
//
//    ImVec2 finalPos = *this->startPos * l0 + *this->midPos * l1 + *this->endPos * l2;
//
//    finalPos.x = finalPos.x < 0.0f ? 0.0f : finalPos.x;
//    finalPos.x = finalPos.x > 1.0f ? 1.0f : finalPos.x;
//
//    finalPos.y = finalPos.y < 0.0f ? 0.0f : finalPos.y;
//    finalPos.y = finalPos.y > 1.0f ? 1.0f : finalPos.y;
//
//    return finalPos;
//}
//
//bool ImGui::BezierEditor(const char* label, std::vector<ImGui::BezierCurve>* curves, std::unordered_set<ImVec2>* points) {
//
//    ImGuiWindow* window = GetCurrentWindow();
//    if (window->SkipItems)
//        return false;
//
//    ImGuiContext& g = *GImGui;
//    const ImGuiStyle& style = g.Style;
//    const ImGuiID id = window->GetID(label);
//
//    const float topbarHeight = 30.0f;
//    const ImVec2 pos = window->DC.CursorPos;
//    const ImVec2 sizeAvailable = GetContentRegionAvail();
//    const ImVec2 size = CalcItemSize(ImVec2(0.0f, 105.0f + topbarHeight), sizeAvailable.x, sizeAvailable.y);
//
//    ImRect topbb(pos, pos + ImVec2(size.x, topbarHeight));
//
//    ImVec2 fullSize(size.x, size.y);
//    ImRect fullbb(pos + ImVec2(0.0f, topbarHeight), pos + fullSize);
//
//    ImVec2 editorSize(size.y, size.y - topbarHeight);
//
//    const ImRect editorbb(
//        pos + ImVec2(size.x / 2.0f - editorSize.x / 2.0f, topbarHeight),
//        pos + ImVec2(size.x / 2.0f + editorSize.x / 2.0f, size.y)
//    );
//
//    ItemSize(size, style.FramePadding.y);
//    if (!ItemAdd(editorbb, id))
//        return false;
//
//    static AnimEngine* animEngine = nullptr;
//    if (!classManager.getClass<AnimEngine>("AnimEngine", &animEngine))
//        return false;
//
//    // Draw label in the topbar
//    ImVec2 labelSize = ImGui::CalcTextSize(label);
//    ImVec2 textPos = topbb.Min + ImVec2(0.0f, topbarHeight / 2.0f - labelSize.y / 2.0f);
//    window->DrawList->AddText(textPos, ImGui::textColorMedium, label);
//
//    static std::vector<const char*> comments = { "Double-click to add points", "Double-right-click to remove", "Drag purple dots to adjust curve", "Right-click purple dots to reset" };
//
//    // If the curve list is empty add 2 curves
//    if (curves->empty()) {
//
//        points->reserve(50);
//        
//        auto [curve1Beg, _inserted1] = points->insert(ImVec2(0.0f, 1.0f));
//        auto [curve1Mid, _inserted2] = points->insert(ImVec2(0.25f, 0.5f));
//        auto [curve1End, _inserted3] = points->insert(ImVec2(0.5f, 0.0f));
//
//        auto& curve1 = curves->emplace_back(
//            const_cast<ImVec2*>(&(*curve1Beg)), // MSVC ._Ptr
//            const_cast<ImVec2*>(&(*curve1Mid)), // MSVC ._Ptr
//            const_cast<ImVec2*>(&(*curve1End))  // MSVC ._Ptr
//        );
//        curve1.boundRect = editorbb;
//        
//        auto [curve2Mid, _inserted4] = points->insert(ImVec2(0.75f, 0.5f));
//        auto [curve2End, _inserted5] = points->insert(ImVec2(1.0f, 1.0f));
//        
//        auto& curve2 = curves->emplace_back(
//            const_cast<ImVec2*>(&(*curve1End)), // MSVC ._Ptr
//            const_cast<ImVec2*>(&(*curve2Mid)), // MSVC ._Ptr
//            const_cast<ImVec2*>(&(*curve2End))  // MSVC ._Ptr
//        );
//        curve2.boundRect = editorbb;
//    }
//    
//    ImVec2 mousePos = g.IO.MousePos;
//    bool leftMouseDown = g.IO.MouseDown[0];
//    bool rightMouseDown = g.IO.MouseDown[1];
//    
//    // Draw the background
//    window->DrawList->AddRectFilled(fullbb.Min, fullbb.Max, ImGui::accentGrayMedium1, 5.0f);
//
//    // Draw the grid
//    int gridSteps = 4;
//    ImVec2 gridStep = (editorbb.Max - editorbb.Min) / gridSteps;
//    for (int i = 0; i < gridSteps + 1; i++) {
//        ImVec2 startPosVert = editorbb.Min + ImVec2(gridStep.x, 0.0f) * i;
//        ImVec2 endPosVert = startPosVert + ImVec2(0.0f, editorSize.y);
//        window->DrawList->AddLine(startPosVert, endPosVert, ImGui::accentGrayDark);
//
//        ImVec2 startPosHori = editorbb.Min + ImVec2(0.0f, gridStep.y) * i;
//        ImVec2 endPosHori = startPosHori + ImVec2(editorSize.x, 0.0f);
//        window->DrawList->AddLine(startPosHori, endPosHori, ImGui::accentGrayDark);
//    }
//
//    // Handle every point
//    for (ImGui::BezierCurve& curve : *curves) {
//
//        // midPos->x is locked between startpos and endpos
//        curve.midPos->x = curve.midPos->x < curve.startPos->x ? curve.startPos->x : curve.midPos->x;
//        curve.midPos->x = curve.midPos->x > curve.endPos->x ? curve.endPos->x : curve.midPos->x;
//        curve.startPos->x = curve.startPos->x > curve.endPos->x ? curve.endPos->x : curve.startPos->x;
//
//        // Calculate absolute positions of points
//        ImVec2 absBegPos = editorbb.Min + *curve.startPos * (editorbb.Max - editorbb.Min);
//        ImVec2 absMidPos = editorbb.Min + *curve.midPos * (editorbb.Max - editorbb.Min);
//        ImVec2 absEndPos = editorbb.Min + *curve.endPos * (editorbb.Max - editorbb.Min);
//
//        // Draw the curve with 50 points
//        std::vector<ImVec2> curvePreview;
//        curvePreview.reserve(50);
//        for (int i = 0; i < 50; i++) {
//            float t = static_cast<float>(i) / 50.0f;
//            curvePreview.emplace_back(editorbb.Min + curve.getRelPosAtT(t) * editorSize);
//        }
//        window->DrawList->AddPolyline(curvePreview.data(), 50, ImGui::accentPurpleMedium3, ImDrawFlags_None, 2.5f);
//        
//
//        window->DrawList->AddCircleFilled(absBegPos, 5.0f, ImGui::accentGrayDark);
//        window->DrawList->AddCircleFilled(absBegPos, 4.0f, ImGui::accentPurpleBright);
//
//        window->DrawList->AddCircleFilled(absMidPos, 4.0f, ImGui::accentGrayDark);
//        window->DrawList->AddCircleFilled(absMidPos, 3.0f, ImGui::accentPurpleBright);
//
//        window->DrawList->AddCircleFilled(absEndPos, 5.0f, ImGui::accentGrayDark);
//        window->DrawList->AddCircleFilled(absEndPos, 4.0f, ImGui::accentPurpleBright);
//        
//        // Check whether point is hovered
//        bool begHovered = pow(mousePos.x - absBegPos.x, 2) + pow(mousePos.y - absBegPos.y, 2) < 25.0f;
//        bool midHovered = pow(mousePos.x - absMidPos.x, 2) + pow(mousePos.y - absMidPos.y, 2) < 25.0f;
//        bool endHovered = pow(mousePos.x - absEndPos.x, 2) + pow(mousePos.y - absEndPos.y, 2) < 25.0f;
//
//
//        // Move points if clicked and hovered
//        // Also clamp from 0.0 to 1.0
//        if (leftMouseDown && begHovered || curve.startMoving) {
//            *curve.startPos = (mousePos - editorbb.Min) / (editorbb.Max - editorbb.Min);
//            curve.startMoving = leftMouseDown;
//            curve.startPos->x = curve.startPos->x > 1.0f ? 1.0f : curve.startPos->x;
//            curve.startPos->x = curve.startPos->x < 0.0f ? 0.0f : curve.startPos->x;
//            curve.startPos->y = curve.startPos->y > 1.0f ? 1.0f : curve.startPos->y;
//            curve.startPos->y = curve.startPos->y < 0.0f ? 0.0f : curve.startPos->y;
//        }
//
//        if (leftMouseDown && midHovered || curve.midMoving) {
//            *curve.midPos = (mousePos - editorbb.Min) / (editorbb.Max - editorbb.Min);
//            curve.midMoving = leftMouseDown;
//            curve.midPos->x = curve.midPos->x > 1.0f ? 1.0f : curve.midPos->x;
//            curve.midPos->x = curve.midPos->x < 0.0f ? 0.0f : curve.midPos->x;
//            curve.midPos->y = curve.midPos->y > 1.0f ? 1.0f : curve.midPos->y;
//            curve.midPos->y = curve.midPos->y < 0.0f ? 0.0f : curve.midPos->y;
//        }
//        
//        if (leftMouseDown && endHovered || curve.endMoving) {
//            *curve.endPos = (mousePos - editorbb.Min) / (editorbb.Max - editorbb.Min);
//            curve.endMoving = leftMouseDown;
//            curve.endPos->x = curve.endPos->x > 1.0f ? 1.0f : curve.endPos->x;
//            curve.endPos->x = curve.endPos->x < 0.0f ? 0.0f : curve.endPos->x;
//            curve.endPos->y = curve.endPos->y > 1.0f ? 1.0f : curve.endPos->y;
//            curve.endPos->y = curve.endPos->y < 0.0f ? 0.0f : curve.endPos->y;
//        }
//
//        // If the dot gets rightclicked (middle dot only)
//        // Reset it to 50% in the middle
//        if (rightMouseDown && midHovered)
//            *curve.midPos = *curve.startPos + (*curve.endPos - *curve.startPos) / 2.0f;
//    }
//   
//    //// If the user double left clicks onto a line, create a new bezier curve and connect them
//    //bool leftDoubleClicked = g.IO.MouseClickedCount[0] == 2;
//    //if (leftDoubleClicked) {
//    //
//    //    // Check which point from the line is closest
//    //    // closestPos is where the new point will be
//    //    ImVec2 closestPos = { 0.0f, 0.0f };
//    //
//    //    // tValue will select the start and end point for the new curve
//    //    float tValue = 0.0f;
//    //
//    //    // This is the curve that needs modification
//    //    std::vector<ImGui::BezierCurve>::iterator curveToMod;
//    //
//    //    float smallestDist = FLT_MAX;
//    //
//    //    // Go through all curves and points and get the closest one
//    //    const auto curvesEnd = curves->end();
//    //    for (auto it = curves->begin(); it != curvesEnd; ++it) {
//    //        for (int i = 0; i < 100; i++) {
//    //            float t = static_cast<float>(i) / 100.0f;
//    //            ImVec2 pos = editorbb.Min + it->getRelPosAtT(t) * editorSize;
//    //            float distsq = static_cast<float>(pow(pos.x - mousePos.x, 2) + pow(pos.y - mousePos.y, 2));
//    //         
//    //            if (distsq > 40.0f)
//    //                continue;
//    //            
//    //            if (distsq > smallestDist)
//    //                continue;
//    //            
//    //            curveToMod = it;
//    //            tValue = t;
//    //            smallestDist = distsq;
//    //            closestPos = pos;
//    //        }
//    //    }
//    //
//    //    // Convert pos to relative
//    //    ImVec2 newRelPos = (closestPos - editorbb.Min) / (editorbb.Max - editorbb.Min);
//    //
//    //    ImVec2* newPoint = const_cast<ImVec2*>(&(*points->insert(newRelPos).first));
//    //    
//    //    // We always need to make 2 new points
//    //    if (tValue > 0.0f && tValue < 0.5f) { // Between start and mid
//    //
//    //        // Save the old point
//    //        ImVec2 oldEndPos = *curveToMod->endPos;
//    //        ImVec2* oldEndPtr = curveToMod->endPos;
//    //
//    //        // Modify the curve to make it work
//    //        curveToMod->endPos = curveToMod->midPos;
//    //        curveToMod->midPos = newPoint;
//    //
//    //        // Create new curve
//    //        newRelPos = *curveToMod->endPos - (*curveToMod->endPos - oldEndPos) / 2.0f;
//    //
//    //        ImVec2* newBeg = curveToMod->endPos;
//    //        ImVec2* newMid = const_cast<ImVec2*>(&(*points->insert(newRelPos).first));
//    //        ImVec2* newEnd = oldEndPtr;
//    //
//    //        ImGui::BezierCurve newCurve(newBeg, newMid, newEnd);
//    //        auto curve = curves->insert(curveToMod, newCurve);
//    //        curve->boundRect = editorbb;
//    //    }
//    //
//    //    // We always need to make 2 new points
//    //    if (tValue > 0.5f && tValue < 1.0f) { // Between mid and end
//    //    
//    //        // Save the old point
//    //        ImVec2 oldBegPos = *curveToMod->startPos;
//    //        ImVec2* oldBegPtr = curveToMod->startPos;
//    //    
//    //        // Modify the curve to make it work
//    //        curveToMod->startPos = curveToMod->midPos;
//    //        curveToMod->midPos = newPoint;
//    //    
//    //        // Create new curve
//    //        newRelPos = oldBegPos + (*curveToMod->startPos - oldBegPos) / 2.0f;
//    //        
//    //        ImVec2* newBeg = oldBegPtr;
//    //        ImVec2* newMid = const_cast<ImVec2*>(&(*points->insert(newRelPos).first));
//    //        ImVec2* newEnd = curveToMod->startPos;
//    //        
//    //        ImGui::BezierCurve newCurve(newBeg, newMid, newEnd);
//    //        auto curve = curves->insert(curveToMod, newCurve);
//    //        curve->boundRect = editorbb;
//    //    }
//    //}
//    //
//    //// If the user double right clicks onto a main dot, delete it and reconnect the curves
//    //bool rightDoubleClicked = g.IO.MouseClickedCount[1] == 2;
//    //if (rightDoubleClicked) {
//    //
//    //    // This is the curve that needs modification
//    //    ImGui::BezierCurve* curve1 = nullptr;
//    //    ImGui::BezierCurve* curve2 = nullptr;
//    //
//    //    ImVec2* targetPtr = nullptr;
//    //    {
//    //
//    //        // Go through all points and find the closest one
//    //        float smallestDist = FLT_MAX;
//    //
//    //        const auto pointsEnd = points->end();
//    //
//    //        for (auto it = points->begin(); it != pointsEnd; it++) {
//    //            ImVec2 absPos = *it * editorbb.GetSize() + editorbb.Min;
//    //            float distsq = static_cast<float>(pow(absPos.x - mousePos.x, 2) + pow(absPos.y - mousePos.y, 2));
//    //            
//    //            if (distsq > 25.0f)
//    //                continue;
//    //            
//    //            if (distsq > smallestDist)
//    //                continue;
//    //            
//    //            smallestDist = distsq;
//    //            targetPtr = const_cast<ImVec2*>(&(*it));
//    //        }
//    //
//    //        // Find 2 curves with the same ptr as end / start
//    //        for (ImGui::BezierCurve& curve : *curves) {
//    //            if (curve.startPos == targetPtr)
//    //                curve1 = &curve;
//    //            if (curve.endPos == targetPtr)
//    //                curve2 = &curve;
//    //        }
//    //    }
//    //
//    //    if (curve1 == nullptr || curve2 == nullptr)
//    //        return false;
//    //
//    //    // curve1 has the point as start
//    //    // curve2 has the point as end
//    //
//    //    // curve2 startPos == final startPos
//    //    // curve1 startPos == final midPos
//    //    // curve1 endPos == final endPos
//    //
//    //    // Delete both mid points
//    //
//    //    //*curve2->startPos = ImVec2(0.1f, 0.9f);
//    //    //curve2->midPos = curve1->startPos;
//    //    //curve2->endPos = curve1->endPos;
//    //
//    //    //curve2->midPos->x += 0.5f;
//    //
//    //    points->erase(*curve1->midPos);
//    //    points->erase(*curve2->midPos);
//    //
//    //    curve2->endPos = curve1->endPos;
//    //    curve2->midPos = targetPtr;
//    //    
//    //    for (auto it = curves->begin(); it != curves->end(); ++it) {
//    //        if (&(*it) != curve1)
//    //            continue;
//    //        curves->erase(it);
//    //        break;
//    //    }
//    //
//    //
//    //}
//
//    return false;
//}

void ImGui::cBeginChild(const char* label, const ImVec2& sizeArg, const char* icon) {

    ImGuiContext& g = *GImGui;
    ImGuiWindow* parentWindow = g.CurrentWindow;
    ImGuiStyle& style = g.Style;
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove;

    const ImVec2 sizeAvailable = GetContentRegionAvail();
    const ImVec2 size = CalcItemSize(sizeArg, sizeAvailable.x, sizeAvailable.y);

    const float titleHeight = 45.0f;
    const ImRect fullbb = {
       g.CurrentWindow->DC.CursorPos,
       ImVec2(
           g.CurrentWindow->DC.CursorPos.x + size.x,
           g.CurrentWindow->DC.CursorPos.y + size.y
       )
    };

    SetNextWindowSize(size);

    g.NextWindowData.ChildFlags = 0;
    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasChildFlags;

    const char* tempWindowName;
    ImGuiID id = GetCurrentWindow()->GetID(label);
    ImFormatStringToTempBuffer(&tempWindowName, NULL, "%s/%s_%08X", parentWindow->Name, label, id);

    const bool ret = Begin(tempWindowName, NULL, windowFlags);
    if (!ret)
        return;

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - style.WindowPadding.y + style.ChildBorderSize);

    ImGuiWindow* childWindow = g.CurrentWindow;
    childWindow->ChildId = id;
    const ImVec2 labelSize = ImGui::CalcTextSize(label);
    const ImVec2 labelPos = { fullbb.Min.x + style.WindowPadding.x, fullbb.Min.y + titleHeight / 2.0f - labelSize.y / 2.0f };
    childWindow->DrawList->AddText(labelPos, ImGui::textColorDark, label);

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + titleHeight);
}

void ImGui::cEndChild() {
    ImGuiContext& g = *GImGui;
    ImGuiWindow* childWindow = g.CurrentWindow;

    const ImGuiID backupWithinEndChildId = g.WithinEndChildID;
    IM_ASSERT(childWindow->Flags & ImGuiWindowFlags_ChildWindow);

    g.WithinEndChildID = childWindow->ID;
    ImVec2 childSize = childWindow->Size;
    End();
    if (childWindow->BeginCount == 1) {
        ImGuiWindow* parentWindow = g.CurrentWindow;
        ImRect bb(parentWindow->DC.CursorPos, parentWindow->DC.CursorPos + childSize);
        ItemSize(childSize);
        const bool navFlattened = (childWindow->ChildFlags & ImGuiChildFlags_NavFlattened) != 0;
        if ((childWindow->DC.NavLayersActiveMask != 0 || childWindow->DC.NavWindowHasScrollY) && !navFlattened) {
            ItemAdd(bb, childWindow->ChildId);
            RenderNavCursor(bb, childWindow->ChildId);

            if (childWindow->DC.NavLayersActiveMask == 0 && childWindow == g.NavWindow)
                RenderNavCursor(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavRenderCursorFlags_Compact);
        }
        else {
            ItemAdd(bb, childWindow->ChildId, NULL, ImGuiItemFlags_NoNav);

            if (navFlattened)
                parentWindow->DC.NavLayersActiveMaskNext |= childWindow->DC.NavLayersActiveMaskNext;
        }
        if (g.HoveredWindow == childWindow)
            g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
        childWindow->DC.ChildItemStatusFlags = g.LastItemData.StatusFlags;
    }

    g.WithinEndChildID = backupWithinEndChildId;
    g.LogLinePosY = -FLT_MAX;
}

bool ImGui::TabButton(const char* label, const char* icon, bool* state) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 size = { 120.0f, 20.0f };
    const ImRect fullbb(pos, pos + size);
    ItemSize(size);
    if (!ItemAdd(fullbb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(fullbb, id, &hovered, &held, ImGuiButtonFlags_MouseButtonLeft);
    if (pressed && !*state)
        *state = !*state;

    static AnimEngine* animEngine = nullptr;
    if (!classManager.getClass<AnimEngine>("AnimEngine", &animEngine))
        return false;
    static WidgetController* controller = nullptr;
    if (!animEngine->getController(id, &controller, true))
        return false;

    controller->updateState(hovered, *state);

    const ImColor iconColor = controller->processColor(ImGui::textColorDark, ImGui::textColorLight, controller->tClick);
    const ImColor textCol = controller->processColor(ImGui::textColorDark, ImGui::textColorLight, controller->tClick);

    const ImVec2 labelSize = CalcTextSize(label, NULL, true);
    const ImVec2 iconSize = CalcTextSize(icon, NULL, true);

    const float totalWidth = labelSize.x + iconSize.x + style.ItemInnerSpacing.x * 2;

    const ImVec2 iconPos = {
        pos.x + size.x / 2.0f - totalWidth / 2.0f,
        pos.y + size.y / 2.0f - iconSize.y / 2.0f
    };
    window->DrawList->AddText(iconPos, iconColor, icon);

    const ImVec2 textPos = {
       iconPos.x + iconSize.x + style.ItemInnerSpacing.x,
       iconPos.y + iconSize.y / 2.0f - labelSize.y / 2.0f
    };
    window->DrawList->AddText(textPos, textCol, label);

    return pressed;
}
bool ImGui::TopBarTabs(const std::vector<std::string>& tabNames, std::string* currentSelected, const std::unordered_map<std::string, const char*>& tabIcons) {

    bool hasChanged = false;

    static ImVec2 windowSize = ImGui::GetWindowSize();
    static ImGuiStyle* style = &ImGui::GetStyle();
    static size_t tabCount = tabIcons.size();

    ImVec2 nextButtonPos = {
        windowSize.x - 120.0f * tabCount - style->ItemInnerSpacing.x * tabCount,
        windowSize.y / 2 - 40.0f / 2.0f
    };

    const auto& iconEnd = tabIcons.end();
    for (const auto& tabName : tabNames) {

        const auto& foundIcon = tabIcons.find(tabName);
        if (foundIcon == iconEnd)
            continue;

        ImGui::SameLine();

        ImGui::SetCursorPos(nextButtonPos);

        // Tab buttons are always 120x40
        nextButtonPos.x += 120.0f + style->ItemInnerSpacing.x;

        bool isSelected = tabName == *currentSelected;
        if (!ImGui::TabButton(tabName.c_str(), foundIcon->second, &isSelected))
            continue;

        *currentSelected = tabName;
        hasChanged = true;
    }

    return hasChanged;
}

bool ImGui::ComboBox(const char* label, const std::vector<ComboOption>& options, int* selectedIndex) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    const ImVec2 sizeAvailable = GetContentRegionAvail();
    ImVec2 size = CalcItemSize(ImVec2(0.0f, 80.0f), sizeAvailable.x, sizeAvailable.y);

    ImRect fullbb(pos, pos + size);

    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(fullbb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(fullbb, id, &hovered, &held, ImGuiButtonFlags_MouseButtonLeft);

    static AnimEngine* animEngine = nullptr;
    if (!classManager.getClass<AnimEngine>("AnimEngine", &animEngine))
        return false;
    static WidgetController* controller = nullptr;
    if (!animEngine->getController(id, &controller, true))
        return false;
    controller->updateState(hovered, pressed);

    // Background
    ImColor backgroundColor = controller->processColor(ImGui::accentPurpleMedium1, ImGui::accentPurpleMedium2, controller->tHover);
    window->DrawList->AddRectFilled(fullbb.Min, fullbb.Max, backgroundColor, 10.0f);

    // Icon background, centered left 80x80
    ImVec2 iconbbSize(40.0f, 40.0f);
    ImRect iconbb(
        pos + ImVec2(size.y, size.y) / 2.0f - iconbbSize / 2.0f,
        pos + ImVec2(size.y, size.y) / 2.0f - iconbbSize / 2.0f + iconbbSize
    );
    window->DrawList->AddRectFilled(iconbb.Min, iconbb.Max, ImGui::accentIconBackground, 10.0f);

    // Icon
    const ComboOption& selectedOption = options[*selectedIndex];
    ImVec2 iconSize = ImGui::CalcTextSize(selectedOption.icon);
    ImVec2 iconPos = iconbb.GetCenter() - iconSize / 2.0f;
    window->DrawList->AddText(iconPos, ImGui::textColorLight, selectedOption.icon);

    // Option title
    ImVec2 titlePos(
        iconbb.Max.x + style.WindowPadding.x,
        iconbb.Min.y
    );
    window->DrawList->AddText(titlePos, ImGui::textColorLight, selectedOption.name);

    // Option description
    ImVec2 descriptionPos(
        iconbb.Max.x + style.WindowPadding.x,
        iconbb.Min.y + iconbb.GetHeight() / 2.0f
    );
    window->DrawList->AddText(descriptionPos, ImGui::textColorMedium, selectedOption.description);

    // Expand arrow
    ImVec2 expandArea(40.0f, size.y); // center in 40px and full height
    ImRect arrowbb(
        ImVec2(fullbb.Max.x - expandArea.x, fullbb.Min.y),
        ImVec2(fullbb.Max.x, fullbb.Max.y)
    );

    ImVec2 arrowPoints[] = {
        ImVec2(arrowbb.Min.x + expandArea.x / 3.0f, arrowbb.Min.y + expandArea.y / 2.0f - expandArea.x / 12.0f),
        ImVec2(arrowbb.Min.x + expandArea.x / 2.0f, arrowbb.Min.y + expandArea.y / 2.0f + expandArea.x / 12.0f),
        ImVec2(arrowbb.Max.x - expandArea.x / 3.0f, arrowbb.Min.y + expandArea.y / 2.0f - expandArea.x / 12.0f),
    };
    window->DrawList->AddPolyline(arrowPoints, 3, ImGui::textColorMedium, ImDrawFlags_None, 1.0f);

    const ImGuiID popupId = ImHashStr("##CustomComboPopup", 0, id);
    bool popupOpen = IsPopupOpen(popupId, ImGuiPopupFlags_None);
    if (pressed && !popupOpen) {
        OpenPopupEx(popupId, ImGuiPopupFlags_None);
        popupOpen = true;
    }

    if (!IsPopupOpen(popupId, ImGuiPopupFlags_None)) {
        g.NextWindowData.ClearFlags();
        return false;
    }

    ImGuiWindowFlags windowFlags = 
        ImGuiWindowFlags_AlwaysAutoResize | 
        ImGuiWindowFlags_Popup | 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImColor oldStyleCol = ImGui::accentPurpleMedium1;
    ImGui::accentPurpleMedium1 = ImGui::GetColorU32(ImGuiCol_PopupBg);

    SetNextWindowPos(ImVec2(fullbb.Min.x, fullbb.Max.y + style.FramePadding.y));
    SetNextWindowSizeConstraints(ImVec2(size.x, 0), ImVec2(size.x, FLT_MAX));
    bool ret = Begin("##CustomComboPopup", NULL, windowFlags);
    if (!ret) {
        EndPopup();
        IM_ASSERT(0);
        ImGui::accentPurpleMedium1 = oldStyleCol;
        return false;
    }
    ImGui::PopStyleVar(); // WindowPadding

    // If the name of the current selected and the option match
    // Skip it
    int index = -1;
    for (const auto& option : options) {
        index++;
        if (strcmp(option.name, options[*selectedIndex].name) == 0)
            continue;
        if (!ImGui::ComboButton(option.name, option.description, option.icon))
            continue;
        *selectedIndex = index;
        ImGui::CloseCurrentPopup();
    }

    ImGui::accentPurpleMedium1 = oldStyleCol;

    EndPopup();

    return false;
}
bool ImGui::ComboButton(const char* label, const char* description, const char* icon) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    const ImVec2 sizeAvailable = GetContentRegionAvail();
    ImVec2 size = CalcItemSize(ImVec2(0.0f, 80.0f), sizeAvailable.x, sizeAvailable.y);

    ImRect fullbb(pos, pos + size);

    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(fullbb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(fullbb, id, &hovered, &held, ImGuiButtonFlags_MouseButtonLeft);

    static AnimEngine* animEngine = nullptr;
    if (!classManager.getClass<AnimEngine>("AnimEngine", &animEngine))
        return false;
    static WidgetController* controller = nullptr;
    if (!animEngine->getController(id, &controller, true))
        return false;
    controller->updateState(hovered, pressed);

    // Background
    ImColor backgroundColor = controller->processColor(ImGui::accentPurpleMedium1, ImGui::accentPurpleMedium2, controller->tHover);
    window->DrawList->AddRectFilled(fullbb.Min, fullbb.Max, backgroundColor, 10.0f);

    // Icon background, centered left 80x80
    ImVec2 iconbbSize(40.0f, 40.0f);
    ImRect iconbb(
        pos + ImVec2(size.y, size.y) / 2.0f - iconbbSize / 2.0f,
        pos + ImVec2(size.y, size.y) / 2.0f - iconbbSize / 2.0f + iconbbSize
    );
    window->DrawList->AddRectFilled(iconbb.Min, iconbb.Max, ImGui::accentIconBackground, 10.0f);

    // Icon
    ImVec2 iconSize = ImGui::CalcTextSize(icon);
    ImVec2 iconPos = iconbb.GetCenter() - iconSize / 2.0f;
    window->DrawList->AddText(iconPos, ImGui::textColorLight, icon);

    // Option title
    ImVec2 titlePos(
        iconbb.Max.x + style.WindowPadding.x,
        iconbb.Min.y
    );
    window->DrawList->AddText(titlePos, ImGui::textColorLight, label);

    // Option description
    ImVec2 descriptionPos(
        iconbb.Max.x + style.WindowPadding.x,
        iconbb.Min.y + iconbb.GetHeight() / 2.0f
    );
    window->DrawList->AddText(descriptionPos, ImGui::textColorMedium, description);

    return pressed;
}

bool ImGui::ChildContentSlider(std::function<void()> currentContent, std::function<void()> nextContent, int direction) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;

    double curTime = ImGui::GetTime();
    static double startTime = curTime;

    if ((curTime - startTime) > 0.22f)
        startTime = curTime;

    ImVec2 cursorPos = ImGui::GetCursorPos();

    // Modify the cursorpos based on time and the width
    float t = static_cast<float>(curTime - startTime) / 0.2f; // 0.0 -> X
    t = std::min(t, 1.0f); // 0.0 -> 1.0;
    t = t < 0.5f ? 4.0f * t * t * t : 1.0f - pow(-2.0f * t + 2.0f, 3.0f) / 2.0f; // https://easings.net/#easeInOutCubic
    float offset = t * window->Size.x;
    cursorPos.x += direction == 1 ? offset : -offset;
    
    ImGui::SetCursorPos(cursorPos);
    currentContent();

    cursorPos.x += direction == -1 ? window->Size.x : -window->Size.x;
    ImGui::SetCursorPos(cursorPos);
    nextContent();

    if (t >= 1.0f)
        return false; // Finished

    return true; // Moving
}
