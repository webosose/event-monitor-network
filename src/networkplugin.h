// Copyright (c) 2016-2018 LG Electronics, Inc.
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

#pragma once

#include <event-monitor-api/pluginbase.hpp>

class NetworkPlugin: public EventMonitor::PluginBase
{
public:
	NetworkPlugin(EventMonitor::Manager *manager);
	virtual ~NetworkPlugin();
	void startMonitoring();
	EventMonitor::UnloadResult stopMonitoring(const std::string &service);

private:
	void connmanStatusCallback(pbnjson::JValue &previousValue,
	                           pbnjson::JValue &value);
	void blockToasts(unsigned int timeMs);
private:
	//for ethernet toast
	bool toastsBlocked;
};
