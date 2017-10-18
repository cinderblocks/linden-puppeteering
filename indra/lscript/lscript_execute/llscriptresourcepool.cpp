/** 
 * @file llscriptresourcepool.cpp
 * @brief Collection of limited script resources
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

#include "llscriptresourcepool.h"
#include "llscriptresourceconsumer.h"
#include "lloptionreader.h"

LLScriptResourcePool LLScriptResourcePool::null(NULL);

LLScriptResourcePool::LLScriptResourcePool(LLOptionReader* options_reader)
{ 
	mResources[SCRIPT_RESOURCE_URL].isLimitEnforced(true);

	bool enforce_memory_limits = false;
	if(options_reader)
	{
		enforce_memory_limits = options_reader->getOption("script_memory_limits_enforced", LLSD(false)).asBoolean();
	}
	
	mResources[SCRIPT_RESOURCE_MEMORY].isLimitEnforced(enforce_memory_limits);
}
bool LLScriptResourcePool::isInPool(const LLScriptResourceConsumer& consumer) const
{
	return &consumer.getScriptResourcePool() == this;
}

bool LLScriptResourcePool::switchToPool(LLScriptResourceConsumer& consumer)
{
	// Return true if we're already in this pool
	if (canSwitchToPool(consumer))
	{
		LLScriptResourcePool& pool = consumer.getScriptResourcePool();

		for (U32 i = 0; i < SCRIPT_RESOURCE_COUNT; ++i)
		{
			LLScriptResourceType e = (LLScriptResourceType)i;
			pool.release(e, consumer.getRequiredResource(e));
		}
		consumer.setScriptResourcePool(*this);
		for (U32 i = 0; i < SCRIPT_RESOURCE_COUNT; ++i)
		{
			LLScriptResourceType e = (LLScriptResourceType)i;
			request(e, consumer.getRequiredResource(e));
		}

		return true;
	}

	return false;
}

bool LLScriptResourcePool::canSwitchToPool(const LLScriptResourceConsumer& consumer) const
{
	// Return true if we're already in this pool
	if (isInPool(consumer))
	{
		return true;
	}
	
 	for (U32 i = 0; i < SCRIPT_RESOURCE_COUNT; ++i)
 	{
		LLScriptResourceType e = (LLScriptResourceType)i;
		if (mResources[i].isLimitEnforced())
 		{
			if (consumer.getRequiredResource(e) >
				mResources[i].getAvailable())
			{
				return false;
			}
 		}
 	}

	return true;
}

void LLScriptResourcePool::setAvailable(LLScriptResourceType type, S32 amount)
{
	mResources[type].setTotal(amount);
}


bool LLScriptResourcePool::request(LLScriptResourceType type, S32 amount)
{
	return mResources[type].request(amount);
}

void LLScriptResourcePool::release(LLScriptResourceType type, S32 amount)
{
	mResources[type].release(amount);
}

const LLScriptResource& LLScriptResourcePool::getResource(LLScriptResourceType type) const 
{ 
	return mResources[type]; 
}
