// MIT License
//
// Copyright (C) 2016-2025 WinToast - Mohammed Boujemaoui <mohabouje@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef WINTOASTTEMPLATE_H
#define WINTOASTTEMPLATE_H

#include <cstdint>
#include <string>
#include <vector>

#include "Platform.h"

namespace WinToastLib {
    struct WinToastTemplate final {
        enum class Scenario : int32_t { Default, Alarm, IncomingCall, Reminder };
        enum class Duration : int32_t { System, Short, Long };
        enum class AudioOption : int32_t { Default, Silent, Loop };
        enum class TextField : int32_t { FirstLine, SecondLine, ThirdLine };

        enum class Type : int32_t {
            ImageAndText01 = ToastTemplateType_ToastImageAndText01,
            ImageAndText02 = ToastTemplateType_ToastImageAndText02,
            ImageAndText03 = ToastTemplateType_ToastImageAndText03,
            ImageAndText04 = ToastTemplateType_ToastImageAndText04,
            Text01         = ToastTemplateType_ToastText01,
            Text02         = ToastTemplateType_ToastText02,
            Text03         = ToastTemplateType_ToastText03,
            Text04         = ToastTemplateType_ToastText04
        };

        enum class AudioSystemFile : int32_t {
            DefaultSound,
            IM,
            Mail,
            Reminder,
            SMS,
            Alarm,
            Alarm2,
            Alarm3,
            Alarm4,
            Alarm5,
            Alarm6,
            Alarm7,
            Alarm8,
            Alarm9,
            Alarm10,
            Call,
            Call1,
            Call2,
            Call3,
            Call4,
            Call5,
            Call6,
            Call7,
            Call8,
            Call9,
            Call10,
        };

        enum class CropHint : int32_t {
            Square,
            Circle,
        };

        WinToastTemplate(_In_ Type type = Type::ImageAndText02);
        ~WinToastTemplate();

        void setFirstLine(_In_ std::wstring const& text);
        void setSecondLine(_In_ std::wstring const& text);
        void setThirdLine(_In_ std::wstring const& text);
        void setTextField(_In_ std::wstring const& txt, _In_ TextField pos);
        void setAttributionText(_In_ std::wstring const& attributionText);
        void setImagePath(_In_ std::wstring const& imgPath, _In_ CropHint cropHint = CropHint::Square);
        void setHeroImagePath(_In_ std::wstring const& imgPath, _In_ bool inlineImage = false);
        void setAudioPath(_In_ AudioSystemFile audio);
        void setAudioPath(_In_ std::wstring const& audioPath);
        void setAudioOption(_In_ AudioOption audioOption);
        void setDuration(_In_ Duration duration);
        void setExpiration(_In_ INT64 millisecondsFromNow);
        void setScenario(_In_ Scenario scenario);
        void addAction(_In_ std::wstring const& label);
        void addInput();

        std::size_t textFieldsCount() const;
        std::size_t actionsCount() const;
        bool hasImage() const;
        bool hasHeroImage() const;
        std::vector<std::wstring> const& textFields() const;
        std::wstring const& textField(_In_ TextField pos) const;
        std::wstring const& actionLabel(_In_ std::size_t pos) const;
        std::wstring const& imagePath() const;
        std::wstring const& heroImagePath() const;
        std::wstring const& audioPath() const;
        std::wstring const& attributionText() const;
        std::wstring const& scenario() const;
        INT64 expiration() const;
        Type type() const;
        AudioOption audioOption() const;
        Duration duration() const;
        bool isToastGeneric() const;
        bool isInlineHeroImage() const;
        bool isCropHintCircle() const;
        bool isInput() const;

    private:
        bool _hasInput{false};

        std::vector<std::wstring> _textFields{};
        std::vector<std::wstring> _actions{};
        std::wstring _imagePath{};
        std::wstring _heroImagePath{};
        bool _inlineHeroImage{false};
        std::wstring _audioPath{};
        std::wstring _attributionText{};
        std::wstring _scenario{L"Default"};
        INT64 _expiration{0};
        AudioOption _audioOption{AudioOption::Default};
        Type _type{Type::Text01};
        Duration _duration{Duration::System};
        CropHint _cropHint{CropHint::Square};
    };
}

#endif //WINTOASTTEMPLATE_H