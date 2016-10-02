#pragma once
#ifdef WINTOASTLIB_EXPORTS
#define WINTOASTLIB_API __declspec(dllexport)
#else
#define WINTOASTLIB_API __declspec(dllimport)
#endif

#include "WinToastHandler.h"
#include <winstring.h>
#include <vector>
using namespace std;

class WINTOASTLIB_API WinToastTemplate
{
public:
	enum WinToastTemplateType {
		ImageWithOneLine = ToastTemplateType::ToastTemplateType_ToastImageAndText01,
		ImageWithTwoLines = ToastTemplateType::ToastTemplateType_ToastImageAndText02,
		ImageWithThreeLines = ToastTemplateType::ToastTemplateType_ToastImageAndText03,
		ImageWithFourLines = ToastTemplateType::ToastTemplateType_ToastImageAndText04,
		TextOneLine = ToastTemplateType::ToastTemplateType_ToastText01,
		TextTwoLines = ToastTemplateType::ToastTemplateType_ToastText02,
		TextThreeLines = ToastTemplateType::ToastTemplateType_ToastText03,
		TextFourLines = ToastTemplateType::ToastTemplateType_ToastText04,
		UnknownTemplate = -1
	};

	WinToastTemplate();
	WinToastTemplate(const WinToastTemplateType& type);
	~WinToastTemplate();
	virtual WinToastHandler* handler() const;

	int						textFieldsCount() const { return _textFieldsCount; }
	bool					hasImage() const { return _hasImage; }
	vector<wstring>			textFields() const { return _textFields; }
	wstring					textField(int pos) const { return _textFields[pos]; }
	wstring					imagePath() const { return _imagePath; }
	WinToastTemplateType	type() const { return _type; }
	void					setTextField(const wstring& txt, int pos);
	void					setImagePath(const wstring& imgPath);
private:
	int						_textFieldsCount;
	bool					_hasImage;
	vector<wstring>			_textFields;
	wstring					_imagePath;
	WinToastTemplateType    _type;
	void initComponentsFromType();
};

