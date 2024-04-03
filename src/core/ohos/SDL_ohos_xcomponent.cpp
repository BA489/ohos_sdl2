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

#ifdef __OHOS__

#include <js_native_api.h>
#include <js_native_api_types.h>
#include <hilog/log.h>

#include "SDL_stdinc.h"
#include "SDL_assert.h"
#include "SDL_atomic.h"
#include "SDL_hints.h"
#include "SDL_log.h"
#include "SDL_main.h"
#include "SDL_timer.h"

#include "SDL_system.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "../../events/SDL_events_c.h"
#include "../../video/ohos/SDL_ohosvideo.h"
#include "../../video/ohos/SDL_ohostouch.h"
#include "../../video/ohos/SDL_ohosmouse.h"
#include "../../video/ohos/SDL_ohoskeyboard.h"
#ifdef __cplusplus
}
#endif

#include <ace/xcomponent/native_xcomponent_key_event.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "SDL_ohos_xcomponent.h"

OHNativeWindow *gNative_window = NULL;
static OH_NativeXComponent_Callback callback;
static OH_NativeXComponent_MouseEvent_Callback mouseCallback;

/* Callbacks*/
static void
OnSurfaceCreatedCB(OH_NativeXComponent *component, void *window)
{
    gNative_window = (OHNativeWindow *)window;
    uint64_t width;
    uint64_t height;
    OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    OHOS_SetScreenSize((int)width, (int)height);
    if (OHOS_Window) {
        SDL_WindowData *data = (SDL_WindowData *)OHOS_Window->driverdata;
        data->native_window = (OHNativeWindow *)(window);
        if (data->native_window == NULL) {
            SDL_SetError("Could not fetch native window from UI thread");
        }
    }
    SDL_AtomicSet(&bWindowCreateFlag, SDL_TRUE);
}

static void
OnSurfaceChangedCB(OH_NativeXComponent *component, void *window)
{
    uint64_t width;
    uint64_t height;
    OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    OHOS_SetScreenSize((int)width, (int)height);
    if (OHOS_Window) {
        SDL_VideoDevice *_this = SDL_GetVideoDevice();
        SDL_WindowData *data = (SDL_WindowData *)OHOS_Window->driverdata;
        OHOS_SendResize(OHOS_Window);

        /* If the surface has been previously destroyed by onNativeSurfaceDestroyed, recreate it here */
        if (data->egl_surface == EGL_NO_SURFACE) {
            data->egl_surface = SDL_EGL_CreateSurface(_this, (NativeWindowType)data->native_window);
        }
    }
}

static void
OnSurfaceDestroyedCB(OH_NativeXComponent *component, void *window)
{
    int nb_attempt = 50;
    gNative_window = nullptr;
retry:

    if (OHOS_Window) {
        SDL_VideoDevice *_this = SDL_GetVideoDevice();
        SDL_WindowData *data = (SDL_WindowData *)OHOS_Window->driverdata;

        /* Wait for Main thread being paused and context un-activated to release 'egl_surface' */
        if (!data->backup_done) {
            nb_attempt -= 1;
            if (nb_attempt == 0) {
                SDL_SetError("Try to release egl_surface with context probably still active");
            } else {
                SDL_Delay(10);
                goto retry;
            }
        }

        if (data->egl_surface != EGL_NO_SURFACE) {
            SDL_EGL_DestroySurface(_this, data->egl_surface);
            data->egl_surface = EGL_NO_SURFACE;
        }

        if (data->native_window) {
            SDL_free(data->native_window);
            data->native_window = NULL;
        }

        /* GL Context handling is done in the event loop because this function is run from the Java thread */
    }
}

/* Key */
void
onKeyEvent(OH_NativeXComponent *component, void *window)
{
    OH_NativeXComponent_KeyEvent *keyEvent = NULL;
    if (OH_NativeXComponent_GetKeyEvent(component, &keyEvent) >= 0) {
        OH_NativeXComponent_KeyAction action;
        OH_NativeXComponent_KeyCode code;
        OH_NativeXComponent_EventSourceType sourceType;

        OH_NativeXComponent_GetKeyEventAction(keyEvent, &action);
        OH_NativeXComponent_GetKeyEventCode(keyEvent, &code);

        if (OH_NATIVEXCOMPONENT_KEY_ACTION_DOWN == action) {
            OHOS_OnKeyDown(code);
        } else if (OH_NATIVEXCOMPONENT_KEY_ACTION_UP == action) {
            OHOS_OnKeyUp(code);
        }
    }
}

/* Touch */
void
onNativeTouch(OH_NativeXComponent *component, void *window)
{
    OH_NativeXComponent_TouchEvent touchEvent;
    float tiltX = 0.0f;
    float tiltY = 0.0f;
    OH_NativeXComponent_TouchPointToolType toolType = OH_NATIVEXCOMPONENT_TOOL_TYPE_UNKNOWN;

    SDL_LockMutex(OHOS_PageMutex);
    OH_NativeXComponent_GetTouchEvent(component, window, &touchEvent);
    OH_NativeXComponent_GetTouchPointToolType(component, 0, &toolType);
    tiltX = touchEvent.x;
    tiltY = touchEvent.y;
    OHOS_OnTouch(OHOS_Window, touchEvent.deviceId, touchEvent.id, touchEvent.type, tiltX, tiltY, touchEvent.force);

    SDL_UnlockMutex(OHOS_PageMutex);
}

/* Mouse */
void
onNativeMouse(OH_NativeXComponent *component, void *window)
{
    OH_NativeXComponent_MouseEvent mouseEvent;
    int32_t ret = OH_NativeXComponent_GetMouseEvent(component, window, &mouseEvent);
    SDL_LockMutex(OHOS_PageMutex);

    OHOS_OnMouse(OHOS_Window, mouseEvent.button, mouseEvent.action, mouseEvent.x, mouseEvent.y, SDL_TRUE);

    SDL_UnlockMutex(OHOS_PageMutex);
}

static void
OnDispatchTouchEventCB(OH_NativeXComponent *component, void *window)
{
    OH_NativeXComponent_TouchEvent touchEvent;
    int32_t ret = OH_NativeXComponent_GetTouchEvent(component, window, &touchEvent);
    SDL_LockMutex(OHOS_PageMutex);
    OHOS_OnTouch(OHOS_Window, touchEvent.deviceId, touchEvent.id, touchEvent.type, touchEvent.x, touchEvent.y, touchEvent.force);
    SDL_UnlockMutex(OHOS_PageMutex);
}

void
OnHoverEvent(OH_NativeXComponent *component, bool isHover)
{
}

void
OnFocusEvent(OH_NativeXComponent *component, void *window)
{
}

void
OnBlurEvent(OH_NativeXComponent *component, void *window)
{
}

void
OHOS_XcomponentExport(napi_env env, napi_value exports)
{
    napi_value exportInstance = NULL;
    OH_NativeXComponent *nativeXComponent = NULL;
    if ((NULL == env) || (NULL == exports)) {
        return;
    }
    
    if (napi_ok != napi_get_named_property(env, exports, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance)) {
        return;
    }
    
    if (napi_ok != napi_unwrap(env, exportInstance, (void **)(&nativeXComponent))) {
        return;
    }
    
    callback.OnSurfaceCreated = OnSurfaceCreatedCB;
    callback.OnSurfaceChanged = OnSurfaceChangedCB;
    callback.OnSurfaceDestroyed = OnSurfaceDestroyedCB;
    callback.DispatchTouchEvent = onNativeTouch;
    OH_NativeXComponent_RegisterCallback(nativeXComponent, &callback);

    mouseCallback.DispatchMouseEvent = OnDispatchTouchEventCB;
    mouseCallback.DispatchMouseEvent = onNativeMouse;
    mouseCallback.DispatchHoverEvent = OnHoverEvent;
    OH_NativeXComponent_RegisterMouseEventCallback(nativeXComponent, &mouseCallback);

    OH_NativeXComponent_RegisterKeyEventCallback(nativeXComponent, onKeyEvent);
    OH_NativeXComponent_RegisterFocusEventCallback(nativeXComponent, OnFocusEvent);
    OH_NativeXComponent_RegisterBlurEventCallback(nativeXComponent, OnBlurEvent);
    return;
}

#endif /* __OHOS__ */

/* vi: set ts=4 sw=4 expandtab: */
