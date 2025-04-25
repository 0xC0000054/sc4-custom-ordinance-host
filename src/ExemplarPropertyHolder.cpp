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

#include "ExemplarPropertyHolder.h"
#include "cGZPersistResourceKey.h"
#include "cIGZIStream.h"
#include "cIGZOStream.h"
#include "cIGZPersistResourceManager.h"
#include "GlobalPointers.h"
#include "GZStreamUtil.h"

ExemplarPropertyHolder::ExemplarPropertyHolder()
	: refCount(0), defaultExemplar()
{
}

ExemplarPropertyHolder::ExemplarPropertyHolder(const ExemplarPropertyHolder& other)
	: refCount(other.refCount), defaultExemplar(other.defaultExemplar)
{
}

ExemplarPropertyHolder::ExemplarPropertyHolder(ExemplarPropertyHolder&& other) noexcept
	: refCount(other.refCount), defaultExemplar(std::move(other.defaultExemplar))
{
}

ExemplarPropertyHolder& ExemplarPropertyHolder::operator=(const ExemplarPropertyHolder& other)
{
	refCount = other.refCount;
	defaultExemplar = other.defaultExemplar;

	return *this;
}

ExemplarPropertyHolder& ExemplarPropertyHolder::operator=(ExemplarPropertyHolder&& other) noexcept
{
	refCount = other.refCount;
	defaultExemplar = std::move(other.defaultExemplar);

	return *this;
}

bool ExemplarPropertyHolder::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cISCPropertyHolder)
	{
		*ppvObj = static_cast<cISCPropertyHolder*>(this);
		AddRef();

		return true;
	}
	else if (riid == GZIID_cISCExemplarPropertyHolder)
	{
		*ppvObj = static_cast<cISCExemplarPropertyHolder*>(this);
		AddRef();

		return true;
	}
	else if (riid == GZIID_cIGZUnknown)
	{
		*ppvObj = static_cast<cIGZUnknown*>(static_cast<cISCPropertyHolder*>(this));
		AddRef();

		return true;
	}

	*ppvObj = nullptr;
	return false;
}

uint32_t ExemplarPropertyHolder::AddRef()
{
	return ++refCount;
}

uint32_t ExemplarPropertyHolder::Release()
{
	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}

bool ExemplarPropertyHolder::HasProperty(uint32_t dwProperty) const
{
	bool result = false;

	if (defaultExemplar)
	{
		result = defaultExemplar->AsISCPropertyHolder()->HasProperty(dwProperty);
	}

	return result;
}

bool ExemplarPropertyHolder::GetPropertyList(cIGZUnknownList** ppList) const
{
	bool result = false;

	if (defaultExemplar)
	{
		result = defaultExemplar->AsISCPropertyHolder()->GetPropertyList(ppList);
	}

	return result;
}

cISCProperty* ExemplarPropertyHolder::GetProperty(uint32_t dwProperty) const
{
	cISCProperty* pProperty = nullptr;

	if (defaultExemplar)
	{
		pProperty = defaultExemplar->AsISCPropertyHolder()->GetProperty(dwProperty);
	}

	return pProperty;
}

bool ExemplarPropertyHolder::GetProperty(uint32_t dwProperty, uint32_t& dwValueOut) const
{
	bool result = false;

	if (defaultExemplar)
	{
		result = defaultExemplar->AsISCPropertyHolder()->GetProperty(dwProperty, dwValueOut);
	}

	return result;
}

bool ExemplarPropertyHolder::GetProperty(uint32_t dwProperty, cIGZString& szValueOut) const
{
	bool result = false;

	if (defaultExemplar)
	{
		result = defaultExemplar->AsISCPropertyHolder()->GetProperty(dwProperty, szValueOut);
	}

	return result;
}

bool ExemplarPropertyHolder::GetProperty(uint32_t dwProperty, uint32_t riid, void** ppvObj) const
{
	bool result = false;

	if (defaultExemplar)
	{
		result = defaultExemplar->AsISCPropertyHolder()->GetProperty(dwProperty, riid, ppvObj);
	}

	return result;
}

bool ExemplarPropertyHolder::GetProperty(uint32_t dwProperty, void* pUnknown, uint32_t& dwUnknownOut) const
{
	bool result = false;

	if (defaultExemplar)
	{
		result = defaultExemplar->AsISCPropertyHolder()->GetProperty(dwProperty, pUnknown, dwUnknownOut);
	}

	return result;
}

bool ExemplarPropertyHolder::AddProperty(cISCProperty* pProperty, bool bUnknown)
{
	return false;
}

bool ExemplarPropertyHolder::AddProperty(uint32_t dwProperty, cIGZVariant const* pVariant, bool bUnknown)
{
	return false;
}

bool ExemplarPropertyHolder::AddProperty(uint32_t dwProperty, uint32_t dwValue, bool bUnknown)
{
	return false;
}

bool ExemplarPropertyHolder::AddProperty(uint32_t dwProperty, cIGZString const& szValue)
{
	return false;
}

bool ExemplarPropertyHolder::AddProperty(uint32_t dwProperty, int32_t lValue, bool bUnknown)
{
	return false;
}

bool ExemplarPropertyHolder::AddProperty(uint32_t dwProperty, void* pUnknown, uint32_t dwUnknown, bool bUnknown)
{
	return false;
}

bool ExemplarPropertyHolder::CopyAddProperty(cISCProperty* pProperty, bool bUnknown)
{
	return false;
}

bool ExemplarPropertyHolder::RemoveProperty(uint32_t dwProperty)
{
	return false;
}

bool ExemplarPropertyHolder::RemoveAllProperties(void)
{
	return false;
}

bool ExemplarPropertyHolder::EnumProperties(FunctionPtr1 pFunction1, void* pData) const
{
	bool result = false;

	if (defaultExemplar)
	{
		result = defaultExemplar->AsISCPropertyHolder()->EnumProperties(pFunction1, pData);
	}

	return result;
}

bool ExemplarPropertyHolder::EnumProperties(FunctionPtr2 pFunction2, FunctionPtr1 pFunctionPipe) const
{
	bool result = false;

	if (defaultExemplar)
	{
		result = defaultExemplar->AsISCPropertyHolder()->EnumProperties(pFunction2, pFunctionPipe);
	}

	return result;
}

bool ExemplarPropertyHolder::CompactProperties(void)
{
	return false;
}

bool ExemplarPropertyHolder::SetDefaultExemplar(cISCResExemplar* pExemplar)
{
	defaultExemplar = pExemplar;
	return true;
}

cISCResExemplar* ExemplarPropertyHolder::GetDefaultExemplar(void)
{
	return defaultExemplar;
}

bool ExemplarPropertyHolder::Read(cIGZIStream& stream)
{
	uint32_t version = 0;

	if (!stream.GetUint32(version) || version != 1)
	{
		return false;
	}

	cGZPersistResourceKey key;

	if (!GZStreamUtil::ReadResKey(stream, key))
	{
		return false;
	}

	if (key.type != 0 && key.group != 0 && key.instance != 0)
	{
		cRZAutoRefCount<cISCResExemplar> exemplar;

		if (spRM->GetPrivateResource(
			key,
			GZIID_cISCResExemplar,
			exemplar.AsPPVoid(),
			0,
			nullptr))
		{
			defaultExemplar = exemplar;
		}
	}
	else
	{
		defaultExemplar.Reset();
	}

	return true;
}

bool ExemplarPropertyHolder::Write(cIGZOStream& stream)
{
	if (!stream.SetUint32(1)) // version
	{
		return false;
	}

	cGZPersistResourceKey key;

	if (defaultExemplar)
	{
		defaultExemplar->GetKey(key);
	}

	return GZStreamUtil::WriteResKey(stream, key);
}

