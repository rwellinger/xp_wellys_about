/*
 * xp_wellys_about - shared "About the developer" page for thWelly's X-Plane plugins
 * Copyright (c) 2026 thWelly
 *
 * The source code is licensed under the MIT License; the profile photo is not.
 * See LICENSE.
 */

#include "wellys_about.hpp"
#include "profile_photo.hpp"
#include <XPLMGraphics.h>
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h> // gl.h on Windows depends on it
#endif
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

// Windows' gl.h stops at OpenGL 1.1.
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

namespace
{

int s_texture = 0;

static_assert(WellysAbout::ProfilePhoto::width * 3 % 4 == 0,
              "glTexImage2D expects 4-byte aligned rows unless GL_UNPACK_ALIGNMENT is changed");

void upload_photo()
{
    using namespace WellysAbout::ProfilePhoto;

    // Through XPLM rather than glGenTextures/glBindTexture, so X-Plane's own texture
    // state cache stays in sync with what is bound.
    XPLMGenerateTextureNumbers(&s_texture, 1);
    XPLMBindTexture2d(s_texture, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb);
}

} // namespace

ImTextureID WellysAbout::photo_texture()
{
    if (s_texture == 0)
        upload_photo();
    return static_cast<ImTextureID>(s_texture);
}

void WellysAbout::release_photo()
{
    if (s_texture == 0)
        return;
    const auto texture = static_cast<GLuint>(s_texture);
    glDeleteTextures(1, &texture);
    s_texture = 0;
}
