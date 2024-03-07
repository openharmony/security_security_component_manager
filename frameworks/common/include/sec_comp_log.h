/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SECURITY_COMPONENT_LOG_H
#define SECURITY_COMPONENT_LOG_H

#ifdef HILOG_ENABLE

#include "hilog/log.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD005A05

static constexpr unsigned int SECURITY_DOMAIN_SECURITY_COMPONENT = 0xD005A05;

#define SC_LOG_FATAL(label, fmt, ...)            \
    ((void)HILOG_IMPL(label.type, LOG_FATAL, label.domain, label.tag, \
    "[%{public}s]" fmt, __FUNCTION__, ##__VA_ARGS__))
#define SC_LOG_ERROR(label, fmt, ...)            \
    ((void)HILOG_IMPL(label.type, LOG_ERROR, label.domain, label.tag, \
    "[%{public}s]" fmt, __FUNCTION__, ##__VA_ARGS__))
#define SC_LOG_WARN(label, fmt, ...)            \
    ((void)HILOG_IMPL(label.type, LOG_WARN, label.domain, label.tag, \
    "[%{public}s]" fmt, __FUNCTION__, ##__VA_ARGS__))
#define SC_LOG_INFO(label, fmt, ...)            \
    ((void)HILOG_IMPL(label.type, LOG_INFO, label.domain, label.tag, \
    "[%{public}s]" fmt, __FUNCTION__, ##__VA_ARGS__))
#define SC_LOG_DEBUG(label, fmt, ...)            \
    ((void)HILOG_IMPL(label.type, LOG_DEBUG, label.domain, label.tag, \
    "[%{public}s]" fmt, __FUNCTION__, ##__VA_ARGS__))

#else

#include <cstdio>

#undef LOG_TAG

#define SC_LOG_DEBUG(fmt, ...) printf("[%s] debug: %s: " fmt "\n", LOG_TAG, __func__, ##__VA_ARGS__)
#define SC_LOG_INFO(fmt, ...) printf("[%s] info: %s: " fmt "\n", LOG_TAG, __func__, ##__VA_ARGS__)
#define SC_LOG_WARN(fmt, ...) printf("[%s] warn: %s: " fmt "\n", LOG_TAG, __func__, ##__VA_ARGS__)
#define SC_LOG_ERROR(fmt, ...) printf("[%s] error: %s: " fmt "\n", LOG_TAG, __func__, ##__VA_ARGS__)
#define SC_LOG_FATAL(fmt, ...) printf("[%s] fatal: %s: " fmt "\n", LOG_TAG, __func__, ##__VA_ARGS__)

#endif  // HILOG_ENABLE

#endif  // SECURITY_COMPONENT_LOG_H
