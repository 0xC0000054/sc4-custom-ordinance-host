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

#include "RCIGroupPopulationAvailabilityCondition.h"
#include "PopulationProvider.h"

RCIGroupPopulationAvailabilityCondition::RCIGroupPopulationAvailabilityCondition()
	: demandID(0), minPopulation(0)
{
}

RCIGroupPopulationAvailabilityCondition::RCIGroupPopulationAvailabilityCondition(
	RCIGroup group,
	int32_t minPopulation)
	: demandID(static_cast<uint32_t>(group)), minPopulation(minPopulation)
{
}

bool RCIGroupPopulationAvailabilityCondition::CheckCondition() const
{
	return PopulationProvider::GetRCIGroupPopulation(demandID) >= minPopulation;
}

IAvailabilityCondition::Type RCIGroupPopulationAvailabilityCondition::GetType() const
{
	return IAvailabilityCondition::Type::RCIGroupPopulation;
}

bool RCIGroupPopulationAvailabilityCondition::Read(cIGZIStream& stream)
{
	uint32_t version = 0;

	if (!stream.GetUint32(version) || version != 1)
	{
		return false;
	}

	if (!stream.GetUint32(demandID))
	{
		return false;
	}

	if (!stream.GetSint32(minPopulation))
	{
		return false;
	}

	return true;
}

bool RCIGroupPopulationAvailabilityCondition::Write(cIGZOStream& stream) const
{
	if (!stream.SetUint32(1)) // version
	{
		return false;
	}

	if (!stream.SetUint32(demandID))
	{
		return false;
	}

	if (!stream.SetSint32(minPopulation))
	{
		return false;
	}

	return true;
}
