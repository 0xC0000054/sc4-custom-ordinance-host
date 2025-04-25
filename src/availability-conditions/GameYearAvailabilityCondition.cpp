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

#include "GameYearAvailabilityCondition.h"
#include "cIGZDate.h"
#include "cISC4Simulator.h"
#include "GlobalPointers.h"

GameYearAvailabilityCondition::GameYearAvailabilityCondition()
	: yearFirstAvailable(0)
{
}

GameYearAvailabilityCondition::GameYearAvailabilityCondition(uint32_t yearFirstAvailable)
	: yearFirstAvailable(yearFirstAvailable)
{
}

bool GameYearAvailabilityCondition::CheckCondition() const
{
	bool result = false;

	if (spSimulator)
	{
		uint32_t currentYear = 0;

		spSimulator->GetSimDate(&currentYear, nullptr, nullptr, nullptr, nullptr);

		result = currentYear >= yearFirstAvailable;
	}

	return result;
}

IAvailabilityCondition::Type GameYearAvailabilityCondition::GetType() const
{
	return IAvailabilityCondition::Type::GameYear;
}

bool GameYearAvailabilityCondition::Read(cIGZIStream& stream)
{
	uint32_t version = 0;

	if (!stream.GetUint32(version) || version != 1)
	{
		return false;
	}

	if (!stream.GetUint32(yearFirstAvailable))
	{
		return false;
	}

	return true;
}

bool GameYearAvailabilityCondition::Write(cIGZOStream& stream) const
{
	if (!stream.SetUint32(1)) // version
	{
		return false;
	}

	if (!stream.SetUint32(yearFirstAvailable))
	{
		return false;
	}

	return true;
}
