#include "stdafx.h"
#include "WinToastTemplate.h"


WinToastTemplate::WinToastTemplate(int txtFieldCount, bool hasImage) :
	_textFieldsCount(txtFieldCount),
	_hasImage(hasImage)
{
	initComponentsFromConfiguration();
}

WinToastTemplate::WinToastTemplate(const WinToastTemplateType& type) : 
	_type(type)
{
	initComponentsFromType();
}


WinToastTemplate::~WinToastTemplate()
{
	_textFields.clear();
}


void WinToastTemplate::setTextField(const wstring& txt, int pos) {
	_textFields[pos] = txt;
}
void WinToastTemplate::setImagePath(const wstring& imgPath) {
	if (!_hasImage)
		return;
	_imagePath = imgPath;
}

void WinToastTemplate::initComponentsFromType() {
	_hasImage = _type < ToastTemplateType_ToastText01;
	_textFieldsCount = (_hasImage ? _type : _type - ToastTemplateType_ToastText01) + 1;
	_textFields.reserve(_textFieldsCount);
}

void WinToastTemplate::initComponentsFromConfiguration() {
	_textFields.reserve(_textFieldsCount);
	_type = static_cast<WinToastTemplateType> ((_textFieldsCount - 1) + (_hasImage ? 0 : ToastTemplateType_ToastText01));
}
