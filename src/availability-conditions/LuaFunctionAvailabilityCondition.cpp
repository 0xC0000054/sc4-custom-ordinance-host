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

#include "LuaFunctionAvailabilityCondition.h"
#include "cISCLua.h"
#include "GlobalPointers.h"
#include "Logger.h"

LuaFunctionAvailabilityCondition::LuaFunctionAvailabilityCondition()
	: functionName()
{
}

LuaFunctionAvailabilityCondition::LuaFunctionAvailabilityCondition(const cRZBaseString& name)
	: functionName(name)
{
}

bool LuaFunctionAvailabilityCondition::CheckCondition() const
{
	bool result = false;

	if (spLua)
	{
		int32_t top = spLua->GetTop();
		spLua->GetGlobal(functionName.ToChar());

		if (spLua->GetTop() != top)
		{
			cIGZLua5Thread::LuaType type = spLua->Type(-1);

			if (type == cIGZLua5Thread::LuaTypeFunction)
			{
				// The Lua function we are calling has the following C++ signature:
				// bool function_name()
				//
				// The calling sequence is:
				// 1. Call the function.
				// 2. Read the returned Boolean off the Lua stack.

				int32_t status = spLua->CallProtected(0, 1);

				if (status == 0)
				{
					if (spLua->IsBoolean(-1))
					{
						result = spLua->ToBoolean(-1);
					}
					else
					{
						LogLuaFunctionCallError("The function return value must be a Boolean (true or false).");
					}
				}
				else
				{
					LogLuaFunctionCallError("The game returned an error when calling the function.");
				}
			}
			else
			{
				LogLuaFunctionCallError("The function has the wrong type.");
			}

			spLua->SetTop(top);
		}
		else
		{
			LogLuaFunctionCallError("The function was not found.");
		}
	}

	return result;
}

IAvailabilityCondition::Type LuaFunctionAvailabilityCondition::GetType() const
{
	return IAvailabilityCondition::Type::LuaFunction;
}

bool LuaFunctionAvailabilityCondition::Read(cIGZIStream& stream)
{
	uint32_t version = 0;

	if (!stream.GetUint32(version) || version != 1)
	{
		return false;
	}

	if (!stream.GetGZStr(functionName))
	{
		return false;
	}

	return true;
}

bool LuaFunctionAvailabilityCondition::Write(cIGZOStream& stream) const
{
	if (!stream.SetUint32(1)) // version
	{
		return false;
	}

	if (!stream.SetGZStr(functionName))
	{
		return false;
	}

	return true;
}

void LuaFunctionAvailabilityCondition::LogLuaFunctionCallError(const char* message) const
{
	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Error,
		"Error calling Lua function '%s': %s",
		functionName.ToChar(),
		message);
}
