/**
 * @file   stringize.h
 * @author Nat Goodspeed
 * @date   2008-12-17
 * @brief  stringize(item) template function and STRINGIZE(expression) macro
 * 
 * $LicenseInfo:firstyear=2008&license=viewergpl$
 * 
 * Copyright (c) 2008-2009, Linden Research, Inc.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#if ! defined(LL_STRINGIZE_H)
#define LL_STRINGIZE_H

#include <sstream>

/**
 * stringize(item) encapsulates an idiom we use constantly, using
 * operator<<(std::ostringstream&, TYPE) followed by std::ostringstream::str()
 * to render a string expressing some item.
 */
template <typename T>
std::string stringize(const T& item)
{
    std::ostringstream out;
    out << item;
    return out.str();
}

/**
 * STRINGIZE(item1 << item2 << item3 ...) effectively expands to the
 * following:
 * @code
 * std::ostringstream out;
 * out << item1 << item2 << item3 ... ;
 * return out.str();
 * @endcode
 */
#define STRINGIZE(EXPRESSION) (static_cast<std::ostringstream&>(Stringize() << EXPRESSION).str())

/**
 * Helper class for STRINGIZE() macro. Ideally the body of
 * STRINGIZE(EXPRESSION) would look something like this:
 * @code
 * (std::ostringstream() << EXPRESSION).str()
 * @endcode
 * That doesn't work because each of the relevant operator<<() functions
 * accepts a non-const std::ostream&, to which you can't pass a temp instance
 * of std::ostringstream. Stringize plays the necessary const tricks to make
 * the whole thing work.
 */
class Stringize
{
public:
    /**
     * This is the essence of Stringize. The leftmost << operator (the one
     * coded in the STRINGIZE() macro) engages this operator<<() const method
     * on the temp Stringize instance. Every other << operator (ones embedded
     * in EXPRESSION) simply sees the std::ostream& returned by the first one.
     *
     * Finally, the STRINGIZE() macro downcasts that std::ostream& to
     * std::ostringstream&.
     */
    template <typename T>
    std::ostream& operator<<(const T& item) const
    {
        mOut << item;
        return mOut;
    }

private:
    mutable std::ostringstream mOut;
};

#endif /* ! defined(LL_STRINGIZE_H) */
