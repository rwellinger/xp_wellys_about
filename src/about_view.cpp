/*
 * xp_wellys_about - shared "About the developer" page for thWelly's X-Plane plugins
 * Copyright (c) 2026 thWelly
 *
 * The source code is licensed under the MIT License; the profile photo is not.
 * See LICENSE.
 */

#include "wellys_about.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>

namespace
{

constexpr const char *DEVELOPER_NAME = "Robert Wellinger";
constexpr const char *DEVELOPER_LINE = "thWelly  |  Switzerland";
constexpr const char *PLUGINS_URL    = "https://thwelly.ch/xplane-plugins/";
constexpr const char *COFFEE_URL     = "https://buymeacoffee.com/thwelly";

// Sizes are multiples of the font size, so the page follows whatever UI scale the
// host plugin applies.
constexpr float PHOTO_SIZE_EM   = 7.5f;
constexpr float PHOTO_GAP_EM    = 1.2f;
constexpr float NAME_SCALE      = 1.5f;
constexpr float WINDOW_WIDTH_EM = 36.f;

void open_url(const char *url)
{
    const std::string quoted = std::string("\"") + url + "\"";
#if defined(__APPLE__)
    std::system(("open " + quoted).c_str()); // NOLINT(bugprone-command-processor)
#elif defined(_WIN32)
    std::system(("start \"\" " + quoted).c_str()); // NOLINT(bugprone-command-processor)
#else
    std::system(("xdg-open " + quoted).c_str()); // NOLINT(bugprone-command-processor)
#endif
}

// The style's accent: every ImGui theme, the default ones included, gives its check
// marks the highlight colour.
ImVec4 accent_with_alpha(float alpha)
{
    ImVec4 colour = ImGui::GetStyleColorVec4(ImGuiCol_CheckMark);
    colour.w      = alpha;
    return colour;
}

void draw_photo(ImTextureID photo)
{
    const float  size     = ImGui::GetFontSize() * PHOTO_SIZE_EM;
    const float  rounding = size * 0.08f;
    const ImVec2 min      = ImGui::GetCursorScreenPos();
    const ImVec2 max(min.x + size, min.y + size);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    if (photo != ImTextureID{})
        draw_list->AddImageRounded(photo, min, max, ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), IM_COL32_WHITE, rounding);
    else
        draw_list->AddRectFilled(min, max, ImGui::GetColorU32(ImGuiCol_FrameBg), rounding);

    ImGui::Dummy(ImVec2(size, size));
}

// Larger than the body text without needing a second font: PushFont(nullptr, size)
// only exists from ImGui 1.92 on, AddText with a size works in every version.
void draw_name()
{
    const float size = ImGui::GetFontSize() * NAME_SCALE;
    ImGui::GetWindowDrawList()->AddText(ImGui::GetFont(), size, ImGui::GetCursorScreenPos(),
                                        ImGui::GetColorU32(ImGuiCol_Text), DEVELOPER_NAME);
    ImGui::Dummy(ImVec2(ImGui::CalcTextSize(DEVELOPER_NAME).x * NAME_SCALE, size));
}

void link_button(const char *label, const char *url, bool primary)
{
    if (primary)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, accent_with_alpha(0.85f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, accent_with_alpha(1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, accent_with_alpha(0.7f));
    }
    if (ImGui::Button(label))
        open_url(url);
    if (primary)
        ImGui::PopStyleColor(3);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("%s", url);
}

void draw_profile(const WellysAbout::PluginInfo &plugin, ImTextureID photo)
{
    draw_photo(photo);
    ImGui::SameLine(0.f, ImGui::GetFontSize() * PHOTO_GAP_EM);

    ImGui::BeginGroup();
    draw_name();
    ImGui::TextDisabled("%s", DEVELOPER_LINE);
    ImGui::Spacing();
    ImGui::TextWrapped("Software developer for more than 30 years, passionate about aviation and AI. "
                       "%s is one of several X-Plane plugins I build in my spare time.",
                       plugin.name);
    ImGui::Spacing();
    link_button("More X-Plane plugins", PLUGINS_URL, true);
    ImGui::SameLine();
    link_button("Buy me a coffee", COFFEE_URL, false);
    ImGui::EndGroup();
}

void draw_license(const WellysAbout::PluginInfo &plugin)
{
    ImGui::Spacing();
    ImGui::SeparatorText("License");
    ImGui::Text("%s v%s", plugin.name, plugin.version);
    ImGui::TextUnformatted(plugin.copyright);
    ImGui::TextWrapped("%s", plugin.license);

    if (!plugin.third_party.empty())
    {
        ImGui::Spacing();
        ImGui::TextDisabled("Includes");
        for (const WellysAbout::Component &component : plugin.third_party)
            ImGui::BulletText("%s  (%s)", component.name, component.license);
    }

    ImGui::Spacing();
    ImGui::TextDisabled("The full license texts ship with the plugin.");
}

} // namespace

void WellysAbout::draw_content(const PluginInfo &plugin, ImTextureID photo)
{
    draw_profile(plugin, photo);
    draw_license(plugin);
}

void WellysAbout::draw_window(bool &open, const PluginInfo &plugin, ImTextureID photo)
{
    const ImGuiIO &io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_FirstUseEver,
                            ImVec2(0.5f, 0.5f));
    // A fixed width gives the text something to wrap against; the zero height fits
    // the window to its content on every frame.
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetFontSize() * WINDOW_WIDTH_EM, 0.f), ImGuiCond_Always);

    char title[128];
    std::snprintf(title, sizeof(title), "About %s##wellys_about", plugin.name);
    if (ImGui::Begin(title, &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
        draw_content(plugin, photo);
    ImGui::End();
}
