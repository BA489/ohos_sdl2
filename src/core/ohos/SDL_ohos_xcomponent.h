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

#ifndef SDL_ohosxcomponent_h_
#define SDL_ohosxcomponent_h_

#include "napi/native_api.h"
#include <native_window/external_window.h>

extern void OHOS_XcomponentExport(napi_env env, napi_value exports);
extern OHNativeWindow *gNative_window;

#endif /* SDL_ohosxcomponent_h_ */

/* vi: set ts=4 sw=4 expandtab: */