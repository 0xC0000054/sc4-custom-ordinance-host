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

#include "LuaFunctionIncomeFactor.h"
#include "cISCLua.h"
#include "GlobalPointers.h"
#include "Logger.h"

LuaFunctionIncomeFactor::LuaFunctionIncomeFactor()
	: functionName()
{
}

LuaFunctionIncomeFactor::LuaFunctionIncomeFactor(const cRZBaseString& name)
	: functionName(name)
{
}

IMonthlyIncomeFactor::Type LuaFunctionIncomeFactor::GetType() const
{
	return IMonthlyIncomeFactor::Type::LuaFunction;
}

double LuaFunctionIncomeFactor::Calculate(double monthlyIncome) const
{
	double result = monthlyIncome;

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
				// double function_name(double monthlyIncome)
				//
				// The calling sequence is:
				// 1. Push the parameter onto the Lua stack.
				// 2. Call the function.
				// 3. Read the returned number off the Lua stack.

				spLua->PushNumber(monthlyIncome);
				int32_t status = spLua->CallProtected(1, 1);

				if (status == 0)
				{
					if (spLua->IsNumber(-1))
					{
						result = spLua->ToNumber(-1);
					}
					else
					{
						LogLuaFunctionCallError("The function return value must be a number.");
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

bool LuaFunctionIncomeFactor::Read(cIGZIStream& stream)
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

bool LuaFunctionIncomeFactor::Write(cIGZOStream& stream) const
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

void LuaFunctionIncomeFactor::LogLuaFunctionCallError(const char* message) const
{
	Logger::GetInstance().WriteLineFormatted(
		LogLevel::Error,
		"Error calling Lua function '%s': %s",
		functionName.ToChar(),
		message);
}
