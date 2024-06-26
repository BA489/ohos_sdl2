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

#ifdef SDL_VIDEO_DRIVER_OHOS
#if SDL_VIDEO_DRIVER_OHOS
#endif

#include "SDL_ohosevents.h"
#include "SDL_events.h"
#include "../SDL_sysvideo.h"
#include "../../events/SDL_events_c.h"

/* Can't include sysaudio "../../audio/ohos/SDL_ohosaudio.h"
 * because of THIS redefinition */

#if !SDL_OHOS_DISABLED && SDL_AUDIO_DRIVER_OHOS
extern void OHOSAUDIO_ResumeDevices(void);
extern void OHOSAUDIO_PauseDevices(void);
#else
static void OHOSAUDIO_ResumeDevices(void) {}
static void OHOSAUDIO_PauseDevices(void) {}
#endif

#if !SDL_AUDIO_DISABLED && SDL_AUDIO_DRIVER_OPENSLES
extern void openslES_ResumeDevices(void);
extern void openslES_PauseDevices(void);
#else
static void openslES_ResumeDevices(void) {}
static void openslES_PauseDevices(void) {}
#endif

/* Number of 'type' events in the event queue */
static int SDL_NumberOfEvents(Uint32 type) { return SDL_PeepEvents(NULL, 0, SDL_PEEKEVENT, type, type); }

static void OHOS_EGL_context_restore(SDL_Window *window)
{
    if (window) {
        SDL_Event event;
        SDL_WindowData *data = (SDL_WindowData *)window->driverdata;
        if (SDL_GL_MakeCurrent(window, (SDL_GLContext)data->egl_context) < 0) {
            /* The context is no longer valid, create a new one */
            data->egl_context = (EGLContext)SDL_GL_CreateContext(window);
            SDL_GL_MakeCurrent(window, (SDL_GLContext)data->egl_context);
            event.type = SDL_RENDER_DEVICE_RESET;
            SDL_PushEvent(&event);
        }
        data->backup_done = 0;
    }
}

static void OHOS_EGL_context_backup(SDL_Window *window)
{
    if (window) {
        /* Keep a copy of the EGL Context so we can try to restore it when we resume */
        SDL_WindowData *data = (SDL_WindowData *)window->driverdata;
        data->egl_context = SDL_GL_GetCurrentContext();
        /* We need to do this so the EGLSurface can be freed */
        SDL_GL_MakeCurrent(window, NULL);
        data->backup_done = 1;
    }
}

void OHOS_PUMPEVENTS_Blocking(SDL_VideoDevice *thisDevice)
{
    SDL_Window *ohosWindow = thisDevice->windows;
    SDL_VideoData *videodata = (SDL_VideoData *)thisDevice->driverdata;
    if (videodata->isPaused) {
        SDL_bool isContextExternal = SDL_IsVideoContextExternal();
        /* Make sure this is the last thing we do before pausing */
        if (!isContextExternal) {
            SDL_LockMutex(g_ohosPageMutex);
            OHOS_EGL_context_backup(ohosWindow);
            SDL_UnlockMutex(g_ohosPageMutex);
        }

        OHOSAUDIO_PauseDevices();
        openslES_PauseDevices();

        if (SDL_SemWait(g_ohosResumeSem) == 0) {
            videodata->isPaused = 0;

            /* g_ohosResumeSem was signaled */
            SDL_SendAppEvent(SDL_APP_WILLENTERFOREGROUND);
            SDL_SendAppEvent(SDL_APP_DIDENTERFOREGROUND);
            SDL_SendWindowEvent(ohosWindow, SDL_WINDOWEVENT_RESTORED, 0, 0);

            OHOSAUDIO_ResumeDevices();
            openslES_ResumeDevices();

            /* Restore the GL Context from here, as this operation is thread dependent */
            if (!isContextExternal && !SDL_HasEvent(SDL_QUIT)) {
                SDL_LockMutex(g_ohosPageMutex);
                OHOS_EGL_context_restore(ohosWindow);
                SDL_UnlockMutex(g_ohosPageMutex);
            }

            /* Make sure SW Keyboard is restored when an app becomes foreground */
            if (SDL_IsTextInputActive()) {
                OHOS_StartTextInput(thisDevice); /* Only showTextInput */
            }
        }
    } else {
        if (videodata->isPausing || SDL_SemTryWait(g_ohosPauseSem) == 0) {
            /* g_ohosPauseSem was signaled */
            if (videodata->isPausing == 0) {
                SDL_SendWindowEvent(ohosWindow, SDL_WINDOWEVENT_MINIMIZED, 0, 0);
                SDL_SendAppEvent(SDL_APP_WILLENTERBACKGROUND);
                SDL_SendAppEvent(SDL_APP_DIDENTERBACKGROUND);
            }
            /* We've been signaled to pause (potentially several times), but before we block ourselves,
             * we need to make sure that the very last event (of the first pause sequence, if several)
             * has reached the app */
            if (SDL_NumberOfEvents(SDL_APP_DIDENTERBACKGROUND) > SDL_SemValue(g_ohosPauseSem)) {
                videodata->isPausing = 1;
            } else {
                videodata->isPausing = 0;
                videodata->isPaused = 1;
            }
        }
    }
}

void OHOS_PUMPEVENTS_NonBlocking(SDL_VideoDevice *thisDevice)
{
    SDL_Window *ohosWindow = thisDevice->windows;
    SDL_VideoData *videodata = (SDL_VideoData *)thisDevice->driverdata;
    static int backup_context = 0;

    if (videodata->isPaused) {
        SDL_bool isContextExternal = SDL_IsVideoContextExternal();
        if (backup_context) {
            if (!isContextExternal) {
                SDL_LockMutex(g_ohosPageMutex);
                OHOS_EGL_context_backup(ohosWindow);
                SDL_UnlockMutex(g_ohosPageMutex);
            }

            OHOSAUDIO_PauseDevices();
            openslES_PauseDevices();

            backup_context = 0;
        }
        if (SDL_SemTryWait(g_ohosResumeSem) == 0) {
            videodata->isPaused = 0;

            /* g_ohosResumeSem was signaled */
            SDL_SendAppEvent(SDL_APP_WILLENTERFOREGROUND);
            SDL_SendAppEvent(SDL_APP_DIDENTERFOREGROUND);
            SDL_SendWindowEvent(ohosWindow, SDL_WINDOWEVENT_RESTORED, 0, 0);

            OHOSAUDIO_ResumeDevices();
            openslES_ResumeDevices();

            /* Restore the GL Context from here, as this operation is thread dependent */
            if (!isContextExternal && !SDL_HasEvent(SDL_QUIT)) {
                SDL_LockMutex(g_ohosPageMutex);
                OHOS_EGL_context_restore(ohosWindow);
                SDL_UnlockMutex(g_ohosPageMutex);
            }

            /* Make sure SW Keyboard is restored when an app becomes foreground */
            if (SDL_IsTextInputActive()) {
                OHOS_StartTextInput(thisDevice->windows); /* Only showTextInput */
            }
        }
    } else {
        if (videodata->isPausing || SDL_SemTryWait(g_ohosPauseSem) == 0) {
            /* g_ohosPauseSem was signaled */
            if (videodata->isPausing == 0) {
                SDL_SendWindowEvent(ohosWindow, SDL_WINDOWEVENT_MINIMIZED, 0, 0);
                SDL_SendAppEvent(SDL_APP_WILLENTERBACKGROUND);
                SDL_SendAppEvent(SDL_APP_DIDENTERBACKGROUND);
            }
            /* We've been signaled to pause (potentially several times), but before we block ourselves,
             * we need to make sure that the very last event (of the first pause sequence, if several)
             * has reached the app */
            if (SDL_NumberOfEvents(SDL_APP_DIDENTERBACKGROUND) > SDL_SemValue(g_ohosPauseSem)) {
                videodata->isPausing = 1;
            } else {
                videodata->isPausing = 0;
                videodata->isPaused = 1;
                backup_context = 1;
            }
        }
    }
}

#endif /* SDL_VIDEO_DRIVER_OHOS */

/* vi: set ts=4 sw=4 expandtab: */
