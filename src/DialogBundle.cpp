#include "DialogBundle.hpp"

#include <cassert>

DialogBundle::DialogBundle(Context context, const Recti& bounds)
	: mBounds{bounds}
	, mContext{context}
{
}
