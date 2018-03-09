# Copyright (c) 2016-2018 LG Electronics, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 2.8.7)

project(event-monitor-network CXX)

include(webOS/webOS)
webos_modules_init(1 0 0 QUALIFIER RC7)
webos_component(1 0 0)

webos_add_compiler_flags(ALL -Wall -std=c++11)

include(FindPkgConfig)

pkg_check_modules(GLIB2 REQUIRED glib-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${GLIB2_CFLAGS_OTHER})

pkg_check_modules(PBNJSON_CPP REQUIRED pbnjson_cpp)
include_directories(${PBNJSON_CPP_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${PBNJSON_CPP_CFLAGS_OTHER})

pkg_check_modules(PMLOG REQUIRED PmLogLib)
include_directories(${PMLOG_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${PMLOG_CFLAGS_OTHER})

pkg_check_modules(I18N REQUIRED webosi18n)
include_directories(${I18N_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${I18N_CFLAGS_OTHER})

# Require that all undefined symbols are satisfied by the libraries from target_link_libraries()
webos_add_linker_options(ALL --no-undefined)

######## Service ########

set(LIBS
        ${GLIB2_LDFLAGS}
        ${PBNJSON_CPP_LDFLAGS}
        ${PMLOG_LDFLAGS}
        ${I18N_LDFLAGS}
        )

#############################################################################################
################################ DISTRIBUTION BRANCH GUIDE ##################################
#############################################################################################
# Below is the sample code how to branch the compilation based on each webos distribution
# such as tv, wearable, signage etc. Since they have their own UX (User Experience) scene,
# network plugin for event-monitor should be modified for each distribution to meet their
# requirement. For this requirement, we can maintain a single file to be chosen and compiled
# for each distribution such as networkplugin_{tv, wearable, signage}.cpp using
# "WEBOS_TARGET_DISTRO" variable inherited from oe build which implies which distribution
# is being built.
#
#set(PLUGIN_IMPL "")

#if(WEBOS_TARGET_DISTRO STREQUAL "apollo")
    set(PLUGIN_IMPL "webos")
#endif()

#message(STATUS "Selected ${PLUGIN_IMPL} implementation ..")

#file(GLOB SOURCES
#        "src/errors.cpp"
#        src/networkplugin_${PLUGIN_IMPL}.cpp)
#############################################################################################

file(GLOB SOURCES src/*.cpp)

webos_configure_source_files(SOURCES src/config.h)
webos_add_compiler_flags(ALL -I${CMAKE_CURRENT_BINARY_DIR}/Configured/src)

set(CMAKE_SHARED_MODULE_PREFIX "")
add_library(network-plugin MODULE ${SOURCES})
target_link_libraries(network-plugin ${LIBS})
install(TARGETS network-plugin DESTINATION ${WEBOS_EVENT_MONITOR_PLUGIN_PATH})
