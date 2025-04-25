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
#include "BuildingType.h"

class cIGZIStream;
class cIGZOStream;
class cGZPersistResourceKey;
struct StringResourceKey;

namespace GZStreamUtil
{
	bool ReadBool(cIGZIStream& stream, bool& value);
	bool WriteBool(cIGZOStream& stream, bool value);

	bool ReadBuildingType(cIGZIStream& stream, BuildingType& value);
	bool WriteBuildingType(cIGZOStream& stream, BuildingType value);

	bool ReadResKey(cIGZIStream& stream, cGZPersistResourceKey& value);
	bool WriteResKey(cIGZOStream& stream, const cGZPersistResourceKey& value);

	bool ReadStringResourceKey(cIGZIStream& stream, StringResourceKey& value);
	bool WriteStringResourceKey(cIGZOStream& stream, const StringResourceKey& value);
}