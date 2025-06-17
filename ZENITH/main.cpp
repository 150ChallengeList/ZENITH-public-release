#include <thread>
#include <Windows.h>

#include <Globals.hpp>

#include <WindowManager/WindowManager.hpp>
#include <WindowManager/WindowManager_ImGui.hpp>

#include <ClassManager/ClassManager.hpp>

#include <Inter.hpp>
#include <LucideData.hpp>
#include <LucideDefinitions.hpp>

#include <AnimEngine/CustomWidgets.hpp>

// joinked from maple source lmao

void setStyle() {

    ImGuiStyle* style = &ImGui::GetStyle();
    style->WindowPadding = ImVec2(15.0f, 15.0f);
    style->WindowRounding = 12.0f;
    style->PopupRounding = 12.0f;
    style->FramePadding = ImVec2(8.0f, 6.0f);
    style->ItemSpacing = ImVec2(8.0f, 6.0f);
    style->ItemInnerSpacing = ImVec2(4.0f, 4.0f);

    ImVec4* colors = style->Colors;
    colors[ImGuiCol_Border] = ImGui::rgbaToImVec4(0, 0, 0, 0);
    colors[ImGuiCol_WindowBg] = ImGui::rgbaToImVec4(22, 22, 34, 255);
    colors[ImGuiCol_PopupBg] = ImGui::rgbaToImVec4(30, 30, 46, 255);
    colors[ImGuiCol_ChildBg] = ImGui::rgbaToImVec4(0, 0, 0, 0);

    ImGuiIO* io = &ImGui::GetIO();
    {
        ImFontConfig config;
        config.FontDataOwnedByAtlas = false;
        io->Fonts->AddFontFromMemoryTTF(inter, sizeof(inter), 22.0f, &config);

        config.MergeMode = true;
        config.GlyphMinAdvanceX = 13.0f;
        config.GlyphOffset = ImVec2(0.0f, 6.0f);
        static const ImWchar iconRanges[] = { ICON_MIN_LC, ICON_MAX_LC, 0 };
        io->Fonts->AddFontFromMemoryTTF(lucideFont, sizeof(lucideFont), 24.0f, &config, iconRanges);
        io->Fonts->AddFontFromMemoryTTF(inter, sizeof(inter), 26.0f, &config);
    }
}
void renderFn() {

    ImGuiStyle* style = &ImGui::GetStyle();

    classManager.render();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(900, 565));
    static ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize;
    
    ImGui::Begin("##Zenith", nullptr, flags);
    {
        const ImVec2 windowPos = ImGui::GetWindowPos();
        const ImVec2 windowSize = ImGui::GetWindowSize();
        
        const std::vector<std::string>& tabs = classManager.getTabNames();
        const std::unordered_map<std::string, const char*> tabIcons = {
            { "Assist", ICON_LC_PENCIL },
            { "Replay", ICON_LC_PLAY},
            { "Scripts", ICON_LC_CODE_XML },
            { "Config", ICON_LC_COG }
        };
        
        static std::string currentTab = tabs.front(); // The current tab 
        static std::string changeTo = currentTab;     // For now, the current
        static int scrollDirection = 0;
    
        ImGui::BeginChild("##TopBar", ImVec2(0.0f, 70.0f));
        {
            ImGui::BeginChild("##Logo", ImVec2(200.0f, 0.0f));
            {
                ImGui::Text("[Logo]");
            }
            ImGui::EndChild();
        
            ImGui::SameLine();
        
            ImGui::BeginChild("##Tabs", ImVec2(0.0f, 0.0f));
            {
                static std::string selected = tabs.front();
                if (ImGui::TopBarTabs(tabs, &selected, tabIcons)) {
                    currentTab = changeTo;  // Will change to itself
                    changeTo = selected;    // Will change to the next one
    
                    // Figure out what direction it should move (very bad code)
                    const auto& tabsBegin = tabs.begin();
                    const auto& tabsEnd = tabs.end();
    
                    const auto& it1 = std::find(tabsBegin, tabsEnd, currentTab);
                    const auto& it2 = std::find(tabsBegin, tabsEnd, changeTo);
    
                    if (it1 != tabsEnd && it2 != tabsEnd && it1 > it2)
                        scrollDirection = 1;
                    else
                        scrollDirection = -1;
                }
            }
            ImGui::EndChild();
        }
        ImGui::EndChild();
        
        ImGui::BeginChild("##BottomContent", ImVec2(870.0f, 0.0f));
        {
            if (changeTo != currentTab) {
                // Horrible, cursed, but works
                if (!ImGui::ChildContentSlider(
                    [&]() {
                        ImGui::BeginChild("##CurContent", ImVec2(870.0f, 0.0f));
                        {
                            classManager.renderTab(currentTab);
                        }
                        ImGui::EndChild();
                    },
                    [&]() {
                        ImGui::BeginChild("##NewContent", ImVec2(870.0f, 0.0f));
                        {
                            classManager.renderTab(changeTo);
                        }
                        ImGui::EndChild();
                    },
                    scrollDirection))
                    currentTab = changeTo;
            } else {
                classManager.renderTab(currentTab);
            }
        }
        ImGui::EndChild();
    }
    
    ImGui::End();
}

void presentLoop() {
    while (!globals.window->shouldClose)
        globals.window->present();
}

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd) {

    AllocConsole();
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

    globals.window = new Window();
    globals.window->initWindow(instance, L"ZENITH", 1920 / 2 - 900 / 2, 1080 / 2 - 565 / 2, 900, 565, renderFn);
    setStyle();

    classManager.setState(&globals);
    
    classManager.addClass<Osu>("Osu");

    classManager.addClass<BeatmapParser>("Parser");
    classManager.addClass<AnimEngine>("AnimEngine");

    classManager.addClass<AimAssist>("AimAssist", "Assist");
    classManager.addClass<ReplayBot>("ReplayBot", "Replay");
    classManager.addClass<Cfg>("Cfg", "Config");
    classManager.addClass<Scripts>("Scripts", "Scripts");

    classManager.getClass<Osu>("Osu", &globals.osu);
    classManager.getClass<Cfg>("Cfg", &globals.cfg);
    classManager.getClass<Scripts>("Scripts", &globals.scripts);
    classManager.getClass<ReplayBot>("ReplayBot", &globals.replayBot);
    classManager.getClass<AimAssist>("AimAssist", &globals.aimAssist);
    classManager.getClass<BeatmapParser>("Parser", &globals.beatMapParser);

    classManager.init();

    while (!globals.window->shouldClose) {

        classManager.update();
        globals.beatMapParser->watchWindow(globals.osu->windowHandle);
        static long long accumulated = 0;
        static std::chrono::steady_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

        std::chrono::steady_clock::time_point curTime = std::chrono::high_resolution_clock::now();
        accumulated += std::chrono::duration_cast<std::chrono::nanoseconds>(curTime - lastTime).count();
        lastTime = curTime;

        // This runs at full speed
        globals.window->update();

        // This at 240hz
        if (accumulated < 4166666)
            continue;

        globals.window->render();
        globals.window->present();
        accumulated = 0;
    }

    globals.window->deinitWindow();

    return 0;
}