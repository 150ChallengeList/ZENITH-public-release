#include "CustomWidgets.hpp"
#include <imgui_internal.h>
#include "AnimEngine.hpp"
#include <unordered_set>
#include <imgui.h>

// Without that 100+ errors

inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) {
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) {
    return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline ImVec2 operator*(const ImVec2& lhs, float scalar) {
    return ImVec2(lhs.x * scalar, lhs.y * scalar);
}

inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) {
    return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y);
}

inline ImVec2 operator/(const ImVec2& lhs, float scalar) {
    return ImVec2(lhs.x / scalar, lhs.y / scalar);
}

inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) {
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

inline ImVec2& operator*=(ImVec2& lhs, float scalar) {
    lhs.x *= scalar;
    lhs.y *= scalar;
    return lhs;
}

inline ImVec2& operator/=(ImVec2& lhs, float scalar) {
    lhs.x /= scalar;
    lhs.y /= scalar;
    return lhs;
}

bool ImGui::Switch(const char* label, bool* state) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    const ImVec2 sizeAvailable = GetContentRegionAvail();
    ImVec2 size = CalcItemSize(ImVec2(0.0f, 30.0f), sizeAvailable.x, sizeAvailable.y);

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

bool ImGui::SliderFlt(const char* label, float* state, float min, float max, const char* fmt) {
    return SliderWrapper(label, state, ImGuiDataType_Float, &min, &max, fmt);
}
bool ImGui::SliderInt(const char* label, int* state, int min, int max, const char* fmt) {
    return SliderWrapper(label, state, ImGuiDataType_S32, &min, &max, fmt);
}
bool ImGui::SliderDouble(const char* label, double* state, double min, double max, const char* fmt) {
    return SliderWrapper(label, state, ImGuiDataType_Double, &min, &max, fmt);
}
bool ImGui::SliderWrapper(const char* label, void* state, ImGuiDataType dataType, const void* min, const void* max, const char* fmt) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 labelSize = CalcTextSize(label, NULL, true);
    const ImVec2 sizeAvailable = GetContentRegionAvail();

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcItemSize(ImVec2(0.0f, 40.0f), sizeAvailable.x, sizeAvailable.y);

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

bool ImGui::BezierEditor(const char* label, ImVec2* pos1, ImVec2* pos2) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImGuiID resetId = window->GetID("reset");
    const ImGuiID previewId = window->GetID("preview");

    ImVec2 pos = window->DC.CursorPos;
    const ImVec2 sizeAvailable = GetContentRegionAvail();
    ImVec2 size = CalcItemSize(ImVec2(0.0f, 140.0f), sizeAvailable.x, sizeAvailable.y);

    ImRect topbb(pos, pos + ImVec2(size.x, 40.0f));

    ImVec2 editorSize(size.x, 100.0f);
    const ImRect editorbb(pos + ImVec2(0, 25), pos + editorSize + ImVec2(0, 25));
    ItemSize(size, style.FramePadding.y);
    if (!ItemAdd(editorbb, id))
        return false;

    static AnimEngine* animEngine = nullptr;
    if (!classManager.getClass<AnimEngine>("AnimEngine", &animEngine))
        return false;

    // Top left of editor
    ImVec2 startPos = { editorbb.Min.x, editorbb.Max.y };

    // Bottom right of editor
    ImVec2 endPos = { editorbb.Max.x, editorbb.Min.y };

    // Left grabable circle
    ImVec2 leftCircle = startPos + ImVec2(pos1->x * editorSize.x, -(pos1->y * editorSize.y));

    // Right grabable circle
    ImVec2 rightCircle = endPos - ImVec2(pos2->x * editorSize.x, -(pos2->y * editorSize.y));

    // Return true if the curve was modified
    bool isModified = false;

    // Check if the user is pressing on the circles
    static bool isLeftPressed = false;
    static bool isRightPressed = false;
    float distanceLeft = static_cast<float>(pow(g.IO.MousePos.x - leftCircle.x, 2)) + static_cast<float>(pow(g.IO.MousePos.y - leftCircle.y, 2));
    if (g.IO.MouseDown[0]) {
        if (!isLeftPressed && distanceLeft < 30.0f && !isRightPressed)
            isLeftPressed = true;
    }
    else {
        isLeftPressed = false;
    }
    if (isLeftPressed) {
        pos1->x = (g.IO.MousePos.x - editorbb.Min.x) / editorSize.x;
        pos1->x = pos1->x > 1.0f ? 1.0f : pos1->x;
        pos1->x = pos1->x < 0.0f ? 0.0f : pos1->x;

        pos1->y = (editorbb.Min.y - g.IO.MousePos.y + editorSize.y) / editorSize.y;
        pos1->y = pos1->y > 1.0f ? 1.0f : pos1->y;
        pos1->y = pos1->y < 0.0f ? 0.0f : pos1->y;
        isModified = true;
    }

    float distanceRight = static_cast<float>(pow(g.IO.MousePos.x - rightCircle.x, 2)) + static_cast<float>(pow(g.IO.MousePos.y - rightCircle.y, 2));
    if (g.IO.MouseDown[0]) {
        if (!isRightPressed && distanceRight < 30.0f && !isLeftPressed)
            isRightPressed = true;
    }
    else {
        isRightPressed = false;
    }
    if (isRightPressed) {
        pos2->x = (editorbb.Min.x - g.IO.MousePos.x + editorSize.x) / editorSize.x;
        pos2->x = pos2->x > 1.0f ? 1.0f : pos2->x;
        pos2->x = pos2->x < 0.0f ? 0.0f : pos2->x;

        pos2->y = (g.IO.MousePos.y - editorbb.Min.y) / editorSize.y;
        pos2->y = pos2->y > 1.0f ? 1.0f : pos2->y;
        pos2->y = pos2->y < 0.0f ? 0.0f : pos2->y;
        isModified = true;
    }

    // Background
    window->DrawList->AddRectFilled(editorbb.Min, editorbb.Max, ImGui::accentGrayMedium1, 10.0f);

    // Background cross
    window->DrawList->AddLine(
        editorbb.GetCenter() + ImVec2(0.0f, editorbb.GetHeight() / 2),
        editorbb.GetCenter() - ImVec2(0.0f, editorbb.GetHeight() / 2),
        ImGui::accentGrayMedium2,
        2.0f
    );
    window->DrawList->AddLine(
        editorbb.GetCenter() + ImVec2(editorbb.GetWidth() / 2, 0.0f),
        editorbb.GetCenter() - ImVec2(editorbb.GetWidth() / 2, 0.0f),
        ImGui::accentGrayMedium2,
        2.0f
    );

    // left circle's dotted lines
    float leftDistance = static_cast<float>(sqrt(pow(startPos.x - leftCircle.x, 2) + pow(startPos.y - leftCircle.y, 2)));
    int leftPointCount = static_cast<int>(round(leftDistance)) / 10;
    for (int i = 0; i < leftPointCount; i++) {
        float t = static_cast<float>(i) / leftPointCount;

        ImVec2 posStart = startPos * t + leftCircle * (1.0f - t);
        posStart += (startPos - leftCircle) / (leftDistance / 3.0f);

        ImVec2 posEnd = startPos * t + leftCircle * (1.0f - t);
        posEnd -= (startPos - leftCircle) / (leftDistance / 3.0f);

        window->DrawList->AddLine(posStart, posEnd, ImGui::accentGrayDark, 2.0f);
    }

    // left circle to grab
    window->DrawList->AddCircleFilled(leftCircle, 5.0f, ImGui::accentPurpleBright);

    // right circle's dotted lines
    float rightDistance = static_cast<float>(sqrt(pow(endPos.x - rightCircle.x, 2) + pow(endPos.y - rightCircle.y, 2)));
    int rightPointCount = static_cast<int>(round(rightDistance)) / 10;
    for (int i = 0; i < rightPointCount; i++) {
        float t = static_cast<float>(i) / rightPointCount;

        ImVec2 posStart = endPos * t + rightCircle * (1.0f - t);
        posStart += (endPos - rightCircle) / (rightDistance / 3.0f);

        ImVec2 posEnd = endPos * t + rightCircle * (1.0f - t);
        posEnd -= (endPos - rightCircle) / (rightDistance / 3.0f);

        window->DrawList->AddLine(posStart, posEnd, ImGui::accentGrayDark, 2.0f);
    }

    // right circle to grab
    window->DrawList->AddCircleFilled(rightCircle, 5.0f, ImGui::accentPurpleBright);

    // Draw the bezier curve
    static ImGuiID previewEditor = 0;
    static double previewStartTime = 0.0;
    for (int i = 0; i < 100; i++) {
        float t = static_cast<float>(i) / 100.0f;

        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;

        static ImVec2 posBefore = startPos;
        ImVec2 posNow =
            startPos * uuu +
            (startPos + ImVec2(pos1->x, -pos1->y) * editorSize) * 3.0f * uu * t +
            (endPos - ImVec2(pos2->x, -pos2->y) * editorSize) * 3.0f * u * tt +
            endPos * ttt;

        if (i == 0) {
            posBefore = posNow;
            continue;
        }

        window->DrawList->AddLine(posBefore, posNow, ImGui::accentPurpleBright, 3.0f);

        // If the previeEditor is this one, show the flow of the bezier curve
        if (previewEditor == id && static_cast<int>(ImGui::GetTime() * 85 - previewStartTime * 85) == i) {
            window->DrawList->AddLine(ImVec2(editorbb.Min.x, posNow.y), ImVec2(editorbb.Max.x, posNow.y), ImGui::accentIconBackground, 3.0f);
            window->DrawList->AddCircleFilled(posNow, 4.0f, ImGui::accentIconBackground);
        }

        posBefore = posNow;
    }

    // Reset the preview editor
    if (previewEditor == id && (ImGui::GetTime() - previewStartTime) > 1.5) {
        previewEditor = 0;
        previewStartTime = 0.0;
    }

    // Label of the widget
    ImVec2 labelPos = pos;
    window->DrawList->AddText(labelPos, ImGui::textColorDark, label);

    // Preview button to see the animation flow
    static WidgetController* previewController = nullptr;
    if (!animEngine->getController(previewId, &previewController, true))
        return false;
    ImVec2 previewLabelSize = ImGui::CalcTextSize("Preview");
    ImVec2 previewLabelPos = {
        ImVec2(
            topbb.Max.x - previewLabelSize.x,
            pos.y
        )
    };
    bool previewHovered =
        previewLabelPos.x < g.IO.MousePos.x &&
        previewLabelPos.y < g.IO.MousePos.y &&
        (previewLabelPos.x + previewLabelSize.x) > g.IO.MousePos.x &&
        (previewLabelPos.y + previewLabelSize.y) > g.IO.MousePos.y;
    previewController->updateState(previewHovered, false);
    ImColor previewTextCol = previewController->processColor(ImGui::textColorDark, ImGui::textColorLight, previewController->tHover);
    window->DrawList->AddText(previewLabelPos, previewTextCol, "Preview");
    if (previewHovered && g.IO.MouseDown[0] && previewEditor == 0) {
        previewEditor = id;
        previewStartTime = ImGui::GetTime();
    }

    // Reset button to reset the positions
    static WidgetController* resetController = nullptr;
    if (!animEngine->getController(resetId, &resetController, true))
        return false;
    ImVec2 resetLabelSize = ImGui::CalcTextSize("Reset");
    ImVec2 resetLabelPos = {
        ImVec2(
            topbb.Max.x - resetLabelSize.x - previewLabelSize.x - g.Style.WindowPadding.x,
            pos.y
        )
    };
    bool resetHovered =
        resetLabelPos.x < g.IO.MousePos.x &&
        resetLabelPos.y < g.IO.MousePos.y &&
        (resetLabelPos.x + resetLabelSize.x) > g.IO.MousePos.x &&
        (resetLabelPos.y + resetLabelSize.y) > g.IO.MousePos.y;
    resetController->updateState(resetHovered, false);
    ImColor resetTextCol = resetController->processColor(ImGui::textColorDark, ImGui::textColorLight, resetController->tHover);
    window->DrawList->AddText(resetLabelPos, resetTextCol, "Reset");
    if (resetHovered && g.IO.MouseDown[0]) {
        pos1->x = 0.0f;
        pos2->x = 0.0f;
        pos1->y = 0.0f;
        pos2->y = 0.0f;
        isModified = true;
    }

    return isModified;
}

void ImGui::cBeginChild(const char* label, const ImVec2& sizeArg, const char* icon) {

    ImGuiContext& g = *GImGui;
    ImGuiWindow* parentWindow = g.CurrentWindow;
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

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - g.Style.WindowPadding.y + g.Style.ChildBorderSize);

    ImGuiWindow* childWindow = g.CurrentWindow;
    childWindow->ChildId = id;
    const ImVec2 labelSize = ImGui::CalcTextSize(label);
    const ImVec2 labelPos = { fullbb.Min.x + g.Style.WindowPadding.x, fullbb.Min.y + titleHeight / 2.0f - labelSize.y / 2.0f };
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

    const ImColor iconColor = controller->processColor(ImGui::textColorDark, ImGui::accentIconBackground, controller->tClick);
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
bool ImGui::TopBarTabs(const std::vector<std::string>& tabNames, std::string* currentSelected, const std::unordered_map<std::string, const char*> tabIcons) {

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
        iconbb.Max.x + g.Style.WindowPadding.x,
        iconbb.Min.y
    );
    window->DrawList->AddText(titlePos, ImGui::textColorLight, selectedOption.name);

    // Option description
    ImVec2 descriptionPos(
        iconbb.Max.x + g.Style.WindowPadding.x,
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

    SetNextWindowPos(ImVec2(fullbb.Min.x, fullbb.Max.y + g.Style.FramePadding.y));
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
        if (!ImGui::CustomButton(option.name, option.description, option.icon))
            continue;
        *selectedIndex = index;
        ImGui::CloseCurrentPopup();
    }

    ImGui::accentPurpleMedium1 = oldStyleCol;

    EndPopup();

    return false;
}
bool ImGui::CustomButton(const char* label, const char* description, const char* icon) {

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
        iconbb.Max.x + g.Style.WindowPadding.x,
        iconbb.Min.y
    );
    window->DrawList->AddText(titlePos, ImGui::textColorLight, label);

    // Option description
    ImVec2 descriptionPos(
        iconbb.Max.x + g.Style.WindowPadding.x,
        iconbb.Min.y + iconbb.GetHeight() / 2.0f
    );
    window->DrawList->AddText(descriptionPos, ImGui::textColorMedium, description);

    return pressed;
}


//bool ImGui::ChildContentSlider(std::function<void()> currentContent, std::function<void()> nextContent, int direction) {
//
//    ImGuiWindow* window = GetCurrentWindow();
//    if (window->SkipItems)
//        return false;
//
//    ImGuiContext& g = *GImGui;
//
//    double curTime = ImGui::GetTime();
//    static double startTime = curTime;
//
//    if ((curTime - startTime) > 0.12f)
//        startTime = curTime;
//
//    ImVec2 cursorPos = ImGui::GetCursorPos();
//
//    // Modify the cursorpos based on time and the width
//    float t = static_cast<float>(curTime - startTime) / 0.1f; // 0.0 -> X
//    t = std::min(t, 1.0f); // 0.0 -> 1.0;
//    t = t < 0.5f ? 4.0f * t * t * t : 1.0f - pow(-2.0f * t + 2.0f, 3.0f) / 2.0f; // https://easings.net/#easeInOutCubic
//    float offset = t * window->Size.x;
//    cursorPos.x += direction == 1 ? offset : -offset;
//    
//    ImGui::SetCursorPos(cursorPos);
//    currentContent();
//
//    cursorPos.x += direction == -1 ? window->Size.x : -window->Size.x;
//    ImGui::SetCursorPos(cursorPos);
//    nextContent();
//
//    if (t >= 1.0f)
//        return false; // Finished
//
//    return true; // Moving
//}
