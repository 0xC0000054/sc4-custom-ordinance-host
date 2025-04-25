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

#include "BuildingCountProvider.h"
#include "GlobalPointers.h"
#include "cISC4FireProtectionSimulator.h"
#include "cISC4PoliceSimulator.h"
#include "cISC4ResidentialSimulator.h"

uint32_t BuildingCountProvider::GetBuildingCount(BuildingType type)
{
	switch (type)
	{
	case BuildingType::FireStation:
		return spFireProtectionSim ? spFireProtectionSim->GetFireStationCount() : 0;
	case BuildingType::Hospital:
		return spResidentialSim ? spResidentialSim->GetHospitalBuildingCount() : 0;
	case BuildingType::Jail:
		return spPoliceSim ? spPoliceSim->GetJailCount() : 0;
	case BuildingType::PoliceStation:
		return spPoliceSim ? spPoliceSim->GetPoliceStationCount() : 0;
	case BuildingType::School:
		return spResidentialSim ? spResidentialSim->GetSchoolBuildingCount() : 0;
	default:
		return 0;
	}
}
