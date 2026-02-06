/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2003 Thomas Combeleran
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "common/io.hpp"
#include "common/toolbox.hpp"

#include "samnmax.hpp"

#include <cstring>
#include <clocale>

#include <iomanip>
#include <iostream>
#include <set>


int main(int argc, const char **argv) try
{
	std::setlocale(LC_CTYPE, "");

	return SamnMaxPatcher::main(argc, argv);
}
catch (std::logic_error &e)
{
	std::cerr << "BUG: " << e.what() << std::endl;
}
catch (std::exception &e)
{
	std::cerr << "ERROR: " << e.what() << std::endl;
}
catch (...)
{
	std::cerr << "CRASH: Unexpected exception" << std::endl;
}



int SamnMaxPatcher::main(int argc, const char **argv)
{
	int g;

	SamnMaxPatcher::_getOptions(argc, argv, ScummRp::_rpParameters);
	ScummIO::setInfoSlots(ScummRp::_infoSlots);
	ScummIO::info(INF_GLOBAL, xsprintf("%s %s (build %s) by %s", ScummRp::NAME, ScummRp::VERSION, SCUMMTR_BUILD_DATE, ScummRp::AUTHOR));
	ScummIO::info(INF_GLOBAL, "");

	if (ScummRp::_invalidOptions())
	{
		ScummRp::_usage();
		return -1;
	}

	g = ScummRp::_findGameDef(ScummRp::_paramGameId);
	if (g == -1)
	{
		ScummRp::_listGames();
		return 0;
	}

	ScummRp::_filterTag = 0;
	for (int i = 0; ScummRp::_paramTag[i] != '\0'; ++i)
		ScummRp::_filterTag = (ScummRp::_filterTag << 8) | ScummRp::_paramTag[i];

	ScummRp::_game = ScummRp::_gameDef[g];

#ifndef SCUMMRP_OK_TO_CORRUPT_MANIACV2
	if (ScummRp::_game.version == 2 && ScummRp::_game.id == GID_MANIAC && ScummRp::_options & ScummRp::OPT_IMPORT)
		ScummIO::fatal("Modifying Maniac Mansion V2 is known to corrupt it");
#endif

	if (ScummRp::_options & ScummRp::OPT_IMPORT)
		ScummRp::_fileOptions |= BlocksFile::BFOPT_BACKUP;
	else
		ScummRp::_fileOptions |= BlocksFile::BFOPT_READONLY;

	if (ScummRp::_game.version <= 4)
		ScummRp::_fileOptions &= ~BlocksFile::BFOPT_SEQFILE;
	if (ScummRp::_game.version >= 7)
		ScummRp::_fileOptions |= BlocksFile::BFOPT_SEQFILE;

	if (ScummRp::_game.version < 4)
		ScummRp::_processGameFilesV123();
	else
		SamnMaxPatcher::_processGameFilesV4567();

	ScummRp::_backupSystem.applyChanges();

	return 0;
}


void SamnMaxPatcher::_getOptions(int argc, const char **argv, const ScummRp::Parameter *params)
{
	char pendingParams[MAX_PARAMS + 1];

	pendingParams[0] = '\0';
	for (int i = 1; i < argc && argv[i] && ScummRp::_readOption(argv[i], pendingParams); ++i)
	{
		for (int j = 0; pendingParams[j] != '\0'; pendingParams[j++] = '\0')
		{
			for (int k = 0; params[k].c != '\0'; ++k)
			{
				if (params[k].c != pendingParams[j])
					continue;

				++i;
				if (i < argc && argv[i])
				{
					strncpy(params[k].value, argv[i], params[k].maxSize - 1);
					params[k].value[params[k].maxSize - 1] = '\0';
#if defined(_WIN32) || defined(__MSDOS__)
					if (params[k].isPath)
						for (char *p = strchr(params[k].value, '\\'); p; p = strchr(params[k].value, '\\'))
							*p++ = '/';
#endif
				}
				break;
			}
		}
	}
}

void SamnMaxPatcher::_processGameFilesV4567()
{
	char dataFileName[32];
	std::string indexPath(ScummRp::_paramGameDir);
	TreeBlockPtr index;
	int numberOfDisks;

	indexPath += '/';
	indexPath += ScummRp::_game.indexFileName;

	index = ScummRp::_newIndex(indexPath.c_str());
	ScummRp::_exploreIndex<ScummRp::ACT_LOAD>(*index);

	numberOfDisks = ScummRp::_mainTocSet.roomToc.numberOfDisks();
	ScummRp::_prepareTmpIndex();
	for (int i = 1; i < numberOfDisks; ++i)
	{
		std::string dataPath(ScummRp::_paramGameDir);
		TreeBlockPtr disk;

		snprintf(dataFileName, sizeof(dataFileName), ScummRp::_game.dataFileName, i); // ignore -Wformat-security here, ScummRp::_game.dataFileName is internal and safe
		dataPath += '/';
		dataPath += dataFileName;
		disk = new BlocksFile(dataPath.c_str(), ScummRp::_fileOptions, ScummRp::_backupSystem, i, MKTAG4('D','I','S','K'), ScummRp::_game.dataXorKey);
		if (ScummRp::_options & ScummRp::OPT_IMPORT)
			ScummRp::_explore(*disk, ScummRp::ACT_IMPORT);
		else
			ScummRp::_explore(*disk, ScummRp::ACT_EXPORT);
		ScummRp::_mergeTmpIndex();
	}

	ScummRp::_updateMainIndex();

	if (ScummRp::_options & ScummRp::OPT_IMPORT)
	{
		ScummIO::setQuiet(true);
		ScummRp::_exploreIndex<ScummRp::ACT_SAVE>(*index);
		ScummIO::setQuiet(false);
	}
}
