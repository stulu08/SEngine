#include "StyleEditor.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <Editor/EditorApp.h>

namespace Stulu {
    static void NodeFont(ImFont* font) {
        using namespace ImGui;
        bool opened = TreeNode(font, "Font: \"%s\"\n%.2f px, %d glyphs, %d file(s)",
            font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size, font->ConfigDataCount);
        SameLine();
        if (SmallButton("Set as default"))
            GetIO().FontDefault = font;
        if (!opened)
            return;

        // Display preview text
        PushFont(font);
        Text("The quick brown fox jumps over the lazy dog");
        PopFont();
        // Display details
        SetNextItemWidth(GetFontSize() * 8);
        DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");
        SameLine(); ComponentsRender::drawHelpMarker(
            "Note than the default embedded font is NOT meant to be scaled.\n\n"
            "Font are currently rendered into bitmaps at a given size at the time of building the atlas. "
            "You may oversample them to get some flexibility with scaling. "
            "You can also render at multiple sizes and select which one to use at runtime.\n\n"
            "(Glimmer of hope: the atlas system will be rewritten in the future to make scaling more flexible.)");
        Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
        char c_str[5];
        Text("Fallback character: '%s' (U+%04X)", ImTextCharToUtf8(c_str, font->FallbackChar), font->FallbackChar);
        Text("Ellipsis character: '%s' (U+%04X)", ImTextCharToUtf8(c_str, font->EllipsisChar), font->EllipsisChar);
        const int surface_sqrt = (int)ImSqrt((float)font->MetricsTotalSurface);
        Text("Texture Area: about %d px ~%dx%d px", font->MetricsTotalSurface, surface_sqrt, surface_sqrt);
        for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
            if (font->ConfigData)
                if (const ImFontConfig* cfg = &font->ConfigData[config_i])
                    BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d, Offset: (%.1f,%.1f)",
                        config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH, cfg->GlyphOffset.x, cfg->GlyphOffset.y);

        // Display all glyphs of the fonts in separate pages of 256 characters
        if (TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
        {
            ImDrawList* draw_list = GetWindowDrawList();
            const ImU32 glyph_col = GetColorU32(ImGuiCol_Text);
            const float cell_size = font->FontSize * 1;
            const float cell_spacing = GetStyle().ItemSpacing.y;
            for (unsigned int base = 0; base <= IM_UNICODE_CODEPOINT_MAX; base += 256)
            {
                // Skip ahead if a large bunch of glyphs are not present in the font (test in chunks of 4k)
                // This is only a small optimization to reduce the number of iterations when IM_UNICODE_MAX_CODEPOINT
                // is large // (if ImWchar==ImWchar32 we will do at least about 272 queries here)
                if (!(base & 4095) && font->IsGlyphRangeUnused(base, base + 4095))
                {
                    base += 4096 - 256;
                    continue;
                }

                int count = 0;
                for (unsigned int n = 0; n < 256; n++)
                    if (font->FindGlyphNoFallback((ImWchar)(base + n)))
                        count++;
                if (count <= 0)
                    continue;
                if (!TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
                    continue;

                // Draw a 16x16 grid of glyphs
                ImVec2 base_pos = GetCursorScreenPos();
                for (unsigned int n = 0; n < 256; n++)
                {
                    // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions
                    // available here and thus cannot easily generate a zero-terminated UTF-8 encoded string.
                    ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size + cell_spacing), base_pos.y + (n / 16) * (cell_size + cell_spacing));
                    ImVec2 cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
                    const ImFontGlyph* glyph = font->FindGlyphNoFallback((ImWchar)(base + n));
                    draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
                    if (!glyph)
                        continue;
                    font->RenderChar(draw_list, cell_size, cell_p1, glyph_col, (ImWchar)(base + n));
                    if (IsMouseHoveringRect(cell_p1, cell_p2))
                    {
                        BeginTooltip();
                        DebugNodeFontGlyph(font, glyph);
                        EndTooltip();
                    }
                }
                Dummy(ImVec2((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
                TreePop();
            }
            TreePop();
        }
        TreePop();
    }
    void StyleEditor::loadFonts() {
        //we need to make sure everytime the fonts have the same index, so we create a file to store the index
        ST_PROFILING_FUNCTION();
        ST_TRACE("Loading all Fonts from {0}{1}", Application::getStartDirectory(), getEditorDataPath() + "/Fonts/");
        std::string file = getEditorDataPath() + "/Fonts/fonts.txt";
        std::vector<std::string> fonts;

        //load fonts
        if (FileExists(file)) {
            std::unordered_map<std::string, std::string> values;
            int count = 0;
            std::fstream inStream(file, std::ios::in);
            std::string str;
            while (getline(inStream, str)) {
                str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
                size_t seperator = str.find_first_of("=");
                std::string index = str.substr(0, seperator);
                std::string path = str.substr(seperator + 1, str.npos);
                values[index] = path;
                count++;
            }
            inStream.close();
            //add laoded fonts
            int fontCount = 0;
            if (values.find("fontCount") != values.end())
                fontCount = std::stoi(values["fontCount"]);
            for (int i = 0; i < fontCount; i++) {
                fonts.insert(fonts.begin() + i, values[std::to_string(i)]);
            }
        }
        //add all fonts to fonts.txt which are not present
        for (auto& dir : std::filesystem::directory_iterator("Data/Editor/Fonts/")) {
            const auto& path = dir.path();
            if (path.extension() != ".ttf")
                continue;
            if (std::find(fonts.begin(), fonts.end(), path.filename().string()) == fonts.end()) {
                fonts.push_back(path.filename().string());
            }
            
        }
        //save and load fonts
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        if(FileExists(file))
            std::remove(file.c_str());
        std::fstream stream(file, std::ios::out);
        stream << "fontCount=" << fonts.size() << "\n";
        for (int i = 0; i < fonts.size(); i++) {
            io.Fonts->AddFontFromFileTTF(("Data/Editor/Fonts/" + fonts[i]).c_str(), 15.0f);
            stream << i << "=" << fonts[i] << "\n";
        }
        stream.close();
    }
    void StyleEditor::init() {
        ST_PROFILING_FUNCTION();
        ImGui::SetCurrentContext(Application::get().getImGuiLayer()->getContext());
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::LoadIniSettingsFromDisk(io.IniFilename);
        loadFonts();
        io.IniFilename = getEditorProject().windowINI.c_str();
        ImGuiStyle& style = ImGui::GetStyle();
        StyleFileData data = load();
        setStyleByInt(data.style);
        setFontByInt(data.font);
        style.WindowRounding = data.windowRounding;
        style.FrameRounding = data.FrameRounding;
        style.GrabRounding = data.FrameRounding;
        style.WindowBorderSize = (float)data.WindowBorder;
        style.FrameBorderSize = (float)data.FrameBorder;
        style.PopupBorderSize = (float)data.PopupBorder;
        io.FontGlobalScale = data.fontGlobalscale;
        style.AntiAliasedLines = data.AntiAliasedLines;
        style.AntiAliasedLinesUseTex = data.AntiAliasedLinesUseTex;
        style.AntiAliasedFill = data.AntiAliasedFill;
        style.CurveTessellationTol = data.CurveTessellationTol;
        style.CircleTessellationMaxError = data.CircleSegmentMaxError;
        style.Alpha = data.GlobalAlpha;
    }
    StyleEditor::StyleFileData StyleEditor::load() {
        ST_PROFILING_FUNCTION();
        StyleFileData data;
        std::string file = getEditorProject().configPath + "/editor-style.ini";
        if (!FileExists(file)) {
            save();
            return data;
        }
        //name, value
        std::unordered_map<std::string, std::string> values;
        int count = 0;
        std::fstream stream(file, std::ios::in);
        std::string str;
        while (getline(stream, str)) {
            str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
            size_t seperator = str.find_first_of("=");
            std::string name = str.substr(0, seperator);
            std::string data = str.substr(seperator+1, str.npos);
            values[name] = data;
            count++;
        }
        if (count < ST_STYLE_DATA_FILE_COUNT) {
            ST_WARN("Loading default editor settings, file does not contain needed data");
            return data;
        }
        data.font = std::stoi(values["font"]);
        data.style = std::stoi(values["style"]);
        data.windowRounding = std::stof(values["windowRounding"]);
        data.FrameRounding = std::stof(values["FrameRounding"]);
        data.WindowBorder = std::stoi(values["WindowBorder"]);
        data.FrameBorder = std::stoi(values["FrameBorder"]);
        data.PopupBorder = std::stoi(values["PopupBorder"]);
        data.fontGlobalscale = std::stof(values["fontGlobalscale"]);
        data.AntiAliasedLines = std::stoi(values["AntiAliasedLines"]);
        data.AntiAliasedLinesUseTex = std::stoi(values["AntiAliasedLinesUseTex"]);
        data.AntiAliasedFill = std::stoi(values["AntiAliasedFill"]);
        data.CurveTessellationTol = std::stof(values["CurveTessellationTol"]);
        data.CircleSegmentMaxError = std::stof(values["CircleSegmentMaxError"]);
        data.GlobalAlpha = std::stof(values["GlobalAlpha"]);

        ST_TRACE("Loaded Editor style config from {0}", file);
        return data;
    }
    void StyleEditor::save() {
        ST_PROFILING_FUNCTION();
        std::string file = getEditorProject().configPath + "/editor-style.ini";
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGuiStyle& style = ImGui::GetStyle();
        std::remove(file.c_str());
        std::fstream stream(file, std::ios::out);
        
        stream << "font=" << fontIndex << "\n";
        stream << "style=" << styleIndex << "\n";
        stream << "windowRounding=" << style.WindowRounding << "\n";
        stream << "FrameRounding=" << style.FrameRounding << "\n";
        stream << "WindowBorder=" << (int)style.WindowBorderSize << "\n";
        stream << "FrameBorder=" << (int)style.FrameBorderSize << "\n";
        stream << "PopupBorder=" << (int)style.PopupBorderSize << "\n";
        stream << "fontGlobalscale=" << io.FontGlobalScale << "\n";
        stream << "AntiAliasedLines=" << (int)style.AntiAliasedLines << "\n";
        stream << "AntiAliasedLinesUseTex=" << (int)style.AntiAliasedLinesUseTex << "\n";
        stream << "AntiAliasedFill=" << (int)style.AntiAliasedFill << "\n";
        stream << "CurveTessellationTol=" << style.CurveTessellationTol << "\n";
        stream << "CircleSegmentMaxError=" << style.CircleTessellationMaxError << "\n";
        stream << "GlobalAlpha=" << style.Alpha << "\n";

        stream.close();
        ST_TRACE("Saved Editor style config to {0}", file);
    }
    void StyleEditor::drawStyleEditor(ImGuiStyle* ref, bool* open) {
        ST_PROFILING_FUNCTION();
        ImGuiStyle& style = ImGui::GetStyle();
        static ImGuiStyle ref_saved_style;

        // Default to using internal storage as reference
        static bool init = true;
        if (init && ref == NULL)
            ref_saved_style = style;
        init = false;
        if (ref == NULL)
            ref = &ref_saved_style;
        if (ImGui::Begin("Style Editor", open)) {
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

            if (drawStyleSelector())
                ref_saved_style = style;
            drawFontSelector();

            // Simplified Settings (expose floating-pointer border sizes as boolean representing 0.0f or 1.0f)
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
            if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
                style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
            { bool border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
            ImGui::SameLine();
            { bool border = (style.FrameBorderSize > 0.0f);  if (ImGui::Checkbox("FrameBorder", &border)) { style.FrameBorderSize = border ? 1.0f : 0.0f; } }
            ImGui::SameLine();
            { bool border = (style.PopupBorderSize > 0.0f);  if (ImGui::Checkbox("PopupBorder", &border)) { style.PopupBorderSize = border ? 1.0f : 0.0f; } }

            if (ImGui::Button("Save"))
                save();
            ImGui::SameLine();
            if (ImGui::Button("Load")) {
                ImGuiIO& io = ImGui::GetIO(); (void)io;
                StyleFileData data = load();
                setStyleByInt(data.style);
                setFontByInt(data.font);
                style.WindowRounding = data.windowRounding;
                style.FrameRounding = data.FrameRounding;
                style.GrabRounding = data.FrameRounding;
                style.WindowBorderSize = (float)data.WindowBorder;
                style.FrameBorderSize = (float)data.FrameBorder;
                style.PopupBorderSize = (float)data.PopupBorder;
                io.FontGlobalScale = data.fontGlobalscale;
                style.AntiAliasedLines = data.AntiAliasedLines;
                style.AntiAliasedLinesUseTex = data.AntiAliasedLinesUseTex;
                style.AntiAliasedFill = data.AntiAliasedFill;
                style.CurveTessellationTol = data.CurveTessellationTol;
                style.CircleTessellationMaxError = data.CircleSegmentMaxError;
                style.Alpha = data.GlobalAlpha;
            }
            ImGui::Separator();

            if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Colors"))
                {
                    static int output_dest = 0;
                    static bool output_only_modified = true;
                    if (ImGui::Button("Save To Personal File"))
                    {
                        saveColors();
                    }
                    ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
                    ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

                    static ImGuiTextFilter filter;
                    filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

                    static ImGuiColorEditFlags alpha_flags = 0;
                    if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None)) { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
                    if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview)) { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
                    if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
                    ComponentsRender::drawHelpMarker(
                        "In the color list:\n"
                        "Left-click on colored square to open color picker,\n"
                        "Right-click to open edit options menu.");

                    ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NavFlattened);
                    ImGui::PushItemWidth(-160);
                    for (int i = 0; i < ST_SUPPORTED_IMGUI_COLORS_COUNT; i++)
                    {
                        const char* name = ImGui::GetStyleColorName(i);
                        if (!filter.PassFilter(name))
                            continue;
                        ImGui::PushID(i);
                        ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
                        if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
                        {
                            // Tips: in a real user application, you may want to merge and use an icon font into the main font,
                            // so instead of "Save"/"Revert" you'd use icons!
                            // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
                            ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) { ref->Colors[i] = style.Colors[i]; }
                            ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = ref->Colors[i]; }
                        }
                        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
                        ImGui::TextUnformatted(name);
                        ImGui::PopID();
                    }
                    ImGui::PopItemWidth();
                    ImGui::EndChild();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Fonts"))
                {
                    ImGuiIO& io = ImGui::GetIO();
                    ImFontAtlas* atlas = io.Fonts;
                    ImGui::PushItemWidth(120);
                    for (int i = 0; i < atlas->Fonts.Size; i++)
                    {
                        ImFont* font = atlas->Fonts[i];
                        ImGui::PushID(font);
                        NodeFont(font);
                        ImGui::PopID();
                    }

                    if (ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
                    {
                        ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                        ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
                        ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), tint_col, border_col);
                        ImGui::TreePop();
                    }

                    // Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
                    // (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
                    const float MIN_SCALE = 0.3f;
                    const float MAX_SCALE = 2.0f;
                    static float fontWindowScale = 1.0f;
                    if (ImGui::DragFloat("window scale", &fontWindowScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_ClampOnInput)) // Scale only this window
                        ImGui::SetWindowFontScale(fontWindowScale);
                    ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_ClampOnInput); // Scale everything
                    ImGui::PopItemWidth();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Rendering"))
                {
                    ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
                    ImGui::SameLine(); ComponentsRender::drawHelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");
                    ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
                    ImGui::SameLine(); ComponentsRender::drawHelpMarker("Faster lines using texture data. Require back-end to render with bilinear filtering (not point/nearest filtering).");
                    ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
                    ImGui::PushItemWidth(100);
                    ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
                    if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

                    // When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
                    ImGui::DragFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError, 0.01f, 0.10f, 10.0f, "%.2f");
                    if (ImGui::IsItemActive())
                    {
                        ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
                        ImGui::BeginTooltip();
                        ImVec2 p = ImGui::GetCursorScreenPos();
                        float RAD_MIN = 10.0f, RAD_MAX = 80.0f;
                        float off_x = 10.0f;
                        for (int n = 0; n < 7; n++)
                        {
                            const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (7.0f - 1.0f);
                            ImGui::GetWindowDrawList()->AddCircle(ImVec2(p.x + off_x + rad, p.y + RAD_MAX), rad, ImGui::GetColorU32(ImGuiCol_Text), 0);
                            off_x += 10.0f + rad * 2.0f;
                        }
                        ImGui::Dummy(ImVec2(off_x, RAD_MAX * 2.0f));
                        ImGui::EndTooltip();
                    }
                    ImGui::SameLine();
                    ComponentsRender::drawHelpMarker("When drawing circle primitives with \"num_segments == 0\" tesselation will be calculated automatically.");

                    ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
                    ImGui::PopItemWidth();

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            ImGui::PopItemWidth();

        }
        ImGui::End();
    }
    bool StyleEditor::drawStyleSelector() {
        if (ImGui::Combo("Colors##Selector", &styleIndex, "Ocean Dark\0Dark\0Amoled Dark\0Personal\0ImGui Classic\0ImGui Dark\0ImGui Light\0")) {
            setStyleByInt(styleIndex);
        }
        return false;
	}
    void StyleEditor::drawFontSelector() {
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font_current = ImGui::GetFont();
        if (ImGui::BeginCombo("Fonts##Selector", font_current->GetDebugName()))
        {
            for (int n = 0; n < io.Fonts->Fonts.Size; n++)
            {
                ImFont* font = io.Fonts->Fonts[n];
                ImGui::PushID((void*)font);
                if (ImGui::Selectable(font->GetDebugName(), font == font_current))
                    setFontByInt(n);
                ImGui::PopID();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        ComponentsRender::drawHelpMarker(
            (std::string("- You can use additional fonts by placing TrueType Font files (.ttf) in ") + Application::getStartDirectory().c_str() + getEditorDataPath() + "/Fonts/").c_str()
        );
        /*ComponentsRender::drawHelpMarker(
            "- Load additional fonts with io.Fonts->AddFontFromFileTTF().\n"
            "- The font atlas is built when calling io.Fonts->GetTexDataAsXXXX() or io.Fonts->Build().\n"
            "- Read FAQ and docs/FONTS.md for more details.\n"
            "- If you need to add/remove fonts at runtime (e.g. for DPI change), do it before calling NewFrame().");*/
    }
    std::ostream& operator<<(std::ostream& os, const ImVec4& vec4) {
        return os << vec4.x << "," << vec4.y << "," << vec4.z << "," << vec4.w;
    }
    ImVec4 ImVec4fromString(std::string str){
        ImVec4 vec4 = ImVec4(1,1,1,1);
        
        std::stringstream string_stream(str);
        int i = 0; 
        while (string_stream.good()) 
        {
            std::string a;
            getline(string_stream, a, ',');

            switch (i)
            {
                case 0:
                    vec4.x = std::stof(a);
                    break;
                case 1:
                    vec4.y = std::stof(a);
                    break;
                case 2:
                    vec4.z = std::stof(a);
                    break;
                case 3:
                    vec4.w = std::stof(a);
                    break;
            }
            i++;
        }
        return vec4;
        
    }
    void StyleEditor::saveColors() {
        ST_PROFILING_FUNCTION();
        std::string file = getEditorProject().configPath + "/personal-colors.ini";
        ImGuiStyle* style = &ImGui::GetStyle();
        ImVec4* colors = style->Colors;
        std::remove(file.c_str());
        std::fstream stream(file,std::ios::out);
        for (int i = 0; i < ST_SUPPORTED_IMGUI_COLORS_COUNT; i++) {
            stream << i << "=" << colors[i] << "\n";
        }
        stream.close();
        ST_TRACE("Saved Editor style colors to {0}", file);
    }
    void StyleEditor::LoadStyleColors(ImGuiStyle* dst) {
        ST_PROFILING_FUNCTION();
        std::string file = getEditorProject().configPath + "/personal-colors.ini";
        ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
        ImVec4* colors = style->Colors;
        if (!FileExists(file)) {
            saveColors();
            return;
        }
        std::fstream stream(file, std::ios::in);
        std::string str;
        while (getline(stream, str)) {
            str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
            size_t seperator = str.find_first_of("=");
            int type = std::stoi(str.substr(0,seperator));
            ImVec4 color = ImVec4fromString(str.substr(seperator+1, str.npos));
            if(type < ST_SUPPORTED_IMGUI_COLORS_COUNT && type > -1)
                colors[type] = color;
        }
        
        stream.close();
        ST_TRACE("Loaded Editor style colors from {0}", file);
    }

    void StyleEditor::setStyleByInt(int style) {
        styleIndex = style;
        switch (style)
        {
        case 0: ImGui::StyleColorsOceanDark(); break;
        case 1: ImGui::StyleColorsGrayDark(); break;
        case 2: ImGui::StyleColorsAmoledDark(); break;
        case 3: LoadStyleColors(); break;
        case 4: ImGui::StyleColorsClassic(); break;
        case 5: ImGui::StyleColorsDark(); break;
        case 6: ImGui::StyleColorsLight(); break;
        default: ImGui::StyleColorsAmoledDark(); break;
        }
    }
    void StyleEditor::setFontByInt(int style) {
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        fontIndex = style;
        io.FontDefault = io.Fonts->Fonts[style];
    }
}