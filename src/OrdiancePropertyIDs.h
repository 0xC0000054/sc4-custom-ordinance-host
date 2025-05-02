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
#include <cstdint>

// ---------------------------------
// Common ordinance properties
// ---------------------------------

static const uint32_t kOrdinanceNameKey = 0x6A416B21; // String resource key (3 item Uint32 array).
static const uint32_t kOrdinanceName = 0x28EBBC32; // String
static const uint32_t kOrdinanceDescriptionKey = 0xCA416B2A; // String resource key (3 item Uint32 array).
static const uint32_t kOrdinanceDescription = 0x28EBBC34; // String
static const uint32_t kOrdinanceEnactmentIncome = 0x28EBBC37; // int64_t
static const uint32_t kOrdinanceRetracmentIncome = 0x28EBBC38; // int64_t
static const uint32_t kOrdinancemMonthlyConstantIncome = 0x28EBBC39; // int64_t
static const uint32_t kOrdinanceIsIncome = 0x28EBBC3C; // Boolean

// ---------------------------------
// Availability condition properties
// ---------------------------------

// In-game year the ordinance becomes available - Uint32 property.
// Must be greater than SC4's start year of 2000.
static const uint32_t kOrdinanceAvailabilityGameYear = 0x28EBBC35;

// Minimum population conditions - all Uint32 properties.
// Must be greater than zero.

static const uint32_t kOrdinanceAvailabilityMinPopulationResLowWealth = 0x6B23D800;
static const uint32_t kOrdinanceAvailabilityMinPopulationResMediumWealth = 0x6B23D801;
static const uint32_t kOrdinanceAvailabilityMinPopulationResHighWealth = 0x6B23D802;

static const uint32_t kOrdinanceAvailabilityMinPopulationCsLowWealth = 0x6B23D803;
static const uint32_t kOrdinanceAvailabilityMinPopulationCsMediumWealth = 0x6B23D804;
static const uint32_t kOrdinanceAvailabilityMinPopulationCsHighWealth = 0x6B23D805;

static const uint32_t kOrdinanceAvailabilityMinPopulationCoMediumWealth = 0x6B23D806;
static const uint32_t kOrdinanceAvailabilityMinPopulationCoHighWealth = 0x6B23D807;

static const uint32_t kOrdinanceAvailabilityMinPopulationIR = 0x6B23D808;
static const uint32_t kOrdinanceAvailabilityMinPopulationID = 0x6B23D809;
static const uint32_t kOrdinanceAvailabilityMinPopulationIM = 0x6B23D80A;
static const uint32_t kOrdinanceAvailabilityMinPopulationIHT = 0x6B23D80B;
// 0x6B23D80C-0x6B23D81F are reserved for additional minimum population conditions.

// Minimum occupant count conditions - all Uint32 properties.
// Must be greater than zero.

static const uint32_t kOrdinanceAvailabilityMinFireStationCount = 0x6B23D820;
static const uint32_t kOrdinanceAvailabilityMinHospitalCount = 0x6B23D821;
static const uint32_t kOrdinanceAvailabilityMinJailCount = 0x6B23D822;
static const uint32_t kOrdinanceAvailabilityMinPoliceStationCount = 0x6B23D823;
static const uint32_t kOrdinanceAvailabilityMinSchoolBuildingCount = 0x6B23D824;
// 0x6B23D825-0x6B23D82F are reserved for additional minimum occupant counts.

// The name of a Lua function that checks the conditions - String property.
// This property takes precedence over all other availability condition properties.
// Must have a unique name, take no parameters, and return a Boolean.
static const uint32_t kOrdinanceAvailabilityLuaFunction = 0x6B23D830;

// ---------------------------------
// Monthly income factor properties
// ---------------------------------

// Population monthly income factors - all Float32 properties.

// Total residential population factor, most Maxis ordinances use this algorithm.
// This property takes precedence over the residential wealth group properties
// defined below.
// To simplify using the Maxis ordinances as a template, the value is the same
// as the Maxis "Monthly Income Factor" property.
static const uint32_t kOrdinanceMonthlyIncomeFactorResTotalPopulation = 0x28EBBC3A;

static const uint32_t kOrdinanceMonthlyIncomeFactorResLowWealthPopulation = 0x6B23D900;
static const uint32_t kOrdinanceMonthlyIncomeFactorResMediumWealthPopulation = 0x6B23D901;
static const uint32_t kOrdinanceMonthlyIncomeFactorResHighWealthPopulation = 0x6B23D902;

static const uint32_t kOrdinanceMonthlyIncomeFactorCsLowWealthPopulation = 0x6B23D903;
static const uint32_t kOrdinanceMonthlyIncomeFactorCsMediumWealthPopulation = 0x6B23D904;
static const uint32_t kOrdinanceMonthlyIncomeFactorCsHighWealthPopulation = 0x6B23D905;

static const uint32_t kOrdinanceMonthlyIncomeFactorCoMediumWealthPopulation = 0x6B23D906;
static const uint32_t kOrdinanceMonthlyIncomeFactorCoHighWealthPopulation = 0x6B23D907;

static const uint32_t kOrdinanceMonthlyIncomeFactorIRPopulation = 0x6B23D908;
static const uint32_t kOrdinanceMonthlyIncomeFactorIDPopulation = 0x6B23D909;
static const uint32_t kOrdinanceMonthlyIncomeFactorIMPopulation = 0x6B23D90A;
static const uint32_t kOrdinanceMonthlyIncomeFactorIHTPopulation = 0x6B23D90B;
// 0x6B23D90C-0x6B23D91F are reserved for additional population income factors.

// Occupant count factors - all Float32 properties.

static const uint32_t kOrdinanceMonthlyIncomeFactorFireStationCount = 0x6B23D920;
static const uint32_t kOrdinanceMonthlyIncomeFactorHospitalCount = 0x6B23D921;
static const uint32_t kOrdinanceMonthlyIncomeFactorJailCount = 0x6B23D922;
static const uint32_t kOrdinanceMonthlyIncomeFactorPoliceStationCount = 0x6B23D923;
static const uint32_t kOrdinanceMonthlyIncomeFactorSchoolBuildingCount = 0x6B23D924;
// 0x6B23D925-0x6B23D92F are reserved for additional occupant count factors.

// The name of a Lua function that calculates the monthly income - String property.
// This property takes precedence over all other monthly income factor properties.
// Must have a unique name, take the monthly constant income as a parameter, and
// return the calculated monthly expense/income for the ordinance.
static const uint32_t kOrdinanceMonthlyIncomeFactorLuaFunction = 0x6B23D930;
