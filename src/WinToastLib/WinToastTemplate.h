#pragma once
#include <winstring.h>
#include <vector>
using namespace std;

class WinToastTemplate
{
public:
	WinToastTemplate(int txtFieldCount, bool hasImage);
	~WinToastTemplate();
	int					textFieldsCount() const { return _textFieldsCount; }
	bool				hasImage() const { return _hasImage;  }
	vector<wstring>		textFields() const { return _textFields; }
	wstring				textField(int pos) const { return _textFields[pos]; }
	wstring				imagePath() const { return _imagePath;  }
	void				setTextFields(const vector<wstring>& txtF);
	void				setTextField(const wstring& txt, int pos);
	void				setImagePath(const wstring& imgPath);
private:
	int						_textFieldsCount;
	bool					_hasImage;
	vector<wstring>			_textFields;
	wstring			_imagePath;
};

