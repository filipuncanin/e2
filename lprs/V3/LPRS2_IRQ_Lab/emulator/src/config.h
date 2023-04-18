
#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// Config.

#define SHARE_PATH "emulator/share/"

#define SHOW_HEAD_UP_DISPLAY 1
#define FONT_PATH SHARE_PATH"UbuntuMono-B.ttf"

#if 1
// Standard VGA 60Hz
#define FRAME_US 16683
#define VSYNC_US 1430
#else
// 25Hz
#define FRAME_US 40000
#define VSYNC_US 5000
#endif

#define SCREEN_W 640
#define SCREEN_H 480

///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // CONFIG_H
