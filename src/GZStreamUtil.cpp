/*
 * This file is part of SC4CustomOrdinanceHost, a DLL Plugin for SimCity 4
 * that allows new ordinances to be defined in exemplars.
 *
 * Copyright (C) 2025 Nicholas Hayes
 *
 * SC4CustomOrdinanceHost is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * SC4CustomOrdinanceHost is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with SC4CustomOrdinanceHost.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "GZStreamUtil.h"
#include "cGZPersistResourceKey.h"
#include "cIGZIStream.h"
#include "cIGZOStream.h"
#include "cISC4DBSegmentIStream.h"
#include "cISC4DBSegmentOStream.h"
#include "cRZAutoRefCount.h"
#include "StringResourceKey.h"

bool GZStreamUtil::ReadBool(cIGZIStream& stream, bool& value)
{
	uint8_t temp = 0;
	// We use GetVoid because GetUint8 always returns false.
	if (!stream.GetVoid(&temp, 1))
	{
		return false;
	}

	value = temp != 0;
	return true;
}

bool GZStreamUtil::WriteBool(cIGZOStream& stream, bool value)
{
	const uint8_t uint8Value = static_cast<uint8_t>(value);

	return stream.SetVoid(&uint8Value, 1);
}

bool GZStreamUtil::ReadBuildingType(cIGZIStream& stream, BuildingType& value)
{
	uint32_t temp = 0;
	if (!stream.GetUint32(temp))
	{
		return false;
	}

	value = static_cast<BuildingType>(temp);
	return true;
}

bool GZStreamUtil::WriteBuildingType(cIGZOStream& stream, BuildingType value)
{
	return stream.SetUint32(static_cast<uint32_t>(value));
}

bool GZStreamUtil::ReadResKey(cIGZIStream& stream, cGZPersistResourceKey& value)
{
	return stream.GetUint32(value.type)
		&& stream.GetUint32(value.group)
		&& stream.GetUint32(value.instance);
}

bool GZStreamUtil::WriteResKey(cIGZOStream& stream, const cGZPersistResourceKey& value)
{
	return stream.SetUint32(value.type)
		&& stream.SetUint32(value.group)
		&& stream.SetUint32(value.instance);
}

bool GZStreamUtil::ReadStringResourceKey(cIGZIStream& stream, StringResourceKey& value)
{
	return stream.GetUint32(value.groupID)
		&& stream.GetUint32(value.instanceID);
}

bool GZStreamUtil::WriteStringResourceKey(cIGZOStream& stream, const StringResourceKey& value)
{
	return stream.SetUint32(value.groupID)
		&& stream.SetUint32(value.instanceID);
}
