# xp_wellys_about

The shared "About the developer" page of thWelly's X-Plane plugins: profile photo,
short bio, links to [thwelly.ch/xplane-plugins](https://thwelly.ch/xplane-plugins/)
and [Buy Me a Coffee](https://buymeacoffee.com/thwelly), and the license notice of the
plugin showing it.

Source-only Dear ImGui module, three files. No build system of its own: each plugin
compiles the sources into its own binary.

## Requirements

- Dear ImGui **1.91.9 or newer** with the OpenGL2 backend (only `imgui.h` is included;
  no API newer than 1.91.9 is used)
- X-Plane SDK, with the `XPLM` header directory on the include path (`<XPLMGraphics.h>`)
- OpenGL (`-framework OpenGL` on macOS, `OpenGL::GL` elsewhere)
- C++17

No icon font is required — the page draws no icons, so it works with whatever font
subset a plugin ships.

## Getting it

Plugins download a pinned tag in `make setup`, the same way they fetch Dear ImGui:

```make
ABOUT_VERSION  := 1.0.0
ABOUT_SENTINEL := vendor/xp_wellys_about/src/wellys_about.hpp

$(ABOUT_SENTINEL):
	@set -euo pipefail; \
	TMP=$$(mktemp -d); trap "rm -rf $$TMP" EXIT; \
	curl -fsSL "https://github.com/rwellinger/xp_wellys_about/archive/refs/tags/v$(ABOUT_VERSION).tar.gz" \
	     -o "$$TMP/about.tar.gz"; \
	tar -xzf "$$TMP/about.tar.gz" -C "$$TMP/"; \
	mkdir -p vendor/xp_wellys_about; \
	cp -R "$$TMP/xp_wellys_about-$(ABOUT_VERSION)/src" "$$TMP/xp_wellys_about-$(ABOUT_VERSION)/LICENSE" vendor/xp_wellys_about/
```

```cmake
set(ABOUT_DIR ${CMAKE_SOURCE_DIR}/vendor/xp_wellys_about)
target_sources(my_plugin PRIVATE ${ABOUT_DIR}/src/about_view.cpp ${ABOUT_DIR}/src/about_photo.cpp)
target_include_directories(my_plugin SYSTEM PRIVATE ${ABOUT_DIR}/src)
```

`about_view.cpp` has no X-Plane or OpenGL dependency, so headless UI tests can
compile it on its own and pass any non-zero `ImTextureID` as the photo.

## Usage

```cpp
#include "wellys_about.hpp"

static const WellysAbout::PluginInfo about_info{
    "my_plugin",
    MY_PLUGIN_VERSION,
    "Copyright (C) 2026 thWelly",
    "Released under the MIT License, without any warranty.",
    {{"Dear ImGui", "MIT"}},
};
```

**Embedded in an existing screen** (the plugin already has a window with navigation):

```cpp
WellysAbout::draw_content(about_info, WellysAbout::photo_texture());
```

**As its own window**, opened from the plugin's X-Plane menu:

```cpp
static bool s_about_open = false;

// menu handler
s_about_open = true;

// inside the plugin's ImGui frame (between NewFrame and Render)
if (s_about_open)
    WellysAbout::draw_window(s_about_open, about_info, WellysAbout::photo_texture());
```

The plugin's draw callback has to keep producing ImGui frames while the About window
is open, even when the plugin's main window is closed.

**On shutdown**, before the ImGui renderer goes away:

```cpp
WellysAbout::release_photo();
```

## Changing the photo

Replace `images/profile.jpg` and run `./tools/generate_profile_photo.sh` (needs
`python3`; creates a venv with Pillow under `build/`). Commit the regenerated
`src/profile_photo.hpp`, tag a new version, and bump `ABOUT_VERSION` in the plugins.

## License

Source code: MIT. The profile photo is **not** MIT-licensed — all rights reserved,
redistributable only as part of thWelly's plugins. See [LICENSE](LICENSE).
