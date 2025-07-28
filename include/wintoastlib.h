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

#ifndef WINTOASTLIB_H
#define WINTOASTLIB_H

#include <map>
#include <string>

#include "Platform.h"
#include "IWinToastHandler.h"
#include "WinToastTemplate.h"

namespace WinToastLib {
    void setDebugOutputEnabled(bool enabled);

    class WinToast {
    public:
        enum class WinToastError : int32_t {
            NoError,
            NotInitialized,
            SystemNotSupported,
            ShellLinkNotCreated,
            InvalidAppUserModelID,
            InvalidParameters,
            InvalidHandler,
            NotDisplayed,
            UnknownError
        };

        enum class ShortcutResult : int32_t {
            SHORTCUT_UNCHANGED   = 0,
            SHORTCUT_WAS_CHANGED = 1,
            SHORTCUT_WAS_CREATED = 2,

            SHORTCUT_MISSING_PARAMETERS = -1,
            SHORTCUT_INCOMPATIBLE_OS    = -2,
            SHORTCUT_CREATE_FAILED      = -4
        };

        enum class ShortcutPolicy : int32_t {
            /* Don't check, create, or modify a shortcut. */
            SHORTCUT_POLICY_IGNORE,
            /* Require a shortcut with matching AUMI, don't create or modify an existing one. */
            SHORTCUT_POLICY_REQUIRE_NO_CREATE,
            /* Require a shortcut with matching AUMI, create if missing, modify if not matching. This is the default. */
            SHORTCUT_POLICY_REQUIRE_CREATE,
        };

        WinToast();
        virtual ~WinToast();
        [[nodiscard]] static WinToast* instance();
        [[nodiscard]] static bool isCompatible();
        [[nodiscard]] static bool isSupportingModernFeatures();
        [[nodiscard]] static bool isWin10AnniversaryOrHigher();
        [[nodiscard]] static std::wstring configureAUMI(_In_ std::wstring const& companyName, _In_ std::wstring const& productName,
                                          _In_ std::wstring const& subProduct         = std::wstring(),
                                          _In_ std::wstring const& versionInformation = std::wstring());
        [[nodiscard]] static std::wstring const& strerror(_In_ WinToastError error);
        bool initialize(_Out_opt_ WinToastError* error = nullptr);
        [[nodiscard]] bool isInitialized() const;
        bool hideToast(_In_ INT64 id);
        INT64 showToast(_In_ WinToastTemplate const& toast, _In_ IWinToastHandler* eventHandler,
                                _Out_opt_ WinToastError* error = nullptr);
        void clear();
        [[nodiscard]] ShortcutResult createShortcut();

        [[nodiscard]] std::wstring const& appName() const;
        [[nodiscard]] std::wstring const& appUserModelId() const;
        void setAppUserModelId(_In_ std::wstring const& aumi);
        void setAppName(_In_ std::wstring const& appName);
        void setShortcutPolicy(_In_ ShortcutPolicy policy);

    protected:
        struct NotifyData {
            NotifyData() {}
            NotifyData(_In_ ComPtr<IToastNotification> notify, _In_ EventRegistrationToken activatedToken,
                       _In_ EventRegistrationToken dismissedToken, _In_ EventRegistrationToken failedToken) :
                _notify(notify), _activatedToken(activatedToken), _dismissedToken(dismissedToken), _failedToken(failedToken) {}

            ~NotifyData() {
                RemoveTokens();
            }

            void RemoveTokens() {
                if (!_readyForDeletion) {
                    return;
                }

                if (_previouslyTokenRemoved) {
                    return;
                }

                if (!_notify.Get()) {
                    return;
                }

                _notify->remove_Activated(_activatedToken);
                _notify->remove_Dismissed(_dismissedToken);
                _notify->remove_Failed(_failedToken);
                _previouslyTokenRemoved = true;
            }

            void markAsReadyForDeletion() {
                _readyForDeletion = true;
            }

            [[nodiscard]] bool isReadyForDeletion() const {
                return _readyForDeletion;
            }

            [[nodiscard]] IToastNotification* notification() {
                return _notify.Get();
            }

        private:
            ComPtr<IToastNotification> _notify{nullptr};
            EventRegistrationToken _activatedToken{};
            EventRegistrationToken _dismissedToken{};
            EventRegistrationToken _failedToken{};
            bool _readyForDeletion{false};
            bool _previouslyTokenRemoved{false};
        };

        bool _isInitialized{false};
        ShortcutPolicy _shortcutPolicy{ShortcutPolicy::SHORTCUT_POLICY_REQUIRE_CREATE};
        std::wstring _appName{};
        std::wstring _aumi{};
        std::map<INT64, NotifyData> _buffer{};

        void markAsReadyForDeletion(_In_ INT64 id);
        [[nodiscard]] HRESULT validateShellLinkHelper(_Out_ bool& wasChanged);
        [[nodiscard]] HRESULT createShellLinkHelper();
        [[nodiscard]] HRESULT setImageFieldHelper(_In_ IXmlDocument* xml, _In_ std::wstring const& path, _In_ bool isToastGeneric, bool isCropHintCircle);
        [[nodiscard]] HRESULT setHeroImageHelper(_In_ IXmlDocument* xml, _In_ std::wstring const& path, _In_ bool isInlineImage);
        [[nodiscard]] HRESULT setBindToastGenericHelper(_In_ IXmlDocument* xml);
        [[nodiscard]] HRESULT setAudioFieldHelper(_In_ IXmlDocument* xml, _In_ std::wstring const& path,
                            _In_opt_ WinToastTemplate::AudioOption option = WinToastTemplate::AudioOption::Default);
        [[nodiscard]] HRESULT setTextFieldHelper(_In_ IXmlDocument* xml, _In_ std::wstring const& text, _In_ UINT32 pos);
        [[nodiscard]] HRESULT setAttributionTextFieldHelper(_In_ IXmlDocument* xml, _In_ std::wstring const& text);
        [[nodiscard]] HRESULT addActionHelper(_In_ IXmlDocument* xml, _In_ std::wstring const& action, _In_ std::wstring const& arguments);
        [[nodiscard]] HRESULT addDurationHelper(_In_ IXmlDocument* xml, _In_ std::wstring const& duration);
        [[nodiscard]] HRESULT addScenarioHelper(_In_ IXmlDocument* xml, _In_ std::wstring const& scenario);
        [[nodiscard]] HRESULT addInputHelper(_In_ IXmlDocument* xml);
        [[nodiscard]] ComPtr<IToastNotifier> notifier(_In_ bool* succeded) const;
        void setError(_Out_opt_ WinToastError* error, _In_ WinToastError value);
    };
} // namespace WinToastLib
#endif // WINTOASTLIB_H
