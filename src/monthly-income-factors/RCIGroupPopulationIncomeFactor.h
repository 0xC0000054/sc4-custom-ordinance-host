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
#include "IMonthlyIncomeFactor.h"
#include "RCIGroup.h"

class RCIGroupPopulationIncomeFactor : public IMonthlyIncomeFactor
{
public:
	RCIGroupPopulationIncomeFactor();
	RCIGroupPopulationIncomeFactor(RCIGroup group, float factor);

	IMonthlyIncomeFactor::Type GetType() const override;
	double Calculate(double monthlyIncome) const override;

	bool Read(cIGZIStream& stream) override;
	bool Write(cIGZOStream& stream) const override;
private:
	uint32_t demandID;
	float monthlyIncomeFactor;
};

