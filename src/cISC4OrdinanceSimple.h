#pragma once
#include "cISC4Ordinance.h"

class cGZPersistResourceKey;

// This type is the interface for the base class of SC4's internal ordinances.
// Our CustomOrdinamce class implements it to take advantage of the ordinance
// exemplar loading that the game performs when starting a new city.
// That saves us a call to cISC4OrdinanceSimulator::AddOrdinance.

static const uint32_t GZIID_cISC4OrdinanceSimple = 0x8bd4fc74;

class cISC4OrdinanceSimple : public cISC4Ordinance
{
public:
	virtual bool SetKey(cGZPersistResourceKey const& key) = 0;
};