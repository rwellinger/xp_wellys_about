/*
 * xp_wellys_about - shared "About the developer" page for thWelly's X-Plane plugins
 * Copyright (c) 2026 thWelly
 *
 * The source code is licensed under the MIT License; the profile photo is not.
 * See LICENSE.
 */

#pragma once

#include <imgui.h>
#include <vector>

// The developer profile every plugin shows: photo, short bio, links to the plugin
// page and to Buy Me a Coffee, and the plugin's own license notice.
//
// Built against Dear ImGui 1.91.9 and newer, and against no particular theme: all
// colours come from the caller's ImGui style.
namespace WellysAbout
{

struct Component
{
    const char *name;
    const char *license;
};

// What differs from plugin to plugin. The plugins ship under different licenses, so
// the license line is free text rather than assumed.
struct PluginInfo
{
    const char            *name;       // "xp_pilot"
    const char            *version;    // "1.11.0"
    const char            *copyright;  // "Copyright (C) 2026 thWelly"
    const char            *license;    // "Free software under the GNU GPL v3 or later, ..."
    std::vector<Component> third_party; // compiled into the plugin
};

// The profile page, laid out to the width of the current window or child. For
// embedding in a screen of the plugin's own UI. `photo` comes from photo_texture();
// an empty id leaves a placeholder in its place.
void draw_content(const PluginInfo &plugin, ImTextureID photo);

// The same page as a free-standing ImGui window, for plugins that open it from their
// X-Plane menu. Call it inside the plugin's ImGui frame for as long as `open` is true;
// the window's close button sets it to false.
void draw_window(bool &open, const PluginInfo &plugin, ImTextureID photo);

// The photo as an OpenGL texture, uploaded on the first call. Needs X-Plane's GL
// context, so call it from a draw callback.
ImTextureID photo_texture();

// Frees the texture. Call from XPluginStop (or wherever the plugin tears its ImGui
// renderer down); the next photo_texture() uploads it again.
void release_photo();

} // namespace WellysAbout
