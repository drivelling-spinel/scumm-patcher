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

#ifndef PATCHER_SAMNMAX_HPP
#define PATCHER_SAMNMAX_HPP


#include "scummrp/scummrp.hpp"

class SamnMaxPatcher : public ScummRp
{

private:
	static void _patchScript200(std::string &fullpath);

protected:
	static void _getOptions(int argc, const char **argv, const ScummRp::Parameter *params);
	static void _processGameFilesV4567();
	static bool _readOption(const char *arg, char *pendingParams);
	static void _usage();
	static bool _invalidOptions();
	static void _patch(TreeBlock &tree);
	static bool _shouldDescend(TreeBlockPtr &blockPtr);
	static void _patchBlock(TreeBlockPtr &blockPtr, std::string &path, std::string &filename);

public:
	static int main(int argc, const char **argv);

private:
	SamnMaxPatcher();
};

#endif
