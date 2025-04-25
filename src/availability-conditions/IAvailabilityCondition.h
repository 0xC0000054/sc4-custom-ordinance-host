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

#pragma once
#include "cIGZIStream.h"
#include "cIGZOStream.h"

class IAvailabilityCondition
{
public:
	enum class Type : uint32_t
	{
		// These values must not be renumbered because
		// they are serialized in the save game data.

		GameYear = 0,
		BuildingCount = 1,
		RCIGroupPopulation = 2,
		LuaFunction = 3,
	};

	virtual bool CheckCondition() const = 0;
	virtual Type GetType() const = 0;

	virtual bool Read(cIGZIStream& stream) = 0;
	virtual bool Write(cIGZOStream& stream) const = 0;
};