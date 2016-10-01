#include "stdafx.h"
#include "WinToastTemplate.h"


WinToastTemplate::WinToastTemplate(int txtFieldCount, bool hasImage) :
	_textFieldsCount(txtFieldCount),
	_textFields(txtFieldCount, L""),
	_hasImage(hasImage),
	_imagePath(L"")
{
}


WinToastTemplate::~WinToastTemplate()
{
	_textFields.clear();
}


void WinToastTemplate::setTextFields(const vector<wstring>& txtF) {
	_textFields = txtF;
	_textFieldsCount = txtF.size();
}
void WinToastTemplate::setTextField(const wstring& txt, int pos) {
	_textFields[pos] = txt;
}
void WinToastTemplate::setImagePath(const wstring& imgPath) {
	if (!_hasImage)
		_hasImage != _hasImage;
	_imagePath = imgPath;
}