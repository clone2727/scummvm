/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef SPRINT_SCRIPT_H
#define SPRINT_SCRIPT_H

#include "sprint/database.h"

#define DECLARE_OPCODE(x) void x(Context &context, const ScriptOpcode &op)

namespace Sprint {

class SprintEngine;

class ScriptManager {
public:
	ScriptManager(SprintEngine *vm);
	~ScriptManager();

	void execute(const Script &script);

private:
	SprintEngine *_vm;

	struct Context {
		Script script;
		Script::const_iterator curOp;
	};

	typedef void (ScriptManager::*OpcodeProc)(Context &context, const ScriptOpcode &op);

	struct Opcode {
		OpcodeProc proc;
		const char *desc;
	};

	const Opcode *_opcodes;
	void setupOpcodes();

	DECLARE_OPCODE(o_nop);

	DECLARE_OPCODE(o_playMovie);
};

} // End of namespace Sprint

#endif
