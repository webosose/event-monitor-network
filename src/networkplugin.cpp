// Copyright (c) 2016-2019 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include "networkplugin.h"
#include "config.h"
#include "logging.h"
#include "errors.h"

using namespace pbnjson;
using namespace EventMonitor;

PmLogContext logContext;

static const std::string SETTINGS_ICON_URL =
    "/usr/palm/applications/com.palm.app.settings/icon.png";

static const unsigned int TOAST_BOOT_BLOCK_TIME_MS = 7000;
static const unsigned int TOAST_RESUME_BLOCK_TIME_MS = 5000;
static const unsigned int TOAST_SUSPEND_BLOCK_TIME_MS = 5000;


const char *requiredServices[] = {"com.webos.service.connectionmanager",
                                  "com.webos.service.wifi",
                                  nullptr
                                 };

PmLogContext pluginLogContext;

EventMonitor::Plugin *instantiatePlugin(int version,
                                        EventMonitor::Manager *manager)
{
	if (version != EventMonitor::API_VERSION)
	{
		return nullptr;
	}

	return new NetworkPlugin(manager);
}

NetworkPlugin::NetworkPlugin(Manager *_manager):
	PluginBase(_manager, WEBOS_LOCALIZATION_PATH),
	toastsBlocked(false)
{
}

NetworkPlugin::~NetworkPlugin()
{
}

void NetworkPlugin::connmanStatusCallback(JValue &previousValue, JValue &value)
{
	JValue onClickAction = JObject {{"addId",  "com.palm.app.settings"},
	                                {"target", "network"}
	};

	//LOG_INFO(MSGID_NETWORK_PLUGIN_INFO, 0, "connmanStatusCallback");

	if (!this->toastsBlocked)
	{
		// Ethernet toast
		{
			std::string prevState = "";
			std::string curState = "";
			auto status = previousValue["wired"]["state"].asString(prevState);
			if(CONV_OK == status)
			{
				status = value["wired"]["state"].asString(curState);

				// Toast only on state change
				if ((CONV_OK == status) && (curState == "connected" && prevState == "disconnected"))
				{
					LOG_INFO(MSGID_NETWORK_PLUGIN_INFO, 0, "Wired network is connected");
					this->manager->createToast(
							this->getLocString("Wired network is connected."),
							SETTINGS_ICON_URL,
							onClickAction);
				}
				else if ((CONV_OK == status) && (curState == "disconnected" && prevState == "connected"))
				{ 
					LOG_INFO(MSGID_NETWORK_PLUGIN_INFO, 0, "Wired LAN cable disconnected");
					this->manager->createToast(
							this->getLocString("Wired LAN cable disconnected."),
							SETTINGS_ICON_URL,
							onClickAction);
				}
			}
		}
	}
}

void NetworkPlugin::blockToasts(unsigned int timeMs)
{
	this->toastsBlocked = true;
	LOG_DEBUG("Toast block on");

	auto clearBlock = [this](const std::string& timeoutId)
	{
		LOG_DEBUG("Toast block off");
		this->toastsBlocked = false;
	};

	/* Will replace previous timeout if any */
	this->manager->setTimeout("toastUnblock", timeMs, false, clearBlock);
}

void NetworkPlugin::startMonitoring()
{
	LOG_INFO(MSGID_NETWORK_PLUGIN_INFO, 0, "Network monitoring starts");

	this->blockToasts(TOAST_BOOT_BLOCK_TIME_MS);

	JValue params = JObject {{}};

	this->manager->subscribeToMethod(
	    "connman",
	    "luna://com.webos.service.connectionmanager/getStatus",
	    params,
	    std::bind(&NetworkPlugin::connmanStatusCallback, this,
	              std::placeholders::_1, std::placeholders::_2));
}

EventMonitor::UnloadResult NetworkPlugin::stopMonitoring(
    const std::string& service)
{
	return UNLOAD_OK;
}
