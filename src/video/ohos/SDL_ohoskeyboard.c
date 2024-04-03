/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../../SDL_internal.h"

#include <stdbool.h>

#if SDL_VIDEO_DRIVER_OHOS

#include "../../events/SDL_events_c.h"

#include "SDL_ohoskeyboard.h"

#include "../../core/ohos/SDL_ohos.h"


typedef enum {
    KEYCODE_FN = 0,
    KEYCODE_UNKNOWN = -1,
    KEYCODE_HOME = 1,
    KEYCODE_BACK = 2,
    KEYCODE_MEDIA_PLAY_PAUSE = 10,
    KEYCODE_MEDIA_STOP = 11,
    KEYCODE_MEDIA_NEXT = 12,
    KEYCODE_MEDIA_PREVIOUS = 13,
    KEYCODE_MEDIA_REWIND = 14,
    KEYCODE_MEDIA_FAST_FORWARD = 15,
    KEYCODE_VOLUME_UP = 16,
    KEYCODE_VOLUME_DOWN = 17,
    KEYCODE_POWER = 18,
    KEYCODE_CAMERA = 19,
    KEYCODE_VOLUME_MUTE = 22,
    KEYCODE_MUTE = 23,
    KEYCODE_BRIGHTNESS_UP = 40,
    KEYCODE_BRIGHTNESS_DOWN = 41,
    KEYCODE_0 = 2000,
    KEYCODE_1 = 2001,
    KEYCODE_2 = 2002,
    KEYCODE_3 = 2003,
    KEYCODE_4 = 2004,
    KEYCODE_5 = 2005,
    KEYCODE_6 = 2006,
    KEYCODE_7 = 2007,
    KEYCODE_8 = 2008,
    KEYCODE_9 = 2009,
    KEYCODE_STAR = 2010,
    KEYCODE_POUND = 2011,
    KEYCODE_DPAD_UP = 2012,
    KEYCODE_DPAD_DOWN = 2013,
    KEYCODE_DPAD_LEFT = 2014,
    KEYCODE_DPAD_RIGHT = 2015,
    KEYCODE_DPAD_CENTER = 2016,
    KEYCODE_A = 2017,
    KEYCODE_B = 2018,
    KEYCODE_C = 2019,
    KEYCODE_D = 2020,
    KEYCODE_E = 2021,
    KEYCODE_F = 2022,
    KEYCODE_G = 2023,
    KEYCODE_H = 2024,
    KEYCODE_I = 2025,
    KEYCODE_J = 2026,
    KEYCODE_K = 2027,
    KEYCODE_L = 2028,
    KEYCODE_M = 2029,
    KEYCODE_N = 2030,
    KEYCODE_O = 2031,
    KEYCODE_P = 2032,
    KEYCODE_Q = 2033,
    KEYCODE_R = 2034,
    KEYCODE_S = 2035,
    KEYCODE_T = 2036,
    KEYCODE_U = 2037,
    KEYCODE_V = 2038,
    KEYCODE_W = 2039,
    KEYCODE_X = 2040,
    KEYCODE_Y = 2041,
    KEYCODE_Z = 2042,
    KEYCODE_COMMA = 2043,
    KEYCODE_PERIOD = 2044,
    KEYCODE_ALT_LEFT = 2045,
    KEYCODE_ALT_RIGHT = 2046,
    KEYCODE_SHIFT_LEFT = 2047,
    KEYCODE_SHIFT_RIGHT = 2048,
    KEYCODE_TAB = 2049,
    KEYCODE_SPACE = 2050,
    KEYCODE_SYM = 2051,
    KEYCODE_EXPLORER = 2052,
    KEYCODE_ENVELOPE = 2053,
    KEYCODE_ENTER = 2054,
    KEYCODE_DEL = 2055,
    KEYCODE_GRAVE = 2056,
    KEYCODE_MINUS = 2057,
    KEYCODE_EQUALS = 2058,
    KEYCODE_LEFT_BRACKET = 2059,
    KEYCODE_RIGHT_BRACKET = 2060,
    KEYCODE_BACKSLASH = 2061,
    KEYCODE_SEMICOLON = 2062,
    KEYCODE_APOSTROPHE = 2063,
    KEYCODE_SLASH = 2064,
    KEYCODE_AT = 2065,
    KEYCODE_PLUS = 2066,
    KEYCODE_MENU = 2067,
    KEYCODE_PAGE_UP = 2068,
    KEYCODE_PAGE_DOWN = 2069,
    KEYCODE_ESCAPE = 2070,
    KEYCODE_FORWARD_DEL = 2071,
    KEYCODE_CTRL_LEFT = 2072,
    KEYCODE_CTRL_RIGHT = 2073,
    KEYCODE_CAPS_LOCK = 2074,
    KEYCODE_SCROLL_LOCK = 2075,
    KEYCODE_META_LEFT = 2076,
    KEYCODE_META_RIGHT = 2077,
    KEYCODE_FUNCTION = 2078,
    KEYCODE_SYSRQ = 2079,
    KEYCODE_BREAK = 2080,
    KEYCODE_MOVE_HOME = 2081,
    KEYCODE_MOVE_END = 2082,
    KEYCODE_INSERT = 2083,
    KEYCODE_FORWARD = 2084,
    KEYCODE_MEDIA_PLAY = 2085,
    KEYCODE_MEDIA_PAUSE = 2086,
    KEYCODE_MEDIA_CLOSE = 2087,
    KEYCODE_MEDIA_EJECT = 2088,
    KEYCODE_MEDIA_RECORD = 2089,
    KEYCODE_F1 = 2090,
    KEYCODE_F2 = 2091,
    KEYCODE_F3 = 2092,
    KEYCODE_F4 = 2093,
    KEYCODE_F5 = 2094,
    KEYCODE_F6 = 2095,
    KEYCODE_F7 = 2096,
    KEYCODE_F8 = 2097,
    KEYCODE_F9 = 2098,
    KEYCODE_F10 = 2099,
    KEYCODE_F11 = 2100,
    KEYCODE_F12 = 2101,
    KEYCODE_NUM_LOCK = 2102,
    KEYCODE_NUMPAD_0 = 2103,
    KEYCODE_NUMPAD_1 = 2104,
    KEYCODE_NUMPAD_2 = 2105,
    KEYCODE_NUMPAD_3 = 2106,
    KEYCODE_NUMPAD_4 = 2107,
    KEYCODE_NUMPAD_5 = 2108,
    KEYCODE_NUMPAD_6 = 2109,
    KEYCODE_NUMPAD_7 = 2110,
    KEYCODE_NUMPAD_8 = 2111,
    KEYCODE_NUMPAD_9 = 2112,
    KEYCODE_NUMPAD_DIVIDE = 2113,
    KEYCODE_NUMPAD_MULTIPLY = 2114,
    KEYCODE_NUMPAD_SUBTRACT = 2115,
    KEYCODE_NUMPAD_ADD = 2116,
    KEYCODE_NUMPAD_DOT = 2117,
    KEYCODE_NUMPAD_COMMA = 2118,
    KEYCODE_NUMPAD_ENTER = 2119,
    KEYCODE_NUMPAD_EQUALS = 2120,
    KEYCODE_NUMPAD_LEFT_PAREN = 2121,
    KEYCODE_NUMPAD_RIGHT_PAREN = 2122,
    KEYCODE_VIRTUAL_MULTITASK = 2210,
    KEYCODE_SLEEP = 2600,
    KEYCODE_ZENKAKU_HANKAKU = 2601,
    KEYCODE_102ND = 2602,
    KEYCODE_RO = 2603,
    KEYCODE_KATAKANA = 2604,
    KEYCODE_HIRAGANA = 2605,
    KEYCODE_HENKAN = 2606,
    KEYCODE_KATAKANA_HIRAGANA = 2607,
    KEYCODE_MUHENKAN = 2608,
    KEYCODE_LINEFEED = 2609,
    KEYCODE_MACRO = 2610,
    KEYCODE_NUMPAD_PLUSMINUS = 2611,
    KEYCODE_SCALE = 2612,
    KEYCODE_HANGUEL = 2613,
    KEYCODE_HANJA = 2614,
    KEYCODE_YEN = 2615,
    KEYCODE_STOP = 2616,
    KEYCODE_AGAIN = 2617,
    KEYCODE_PROPS = 2618,
    KEYCODE_UNDO = 2619,
    KEYCODE_COPY = 2620,
    KEYCODE_OPEN = 2621,
    KEYCODE_PASTE = 2622,
    KEYCODE_FIND = 2623,
    KEYCODE_CUT = 2624,
    KEYCODE_HELP = 2625,
    KEYCODE_CALC = 2626,
    KEYCODE_FILE = 2627,
    KEYCODE_BOOKMARKS = 2628,
    KEYCODE_NEXT = 2629,
    KEYCODE_PLAYPAUSE = 2630,
    KEYCODE_PREVIOUS = 2631,
    KEYCODE_STOPCD = 2632,
    KEYCODE_CONFIG = 2634,
    KEYCODE_REFRESH = 2635,
    KEYCODE_EXIT = 2636,
    KEYCODE_EDIT = 2637,
    KEYCODE_SCROLLUP = 2638,
    KEYCODE_SCROLLDOWN = 2639,
    KEYCODE_NEW = 2640,
    KEYCODE_REDO = 2641,
    KEYCODE_CLOSE = 2642,
    KEYCODE_PLAY = 2643,
    KEYCODE_BASSBOOST = 2644,
    KEYCODE_PRINT = 2645,
    KEYCODE_CHAT = 2646,
    KEYCODE_FINANCE = 2647,
    KEYCODE_CANCEL = 2648,
    KEYCODE_KBDILLUM_TOGGLE = 2649,
    KEYCODE_KBDILLUM_DOWN = 2650,
    KEYCODE_KBDILLUM_UP = 2651,
    KEYCODE_SEND = 2652,
    KEYCODE_REPLY = 2653,
    KEYCODE_FORWARDMAIL = 2654,
    KEYCODE_SAVE = 2655,
    KEYCODE_DOCUMENTS = 2656,
    KEYCODE_VIDEO_NEXT = 2657,
    KEYCODE_VIDEO_PREV = 2658,
    KEYCODE_BRIGHTNESS_CYCLE = 2659,
    KEYCODE_BRIGHTNESS_ZERO = 2660,
    KEYCODE_DISPLAY_OFF = 2661,
    KEYCODE_BTN_MISC = 2662,
    KEYCODE_GOTO = 2663,
    KEYCODE_INFO = 2664,
    KEYCODE_PROGRAM = 2665,
    KEYCODE_PVR = 2666,
    KEYCODE_SUBTITLE = 2667,
    KEYCODE_FULL_SCREEN = 2668,
    KEYCODE_KEYBOARD = 2669,
    KEYCODE_ASPECT_RATIO = 2670,
    KEYCODE_PC = 2671,
    KEYCODE_TV = 2672,
    KEYCODE_TV2 = 2673,
    KEYCODE_VCR = 2674,
    KEYCODE_VCR2 = 2675,
    KEYCODE_SAT = 2676,
    KEYCODE_CD = 2677,
    KEYCODE_TAPE = 2678,
    KEYCODE_TUNER = 2679,
    KEYCODE_PLAYER = 2680,
    KEYCODE_DVD = 2681,
    KEYCODE_AUDIO = 2682,
    KEYCODE_VIDEO = 2683,
    KEYCODE_MEMO = 2684,
    KEYCODE_CALENDAR = 2685,
    KEYCODE_RED = 2686,
    KEYCODE_GREEN = 2687,
    KEYCODE_YELLOW = 2688,
    KEYCODE_BLUE = 2689,
    KEYCODE_CHANNELUP = 2690,
    KEYCODE_CHANNELDOWN = 2691,
    KEYCODE_LAST = 2692,
    KEYCODE_RESTART = 2693,
    KEYCODE_SLOW = 2694,
    KEYCODE_SHUFFLE = 2695,
    KEYCODE_VIDEOPHONE = 2696,
    KEYCODE_GAMES = 2697,
    KEYCODE_ZOOMIN = 2698,
    KEYCODE_ZOOMOUT = 2699,
    KEYCODE_ZOOMRESET = 2700,
    KEYCODE_WORDPROCESSOR = 2701,
    KEYCODE_EDITOR = 2702,
    KEYCODE_SPREADSHEET = 2703,
    KEYCODE_GRAPHICSEDITOR = 2704,
    KEYCODE_PRESENTATION = 2705,
    KEYCODE_DATABASE = 2706,
    KEYCODE_NEWS = 2707,
    KEYCODE_VOICEMAIL = 2708,
    KEYCODE_ADDRESSBOOK = 2709,
    KEYCODE_MESSENGER = 2710,
    KEYCODE_BRIGHTNESS_TOGGLE = 2711,
    KEYCODE_SPELLCHECK = 2712,
    KEYCODE_COFFEE = 2713,
    KEYCODE_MEDIA_REPEAT = 2714,
    KEYCODE_IMAGES = 2715,
    KEYCODE_BUTTONCONFIG = 2716,
    KEYCODE_TASKMANAGER = 2717,
    KEYCODE_JOURNAL = 2718,
    KEYCODE_CONTROLPANEL = 2719,
    KEYCODE_APPSELECT = 2720,
    KEYCODE_SCREENSAVER = 2721,
    KEYCODE_ASSISTANT = 2722,
    KEYCODE_KBD_LAYOUT_NEXT = 2723,
    KEYCODE_BRIGHTNESS_MIN = 2724,
    KEYCODE_BRIGHTNESS_MAX = 2725,
    KEYCODE_KBDINPUTASSIST_PREV = 2726,
    KEYCODE_KBDINPUTASSIST_NEXT = 2727,
    KEYCODE_KBDINPUTASSIST_PREVGROUP = 2728,
    KEYCODE_KBDINPUTASSIST_NEXTGROUP = 2729,
    KEYCODE_KBDINPUTASSIST_ACCEPT = 2730,
    KEYCODE_KBDINPUTASSIST_CANCEL = 2731,
    KEYCODE_FRONT = 2800,
    KEYCODE_SETUP = 2801,
    KEYCODE_WAKEUP = 2802,
    KEYCODE_SENDFILE = 2803,
    KEYCODE_DELETEFILE = 2804,
    KEYCODE_XFER = 2805,
    KEYCODE_PROG1 = 2806,
    KEYCODE_PROG2 = 2807,
    KEYCODE_MSDOS = 2808,
    KEYCODE_SCREENLOCK = 2809,
    KEYCODE_DIRECTION_ROTATE_DISPLAY = 2810,
    KEYCODE_CYCLEWINDOWS = 2811,
    KEYCODE_COMPUTER = 2812,
    KEYCODE_EJECTCLOSECD = 2813,
    KEYCODE_ISO = 2814,
    KEYCODE_MOVE = 2815,
    KEYCODE_F13 = 2816,
    KEYCODE_F14 = 2817,
    KEYCODE_F15 = 2818,
    KEYCODE_F16 = 2819,
    KEYCODE_F17 = 2820,
    KEYCODE_F18 = 2821,
    KEYCODE_F19 = 2822,
    KEYCODE_F20 = 2823,
    KEYCODE_F21 = 2824,
    KEYCODE_F22 = 2825,
    KEYCODE_F23 = 2826,
    KEYCODE_F24 = 2827,
    KEYCODE_PROG3 = 2828,
    KEYCODE_PROG4 = 2829,
    KEYCODE_DASHBOARD = 2830,
    KEYCODE_SUSPEND = 2831,
    KEYCODE_HP = 2832,
    KEYCODE_SOUND = 2833,
    KEYCODE_QUESTION = 2834,
    KEYCODE_CONNECT = 2836,
    KEYCODE_SPORT = 2837,
    KEYCODE_SHOP = 2838,
    KEYCODE_ALTERASE = 2839,
    KEYCODE_SWITCHVIDEOMODE = 2841,
    KEYCODE_BATTERY = 2842,
    KEYCODE_BLUETOOTH = 2843,
    KEYCODE_WLAN = 2844,
    KEYCODE_UWB = 2845,
    KEYCODE_WWAN_WIMAX = 2846,
    KEYCODE_RFKILL = 2847,
    KEYCODE_CHANNEL = 3001,
    KEYCODE_BTN_0 = 3100,
    KEYCODE_BTN_1 = 3101,
    KEYCODE_BTN_2 = 3102,
    KEYCODE_BTN_3 = 3103,
    KEYCODE_BTN_4 = 3104,
    KEYCODE_BTN_5 = 3105,
    KEYCODE_BTN_6 = 3106,
    KEYCODE_BTN_7 = 3107,
    KEYCODE_BTN_8 = 3108,
    KEYCODE_BTN_9 = 3109
} OHOS_Scancode;

static const struct {
    OHOS_Scancode keycode;
    SDL_Scancode scancode;
} KeyCodeToSDLScancode[] = {
    { KEYCODE_UNKNOWN, SDL_SCANCODE_UNKNOWN },
    { KEYCODE_ENTER, SDL_SCANCODE_RETURN },
    { KEYCODE_ESCAPE, SDL_SCANCODE_ESCAPE },
    { KEYCODE_DEL, SDL_SCANCODE_BACKSPACE },
    { KEYCODE_TAB, SDL_SCANCODE_TAB },
    { KEYCODE_SPACE, SDL_SCANCODE_SPACE },
    { KEYCODE_MINUS, SDL_SCANCODE_MINUS },
    { KEYCODE_EQUALS, SDL_SCANCODE_EQUALS },
    { KEYCODE_LEFT_BRACKET, SDL_SCANCODE_LEFTBRACKET },
    { KEYCODE_RIGHT_BRACKET, SDL_SCANCODE_RIGHTBRACKET },
    { KEYCODE_BACKSLASH, SDL_SCANCODE_BACKSLASH },
    { KEYCODE_SEMICOLON, SDL_SCANCODE_SEMICOLON },
    { KEYCODE_APOSTROPHE, SDL_SCANCODE_APOSTROPHE },
    { KEYCODE_GRAVE, SDL_SCANCODE_GRAVE },
    { KEYCODE_COMMA, SDL_SCANCODE_COMMA },
    { KEYCODE_PERIOD, SDL_SCANCODE_PERIOD },
    { KEYCODE_SLASH, SDL_SCANCODE_SLASH },
    { KEYCODE_CAPS_LOCK, SDL_SCANCODE_CAPSLOCK },
    { KEYCODE_SYSRQ, SDL_SCANCODE_PRINTSCREEN },
    { KEYCODE_SCROLL_LOCK, SDL_SCANCODE_SCROLLLOCK },
    { KEYCODE_BREAK, SDL_SCANCODE_PAUSE },
    { KEYCODE_INSERT, SDL_SCANCODE_INSERT },
    { KEYCODE_MOVE_HOME, SDL_SCANCODE_HOME },
    { KEYCODE_PAGE_UP, SDL_SCANCODE_PAGEUP },
    { KEYCODE_FORWARD_DEL, SDL_SCANCODE_DELETE },
    { KEYCODE_MOVE_END, SDL_SCANCODE_END },
    { KEYCODE_PAGE_DOWN, SDL_SCANCODE_PAGEDOWN },
    { KEYCODE_DPAD_RIGHT, SDL_SCANCODE_RIGHT },
    { KEYCODE_DPAD_LEFT, SDL_SCANCODE_LEFT },
    { KEYCODE_DPAD_DOWN, SDL_SCANCODE_DOWN },
    { KEYCODE_DPAD_UP, SDL_SCANCODE_UP },
    { KEYCODE_NUMPAD_DIVIDE, SDL_SCANCODE_KP_DIVIDE },
    { KEYCODE_NUMPAD_MULTIPLY, SDL_SCANCODE_KP_MULTIPLY },
    { KEYCODE_NUMPAD_SUBTRACT, SDL_SCANCODE_KP_MINUS },
    { KEYCODE_NUMPAD_ADD, SDL_SCANCODE_KP_PLUS },
    { KEYCODE_NUMPAD_ENTER, SDL_SCANCODE_KP_ENTER },
    { KEYCODE_NUMPAD_DOT, SDL_SCANCODE_KP_PERIOD },
    { KEYCODE_POWER, SDL_SCANCODE_POWER },
    { KEYCODE_NUMPAD_EQUALS, SDL_SCANCODE_KP_EQUALS },
    { KEYCODE_HELP, SDL_SCANCODE_HELP },
    { KEYCODE_MENU, SDL_SCANCODE_MENU },
    { KEYCODE_DPAD_CENTER, SDL_SCANCODE_SELECT },
    { KEYCODE_STOP, SDL_SCANCODE_STOP },
    { KEYCODE_AGAIN, SDL_SCANCODE_AGAIN },
    { KEYCODE_UNDO, SDL_SCANCODE_UNDO },
    { KEYCODE_CUT, SDL_SCANCODE_CUT },
    { KEYCODE_COPY, SDL_SCANCODE_COPY },
    { KEYCODE_PASTE, SDL_SCANCODE_PASTE },
    { KEYCODE_FIND, SDL_SCANCODE_FIND },
    { KEYCODE_MUTE, SDL_SCANCODE_MUTE },
    { KEYCODE_VOLUME_UP, SDL_SCANCODE_VOLUMEUP },
    { KEYCODE_VOLUME_DOWN, SDL_SCANCODE_VOLUMEDOWN },
    { KEYCODE_NUMPAD_COMMA, SDL_SCANCODE_KP_COMMA },
    { KEYCODE_YEN, SDL_SCANCODE_INTERNATIONAL3 },
    { KEYCODE_HENKAN, SDL_SCANCODE_INTERNATIONAL4 },
    { KEYCODE_MUHENKAN, SDL_SCANCODE_INTERNATIONAL5 },
    { KEYCODE_HANJA, SDL_SCANCODE_LANG2 },
    { KEYCODE_KATAKANA_HIRAGANA, SDL_SCANCODE_LANG3 },
    { KEYCODE_HIRAGANA, SDL_SCANCODE_LANG4 },
    { KEYCODE_ZENKAKU_HANKAKU, SDL_SCANCODE_LANG5 },
    { KEYCODE_ALTERASE, SDL_SCANCODE_ALTERASE },
    { KEYCODE_CANCEL, SDL_SCANCODE_CANCEL },
    { KEYCODE_NUMPAD_LEFT_PAREN, SDL_SCANCODE_KP_LEFTPAREN },
    { KEYCODE_NUMPAD_RIGHT_PAREN, SDL_SCANCODE_KP_RIGHTPAREN },
    { KEYCODE_NUMPAD_PLUSMINUS, SDL_SCANCODE_KP_PLUSMINUS },
    { KEYCODE_CTRL_LEFT, SDL_SCANCODE_LCTRL },
    { KEYCODE_SHIFT_LEFT, SDL_SCANCODE_LSHIFT },
    { KEYCODE_ALT_LEFT, SDL_SCANCODE_LALT },
    { KEYCODE_META_LEFT, SDL_SCANCODE_LGUI },
    { KEYCODE_CTRL_RIGHT, SDL_SCANCODE_RCTRL },
    { KEYCODE_SHIFT_RIGHT, SDL_SCANCODE_RSHIFT },
    { KEYCODE_ALT_RIGHT, SDL_SCANCODE_RALT },
    { KEYCODE_META_RIGHT, SDL_SCANCODE_RGUI },
    { KEYCODE_MEDIA_NEXT, SDL_SCANCODE_AUDIONEXT },
    { KEYCODE_MEDIA_PREVIOUS, SDL_SCANCODE_AUDIOPREV },
    { KEYCODE_MEDIA_STOP, SDL_SCANCODE_AUDIOSTOP },
    { KEYCODE_MEDIA_PLAY_PAUSE, SDL_SCANCODE_AUDIOPLAY },
    { KEYCODE_VOLUME_MUTE, SDL_SCANCODE_AUDIOMUTE },
    { KEYCODE_EXPLORER, SDL_SCANCODE_WWW },
    { KEYCODE_ENVELOPE, SDL_SCANCODE_MAIL },
    { KEYCODE_CALC, SDL_SCANCODE_CALCULATOR },
    { KEYCODE_COMPUTER, SDL_SCANCODE_COMPUTER },
    { KEYCODE_FIND, SDL_SCANCODE_AC_SEARCH },
    { KEYCODE_HOME, SDL_SCANCODE_AC_HOME },
    { KEYCODE_BACK, SDL_SCANCODE_AC_BACK },
    { KEYCODE_FORWARD, SDL_SCANCODE_AC_FORWARD },
    { KEYCODE_STOP, SDL_SCANCODE_AC_STOP },
    { KEYCODE_REFRESH, SDL_SCANCODE_AC_REFRESH },
    { KEYCODE_BOOKMARKS, SDL_SCANCODE_AC_BOOKMARKS },
    { KEYCODE_BRIGHTNESS_DOWN, SDL_SCANCODE_BRIGHTNESSDOWN },
    { KEYCODE_BRIGHTNESS_UP, SDL_SCANCODE_BRIGHTNESSUP },
    { KEYCODE_KBDILLUM_TOGGLE, SDL_SCANCODE_KBDILLUMTOGGLE },
    { KEYCODE_KBDILLUM_DOWN, SDL_SCANCODE_KBDILLUMDOWN },
    { KEYCODE_KBDILLUM_UP, SDL_SCANCODE_KBDILLUMUP },
    { KEYCODE_MEDIA_EJECT, SDL_SCANCODE_EJECT },
    { KEYCODE_SLEEP, SDL_SCANCODE_SLEEP },
    { KEYCODE_MEDIA_REWIND, SDL_SCANCODE_AUDIOREWIND },
    { KEYCODE_MEDIA_FAST_FORWARD, SDL_SCANCODE_AUDIOFASTFORWARD },
};

void
OHOS_InitKeyboard(void)
{
    SDL_Keycode keymap[SDL_NUM_SCANCODES];

    /* Add default scancode to key mapping */
    SDL_GetDefaultKeymap(keymap);
    SDL_SetKeymap(0, keymap, SDL_NUM_SCANCODES);
}


static SDL_Scancode
TranslateKeycode(int keycode)
{
    int i;
    
    if (keycode >= KEYCODE_A && keycode <= KEYCODE_Z)
    {
        return SDL_SCANCODE_A + (keycode - KEYCODE_A);
    }
    if (keycode >= KEYCODE_0 && keycode <= KEYCODE_9)
    {
        if (keycode != KEYCODE_0)
        {
            return SDL_SCANCODE_1 + (keycode - KEYCODE_1);
        }
        else
        {
            return SDL_SCANCODE_0;
        }
    } 
    if (keycode >= KEYCODE_F1 && keycode <= KEYCODE_F12)
    {
        return SDL_SCANCODE_F1 + (keycode - KEYCODE_F1);
    }
    if (keycode >= KEYCODE_NUMPAD_0 && keycode <= KEYCODE_NUMPAD_9)
    {
        if (keycode != KEYCODE_NUMPAD_0)
        {
            return SDL_SCANCODE_KP_1 + (keycode - KEYCODE_NUMPAD_1);
        }
        else
        {
            return SDL_SCANCODE_KP_0;
        }
    }
    if (keycode >= KEYCODE_F13 && keycode <= KEYCODE_F24)
    {
        return SDL_SCANCODE_F13 + (keycode - KEYCODE_F13);
    }

    for (i = 0; i < SDL_arraysize(KeyCodeToSDLScancode); ++i)
    {
        if (keycode == KeyCodeToSDLScancode[i].keycode)
        {
            return KeyCodeToSDLScancode[i].scancode;
        }
    }
    
    SDL_Log("OHOS TranslateKeycode, unknown keycode=%d\n", keycode);
    return SDL_SCANCODE_UNKNOWN;

}

int
OHOS_OnKeyDown(int keycode)
{
    return SDL_SendKeyboardKey(SDL_PRESSED, TranslateKeycode(keycode));
}

int
OHOS_OnKeyUp(int keycode)
{
    return SDL_SendKeyboardKey(SDL_RELEASED, TranslateKeycode(keycode));
}

SDL_bool
OHOS_HasScreenKeyboardSupport(_THIS)
{
    return SDL_TRUE;
}

SDL_bool
OHOS_IsScreenKeyboardShown(_THIS, SDL_Window * window)
{
    return true;
}

void
OHOS_StartTextInput(_THIS)
{
    OHOS_NAPI_ShowTextInputKeyboard(SDL_TRUE);
}

void
OHOS_StopTextInput(_THIS)
{
    OHOS_NAPI_HideTextInput(1);
}

void
OHOS_SetTextInputRect(_THIS, SDL_Rect *rect)
{
    SDL_VideoData *videodata = (SDL_VideoData *)_this->driverdata;

    if (!rect) {
        SDL_InvalidParamError("rect");
        return;
    }

    videodata->textRect = *rect;
}


#endif /* SDL_VIDEO_DRIVER_OHOS */

/* vi: set ts=4 sw=4 expandtab: */
