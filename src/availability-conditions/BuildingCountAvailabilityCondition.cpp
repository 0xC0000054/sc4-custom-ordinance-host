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

#include "BuildingCountAvailabilityCondition.h"
#include "BuildingCountProvider.h"
#include "GZStreamUtil.h"

BuildingCountAvailabilityCondition::BuildingCountAvailabilityCondition()
	: type(BuildingType::FireStation), minBuildingCount(0)
{
}

BuildingCountAvailabilityCondition::BuildingCountAvailabilityCondition(
	BuildingType type, uint32_t minBuildingCount)
	: type(type), minBuildingCount(minBuildingCount)
{
}

bool BuildingCountAvailabilityCondition::CheckCondition() const
{
	return BuildingCountProvider::GetBuildingCount(type) >= minBuildingCount;
}

IAvailabilityCondition::Type BuildingCountAvailabilityCondition::GetType() const
{
	return IAvailabilityCondition::Type::BuildingCount;
}

bool BuildingCountAvailabilityCondition::Read(cIGZIStream& stream)
{
	uint32_t version = 0;

	if (!stream.GetUint32(version) || version != 1)
	{
		return false;
	}

	if (!GZStreamUtil::ReadBuildingType(stream, type))
	{
		return false;
	}

	if (!stream.GetUint32(minBuildingCount))
	{
		return false;
	}

	return true;
}

bool BuildingCountAvailabilityCondition::Write(cIGZOStream& stream) const
{
	if (!stream.SetUint32(1)) // version
	{
		return false;
	}

	if (!GZStreamUtil::WriteBuildingType(stream, type))
	{
		return false;
	}

	if (!stream.SetUint32(minBuildingCount))
	{
		return false;
	}

	return true;
}
