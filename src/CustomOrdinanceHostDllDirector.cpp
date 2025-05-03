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

#include "Logger.h"
#include "version.h"
#include "cGZPersistResourceKey.h"
#include "cIGZApp.h"
#include "cIGZCheatCodeManager.h"
#include "cIGZCommandParameterSet.h"
#include "cIGZCommandServer.h"
#include "cIGZCOM.h"
#include "cIGZFrameWork.h"
#include "cIGZMessage2Standard.h"
#include "cIGZMessageServer2.h"
#include "cIGZPersistDBRecord.h"
#include "cIGZPersistDBSegment.h"
#include "cIGZPersistDBSegmentMultiPackedFiles.h"
#include "cIGZPersistResourceFactory.h"
#include "cIGZPersistResourceKeyList.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZVariant.h"
#include "cISC4AdvisorSystem.h"
#include "cISC4App.h"
#include "cISC4City.h"
#include "cISC4OrdinanceSimulator.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISCResExemplar.h"
#include "cISCStringDetokenizer.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include "cRZMessage2COMDirector.h"
#include "CustomOrdinance.h"
#include "DebugUtil.h"
#include "GlobalPointers.h"
#include "GZServPtrs.h"
#include "PersistResourceKeyFilterByType.h"
#include "PersistResourceKeyHash.h"
#include "SCPropertyUtil.h"

#include "frozen/unordered_set.h"

#include <array>
#include <string>
#include <unordered_set>
#include <vector>

#include <Windows.h>
#include "wil/resource.h"
#include "wil/result.h"
#include "wil/win32_helpers.h"

static constexpr uint32_t kSC4MessagePostCityInit = 0x26D31EC1;
static constexpr uint32_t kSC4MessagePostCityShutdown = 0x26D31EC3;

static constexpr std::array<uint32_t, 2> RequiredNotifications =
{
	kSC4MessagePostCityInit,
	kSC4MessagePostCityShutdown,
};

static constexpr uint32_t kCustomOrdinanceHostDllDirector = 0xEED7366B;

static constexpr std::string_view PluginLogFileName = "SC4CustomOrdinanceHost.log";

namespace
{
	std::filesystem::path GetDllFolderPath()
	{
		wil::unique_cotaskmem_string modulePath = wil::GetModuleFileNameW(wil::GetModuleInstanceHandle());

		std::filesystem::path temp(modulePath.get());

		return temp.parent_path();
	}

	void PathCombine(cRZBaseString& path, const std::string_view& segment)
	{
		uint32_t length = path.Strlen();

		if (length > 0 && segment.size() > 0)
		{
			if (path.ToChar()[length - 1] != '\\')
			{
				// Append a backslash if necessary.
				path.Append("\\", 1);
			}

			path.Append(segment.data(), segment.size());
		}
	}

	static constexpr frozen::unordered_set<uint32_t, 20> MaxisOrdinanceCLSIDs =
	{
		0xA2BF1DDC, // Carpool Incentive
		0xA2BF1E43, // Clean Air
		0x00D0723D, // CPR Training
		0x40D07236, // Free Clinics
		0x815B4CEF, // Junior Sports League
		0x22F6E80C, // Landfill Gas Recovery
		0xA0D07129, // Legalize Gambling
		0x62BF1DAA, // Mandatory Smoke Detectors
		0x62BF1DB9, // Neighborhood Watch
		0xE0D0722E, // Nuclear Free Zone
		0x22F6E81B, // Paper Reduction Act
		0x82B9999B, // Power Conservation
		0xE0D07233, // Pro Reading Campaign
		0xA2BF1DE5, // Shuttle Service
		0xC2F6E81F, // Tire Recycling
		0x62F6E7CF, // Tourist Promotion
		0x42BF1E18, // Trash Presort
		0xC2BF1E04, // Vehicle Emission Standard
		0x02BF1DFA, // Water Conservation
		0xC2BF1DC5, // Youth Curfew
	};

	struct EnumResourceKeyContext
	{
		cIGZPersistDBSegmentMultiPackedFiles* pMultiPackedFile;
		cIGZPersistResourceFactory* pExemplarResourceFactory;
		std::unordered_set<cGZPersistResourceKey> ordinanceKeys;

		EnumResourceKeyContext(
			cIGZPersistDBSegmentMultiPackedFiles* pMultiPackedFile,
			cIGZPersistResourceFactory* pFactory)
			: pMultiPackedFile(pMultiPackedFile),
			  pExemplarResourceFactory(pFactory),
			  ordinanceKeys()
		{
		}
	};

	void EnumCustomOrdinanceResourceKeys(cGZPersistResourceKey const& key, void* pContext)
	{
		constexpr uint32_t kExemplarType = 0x10;
		constexpr uint32_t kExemplarType_Ordinance = 14;

		EnumResourceKeyContext* pState = static_cast<EnumResourceKeyContext*>(pContext);

		cRZAutoRefCount<cIGZPersistDBSegment> segment;

		if (pState->pMultiPackedFile->FindDBSegment(key, segment.AsPPObj()))
		{
			cRZAutoRefCount<cIGZPersistDBRecord> record;

			if (segment->OpenRecord(key, record.AsPPObj(), cIGZFile::AccessMode::Read))
			{
				cRZAutoRefCount<cISCResExemplar> exemplar;

				if (pState->pExemplarResourceFactory->CreateInstance(
					*record,
					GZIID_cISCResExemplar,
					exemplar.AsPPVoid(),
					0,
					nullptr))
				{
					const cISCPropertyHolder* pPropertyHolder = exemplar->AsISCPropertyHolder();

					uint32_t exemplarType = 0;

					if (SCPropertyUtil::GetPropertyValue(pPropertyHolder, kExemplarType, exemplarType))
					{
						if (exemplarType == kExemplarType_Ordinance)
						{
							// Prevent the Maxis key CLSID values from being reused.
							// The CLSID values must be unique.
							if (MaxisOrdinanceCLSIDs.contains(key.instance))
							{
								Logger::GetInstance().WriteLineFormatted(
									LogLevel::Error,
									"Exemplar instance id 0x%08x is the same as a Maxis ordinance id.",
									key.instance);
							}
							else
							{
								pState->ordinanceKeys.emplace(key);
							}
						}
						else
						{
							Logger::GetInstance().WriteLineFormatted(
								LogLevel::Error,
								"Exemplar 0x%08x, 0x%08x, 0x%08x does not have an ExemplarType of Ordinance.",
								key.type,
								key.group,
								key.instance);
						}
					}
					else
					{
						Logger::GetInstance().WriteLineFormatted(
							LogLevel::Error,
							"Exemplar 0x%08x, 0x%08x, 0x%08x does not have an ExemplarType property.",
							key.type,
							key.group,
							key.instance);
					}
				}
				else
				{
					Logger::GetInstance().WriteLineFormatted(
						LogLevel::Error,
						"Failed to parse 0x%08x, 0x%08x, 0x%08x.",
						key.type,
						key.group,
						key.instance);
				}

				segment->CloseRecord(record);
			}
			else
			{
				Logger::GetInstance().WriteLineFormatted(
					LogLevel::Error,
					"Failed to open DBPF record 0x%08x, 0x%08x, 0x%08x.",
					key.type,
					key.group,
					key.instance);
			}
		}
		else
		{
			Logger::GetInstance().WriteLineFormatted(
				LogLevel::Error,
				"Failed to open the DBPF file for 0x%08x, 0x%08x, 0x%08x.",
				key.type,
				key.group,
				key.instance);
		}
	}
}

cIGZPersistResourceManager* spRM = nullptr;
cISC4DemandSimulator* spDemandSim = nullptr;
cISC4FireProtectionSimulator* spFireProtectionSim = nullptr;
cISC4PoliceSimulator* spPoliceSim = nullptr;
cISC4ResidentialSimulator* spResidentialSim = nullptr;
cISC4Simulator* spSimulator = nullptr;
cISCLua* spLua = nullptr;

class CustomOrdinanceHostDllDirector final : public cRZMessage2COMDirector
{
public:
	CustomOrdinanceHostDllDirector()
	{
		std::filesystem::path dllFolderPath = GetDllFolderPath();

		std::filesystem::path logFilePath = dllFolderPath;
		logFilePath /= PluginLogFileName;

		Logger& logger = Logger::GetInstance();
		logger.Init(logFilePath, LogLevel::Error);
		logger.WriteLogFileHeader("SC4CustomOrdinanceHost v" PLUGIN_VERSION_STR);
	}

	uint32_t GetDirectorID() const override
	{
		return kCustomOrdinanceHostDllDirector;
	}

	bool OnStart(cIGZCOM* pCOM) override
	{
		if (!customOrdinanceResourceKeys.empty())
		{
			mpFrameWork->AddHook(this);
		}

		return true;
	}

	bool GetClassObject(uint32_t rclsid, uint32_t riid, void** ppvObj) override
	{
		// To retrieve an instance of a registered class the framework will call the
		// GetClassObject method whenever it needs the director to provide one.

		const auto it = std::ranges::find(
			customOrdinanceResourceKeys,
			rclsid,
			&cGZPersistResourceKey::instance);

		if (it != customOrdinanceResourceKeys.end())
		{
			auto ordinance = new CustomOrdinance(*it);

			if (ordinance->QueryInterface(riid, ppvObj))
			{
				return true;
			}

			// Interface not found, free the created object.
			ordinance->AddRef();
			ordinance->Release();
		}

		return false;
	}

	void EnumClassObjects(ClassObjectEnumerationCallback pCallback, void* pContext) override
	{
		// The framework calls this method once when initializing a director, after
		// InitializeCOM and before OnStart.
		// The list of class IDs it returns is cached by the framework.

		LoadCustomOrdinances();

		if (!customOrdinanceResourceKeys.empty())
		{
			for (const auto& key : customOrdinanceResourceKeys)
			{
				pCallback(key.instance, 0, pContext);
			}
		}
	}

private:
	bool DoMessage(cIGZMessage2* pMsg) override
	{
		switch (pMsg->GetType())
		{
		case kSC4MessagePostCityInit:
			PostCityInit(static_cast<cIGZMessage2Standard*>(pMsg));
			break;
		case kSC4MessagePostCityShutdown:
			PostCityShutdown();
			break;
		}

		return true;
	}

	void PostCityInit(cIGZMessage2Standard* pStandardMsg)
	{
		cISC4City* pCity = static_cast<cISC4City*>(pStandardMsg->GetVoid1());

		if (pCity)
		{
			spDemandSim = pCity->GetDemandSimulator();
			spFireProtectionSim = pCity->GetFireProtectionSimulator();
			spPoliceSim = pCity->GetPoliceSimulator();
			spResidentialSim = pCity->GetResidentialSimulator();
			spSimulator = pCity->GetSimulator();
			spLua = pCity->GetAdvisorSystem()->GetScriptingContext();

			cISC4OrdinanceSimulator* pOrdinanceSim = pCity->GetOrdinanceSimulator();

			if (pOrdinanceSim)
			{
				for (auto& key : customOrdinanceResourceKeys)
				{
					cISC4Ordinance* pOrdinance = pOrdinanceSim->GetOrdinanceByID(key.instance);

					if (!pOrdinance)
					{
						auto ordinance = cRZAutoRefCount<CustomOrdinance>(
							new CustomOrdinance(key),
							cRZAutoRefCount<CustomOrdinance>::kAddRef);
						ordinance->Init();

						pOrdinanceSim->AddOrdinance(*ordinance);
					}
				}
			}
		}
	}

	void PostCityShutdown()
	{
		spDemandSim = nullptr;
		spFireProtectionSim = nullptr;
		spPoliceSim = nullptr;
		spResidentialSim = nullptr;
		spSimulator = nullptr;
		spLua = nullptr;
	}

	bool PostAppInit() override
	{
		Logger& logger = Logger::GetInstance();

		cIGZPersistResourceManagerPtr resMan;
		spRM = resMan;
		resMan->AddRef();

		cIGZMessageServer2Ptr ms2;

		for (uint32_t messageID : RequiredNotifications)
		{
			if (!ms2->AddNotification(this, messageID))
			{
				logger.WriteLine(LogLevel::Error, "Failed to subscribe to the required notifications.");
				return false;
			}
		}

		return true;
	}

	bool PreAppShutdown() override
	{
		cIGZPersistResourceManager* localRM = spRM;
		spRM = nullptr;

		if (localRM)
		{
			localRM->Release();
		}

		return true;
	}

	cRZBaseString GetCustomOrdinanceDirectoryPath()
	{
		cRZBaseString path;

		if (mpFrameWork)
		{
			cIGZApp* const pApp = mpFrameWork->Application();

			if (pApp)
			{
				cRZAutoRefCount<cISC4App> sc4App;

				if (pApp->QueryInterface(GZIID_cISC4App, sc4App.AsPPVoid()))
				{
					cRZBaseString tempPath;

					sc4App->GetUserPluginDirectory(tempPath);

					if (tempPath.Strlen() > 0)
					{
						PathCombine(tempPath, "140-ordinances\\");
						path = std::move(tempPath);
					}
				}
			}

		}

		return path;
	}

	void LoadCustomOrdinances()
	{
		Logger& logger = Logger::GetInstance();

		customOrdinanceResourceKeys.clear();

		const cRZBaseString customOrdinanceDir = GetCustomOrdinanceDirectoryPath();

		if (customOrdinanceDir.Strlen() > 0)
		{
			logger.WriteLineFormatted(LogLevel::Info, "Ordinance folder path=%s", customOrdinanceDir.ToChar());

			cRZAutoRefCount<cIGZPersistDBSegment> customOrdinanceFiles;

			if (mpCOM->GetClassObject(
				GZCLSID_cGZPersistDBSegmentMultiPackedFiles,
				GZIID_cIGZPersistDBSegment,
				customOrdinanceFiles.AsPPVoid()))
			{
				if (customOrdinanceFiles->Init())
				{
					if (customOrdinanceFiles->SetPath(customOrdinanceDir))
					{
						if (customOrdinanceFiles->Open(true, false))
						{
							cRZAutoRefCount<cIGZPersistResourceKeyList> list;

							if (mpCOM->GetClassObject(
								GZCLSID_cIGZPersistResourceKeyList,
								GZIID_cIGZPersistResourceKeyList,
								list.AsPPVoid()))
							{
								constexpr uint32_t kExemplarType = 0x6534284A;

								cRZAutoRefCount<cIGZPersistResourceKeyFilter> filter(
									new PersistResourceKeyFilterByType(kExemplarType),
									cRZAutoRefCount<cIGZPersistResourceKeyFilter>::kAddRef);

								uint32_t matchCount = customOrdinanceFiles->GetResourceKeyList(list, filter);

								if (matchCount > 0)
								{
									constexpr uint32_t kExemplarResourceFactoryCLSID = 0x453429B3;
									cRZAutoRefCount<cIGZPersistResourceFactory> exemplarResourceFactory;

									if (mpCOM->GetClassObject(
										kExemplarResourceFactoryCLSID,
										GZIID_cIGZPersistResourceFactory,
										exemplarResourceFactory.AsPPVoid()))
									{
										cRZAutoRefCount<cIGZPersistDBSegmentMultiPackedFiles> multiPackedFile;

										if (customOrdinanceFiles->QueryInterface(
											GZIID_cIGZPersistDBSegmentMultiPackedFiles,
											multiPackedFile.AsPPVoid()))
										{
											EnumResourceKeyContext context(multiPackedFile, exemplarResourceFactory);

											list->EnumKeys(EnumCustomOrdinanceResourceKeys, &context);

											const auto& ordinanceKeys = context.ordinanceKeys;

											if (!ordinanceKeys.empty())
											{
												customOrdinanceResourceKeys.reserve(ordinanceKeys.size());

												for (const auto& key : ordinanceKeys)
												{
													customOrdinanceResourceKeys.emplace_back(key);
												}
											}
										}
									}
								}
							}

							customOrdinanceFiles->Close();
						}
					}

					customOrdinanceFiles->Shutdown();
				}
			}

			logger.WriteLineFormatted(
				LogLevel::Info,
				"Found %u ordinance exemplars.",
				customOrdinanceResourceKeys.size());
		}
		else
		{
			logger.WriteLine(LogLevel::Error, "Unable to detect the custom ordinance path.");
		}
	}

	std::vector<cGZPersistResourceKey> customOrdinanceResourceKeys;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static CustomOrdinanceHostDllDirector sDirector;
	return &sDirector;
}