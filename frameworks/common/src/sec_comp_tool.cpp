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
#include "sec_comp_tool.h"

#include <cmath>
#include <cstdint>
#include "sec_comp_log.h"

namespace OHOS {
namespace Security {
namespace SecurityComponent {
namespace {
static constexpr double PI_CIRCLE = 3.1415926;
static constexpr double MIN_HSV_DISTANCE = 25.0;
static constexpr double MAX_RGB_VALUE = 255.0;
static constexpr uint8_t MAX_TRANSPARENT = 0x99; // 60%
static constexpr double ZERO_DOUBLE = 0.0;
static constexpr double THIRTY_ANGLE = 30.0;
static constexpr double SIXTY_ANGLE = 60.0;
static constexpr double ONE_HUNDRED_TWEENTY_ANGLE = 120.0;
static constexpr double ONE_HUNDRED_EIGHTY_ANGLE = 180.0;
static constexpr double TWO_HUNDREDS_FORTY_ANGLE = 240.0;
static constexpr double THREE_HUNDREDS_SIXTY_ANGLE = 360.0;
static constexpr double DEFAULT_R = 100.0;
static const uint8_t MAX_ALPHA = 0xFF;
static const double MIN_CONTRACST_ALPHA = 0.5;

constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_SECURITY_COMPONENT, "SecCompTool"};
}

static inline double MaxValue(double a, double b, double c)
{
    return ((a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c));
}

static inline double MinValue(double a, double b, double c)
{
    return ((a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c));
}

struct HsvColor {
    double h;
    double s;
    double v;
};

static HsvColor RgbToHsv(uint8_t r, uint8_t g, uint8_t b)
{
    HsvColor hsv;
    double red = static_cast<double>(r) / MAX_RGB_VALUE;
    double green = static_cast<double>(g) / MAX_RGB_VALUE;
    double blue = static_cast<double>(b) / MAX_RGB_VALUE;
    double max = MaxValue(red, green, blue);
    double min = MinValue(red, green, blue);
    double delta = max - min;
    if (max == min) {
        hsv.h = ZERO_DOUBLE;
    } else if (max == red) {
        hsv.h = fmod((SIXTY_ANGLE * ((green - blue) / delta) + THREE_HUNDREDS_SIXTY_ANGLE),
            THREE_HUNDREDS_SIXTY_ANGLE);
    } else if (max == green) {
        hsv.h = fmod((SIXTY_ANGLE * ((blue - red) / delta) + ONE_HUNDRED_TWEENTY_ANGLE), THREE_HUNDREDS_SIXTY_ANGLE);
    } else if (max == blue) {
        hsv.h = fmod((SIXTY_ANGLE * ((red - green) / delta) + TWO_HUNDREDS_FORTY_ANGLE), THREE_HUNDREDS_SIXTY_ANGLE);
    }

    if (max == 0) {
        hsv.s = ZERO_DOUBLE;
    } else {
        hsv.s = delta / max;
    }

    hsv.v = max;
    return hsv;
}

static inline double GetHsvDisX(const HsvColor& hsv, double r)
{
    return r * hsv.v * hsv.s * cos(hsv.h / ONE_HUNDRED_EIGHTY_ANGLE * PI_CIRCLE);
}

static inline double GetHsvDisY(const HsvColor& hsv, double r)
{
    return r * hsv.v * hsv.s * sin(hsv.h / ONE_HUNDRED_EIGHTY_ANGLE * PI_CIRCLE);
}

static inline double GetHsvDisZ(const HsvColor& hsv, double h)
{
    return h * (1 - hsv.v);
}

static double HsvDistance(const HsvColor& hsv1, const HsvColor& hsv2)
{
    double rDef = DEFAULT_R;
    double angle = THIRTY_ANGLE;
    double h = rDef * cos(angle / ONE_HUNDRED_EIGHTY_ANGLE * PI_CIRCLE);
    double r = rDef * sin(angle / ONE_HUNDRED_EIGHTY_ANGLE * PI_CIRCLE);

    double x1 = GetHsvDisX(hsv1, r);
    double y1 = GetHsvDisY(hsv1, r);
    double z1 = GetHsvDisZ(hsv1, h);

    double x2 = GetHsvDisX(hsv2, r);
    double y2 = GetHsvDisY(hsv2, r);
    double z2 = GetHsvDisZ(hsv2, h);

    double dx = x1 - x2;
    double dy = y1 - y2;
    double dz = z1 - z2;

    return sqrt((dx * dx) + (dy * dy) + (dz * dz));
}

static bool IsColorAplhaSimilar(const SecCompColor& fgColor, const SecCompColor& bgColor)
{
    double fgAlpha = static_cast<double>(fgColor.argb.alpha) / MAX_ALPHA;
    double bgAlpha = static_cast<double>(bgColor.argb.alpha) / MAX_ALPHA;

    double mixAlpha = fgAlpha + bgAlpha - fgAlpha * bgAlpha;
    if (GreatNotEqual(bgAlpha / mixAlpha, MIN_CONTRACST_ALPHA)) {
        SC_LOG_ERROR(LABEL, "FgAlpha=%{public}x BgAlpha=%{public}x is similar, check failed",
            fgColor.argb.alpha, bgColor.argb.alpha);
        return true;
    }
    return false;
}

bool IsColorSimilar(const SecCompColor& color1, const SecCompColor& color2)
{
    HsvColor hsv1 = RgbToHsv(color1.argb.red, color1.argb.green, color1.argb.blue);
    HsvColor hsv2 = RgbToHsv(color2.argb.red, color2.argb.green, color2.argb.blue);

    double distance = HsvDistance(hsv1, hsv2);
    SC_LOG_DEBUG(LABEL, "Compare color %{public}x %{public}x distance %{public}f",
        color1.value, color2.value, distance);
    return (distance < MIN_HSV_DISTANCE) && IsColorAplhaSimilar(color1, color2);
}

bool IsColorTransparent(const SecCompColor& color)
{
    SC_LOG_DEBUG(LABEL, "Color %{public}x alpha %{public}x", color.value, color.argb.alpha);
    return color.argb.alpha < MAX_TRANSPARENT;
}

bool IsColorFullTransparent(const SecCompColor& color)
{
    return color.argb.alpha == 0;
}
}  // namespace SecurityComponent
}  // namespace Security
}  // namespace OHOS
