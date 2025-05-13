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

#include "CustomOrdinance.h"
#include "cIGZIStream.h"
#include "cIGZOStream.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZVariant.h"
#include "cISCResExemplar.h"
#include "cISCProperty.h"
#include "cRZAutoRefCount.h"
#include "GlobalPointers.h"
#include "GZStreamUtil.h"
#include "Logger.h"
#include "OrdiancePropertyIDs.h"
#include "SCPropertyUtil.h"
#include "SC4Percentage.h"
#include "SafeInt.hpp"
#include "StringResourceManager.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <utility>

#include "BuildingCountAvailabilityCondition.h"
#include "GameYearAvailabilityCondition.h"
#include "LuaFunctionAvailabilityCondition.h"
#include "RCIGroupPopulationAvailabilityCondition.h"

#include "BuildingCountIncomeFactor.h"
#include "LuaFunctionIncomeFactor.h"
#include "RCIGroupPopulationIncomeFactor.h"
#include "TotalResidentialPopulationIncomeFactor.h"

static constexpr std::array<std::pair<uint32_t, BuildingType>, 5> BuildingCountAvailabilityConditions =
{
	std::pair(kOrdinanceAvailabilityMinFireStationCount, BuildingType::FireStation),
	std::pair(kOrdinanceAvailabilityMinHospitalCount, BuildingType::Hospital),
	std::pair(kOrdinanceAvailabilityMinJailCount, BuildingType::Jail),
	std::pair(kOrdinanceAvailabilityMinPoliceStationCount, BuildingType::PoliceStation),
	std::pair(kOrdinanceAvailabilityMinSchoolBuildingCount, BuildingType::School),
};

static constexpr std::array<std::pair<uint32_t, RCIGroup>, 12> RCIGroupMinPopulationAvailabilityConditions =
{
	std::pair(kOrdinanceAvailabilityMinPopulationResLowWealth, RCIGroup::Res1),
	std::pair(kOrdinanceAvailabilityMinPopulationResMediumWealth, RCIGroup::Res2),
	std::pair(kOrdinanceAvailabilityMinPopulationResHighWealth, RCIGroup::Res3),
	std::pair(kOrdinanceAvailabilityMinPopulationCsLowWealth, RCIGroup::Cs1),
	std::pair(kOrdinanceAvailabilityMinPopulationCsMediumWealth, RCIGroup::Cs2),
	std::pair(kOrdinanceAvailabilityMinPopulationCsHighWealth, RCIGroup::Cs3),
	std::pair(kOrdinanceAvailabilityMinPopulationCoMediumWealth, RCIGroup::Co2),
	std::pair(kOrdinanceAvailabilityMinPopulationCoHighWealth, RCIGroup::Co3),
	std::pair(kOrdinanceAvailabilityMinPopulationIR, RCIGroup::IR),
	std::pair(kOrdinanceAvailabilityMinPopulationID, RCIGroup::ID),
	std::pair(kOrdinanceAvailabilityMinPopulationIM, RCIGroup::IM),
	std::pair(kOrdinanceAvailabilityMinPopulationIHT, RCIGroup::IHT),
};

static constexpr std::array<std::pair<uint32_t, RCIGroup>, 3> ResWealthGroupMonthlyIncomeFactors =
{
	std::pair(kOrdinanceMonthlyIncomeFactorResLowWealthPopulation, RCIGroup::Res1),
	std::pair(kOrdinanceMonthlyIncomeFactorResMediumWealthPopulation, RCIGroup::Res2),
	std::pair(kOrdinanceMonthlyIncomeFactorResHighWealthPopulation, RCIGroup::Res3),
};

static constexpr std::array<std::pair<uint32_t, RCIGroup>, 3> CsWealthGroupMonthlyIncomeFactors =
{
	std::pair(kOrdinanceMonthlyIncomeFactorCsLowWealthPopulation, RCIGroup::Cs1),
	std::pair(kOrdinanceMonthlyIncomeFactorCsMediumWealthPopulation, RCIGroup::Cs2),
	std::pair(kOrdinanceMonthlyIncomeFactorCsHighWealthPopulation, RCIGroup::Cs3),
};

static constexpr std::array<std::pair<uint32_t, RCIGroup>, 2> CoWealthGroupMonthlyIncomeFactors =
{
	std::pair(kOrdinanceMonthlyIncomeFactorCoMediumWealthPopulation, RCIGroup::Co2),
	std::pair(kOrdinanceMonthlyIncomeFactorCoHighWealthPopulation, RCIGroup::Co3),
};

static constexpr std::array<std::pair<uint32_t, RCIGroup>, 4> IndustrialMonthlyIncomeFactors =
{
	std::pair(kOrdinanceMonthlyIncomeFactorIRPopulation, RCIGroup::IR),
	std::pair(kOrdinanceMonthlyIncomeFactorIDPopulation, RCIGroup::ID),
	std::pair(kOrdinanceMonthlyIncomeFactorIMPopulation, RCIGroup::IM),
	std::pair(kOrdinanceMonthlyIncomeFactorIHTPopulation, RCIGroup::IHT),
};

static constexpr std::array<std::pair<uint32_t, BuildingType>, 5> BuildingCountMonthlyIncomeFactors =
{
	std::pair(kOrdinanceMonthlyIncomeFactorFireStationCount, BuildingType::FireStation),
	std::pair(kOrdinanceMonthlyIncomeFactorHospitalCount, BuildingType::Hospital),
	std::pair(kOrdinanceMonthlyIncomeFactorJailCount, BuildingType::Jail),
	std::pair(kOrdinanceMonthlyIncomeFactorPoliceStationCount, BuildingType::PoliceStation),
	std::pair(kOrdinanceMonthlyIncomeFactorSchoolBuildingCount, BuildingType::School),
};

namespace
{
	bool ReadAvailabilityConditions(cIGZIStream& stream, std::vector<std::unique_ptr<IAvailabilityCondition>>& vector)
	{
		vector.clear();

		uint32_t count = 0;

		if (!stream.GetUint32(count))
		{
			return false;
		}

		if (count > 0)
		{
			vector.reserve(count);

			for (uint32_t i = 0; i < count; i++)
			{
				uint32_t type = 0;

				if (!stream.GetUint32(type))
				{
					return false;
				}

				std::unique_ptr<IAvailabilityCondition> item;

				switch (static_cast<IAvailabilityCondition::Type>(type))
				{
				case IAvailabilityCondition::Type::BuildingCount:
					item = std::make_unique<BuildingCountAvailabilityCondition>();
					break;
				case IAvailabilityCondition::Type::GameYear:
					item = std::make_unique<GameYearAvailabilityCondition>();
					break;
				case IAvailabilityCondition::Type::LuaFunction:
					item = std::make_unique<LuaFunctionAvailabilityCondition>();
					break;
				case IAvailabilityCondition::Type::RCIGroupPopulation:
					item = std::make_unique<RCIGroupPopulationAvailabilityCondition>();
					break;
				default:
					assert(false);
					break;
				}

				if (item && item->Read(stream))
				{
					vector.push_back(std::move(item));
				}
				else
				{
					return false;
				}
			}
		}

		return true;
	}

	bool ReadMonthlyIncomeFactors(cIGZIStream& stream, std::vector<std::unique_ptr<IMonthlyIncomeFactor>>& vector)
	{
		vector.clear();

		uint32_t count = 0;

		if (!stream.GetUint32(count))
		{
			return false;
		}

		if (count > 0)
		{
			vector.reserve(count);

			for (uint32_t i = 0; i < count; i++)
			{
				uint32_t type = 0;

				if (!stream.GetUint32(type))
				{
					return false;
				}

				std::unique_ptr<IMonthlyIncomeFactor> item;

				switch (static_cast<IMonthlyIncomeFactor::Type>(type))
				{
				case IMonthlyIncomeFactor::Type::BuildingCount:
					item = std::make_unique<BuildingCountIncomeFactor>();
					break;
				case IMonthlyIncomeFactor::Type::LuaFunction:
					item = std::make_unique<LuaFunctionIncomeFactor>();
					break;
				case IMonthlyIncomeFactor::Type::RCIGroupPopulation:
					item = std::make_unique<RCIGroupPopulationIncomeFactor>();
					break;
				case IMonthlyIncomeFactor::Type::TotalResidentialPop:
					item = std::make_unique<TotalResidentialPopulationIncomeFactor>();
					break;
				default:
					assert(false);
					break;
				}

				if (item && item->Read(stream))
				{
					vector.push_back(std::move(item));
				}
				else
				{
					return false;
				}
			}
		}

		return true;
	}

	bool WriteAvailabilityConditions(cIGZOStream& stream, const std::vector<std::unique_ptr<IAvailabilityCondition>>& vector)
	{
		const uint32_t count = vector.size();

		if (!stream.SetUint32(count))
		{
			return false;
		}

		if (count > 0)
		{
			for (const auto& item : vector)
			{
				if (!stream.SetUint32(static_cast<uint32_t>(item->GetType())))
				{
					return false;
				}

				if (!item->Write(stream))
				{
					return false;
				}
			}
		}

		return true;
	}

	bool WriteMonthlyIncomeFactors(cIGZOStream& stream, const std::vector<std::unique_ptr<IMonthlyIncomeFactor>>& vector)
	{
		const uint32_t count = vector.size();

		if (!stream.SetUint32(count))
		{
			return false;
		}

		if (count > 0)
		{
			for (const auto& item : vector)
			{
				if (!stream.SetUint32(static_cast<uint32_t>(item->GetType())))
				{
					return false;
				}

				if (!item->Write(stream))
				{
					return false;
				}
			}
		}

		return true;
	}
}

CustomOrdinance::CustomOrdinance(const cGZPersistResourceKey& exemplarKey)
	: ordinanceExemplarKey(exemplarKey),
	  availabilityConditions(),
	  monthlyIncomeFactors(),
	  name(),
	  nameKey(),
	  description(),
	  descriptionKey(),
	  enactmentIncome(0),
	  retracmentIncome(0),
	  monthlyConstantIncome(0),
	  isIncomeOrdinance(false),
	  monthlyAdjustedIncome(0),
	  available(false),
	  on(false),
	  enabled(false),
	  haveDeserialized(false),
	  miscProperties()
{
}

bool CustomOrdinance::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cISC4Ordinance)
	{
		*ppvObj = static_cast<cISC4Ordinance*>(this);
		AddRef();

		return true;
	}
	else if (riid == GZIID_cIGZSerializable)
	{
		*ppvObj = static_cast<cIGZSerializable*>(this);
		AddRef();

		return true;
	}
	else if (riid == GZIID_cISC4OrdinanceSimple)
	{
		*ppvObj = static_cast<cISC4OrdinanceSimple*>(this);
		AddRef();

		return true;
	}

	return cRZBaseUnknown::QueryInterface(riid, ppvObj);
}

uint32_t CustomOrdinance::AddRef()
{
	return cRZBaseUnknown::AddRef();
}

uint32_t CustomOrdinance::Release()
{
	return cRZBaseUnknown::Release();
}

bool CustomOrdinance::Init(void)
{
	if (!haveDeserialized)
	{
		enabled = true;
		InitFromExemplarData();
	}

	return true;
}

bool CustomOrdinance::Shutdown(void)
{
	enabled = false;
	haveDeserialized = false;

	// Release the loaded exemplar.
	miscProperties.SetDefaultExemplar(nullptr);

	return true;
}

int64_t CustomOrdinance::GetCurrentMonthlyIncome(void)
{
	double monthlyIncome = static_cast<double>(monthlyConstantIncome);

	for (const auto& factor : monthlyIncomeFactors)
	{
		monthlyIncome = factor->Calculate(monthlyIncome);
	}

	int64_t monthlyIncomeInteger = 0;

	if (!SafeCast(monthlyIncome, monthlyIncomeInteger))
	{
		monthlyIncomeInteger = monthlyConstantIncome;

		Logger::GetInstance().WriteLineFormatted(
			LogLevel::Error,
			"Error when calculating the monthly income for '%s' (TGI 0x%08x, 0x%08x, 0x%08x), "
			"%f cannot be represented as a signed 64-bit integer. Returning the monthly constant income.",
			name.ToChar(),
			ordinanceExemplarKey.type,
			ordinanceExemplarKey.group,
			ordinanceExemplarKey.instance,
			monthlyIncome);
	}

	return monthlyIncomeInteger;
}

uint32_t CustomOrdinance::GetID(void) const
{
	return ordinanceExemplarKey.instance;
}

cIGZString* CustomOrdinance::GetName(void)
{
	return &name;
}

cIGZString* CustomOrdinance::GetDescription(void)
{
	return &description;
}

uint32_t CustomOrdinance::GetYearFirstAvailable(void)
{
	// Return a default value to any callers that ask for it.
	return 0;
}

SC4Percentage CustomOrdinance::GetChanceAvailability(void)
{
	// Return a default value to any callers that ask for it.

	SC4Percentage percentage{ 1.0f };

	return percentage;
}

int64_t CustomOrdinance::GetEnactmentIncome(void)
{
	return enactmentIncome;
}

int64_t CustomOrdinance::GetRetracmentIncome(void)
{
	return retracmentIncome;
}

int64_t CustomOrdinance::GetMonthlyConstantIncome(void)
{
	return monthlyConstantIncome;
}

float CustomOrdinance::GetMonthlyIncomeFactor(void)
{
	// Return a default value to any callers that ask for it.
	// Unlike the base game, we allow more than one monthly income factor to
	// be used at once.
	return 1.0f;
}

cISCPropertyHolder* CustomOrdinance::GetMiscProperties()
{
	return &miscProperties;
}

uint32_t CustomOrdinance::GetAdvisorID(void)
{
	return 0;
}

bool CustomOrdinance::IsAvailable(void)
{
	return available;
}

bool CustomOrdinance::IsOn(void)
{
	return available && on;
}

bool CustomOrdinance::IsEnabled(void)
{
	return enabled;
}

int64_t CustomOrdinance::GetMonthlyAdjustedIncome(void)
{
	return monthlyAdjustedIncome;
}

bool CustomOrdinance::CheckConditions(void)
{
	bool result = false;

	if (enabled)
	{
		result = true;

		for (const auto& item : availabilityConditions)
		{
			if (!item->CheckCondition())
			{
				result = false;
				break;
			}
		}
	}

	return result;
}

bool CustomOrdinance::IsIncomeOrdinance(void)
{
	return isIncomeOrdinance;
}

bool CustomOrdinance::Simulate(void)
{
	monthlyAdjustedIncome = GetCurrentMonthlyIncome();
	return true;
}

bool CustomOrdinance::SetAvailable(bool isAvailable)
{
	available = isAvailable;
	monthlyAdjustedIncome = 0;
	return true;
}

bool CustomOrdinance::SetOn(bool isOn)
{
	on = isOn;
	return true;
}

bool CustomOrdinance::SetEnabled(bool isEnabled)
{
	enabled = isEnabled;
	return true;
}

bool CustomOrdinance::ForceAvailable(bool isAvailable)
{
	return SetAvailable(isAvailable);
}

bool CustomOrdinance::ForceOn(bool isOn)
{
	return SetOn(isOn);
}

bool CustomOrdinance::ForceEnabled(bool isEnabled)
{
	return SetEnabled(isEnabled);
}

bool CustomOrdinance::ForceMonthlyAdjustedIncome(int64_t monthlyAdjustedIncome)
{
	monthlyAdjustedIncome = monthlyAdjustedIncome;
	return true;
}

bool CustomOrdinance::SetKey(const cGZPersistResourceKey& key)
{
	// This method is part of SC4's internal cISC4OrdinanceSimple interface, and
	// is used to take advantage of the ordinance exemplar loading that the game
	// performs when starting a new city.
	// That saves us a call to cISC4OrdinanceSimulator::AddOrdinance.
	//
	// The method is a no-op because we set the ordinance exemplar resource key
	// in the class constructor, which is required for the game's deserialization
	// code to work.
	// This class uses the instance id member of exemplar resource key as the
	// class id for GZCOM serialization (see GetGZCLSID).

	return true;
}

bool CustomOrdinance::Write(cIGZOStream& stream)
{
	if (stream.GetError() != 0)
	{
		return false;
	}

	const uint32_t version = 1;
	if (!stream.SetUint32(version))
	{
		return false;
	}

	if (!GZStreamUtil::WriteResKey(stream, ordinanceExemplarKey))
	{
		return false;
	}

	if (!WriteAvailabilityConditions(stream, availabilityConditions))
	{
		return false;
	}

	if (!WriteMonthlyIncomeFactors(stream, monthlyIncomeFactors))
	{
		return false;
	}

	if (!stream.SetGZStr(name))
	{
		return false;
	}

	if (!GZStreamUtil::WriteStringResourceKey(stream, nameKey))
	{
		return false;
	}

	if (!stream.SetGZStr(description))
	{
		return false;
	}

	if (!GZStreamUtil::WriteStringResourceKey(stream, descriptionKey))
	{
		return false;
	}

	if (!stream.SetSint64(enactmentIncome))
	{
		return false;
	}

	if (!stream.SetSint64(retracmentIncome))
	{
		return false;
	}

	if (!stream.SetSint64(retracmentIncome))
	{
		return false;
	}

	if (!stream.SetSint64(monthlyConstantIncome))
	{
		return false;
	}

	if (!stream.SetSint64(monthlyAdjustedIncome))
	{
		return false;
	}

	if (!GZStreamUtil::WriteBool(stream, isIncomeOrdinance))
	{
		return false;
	}

	if (!miscProperties.Write(stream))
	{
		return false;
	}

	if (!GZStreamUtil::WriteBool(stream, available))
	{
		return false;
	}

	if (!GZStreamUtil::WriteBool(stream, on))
	{
		return false;
	}

	if (!GZStreamUtil::WriteBool(stream, enabled))
	{
		return false;
	}

	return true;
}

bool CustomOrdinance::Read(cIGZIStream& stream)
{
	if (stream.GetError() != 0)
	{
		return false;
	}

	uint32_t version = 0;
	if (!stream.GetUint32(version) || version != 1)
	{
		return false;
	}

	if (!GZStreamUtil::ReadResKey(stream, ordinanceExemplarKey))
	{
		return false;
	}

	if (!ReadAvailabilityConditions(stream, availabilityConditions))
	{
		return false;
	}

	if (!ReadMonthlyIncomeFactors(stream, monthlyIncomeFactors))
	{
		return false;
	}

	if (!stream.GetGZStr(name))
	{
		return false;
	}

	if (!GZStreamUtil::ReadStringResourceKey(stream, nameKey))
	{
		return false;
	}

	if (!stream.GetGZStr(description))
	{
		return false;
	}

	if (!GZStreamUtil::ReadStringResourceKey(stream, descriptionKey))
	{
		return false;
	}

	if (!stream.GetSint64(enactmentIncome))
	{
		return false;
	}

	if (!stream.GetSint64(retracmentIncome))
	{
		return false;
	}

	if (!stream.GetSint64(retracmentIncome))
	{
		return false;
	}

	if (!stream.GetSint64(monthlyConstantIncome))
	{
		return false;
	}

	if (!stream.GetSint64(monthlyAdjustedIncome))
	{
		return false;
	}

	if (!GZStreamUtil::ReadBool(stream, isIncomeOrdinance))
	{
		return false;
	}

	if (!miscProperties.Read(stream))
	{
		return false;
	}

	if (!GZStreamUtil::ReadBool(stream, available))
	{
		return false;
	}

	if (!GZStreamUtil::ReadBool(stream, on))
	{
		return false;
	}

	if (!GZStreamUtil::ReadBool(stream, enabled))
	{
		return false;
	}

	haveDeserialized = true;
	LoadLocalizedStringResources();

	return true;
}

uint32_t CustomOrdinance::GetGZCLSID()
{
	return ordinanceExemplarKey.instance;
}

void CustomOrdinance::InitFromExemplarData()
{
	cRZAutoRefCount<cISCResExemplar> exemplar;

	if (spRM->GetResource(
		ordinanceExemplarKey,
		GZIID_cISCResExemplar,
		exemplar.AsPPVoid(),
		0,
		nullptr))
	{
		miscProperties.SetDefaultExemplar(exemplar);

		const cISCPropertyHolder* pPropertyHolder = exemplar->AsISCPropertyHolder();

		ReadCommonOrdinanceProperties(pPropertyHolder);
		ReadAvailabilityConditionProperties(pPropertyHolder);
		ReadMonthlyIncomeFactorProperties(pPropertyHolder);
		LoadLocalizedStringResources();
	}
}

void CustomOrdinance::LoadLocalizedStringResources()
{
	cRZAutoRefCount<cIGZString> localizedName;
	cRZAutoRefCount<cIGZString> localizedDescription;

	if (StringResourceManager::GetLocalizedString(nameKey, localizedName.AsPPObj()))
	{
		if (StringResourceManager::GetLocalizedString(descriptionKey, localizedDescription.AsPPObj()))
		{
			if (localizedName->Strlen() > 0 && !localizedName->IsEqual(this->name, false))
			{
				name.Copy(*localizedName);
			}

			if (localizedDescription->Strlen() > 0 && !localizedDescription->IsEqual(this->description, false))
			{
				description.Copy(*localizedDescription);
			}
		}
	}
}

void CustomOrdinance::ReadCommonOrdinanceProperties(const cISCPropertyHolder* pPropertyHolder)
{
	if (!SCPropertyUtil::GetPropertyValue(pPropertyHolder, kOrdinanceNameKey, nameKey))
	{
		if (!SCPropertyUtil::GetPropertyValue(pPropertyHolder, kOrdinanceName, name))
		{
			name.Sprintf("0x%08x", ordinanceExemplarKey.instance);
		}
	}

	if (!SCPropertyUtil::GetPropertyValue(pPropertyHolder, kOrdinanceDescriptionKey, descriptionKey))
	{
		if (!SCPropertyUtil::GetPropertyValue(pPropertyHolder, kOrdinanceDescription, description))
		{
			description.Sprintf("0x%08x", ordinanceExemplarKey.instance);
		}
	}

	SCPropertyUtil::GetPropertyValue(pPropertyHolder, kOrdinanceEnactmentIncome, enactmentIncome);
	SCPropertyUtil::GetPropertyValue(pPropertyHolder, kOrdinanceRetracmentIncome, retracmentIncome);
	SCPropertyUtil::GetPropertyValue(pPropertyHolder, kOrdinancemMonthlyConstantIncome, monthlyConstantIncome);
	SCPropertyUtil::GetPropertyValue(pPropertyHolder, kOrdinanceIsIncome, isIncomeOrdinance);
}

void CustomOrdinance::ReadAvailabilityConditionProperties(const cISCPropertyHolder* pPropertyHolder)
{
	cRZBaseString luaFunctionName;

	// The Lua function property takes precedence over all other availability condition properties.
	if (SCPropertyUtil::GetPropertyValue(pPropertyHolder, kOrdinanceAvailabilityLuaFunction, luaFunctionName)
		&& luaFunctionName.Strlen() > 0)
	{
		availabilityConditions.push_back(std::make_unique<LuaFunctionAvailabilityCondition>(luaFunctionName));
	}
	else
	{
		uint32_t yearAvailable = 0;

		if (SCPropertyUtil::GetPropertyValue(pPropertyHolder, kOrdinanceAvailabilityGameYear, yearAvailable))
		{
			constexpr uint32_t kSC4StartYear = 2000;

			if (yearAvailable > kSC4StartYear)
			{
				availabilityConditions.push_back(std::make_unique<GameYearAvailabilityCondition>(yearAvailable));
			}
		}

		for (const auto& item : BuildingCountAvailabilityConditions)
		{
			ReadMinBuildingCountAvailabilityCondition(pPropertyHolder, item.first, item.second);
		}

		for (const auto& item : RCIGroupMinPopulationAvailabilityConditions)
		{
			ReadRCIGroupMinPopulationAvailabilityCondition(pPropertyHolder, item.first, item.second);
		}
	}
}

void CustomOrdinance::ReadMonthlyIncomeFactorProperties(const cISCPropertyHolder* pPropertyHolder)
{
	cRZBaseString luaFunctionName;

	// The Lua function property takes precedence over all other monthly income factor properties.
	if (SCPropertyUtil::GetPropertyValue(pPropertyHolder, kOrdinanceMonthlyIncomeFactorLuaFunction, luaFunctionName)
		&& luaFunctionName.Strlen() > 0)
	{
		monthlyIncomeFactors.push_back(std::make_unique<LuaFunctionIncomeFactor>(luaFunctionName));
	}
	else
	{
		float resTotalPopulationIncomeFactor = 0.0f;

		if (SCPropertyUtil::GetPropertyValue(
			pPropertyHolder,
			kOrdinanceMonthlyIncomeFactorResTotalPopulation,
			resTotalPopulationIncomeFactor))
		{
			monthlyIncomeFactors.push_back(std::make_unique<TotalResidentialPopulationIncomeFactor>(resTotalPopulationIncomeFactor));
		}
		else
		{
			// The total residential population property takes precedence over the residential wealth group properties.
			for (const auto& item : ResWealthGroupMonthlyIncomeFactors)
			{
				ReadRCIGroupPopulationMonthlyIncomeFactor(
					pPropertyHolder,
					item.first,
					item.second);
			}
		}

		for (const auto& item : CsWealthGroupMonthlyIncomeFactors)
		{
			ReadRCIGroupPopulationMonthlyIncomeFactor(
				pPropertyHolder,
				item.first,
				item.second);
		}

		for (const auto& item : CoWealthGroupMonthlyIncomeFactors)
		{
			ReadRCIGroupPopulationMonthlyIncomeFactor(
				pPropertyHolder,
				item.first,
				item.second);
		}

		for (const auto& item : IndustrialMonthlyIncomeFactors)
		{
			ReadRCIGroupPopulationMonthlyIncomeFactor(
				pPropertyHolder,
				item.first,
				item.second);
		}

		for (const auto& item : BuildingCountMonthlyIncomeFactors)
		{
			ReadBuildingCountMonthlyIncomeFactor(
				pPropertyHolder,
				item.first,
				item.second);
		}
	}
}

void CustomOrdinance::ReadMinBuildingCountAvailabilityCondition(
	const cISCPropertyHolder* pPropertyHolder,
	uint32_t id,
	BuildingType type)
{
	uint32_t count = 0;

	if (SCPropertyUtil::GetPropertyValue(pPropertyHolder, id, count) && count > 0)
	{
		availabilityConditions.push_back(std::make_unique<BuildingCountAvailabilityCondition>(type, count));
	}
}

void CustomOrdinance::ReadRCIGroupMinPopulationAvailabilityCondition(
	const cISCPropertyHolder* pPropertyHolder,
	uint32_t id,
	RCIGroup type)
{
	uint32_t minPopulation = 0;

	if (SCPropertyUtil::GetPropertyValue(pPropertyHolder, id, minPopulation) && minPopulation > 0)
	{
		availabilityConditions.push_back(std::make_unique<RCIGroupPopulationAvailabilityCondition>(type, minPopulation));
	}
}

void CustomOrdinance::ReadBuildingCountMonthlyIncomeFactor(
	const cISCPropertyHolder* pPropertyHolder,
	uint32_t id,
	BuildingType type)
{
	float factor = 0.0f;

	if (SCPropertyUtil::GetPropertyValue(pPropertyHolder, id, factor))
	{
		monthlyIncomeFactors.push_back(std::make_unique<BuildingCountIncomeFactor>(type, factor));
	}
}

void CustomOrdinance::ReadRCIGroupPopulationMonthlyIncomeFactor(
	const cISCPropertyHolder* pPropertyHolder,
	uint32_t id,
	RCIGroup type)
{
	float factor = 0.0f;

	if (SCPropertyUtil::GetPropertyValue(pPropertyHolder, id, factor))
	{
		monthlyIncomeFactors.push_back(std::make_unique<RCIGroupPopulationIncomeFactor>(type, factor));
	}
}
