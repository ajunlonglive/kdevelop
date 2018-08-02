/*
 * This file is part of KDevelop
 *
 * Copyright 2016 Carlos Nihelton <carlosnsoliveira@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "configgroup.h"

namespace ClangTidy
{
const ConfigGroup::Option ConfigGroup::FilePath{ { "FilePath" }, { "%1" } };
const ConfigGroup::Option ConfigGroup::BuildPath{ { "BuildPath" }, { "--p=%1%2" } };
const ConfigGroup::Option ConfigGroup::AdditionalParameters{ { "AdditionalParameters" }, { "%1" } };
const ConfigGroup::Option ConfigGroup::EnabledChecks{ { "Checks" }, { "%1" } };
const ConfigGroup::Option ConfigGroup::UseConfigFile{ { "UseConfigFile" }, { "--config=" } };
const ConfigGroup::Option ConfigGroup::DumpConfig{ { "DumpConfigToFile" }, { "--dump-config" } };
const ConfigGroup::Option ConfigGroup::ExportFixes{ { "ExportFixes" }, { "--export-fixes=" } };
const ConfigGroup::Option ConfigGroup::HeaderFilter{ { "HeaderFilter" }, { "--header-filter=%1" } };
const ConfigGroup::Option ConfigGroup::CheckSystemHeaders{ { "CheckSystemHeaders" }, { "--system-headers" } };
const ConfigGroup::OptionArray ConfigGroup::m_allOptions{
    { &ConfigGroup::FilePath,
      &ConfigGroup::BuildPath,
      &ConfigGroup::AdditionalParameters,
      &ConfigGroup::EnabledChecks,
      &ConfigGroup::UseConfigFile,
      &ConfigGroup::DumpConfig,
      &ConfigGroup::ExportFixes,
      &ConfigGroup::HeaderFilter,
      &ConfigGroup::CheckSystemHeaders }
};

} // namespace ClangTidy
