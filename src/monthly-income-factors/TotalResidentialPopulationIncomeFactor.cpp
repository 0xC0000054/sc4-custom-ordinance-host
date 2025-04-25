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

#include "TotalResidentialPopulationIncomeFactor.h"
#include "PopulationProvider.h"

TotalResidentialPopulationIncomeFactor::TotalResidentialPopulationIncomeFactor()
	: monthlyIncomeFactor(0)
{
}

TotalResidentialPopulationIncomeFactor::TotalResidentialPopulationIncomeFactor(float factor)
	: monthlyIncomeFactor(factor)
{
}

IMonthlyIncomeFactor::Type TotalResidentialPopulationIncomeFactor::GetType() const
{
	return IMonthlyIncomeFactor::Type::TotalResidentialPop;
}

double TotalResidentialPopulationIncomeFactor::Calculate(double monthlyIncome) const
{
	const int32_t cityPopulation = PopulationProvider::GetTotalResidentialPopulation();
	const double populationIncome = monthlyIncomeFactor * static_cast<double>(cityPopulation);

	return monthlyIncome + populationIncome;
}

bool TotalResidentialPopulationIncomeFactor::Read(cIGZIStream& stream)
{
	uint32_t version = 0;

	if (!stream.GetUint32(version) || version != 1)
	{
		return false;
	}

	if (!stream.GetFloat32(monthlyIncomeFactor))
	{
		return false;
	}

	return true;
}

bool TotalResidentialPopulationIncomeFactor::Write(cIGZOStream& stream) const
{
	if (!stream.SetUint32(1)) // version
	{
		return false;
	}

	if (!stream.SetFloat32(monthlyIncomeFactor))
	{
		return false;
	}

	return true;
}