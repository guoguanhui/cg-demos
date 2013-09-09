#ifndef RadioButton_h__
#define RadioButton_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class RadioButtonGroup;

class _ApiExport RadioButton : public UIElement
{
public:
	static const String RadioStyleName;

public:
	RadioButton();
	virtual ~RadioButton();

	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles /* = nullptr */);
	virtual void Update(float dt);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	virtual bool CanHaveFocus() const;

	void Toggle();
	void SetText(const std::wstring& text);

protected:
	
	std::wstring mText;
	bool mChecked;
	bool mPressed;
	bool mCanUncheck;

	GuiSkin::GuiStyle* mStyle;

private:
	fastdelegate::FastDelegate1<RadioButton*> EventButtonClicked;

	friend class RadioButtonGroup;
};


class _ApiExport RadioButtonGroup : public UIElement
{
public:
	typedef fastdelegate::FastDelegate1<int32_t> SelChangedEventHandler;
	SelChangedEventHandler EventSelectionChanged;

public:
	RadioButtonGroup();
	~RadioButtonGroup();

	void AddButton(RadioButton* btn);
	void AddButton(RadioButton* btn, int32_t idx);

	RadioButton* CheckedButton() const;
	int32_t	CheckedIndex() const;

	const std::list<RadioButton*>& GetButtons() const			{ return mRadioButtons; }
	
protected:
	void HandleButtonClicked(RadioButton* btn);


protected:

	std::list<RadioButton*> mRadioButtons;	
};


}


#endif // RadioButton_h__