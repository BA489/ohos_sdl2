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

#if SDL_VIDEO_DRIVER_OHOS

/* OHOS SDL video driver implementation */

#include "SDL_video.h"
#include "../SDL_egl_c.h"
#include "SDL_ohoswindow.h"

#include "SDL_ohosvideo.h"
#include "SDL_ohosgl.h"
#include "../../core/ohos/SDL_ohos.h"

#include <dlfcn.h>

int
OHOS_GLES_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context)
{
    if (window && context) {
        return SDL_EGL_MakeCurrent(_this, ((SDL_WindowData *) window->driverdata)->egl_surface, context);
    } else {
        return SDL_EGL_MakeCurrent(_this, NULL, NULL);
    }
}

SDL_GLContext
OHOS_GLES_CreateContext(_THIS, SDL_Window * window)
{
    SDL_GLContext ret;

    OHOS_PageMutex_Lock_Running();

    ret = SDL_EGL_CreateContext(_this, ((SDL_WindowData *) window->driverdata)->egl_surface);

    SDL_UnlockMutex(OHOS_PageMutex);

    return ret;
}

int
OHOS_GLES_SwapWindow(_THIS, SDL_Window * window)
{
    int retval;

    SDL_LockMutex(OHOS_PageMutex);

    /* The following two calls existed in the original Java code
     * If you happen to have a device that's affected by their removal,
     * please report to Bugzilla. -- Gabriel
     */
    retval = SDL_EGL_SwapBuffers(_this, ((SDL_WindowData *) window->driverdata)->egl_surface);

    SDL_UnlockMutex(OHOS_PageMutex);

    return retval;
}

int
OHOS_GLES_LoadLibrary(_THIS, const char *path) {
    return SDL_EGL_LoadLibrary(_this, path, (NativeDisplayType) 0, 0);
}

#endif /* SDL_VIDEO_DRIVER_OHOS */

/* vi: set ts=4 sw=4 expandtab: */
