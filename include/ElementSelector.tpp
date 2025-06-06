#pragma once

#include "ElementSelector.hpp"
#include "FileManager.hpp"
#include "Utility.hpp"

#include <cassert>

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
ElementSelector<ElementPtr>::ElementSelector(const Recti& bounds)
	: mBounds{bounds}
	, mSelectedIndx{0}
	, mFirstVisibleIndx{0}
	, mLastingSelectIndxs{}
	, mShuffleRandomEngine()
{
}

/**
* @brief ElementSelector::CrutchConverter - convert vector of Element to vector of Element_Ptr;
* @param elements - vector of Elements
* @return - vector of shared_ptr<Element>
*/
template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
template <typename Element>
	requires NameIsString<Element>
std::vector<ElementPtr> ElementSelector<ElementPtr>::CrutchConverter(
	std::vector<Element> elements)
{
	std::vector<ElementPtr> result;
	for (auto& element : elements)
	{
		result.push_back(std::make_unique<Element>(element));
	}
	return result;
}

/**
 * Load new file, overwrite previous files or append to them, depeneding on value of
 * overwrite
 *
 * @param file file to load
 * @param overwrite if true, then erase previous files, if false, just append new
 * files
 */
template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ElementSelector<ElementPtr>::Load(ElementPtr file, bool overwrite)
{
	if (overwrite)
	{
		mLastingSelectIndxs.clear();
		mFiles.clear();
	}
	mFiles.push_back(std::move(file));
}

/**
 * Load new files, overwrite previous files or append to them, depeneding on value
 * of overwrite
 *
 * @param files files to load
 * @param overwrite if true, then erase previous files, if false, just append new
 * files
 */
template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ElementSelector<ElementPtr>::Load(std::vector<ElementPtr> files, bool overwrite)
{
	if (overwrite)
	{
		mLastingSelectIndxs.clear();
		mFiles.swap(files);
	}
	else
	{
		mFiles.reserve(mFiles.size() + files.size());
		for (auto& file : files)
		{
			mFiles.push_back(std::move(file));
		}
	}
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ElementSelector<ElementPtr>::Update()
{
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ElementSelector<ElementPtr>::HandleResize(const Recti& bounds)
{
	mBounds = bounds;
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ElementSelector<ElementPtr>::SelectUp()
{
	if (mSelectedIndx == 0)
		return;

	mSelectedIndx--;
	if (mSelectedIndx < mFirstVisibleIndx)
	{
		mFirstVisibleIndx--;
	}
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ElementSelector<ElementPtr>::SelectDown()
{
	if (mSelectedIndx + 1 >= mFiles.size())
		return;

	mSelectedIndx++;
	if (mSelectedIndx > mFirstVisibleIndx + mBounds.height - 1)
	{
		mFirstVisibleIndx++;
	}
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ElementSelector<ElementPtr>::SelectByIndx(std::size_t indx)
{
	assert(indx < mFiles.size());

	mSelectedIndx = indx;
	mFirstVisibleIndx = indx;
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ElementSelector<ElementPtr>::SelectByName(const std::string& filename)
{
	for (auto iter{begin(mFiles)}; iter != end(mFiles); ++iter)
	{
		if ((*iter)->name == filename)
		{
			SelectByIndx(std::distance(begin(mFiles), iter));
			return;
		}
	}
	assert(false);
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ElementSelector<ElementPtr>::ClearAllLastingSelections()
{
	mLastingSelectIndxs.clear();
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ElementSelector<ElementPtr>::LastingSelectOnCurrent()
{
	if (mLastingSelectIndxs.contains(mSelectedIndx))
	{
		mLastingSelectIndxs.erase(mSelectedIndx);
	}
	else
	{
		mLastingSelectIndxs.insert(mSelectedIndx);
	}
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ElementSelector<ElementPtr>::ShuffleElements()
{
	std::shuffle(mFiles.begin(), mFiles.end(), mShuffleRandomEngine);
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
ElementSelectorState ElementSelector<ElementPtr>::GetState() const
{
	return ElementSelectorState{mFirstVisibleIndx, mSelectedIndx};
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
void ElementSelector<ElementPtr>::RestoreState(const ElementSelectorState& state)
{
	mFirstVisibleIndx = state.firstVisibleElementIndx;
	mSelectedIndx = state.selectedElementIndx;
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
std::vector<std::size_t> ElementSelector<ElementPtr>::GetLastingSelectedElementsIndices()
	const
{
	std::vector<std::size_t> lastingSelectedIndices;
	for (auto& indx : mLastingSelectIndxs)
	{
		assert(("Bad indx!", indx >= 0 && indx < mFiles.size()));
		lastingSelectedIndices.push_back(indx);
	}
	return lastingSelectedIndices;
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
std::vector<ElementPtr> ElementSelector<ElementPtr>::GetLastingSelectedElements() const
{
	std::vector<ElementPtr> lastingSelected;
	for (auto& indx : mLastingSelectIndxs)
	{
		assert(("Bad indx!", indx >= 0 && indx < mFiles.size()));
		lastingSelected.push_back(mFiles[indx]);
	}
	return lastingSelected;
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
std::vector<ElementPtr> ElementSelector<ElementPtr>::GetElements() const
{
	return mFiles;
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
std::size_t ElementSelector<ElementPtr>::GetSelectedPosition() const
{
	return mSelectedIndx;
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
std::size_t ElementSelector<ElementPtr>::GetElementsSize() const
{
	return mFiles.size();
}

template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
const std::optional<ElementPtr> ElementSelector<ElementPtr>::GetSelected() const
{
	if (mSelectedIndx < mFiles.size())
	{
		return mFiles[mSelectedIndx];
	}

	return std::nullopt;
}
