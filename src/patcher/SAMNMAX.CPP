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
	ScummIO::info(INF_GLOBAL, xsprintf("Patcher tool based on %s by %s", ScummRp::NAME, ScummRp::AUTHOR));
	ScummIO::info(INF_GLOBAL, "");

	if (SamnMaxPatcher::_invalidOptions())
	{
		SamnMaxPatcher::_usage();
		return -1;
	}

	g = ScummRp::_findGameDef(ScummRp::_paramGameId);
	if (g == -1)
	{
		SamnMaxPatcher::_usage();
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

	if (ScummRp::_game.id != GID_SAMNMAX)
		ScummIO::fatal("Only patching Sam and Max Hit the Road is supported");

	ScummRp::_options |= OPT_IMPORT | OPT_EXPORT;

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
	for (int i = 1; i < argc && argv[i] && SamnMaxPatcher::_readOption(argv[i], pendingParams); ++i)
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


bool SamnMaxPatcher::_readOption(const char *arg, char *pendingParams)
{
	int i;
	char c;

	i = 0;
	if (arg[i++] == '-')
	{
		while ((c = arg[i++]) != '\0')
		{
			switch (c)
			{
			case '-':
				return false;
				break;
			case 'q':
				ScummRp::_infoSlots = INF_NULL;
				break;
			case 'v':
				ScummRp::_infoSlots |= INF_DETAIL;
				break;
			case 'V':
				ScummRp::_infoSlots |= INF_LISTING;
				break;
			case 'g':
				ScummRp::_options |= ScummRp::OPT_GAME_FILES;
				ScummRp::_queueParam(pendingParams, c);
				break;
			case 'd':
			case 'p':
				ScummRp::_queueParam(pendingParams, c);
				break;
			default:
				ScummIO::fatal(xsprintf("Unrecognized \"-%c\" option", c));
				break;
			}
		}
	}

	return true;
}


void SamnMaxPatcher::_usage()
{
	std::cout << "options:\n\n";
	std::cout << " -p path    " << "path to the game (default: current directory)\n";
	std::cout << " -d path    " << "path to dumping directory (default: " << ScummRp::_paramDumpingDir << ")\n";
	std::cout << " -g gameid  " << "select a game\n";
	std::cout << " -q         " << "quiet mode\n";
	std::cout << " -v         " << "verbose mode\n";
	std::cout << " -V         " << "more verbose mode (lists blocks)\n\n";
	std::cout << "Examples:\n";
	std::cout << "patcher -g samnmax -p SAMNMAX\n";

	std::cout << "ALWAYS MAKE BACKUPS before making changes to your games!" << std::endl;
}

bool SamnMaxPatcher::_invalidOptions()
{
	static const uint32 exclusive[] = { 0 };
	static const uint32 mandatory[] = { 0 };

	for (int i = 0; mandatory[i] != 0; ++i)
		if (!(ScummRp::_options & mandatory[i]))
			return true;

	for (int i = 0; exclusive[i] != 0; ++i)
		if ((ScummRp::_options & exclusive[i]) == exclusive[i])
			return true;

	return false;
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
		SamnMaxPatcher::_patch(*disk);
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


void SamnMaxPatcher::_patch(TreeBlock &tree)
{
	TreeBlockPtr blockPtr;
	std::set<std::string> processedBlocks;
	std::string path, filename;

	tree.firstBlock();
	while ((blockPtr = tree.nextBlock()) != nullptr)
	{
		path = ScummRp::_paramDumpingDir;
		blockPtr->makePath(path, filename);

		if (blockPtr.is<LFLFPack>() || blockPtr.is<RoomBlock>())
		{
			if(SamnMaxPatcher::_shouldDescend(blockPtr))
				if (processedBlocks.find(filename) == processedBlocks.end())
					SamnMaxPatcher::_patch(*blockPtr);
				else
					ScummIO::warning(xsprintf("%s not unique. Only the first occurence was explored.", filename.c_str()));
		}
		else
		{
			switch (blockPtr->getTag())
			{
			case MKTAG4('L','E','C','F'):
			case MKTAG2('L','E'):
				if (processedBlocks.find(filename) != processedBlocks.end())
					ScummIO::warning(xsprintf("%s not unique. Only the first occurence was explored.", filename.c_str()));

				{
					TreeBlockPtr lecf;

					lecf = new LECFPack(*blockPtr);
					SamnMaxPatcher::_patch(*lecf);
				}
				break;
			default:
				if (ScummRp::_filterTag && blockPtr->getTag() != ScummRp::_filterTag)
					break;

				if (processedBlocks.find(filename) != processedBlocks.end())
					ScummIO::warning(xsprintf("%s not unique. Only the first occurence was processed.", filename.c_str()));

				_patchBlock(blockPtr, path, filename);
				processedBlocks.insert(filename);
			}
		}
	}
}


bool SamnMaxPatcher::_shouldDescend(TreeBlockPtr &blockPtr)
{
	if (blockPtr.is<RoomBlock>()) return true;
	if (blockPtr.is<LFLFPack>()) 
	{
		return blockPtr->getId() == 1;
	}
	return false;
}

void SamnMaxPatcher::_patchBlock(TreeBlockPtr &blockPtr, std::string &path, std::string &filename)
{
	void (*patchFunc)(std::string &) = nullptr;
	if (MKTAG4('L','S','C','R') == blockPtr->getTag() && 200 == blockPtr->getId() )
		patchFunc = SamnMaxPatcher::_patchScript200;

	if (!patchFunc) return;

	xmkdir(path.c_str());
	path += filename;
	blockPtr->dump(path.c_str());
	patchFunc(path);
	blockPtr->update(path.c_str());
}


extern "C" void patchScript200(const char *);

void SamnMaxPatcher::_patchScript200(std::string &fullpath)
{
	patchScript200(fullpath.c_str());
}
