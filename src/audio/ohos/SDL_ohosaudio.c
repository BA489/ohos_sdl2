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

#if SDL_AUDIO_DRIVER_OHOS

/* Output audio to ohos */

#include "SDL_assert.h"
#include "SDL_audio.h"
#include "../SDL_audio_c.h"

#include "SDL_ohosaudio.h"
#include "SDL_ohosaudiomanager.h"


static SDL_AudioDevice* audioDevice = NULL;
static SDL_AudioDevice* captureDevice = NULL;

static int
OHOSAUDIO_OpenDevice(_THIS, void *handle, const char *devname, int iscapture)
{
    SDL_AudioFormat test_format;

    SDL_assert((captureDevice == NULL) || !iscapture);
    SDL_assert((audioDevice == NULL) || iscapture);

    if (iscapture) {
        captureDevice = this;
    } else {
        audioDevice = this;
    }

    this->hidden = (struct SDL_PrivateAudioData *) SDL_calloc(1, (sizeof *this->hidden));
    if (this->hidden == NULL) {
        return SDL_OutOfMemory();
    }

    test_format = SDL_FirstAudioFormat(this->spec.format);
    while (test_format != 0) { /* no "UNKNOWN" constant */
        if ((test_format == AUDIO_U8) ||
			(test_format == AUDIO_S16) ||
			(test_format == AUDIO_F32)) {
            this->spec.format = test_format;
            break;
        }
        test_format = SDL_NextAudioFormat();
    }

    if (test_format == 0) {
        /* Didn't find a compatible format :( */
        return SDL_SetError("No compatible audio format!");
    }

    if (OHOSAUDIO_NATIVE_OpenAudioDevice(iscapture, &this->spec) < 0) {
        return -1;
    }

    SDL_CalculateAudioSpec(&this->spec);

    return 0;
}

static void
OHOSAUDIO_PlayDevice(_THIS)
{
    OHOSAUDIO_NATIVE_WriteAudioBuf();
}

static Uint8 *
OHOSAUDIO_GetDeviceBuf(_THIS)
{
    return (Uint8 *)OHOSAUDIO_NATIVE_GetAudioBuf();
}

static int
OHOSAUDIO_CaptureFromDevice(_THIS, void *buffer, int buflen)
{
    return OHOSAUDIO_NATIVE_CaptureAudioBuffer(buffer, buflen);
}

static void
OHOSAUDIO_FlushCapture(_THIS)
{
    OHOSAUDIO_NATIVE_FlushCapturedAudio();
}

static void
OHOSAUDIO_CloseDevice(_THIS)
{
    /* At this point SDL_CloseAudioDevice via close_audio_device took care of terminating the audio thread
       so it's safe to terminate the Java side buffer and AudioTrack
     */
    OHOSAUDIO_NATIVE_CloseAudioDevice(this->iscapture);
    if (this->iscapture) {
        SDL_assert(captureDevice == this);
        captureDevice = NULL;
    } else {
        SDL_assert(audioDevice == this);
        audioDevice = NULL;
    }
    SDL_free(this->hidden);
}

static int
OHOSAUDIO_Init(SDL_AudioDriverImpl * impl)
{
    /* Set the function pointers */
    impl->OpenDevice = OHOSAUDIO_OpenDevice;
    impl->PlayDevice = OHOSAUDIO_PlayDevice;
    impl->GetDeviceBuf = OHOSAUDIO_GetDeviceBuf;
    impl->CloseDevice = OHOSAUDIO_CloseDevice;
    impl->CaptureFromDevice = OHOSAUDIO_CaptureFromDevice;
    impl->FlushCapture = OHOSAUDIO_FlushCapture;

    /* and the capabilities */
    impl->HasCaptureSupport = SDL_TRUE;
    impl->OnlyHasDefaultOutputDevice = 1;
    impl->OnlyHasDefaultCaptureDevice = 1;

    return 1;   /* this audio target is available. */
}

AudioBootStrap OHOSAUDIO_bootstrap = {
    "ohos", "SDL Ohos audio driver", OHOSAUDIO_Init, 0
};

/* Pause (block) all non already paused audio devices by taking their mixer lock */
void OHOSAUDIO_PauseDevices(void)
{
    /* TODO: Handle multiple devices? */
    struct SDL_PrivateAudioData *private;
    if(audioDevice != NULL && audioDevice->hidden != NULL) {
        private = (struct SDL_PrivateAudioData *) audioDevice->hidden;
        if (SDL_AtomicGet(&audioDevice->paused)) {
            /* The device is already paused, leave it alone */
            private->resume = SDL_FALSE;
        }
        else {
            SDL_LockMutex(audioDevice->mixer_lock);
            SDL_AtomicSet(&audioDevice->paused, 1);
            private->resume = SDL_TRUE;
        }
    }

    if(captureDevice != NULL && captureDevice->hidden != NULL) {
        private = (struct SDL_PrivateAudioData *) captureDevice->hidden;
        if (SDL_AtomicGet(&captureDevice->paused)) {
            /* The device is already paused, leave it alone */
            private->resume = SDL_FALSE;
        }
        else {
            SDL_LockMutex(captureDevice->mixer_lock);
            SDL_AtomicSet(&captureDevice->paused, 1);
            private->resume = SDL_TRUE;
        }
    }
}

/* Resume (unblock) all non already paused audio devices by releasing their mixer lock */
void OHOSAUDIO_ResumeDevices(void)
{
    /* TODO: Handle multiple devices? */
    struct SDL_PrivateAudioData *private;
    if(audioDevice != NULL && audioDevice->hidden != NULL) {
        private = (struct SDL_PrivateAudioData *) audioDevice->hidden;
        if (private->resume) {
            SDL_AtomicSet(&audioDevice->paused, 0);
            private->resume = SDL_FALSE;
            SDL_UnlockMutex(audioDevice->mixer_lock);
        }
    }

    if(captureDevice != NULL && captureDevice->hidden != NULL) {
        private = (struct SDL_PrivateAudioData *) captureDevice->hidden;
        if (private->resume) {
            SDL_AtomicSet(&captureDevice->paused, 0);
            private->resume = SDL_FALSE;
            SDL_UnlockMutex(captureDevice->mixer_lock);
        }
    }
}

#else 

void OHOSAUDIO_ResumeDevices(void) {}
void OHOSAUDIO_PauseDevices(void) {}

#endif /* SDL_AUDIO_DRIVER_OHOS */

/* vi: set ts=4 sw=4 expandtab: */