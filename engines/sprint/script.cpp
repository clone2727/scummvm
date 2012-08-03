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

#include "common/events.h"
#include "common/system.h"
#include "graphics/surface.h"
#include "video/qt_decoder.h"

#include "sprint/script.h"
#include "sprint/sprint.h"

namespace Sprint {

ScriptManager::ScriptManager(SprintEngine *vm) : _vm(vm) {
	setupOpcodes();
}

ScriptManager::~ScriptManager() {
}

#define OPCODE(x) { &ScriptManager::x, #x }
#define NOP OPCODE(o_nop)

void ScriptManager::setupOpcodes() {
	static const Opcode opcodes[] = {
		// 0x00 (0)
		NOP,
		NOP,
		NOP,
		OPCODE(o_drawImage),
		// 0x04 (4)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x08 (8)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x0C (12)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x10 (16)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x14 (20)
		NOP,
		NOP,
		NOP,
		NOP, // set variable to zero?
		// 0x18 (24)
		NOP, // set variable to one?
		NOP, // set variable to value?
		NOP,
		NOP,
		// 0x1C (28)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x20 (32)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x24 (36)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x28 (40)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x2C (44)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x30 (48)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x34 (52)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x38 (56)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x3C (60)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x40 (64)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x44 (68)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x48 (72)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x4C (76)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x50 (80)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x54 (84)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x58 (88)
		NOP,
		NOP, // change to node (?) - first arg: node, second/third args: unknown
		NOP,
		NOP,
		// 0x5C (92)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x60 (96)
		OPCODE(o_playMovie), // play movie (?) - five args
		NOP,
		OPCODE(o_playMovie), // play movie (?) - five args
		NOP,
		// 0x64 (100)
		NOP,
		NOP,
		NOP,
		OPCODE(o_changeAgeRoomNode),
		// 0x68 (104)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x6C (108)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x70 (112)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x74 (116)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x78 (120)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x7C (124)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x80 (128)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x84 (132)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x88 (136)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x8C (140)
		OPCODE(o_playSound),
		NOP,
		NOP,
		NOP,
		// 0x90 (144)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x94 (148)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x98 (152)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0x9C (156)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xA0 (160)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xA4 (164)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xA8 (168)
		NOP,
		NOP, // run sound script (?)
		NOP,
		NOP,
		// 0xAC (172)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xB0 (176)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xB4 (180)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xB8 (184)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xBC (188)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xC0 (192)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xC4 (196)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xC8 (200)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xCC (204)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xD0 (208)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xD4 (212)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xD8 (216)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xDC (220)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xE0 (224)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xE4 (228)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xE8 (232)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xEC (236)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xF0 (240)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xF4 (244)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xF8 (248)
		NOP,
		NOP,
		NOP,
		NOP,
		// 0xFC (252)
		NOP,
		NOP,
		NOP,
		NOP
	};

	_opcodes = opcodes;
}

#undef OPCODE
#undef NOP

void ScriptManager::execute(const Script &script) {
	Context context;
	context.script = script;

	for (context.curOp = context.script.begin(); context.curOp != context.script.end(); context.curOp++) {
		const ScriptOpcode &op = *context.curOp;
		(this->*(_opcodes[op.op].proc))(context, op);
	}
}

void ScriptManager::o_nop(Context &context, const ScriptOpcode &op) {
	warning("Unknown opcode %d", op.op);
}

void ScriptManager::o_drawImage(Context &context, const ScriptOpcode &op) {
	warning("STUB: Draw image %d", op.args[0]);
}

void ScriptManager::o_playMovie(Context &context, const ScriptOpcode &op) {
	// HACK: Just throwing something together to play a video for now
	// This opcode is definitely movie related since the first argument
	// contains the logo movies in an intro script

	Common::String movieName = _vm->_database->getMovieName(op.args[0]);

	Video::QuickTimeDecoder decoder;
	if (!decoder.loadFile(movieName))
		error("Failed to open movie '%s'", movieName.c_str());

	uint16 x = (g_system->getWidth() - decoder.getWidth()) / 2;
	uint16 y = (g_system->getHeight() - decoder.getHeight()) / 2;

	decoder.start();

	while (!_vm->shouldQuit() && !decoder.endOfVideo()) {
		if (decoder.needsUpdate()) {
			const Graphics::Surface *frame = decoder.decodeNextFrame();

			if (frame) {
				if (frame->format != g_system->getScreenFormat()) {
					Graphics::Surface *tempFrame = frame->convertTo(g_system->getScreenFormat());
					g_system->copyRectToScreen(tempFrame->getPixels(), tempFrame->pitch, x, y, tempFrame->w, tempFrame->h);
					tempFrame->free();
					delete tempFrame;
				} else {
					g_system->copyRectToScreen(frame->getPixels(), frame->pitch, x, y, frame->w, frame->h);					
				}

				g_system->updateScreen();
			}
		}

		Common::Event event;
		while (g_system->getEventManager()->pollEvent(event))
			;

		g_system->delayMillis(10);
	}
}

void ScriptManager::o_changeAgeRoomNode(Context &context, const ScriptOpcode &op) {
	_vm->changeToNode(op.args[0], op.args[1], op.args[2]);
}

void ScriptManager::o_playSound(Context &context, const ScriptOpcode &op) {
	warning("STUB: Play sound '%s' at volume %d", _vm->_database->getSoundName(op.args[0]).c_str(), op.args[1] * 255 / 100);
}

} // End of namespace Sprint
