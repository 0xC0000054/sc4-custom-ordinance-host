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
#include "cISCPropertyHolder.h"
#include "cISCExemplarPropertyHolder.h"
#include "cIGZSerializable.h"
#include "cISCResExemplar.h"
#include "cRZAutoRefCount.h"

// A read-only property holder that wraps an exemplar.
class ExemplarPropertyHolder
	: public cISCPropertyHolder,
	  public cISCExemplarPropertyHolder
{
public:
	ExemplarPropertyHolder();

	ExemplarPropertyHolder(const ExemplarPropertyHolder& other);
	ExemplarPropertyHolder(ExemplarPropertyHolder&& other) noexcept;

	ExemplarPropertyHolder& operator=(const ExemplarPropertyHolder& other);
	ExemplarPropertyHolder& operator=(ExemplarPropertyHolder&& other) noexcept;

	// cIGZUnknown

	bool QueryInterface(uint32_t riid, void** ppvObj);
	uint32_t AddRef();
	uint32_t Release();

	// cISCPropertyHolder

	bool HasProperty(uint32_t dwProperty) const;

	bool GetPropertyList(cIGZUnknownList** ppList) const;
	cISCProperty* GetProperty(uint32_t dwProperty) const;
	bool GetProperty(uint32_t dwProperty, uint32_t& dwValueOut) const;
	bool GetProperty(uint32_t dwProperty, cIGZString& szValueOut) const;
	bool GetProperty(uint32_t dwProperty, uint32_t riid, void** ppvObj) const;
	bool GetProperty(uint32_t dwProperty, void* pUnknown, uint32_t& dwUnknownOut) const;

	bool AddProperty(cISCProperty* pProperty, bool bUnknown);
	bool AddProperty(uint32_t dwProperty, cIGZVariant const* pVariant, bool bUnknown);
	bool AddProperty(uint32_t dwProperty, uint32_t dwValue, bool bUnknown);
	bool AddProperty(uint32_t dwProperty, cIGZString const& szValue);
	bool AddProperty(uint32_t dwProperty, int32_t lValue, bool bUnknown);
	bool AddProperty(uint32_t dwProperty, void* pUnknown, uint32_t dwUnknown, bool bUnknown);

	bool CopyAddProperty(cISCProperty* pProperty, bool bUnknown);

	bool RemoveProperty(uint32_t dwProperty);
	bool RemoveAllProperties(void);

	bool EnumProperties(FunctionPtr1 pFunction1, void* pData) const;
	bool EnumProperties(FunctionPtr2 pFunction2, FunctionPtr1 pFunctionPipe) const;

	bool CompactProperties(void);

	// cISCExemplarPropertyHolder

	bool SetDefaultExemplar(cISCResExemplar* pExemplar);
	cISCResExemplar* GetDefaultExemplar(void);

	bool Write(cIGZOStream& stream);
	bool Read(cIGZIStream& stream);
private:
	uint32_t refCount;
	cRZAutoRefCount<cISCResExemplar> defaultExemplar;
};

