#pragma once

#include "DatabaseIdentifiers.hpp"
#include "FileManager.hpp"
#include "Primitives.hpp"
#include "Utility.hpp"

#include <concepts>
#include <memory>
#include <optional>
#include <random>
#include <unordered_set>

/**
 * @brief The ElementSelectorState - struct to store element selector state@n
 *
 * - It's needed to be able to save & restore Element Selector state
 */
struct ElementSelectorState
{
	ElementSelectorState(size_t firstVisible, size_t selected);

	size_t firstVisibleElementIndx;
	size_t selectedElementIndx;
};

template <typename Element>
concept PtrAndNameIsString = requires(Element el) {
								 {
									 el->name + "a"
									 } -> std::same_as<std::string>;
							 };
template <typename Element>
concept NameIsString = std::is_same<decltype(Element::name), std::string>::value;

template <typename Element>
concept ElementIsDBPlaylistPtr = std::is_same<Element, DBPlaylist_Ptr>::value;

template <typename Element>
concept ElementIsFilePtr = std::is_same<Element, File_Ptr>::value;

/**
 * @brief The ElementSelector class - class to select & draw list of elements
 *
 * - ElementPtr has concept restrictions: It should have an std::string member name@n
 * - For now ElementPtr is File_Ptr or DBPlaylist_Ptr@n
 * - Scrollable@n
 * - Able to select elements: one at a time like observing whats in the list@n
 * - Able to long select elements: can be multiple at a time, like for some further actions@n
 * - It's methods defined in ElementSelector.tpp file@n
 */
template <typename ElementPtr>
	requires PtrAndNameIsString<ElementPtr>
class ElementSelector
{
public:
	template <typename Element>
		requires NameIsString<Element>
	static std::vector<ElementPtr> CrutchConverter(std::vector<Element> elements);

public:
	ElementSelector(const Recti& bounds);

	void Load(std::vector<ElementPtr> element, bool overwrite);
	void Load(ElementPtr element, bool overwrite);

	void Update();
	void HandleResize(const Recti& bounds);

	void SelectUp();
	void SelectDown();
	void SelectByIndx(std::size_t indx);
	void SelectByName(const std::string& filename);

	void ClearAllLastingSelections();
	void LastingSelectOnCurrent();

	ElementSelectorState GetState() const;
	void				 RestoreState(const ElementSelectorState& state);

	std::vector<std::size_t> GetLastingSelectedElementsIndices() const;
	std::vector<ElementPtr>	 GetLastingSelectedElements() const;
	std::vector<ElementPtr>	 GetElements() const;

	const std::optional<ElementPtr> GetSelected() const;
	std::size_t						GetSelectedPosition() const;
	std::size_t						GetElementsSize() const;

	/**
	 * @brief Draw method defined twice:
	 * this one is for DBPlaylistPtr. Required in NewElementsDialogBundle, to have a list of Database "folders".
	 */
	template <typename U = ElementPtr>
		requires ElementIsDBPlaylistPtr<U>
	void Draw() const
	{
		int x = mBounds.left + 1;
		int y = mBounds.top;
		int xSpace = mBounds.width - 1;

		int iBound =
			std::min((int) mFirstVisibleIndx + mBounds.height, (int) mFiles.size());
		for (int i = mFirstVisibleIndx; i < iBound; ++i)
		{
			if (i == mSelectedIndx)
			{
				attron(A_REVERSE);
			}
			if (mLastingSelectIndxs.contains(i))
			{
				attron(COLOR_PAIR(3));
			}

			PrintwInLimitedX(Vector2i{x, y}, mFiles[i]->name, x + xSpace);
			attroff(A_REVERSE | COLOR_PAIR(0));
			++y;

			attroff(COLOR_PAIR(2));
		}

		if (mFiles.empty())
		{
			printwMsgBox(
				"Wow such empty, Can you pleeeeease open another directory?", mBounds);
		}
	}

	/**
		 * @brief Draw method defined twice:
		 * this one is for all other elements (File_Ptr for now all the other elemenrs)
		 */
	template <typename U = ElementPtr>
		requires ElementIsFilePtr<U>
	void Draw() const
	{
		int x = mBounds.left + 1;
		int y = mBounds.top;
		int xSpace = mBounds.width - 1;

		int iBound =
			std::min((int) mFirstVisibleIndx + mBounds.height, (int) mFiles.size());
		for (int i = mFirstVisibleIndx; i < iBound; ++i)
		{
			if (i == mSelectedIndx)
			{
				attron(A_REVERSE);
			}
			if (mFiles[i]->type == File::Type::Directory)
			{
				attron(COLOR_PAIR(2));
			}
			if (mLastingSelectIndxs.contains(i))
			{
				attron(COLOR_PAIR(3));
			}

			PrintwInLimitedX(Vector2i{x, y}, mFiles[i]->name, x + xSpace);
			attroff(A_REVERSE | COLOR_PAIR(0));
			++y;

			attroff(COLOR_PAIR(2));
		}

		if (mFiles.empty())
		{
			printwMsgBox(
				"Wow such empty, Can you pleeeeease open another directory?", mBounds);
		}
	}

	void ShuffleElements();

	/**
	 * @brief SortDirsFirst - defined for File_Ptr to sort dirs first.
	 */
	template <typename U = ElementPtr,
		std::enable_if_t<std::is_same<U, File_Ptr>::value, bool> = true>
	void SortDirsFirst()
	{
		if (mFiles.empty())
			return;

		bool	 parentDirExist = false;
		File_Ptr parent_dir = nullptr;
		for (auto iter{begin(mFiles)}; iter != end(mFiles); ++iter)
		{
			if ((*iter)->name == "..")
			{
				parent_dir = *iter;
				parentDirExist = true;
				mFiles.erase(iter);
				break;
			}
		}
		// parent_dir goes first
		std::vector<File_Ptr> sortedFiles;
		if (parentDirExist)
			sortedFiles.push_back(parent_dir);

		for (auto& file : mFiles)
		{
			if (file->type == File::Type::Directory)
			{
				sortedFiles.push_back(file);
			}
		}

		for (auto& file : mFiles)
		{
			if (file->type == File::Type::Directory)
				continue;

			sortedFiles.push_back(std::move(file));
		}
		mFiles = std::move(sortedFiles);
	}

private:
	Recti					   mBounds;
	std::size_t				   mSelectedIndx;
	std::size_t				   mFirstVisibleIndx;
	std::vector<ElementPtr>	   mFiles;
	std::unordered_set<size_t> mLastingSelectIndxs;
	std::default_random_engine mShuffleRandomEngine;
};

#include "ElementSelector.tpp"

// Don't know where it will be better to place it, for now will place it there
using FileSelector = ElementSelector<File_Ptr>;
