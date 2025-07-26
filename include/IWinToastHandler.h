/**
* MIT License
 *
 * Copyright (C) 2016-2025 WinToast v1.3.2 - Mohammed Boujemaoui <mohabouje@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef IWINTOASTHANDLER_H
#define IWINTOASTHANDLER_H

#include <cstdint>

#include "Platform.h"

namespace WinToastLib {
    using namespace ABI::Windows::UI::Notifications;

    struct IWinToastHandler {
        enum class WinToastDismissalReason : int32_t {
            UserCanceled      = ToastDismissalReason_UserCanceled,
            ApplicationHidden = ToastDismissalReason_ApplicationHidden,
            TimedOut          = ToastDismissalReason_TimedOut
        };

        virtual ~IWinToastHandler()                                      = default;
        virtual void toastActivated() const                              = 0;
        virtual void toastActivated(int actionIndex) const               = 0;
        virtual void toastActivated(std::wstring response) const         = 0;
        virtual void toastDismissed(WinToastDismissalReason state) const = 0;
        virtual void toastFailed() const                                 = 0;
    };
}

#endif //IWINTOASTHANDLER_H