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
#include "cRZBaseUnknown.h"
#include "BuildingType.h"
#include "cGZPersistResourceKey.h"
#include "cISC4OrdinanceSimple.h"
#include "cIGZSerializable.h"
#include "cRZBaseString.h"
#include "ExemplarPropertyHolder.h"
#include "IAvailabilityCondition.h"
#include "IMonthlyIncomeFactor.h"
#include "RCIGroup.h"
#include "StringResourceKey.h"
#include <memory>
#include <vector>

class CustomOrdinance final
	: public cRZBaseUnknown,
	  public cISC4OrdinanceSimple,
	  private cIGZSerializable
{
public:
	// Setting the ordinance exemplar resource key in the class constructor is
	// required for the game's deserialization code to work.
	// This class uses the instance id member of exemplar resource key as the
	// class id for GZCOM serialization (see GetGZCLSID).

	CustomOrdinance(const cGZPersistResourceKey& exemplarKey);

	CustomOrdinance(const CustomOrdinance& other) = delete;
	CustomOrdinance(CustomOrdinance&& other) = delete;

	CustomOrdinance& operator=(const CustomOrdinance& other) = delete;
	CustomOrdinance& operator=(CustomOrdinance&& other) = delete;

	// cIGZUnknown

	bool QueryInterface(uint32_t riid, void** ppvObj);

	uint32_t AddRef();

	uint32_t Release();

	// cISC4Ordinance

	bool Init(void);
	bool Shutdown(void);

	/**
	 * @brief Gets the monthly income/expense for this ordinance.
	 * @return The monthly income/expense for this ordinance.
	 * @remarks This method uses a default algorithm of
	 * <monthly constent income> + (<city population> x <monthly income factor>).
	 * This method can be overridden to use a custom algorithm.
	*/
	int64_t GetCurrentMonthlyIncome(void);

	/**
	 * @brief Gets the unique ordinance ID.
	 * @return The unique ordinance ID.
	*/
	uint32_t GetID(void) const;

	/**
	 * @brief Gets the ordinance name.
	 * @return The ordinance name.
	*/
	cIGZString* GetName(void);

	/**
	 * @brief Gets the ordinance description.
	 * @return The ordinance description.
	*/
	cIGZString* GetDescription(void);

	/**
	 * @brief Gets the in-game year that the ordinance becomes available.
	 * @return The in-game year that the ordinance becomes available.
	 *
	 * @remarks By default the ordinances will be available at the start of the game.
	 * This method can be overridden to set a custom start year.
	 * Note that in SimCity 4 the in-game date starts in the year 2000.
	*/
	uint32_t GetYearFirstAvailable(void);

	/**
	 * @brief The chance that the ordinance will become available each month.
	 * This feature is not implemented.
	 *
	 * @return The chance that the ordinance will become available each month.
	*/
	SC4Percentage GetChanceAvailability(void);

	/**
	 * @brief Gets the revenue generated by enacting this ordinance.
	 * @return The revenue generated by enacting this ordinance.
	*/
	int64_t GetEnactmentIncome(void);

	/**
	 * @brief Gets the cost of retracting this ordinance.
	 * @return The cost of retracting this ordinance.
	*/
	int64_t GetRetracmentIncome(void);

	/**
	 * @brief Gets the monthly constant income/expense value for the ordinance.
	 * @return The monthly constant income/expense value for the ordinance.
	*/
	int64_t GetMonthlyConstantIncome(void);

	/**
	 * @brief Gets the factor applied to the ordinance cost.
	 * @return The factor applied to the ordinance cost.
	*/
	float GetMonthlyIncomeFactor(void);

	/**
	 * @brief Gets a collection of the ordinance properties/effects.
	 * @return The ordinance properties/effects.
	*/
	cISCPropertyHolder* GetMiscProperties();

	/**
	 * @brief Gets a value that identifies the in-game advisor related to this
	 * ordinance. This is unused in SimCity 4 and always returns zero.
	 * @return Zero.
	*/
	uint32_t GetAdvisorID(void);

	bool IsAvailable(void);

	bool IsOn(void);

	bool IsEnabled(void);

	/**
	 * @brief Gets the monthly adjusted income.
	 * @return The monthly adjusted income.
	*/
	int64_t GetMonthlyAdjustedIncome(void);

	/**
	 * @brief Defines the conditions that are required for the ordinance to become available.
	 * @return True if the ordinance should become available in the menu; otherwise, false.
	 * @remarks By default the only required condition is the starting year @see GetYearFirstAvailable.
	 * This method can be overridden to provide custom conditions for the ordinance availability.
	*/
	bool CheckConditions(void);

	/**
	 * @brief Gets a value indicating whether this ordinance generates income.
	 * @return True if this ordinance generates income; otherwise, false.
	*/
	bool IsIncomeOrdinance(void);

	/**
	 * @brief This is called during the monthly ordinance simulation.
	 * @return True if successful; otherwise, false.
	*/
	virtual bool Simulate(void);

	bool SetAvailable(bool isAvailable);

	bool SetOn(bool isOn);

	bool SetEnabled(bool isEnabled);

	bool ForceAvailable(bool isAvailable);

	bool ForceOn(bool isOn);

	bool ForceEnabled(bool isEnabled);

	bool ForceMonthlyAdjustedIncome(int64_t monthlyAdjustedIncome);

	// cISC4OrdinanceSimple

	bool SetKey(const cGZPersistResourceKey& key);

private:
	// cIGZSerializable

	bool Write(cIGZOStream& stream);
	bool Read(cIGZIStream& stream);
	uint32_t GetGZCLSID();

	// Private members

	void InitFromExemplarData();

	void LoadLocalizedStringResources();

	void ReadCommonOrdinanceProperties(const cISCPropertyHolder* pPropertyHolder);
	void ReadAvailabilityConditionProperties(const cISCPropertyHolder* pPropertyHolder);
	void ReadMonthlyIncomeFactorProperties(const cISCPropertyHolder* pPropertyHolder);

	void ReadMinBuildingCountAvailabilityCondition(
		const cISCPropertyHolder* pPropertyHolder,
		uint32_t id,
		BuildingType type);
	void ReadRCIGroupMinPopulationAvailabilityCondition(
		const cISCPropertyHolder* pPropertyHolder,
		uint32_t id,
		RCIGroup type);

	void ReadBuildingCountMonthlyIncomeFactor(
		const cISCPropertyHolder* pPropertyHolder,
		uint32_t id,
		BuildingType type);
	void ReadRCIGroupPopulationMonthlyIncomeFactor(
		const cISCPropertyHolder* pPropertyHolder,
		uint32_t id,
		RCIGroup type);

	// The int64_t fields are first to eliminate 8 bytes of alignment padding.

	int64_t enactmentIncome;
	int64_t retracmentIncome;
	int64_t monthlyConstantIncome;
	int64_t monthlyAdjustedIncome;
	cGZPersistResourceKey ordinanceExemplarKey;
	std::vector<std::unique_ptr<IAvailabilityCondition>> availabilityConditions;
	std::vector<std::unique_ptr<IMonthlyIncomeFactor>> monthlyIncomeFactors;
	cRZBaseString name;
	StringResourceKey nameKey;
	cRZBaseString description;
	StringResourceKey descriptionKey;
	ExemplarPropertyHolder miscProperties;
	bool isIncomeOrdinance;
	bool available;
	bool on;
	bool enabled;
	bool haveDeserialized;
};

