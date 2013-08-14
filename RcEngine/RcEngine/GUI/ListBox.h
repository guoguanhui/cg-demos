#ifndef ListBox_h__
#define ListBox_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport ListBox : public UIElement
{
public:
	ListBox();
	virtual ~ListBox();

	virtual void OnResize();
	virtual void Initialize(const GuiSkin::StyleMap* styles /* = nullptr */);

	virtual bool CanHaveFocus() const;

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	virtual void UpdateRect();

	void SetBorder(int32_t border, int32_t margin);
	void SetScrollBarWidth(int32_t width);

	void AddItem(const std::wstring& text);
	void InsertItem(int32_t index, const std::wstring& text);
	void RemoveItem(int32_t index);
	void RemoveAllItems();

	void SetSelectedIndex(int32_t index); 
	int32_t GetSelectecIndex() const;

protected:

	std::vector<std::wstring> mItems;

	ScrollBar* mVertScrollBar;

	int32_t mScrollBarWidth;

	int32_t mSelectedIndex;

	int32_t mTextRowHeight;

	int32_t mNumVisibleItems;

	/**
	 * Hack:
	 *  Normal: background image
	 *  Hover:  selection image
	 */
	GuiSkin::GuiStyle* mLisBoxStyle;

	IntRect mTextRegion;
	IntRect mSelectionRegion;

	int32_t mBorder, mMargin;

	bool mPressed;
};


}


#endif // ListBox_h__
