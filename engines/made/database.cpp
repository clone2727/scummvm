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
 * $URL$
 * $Id$
 *
 */

#include "common/system.h"
#include "common/endian.h"
#include "common/util.h"
#include "common/savefile.h"

#include "made/database.h"

namespace Made {

/*
	Class types:
	  0x7FFF  byte array
	  0x7FFE  word array
	< 0x7FFE  object
*/

Object::Object() : _objData(NULL), _freeData(false) {
}

Object::~Object() {
	if (_freeData && _objData)
		delete[] _objData;
}

int Object::loadVersion2(Common::SeekableReadStream &source) {

	if (_freeData && _objData)
		delete[] _objData;

	_freeData = true;
	
	byte header[4];
	source.read(header, 4);
	
	uint16 type = READ_LE_UINT16(header);
	if (type == 0x7FFF) {
		_objSize = READ_LE_UINT16(header + 2);
	} else if (type == 0x7FFE) {
		_objSize = READ_LE_UINT16(header + 2) * 2;
	} else if (type < 0x7FFE) {
		byte count1 = header[2];
		byte count2 = header[3];
		_objSize = (count1 + count2) * 2;
	}
	_objSize += 6;
	_objData = new byte[_objSize];
	WRITE_LE_UINT16(_objData, 1);
	memcpy(_objData + 2, header, 4);
	source.read(_objData + 6, _objSize - 6);

	return _objSize - 2;

}

int Object::saveVersion2(Common::WriteStream &dest) {
	dest.write(_objData + 2, _objSize - 2);
	return 0;
}

int Object::loadVersion3(Common::SeekableReadStream &source) {
	_freeData = true;
	source.readUint16LE(); // skip flags
	uint16 type = source.readUint16LE();
	if (type == 0x7FFF) {
		_objSize = source.readUint16LE();
	} else if (type == 0x7FFE) {
		_objSize = source.readUint16LE() * 2;
	} else if (type < 0x7FFE) {
		byte count1 = source.readByte();
		byte count2 = source.readByte();
		_objSize = (count1 + count2) * 2;
	}
	source.seek(-6, SEEK_CUR);
	_objSize += 6;
	_objData = new byte[_objSize];
	source.read(_objData, _objSize);
	return _objSize;
}

int Object::loadVersion3(byte *source) {
	_objData = source;
	_freeData = false;
	if (getClass() < 0x7FFE) {
		_objSize = (getCount1() + getCount2()) * 2;
	} else {
		_objSize = getSize();
	}
	_objSize += 6;
	return _objSize;
}

uint16 Object::getFlags() const {
	return READ_LE_UINT16(_objData);
}

uint16 Object::getClass() const {
	return READ_LE_UINT16(_objData + 2);
}

uint16 Object::getSize() const {
	return READ_LE_UINT16(_objData + 4);
}

byte Object::getCount1() const {
	return _objData[4];
}

byte Object::getCount2() const {
	return _objData[5];
}

byte *Object::getData() {
	return _objData + 6;
}

const char *Object::getString() {
	if (getClass() == 0x7FFF)
		return (const char*)getData();
	else
		return NULL;
}

void Object::setString(const char *str) {
	if (getClass() == 0x7FFF) {
		char *objStr = (char*)getData();
		if (str)
			strncpy(objStr, str, getSize());
		else
			objStr[0] = '\0';
	}
}

bool Object::isObject() {
	return getClass() < 0x7FFE;
}

bool Object::isVector() {
	return getClass() == 0x7FFF;
}

int16 Object::getVectorSize() {
	if (getClass() == 0x7FFF || getClass() == 0x7FFE) {
		return getSize();
	} else if (getClass() < 0x7FFE) {
		return getCount1() + getCount2();
	} else {
		// should never reach here
		error("Unknown object class");
		return 0;
	}
}

int16 Object::getVectorItem(int16 index) {
	if (getClass() == 0x7FFF) {
		byte *vector = (byte*)getData();
		return vector[index];
	} else if (getClass() == 0x7FFE) {
		int16 *vector = (int16*)getData();
		return READ_LE_UINT16(&vector[index]);
	} else if (getClass() < 0x7FFE) {
		int16 *vector = (int16*)getData();
		return READ_LE_UINT16(&vector[index]);
	} else {
		// should never reach here
		error("Unknown object class");
		return 0;
	}
}

void Object::setVectorItem(int16 index, int16 value) {
	if (getClass() == 0x7FFF) {
		byte *vector = (byte*)getData();
		vector[index] = value;
	} else if (getClass() <= 0x7FFE) {
		int16 *vector = (int16*)getData();
		WRITE_LE_UINT16(&vector[index], value);
	}
}

void Object::dump(const char *filename) {
	/*
	FILE *o = fopen(filename, "wb");
	fwrite(_objData, _objSize, 1, o);
	fclose(o);
	*/
}

GameDatabase::GameDatabase(MadeEngine *vm) : _vm(vm) {
	_gameText = NULL;
}

GameDatabase::~GameDatabase() {
	if (_gameState)
		delete[] _gameState;
	if (_gameText)
		delete[] _gameText;
}

void GameDatabase::open(const char *filename) {
	debug(1, "GameDatabase::open() Loading from %s", filename);
	Common::File fd;
	if (!fd.open(filename))
		error("GameDatabase::open() Could not open %s", filename);
	load(fd);
	fd.close();
}

void GameDatabase::openFromRed(const char *redFilename, const char *filename) {
	debug(1, "GameDatabase::openFromRed() Loading from %s->%s", redFilename, filename);
	Common::MemoryReadStream *fileS = RedReader::loadFromRed(redFilename, filename);
	if (!fileS)
		error("GameDatabase::openFromRed() Could not load %s from %s", filename, redFilename);
	load(*fileS);
	delete fileS;
}

void GameDatabase::load(Common::SeekableReadStream &sourceS) {

	if (_vm->getGameID() == GID_MANHOLE || _vm->getGameID() == GID_LGOP2) {
		debug(2, "loading version 2 dat");
		loadVersion2(sourceS);
	} else if (_vm->getGameID() == GID_RTZ) {
		debug(2, "loading version 3 dat");
		loadVersion3(sourceS);
	}

}

void GameDatabase::loadVersion2(Common::SeekableReadStream &sourceS) {
	
	// TODO: Read/verifiy header
	
	sourceS.seek(0x1C);
	
	uint32 textOffs = sourceS.readUint16LE() * 512;
	uint16 objectCount = sourceS.readUint16LE();
	uint16 varObjectCount = sourceS.readUint16LE();
	_gameStateSize = sourceS.readUint16LE() * 2;
	sourceS.readUint16LE(); // unknown
	uint32 objectsOffs = sourceS.readUint16LE() * 512;
	sourceS.readUint16LE(); // unknown
	_mainCodeObjectIndex = sourceS.readUint16LE();
	sourceS.readUint16LE(); // unknown
	uint32 objectsSize = sourceS.readUint32LE() * 2;
	uint32 textSize = objectsOffs - textOffs;

	debug(2, "textOffs = %08X; textSize = %08X; objectCount = %d; varObjectCount = %d; gameStateSize = %d; objectsOffs = %08X; objectsSize = %d\n", textOffs, textSize, objectCount, varObjectCount, _gameStateSize, objectsOffs, objectsSize);

	_gameState = new byte[_gameStateSize + 2];
	memset(_gameState, 0, _gameStateSize);
	setVar(1, objectCount);

	sourceS.seek(textOffs);
	_gameText = new char[textSize];
	sourceS.read(_gameText, textSize);
	// "Decrypt" the text data
	for (uint32 i = 0; i < textSize; i++)
		_gameText[i] += 0x1E;

	sourceS.seek(objectsOffs);

	for (uint32 i = 0; i < objectCount; i++) {
		Object *obj = new Object();
		int objSize = obj->loadVersion2(sourceS);
		// objects are aligned on 2-byte-boundaries, skip unused bytes
		sourceS.skip(objSize % 2);
		_objects.push_back(obj);
	}
	
}

void GameDatabase::loadVersion3(Common::SeekableReadStream &sourceS) {

	// TODO: Read/verifiy header

	sourceS.seek(0x1E);

	uint32 objectIndexOffs = sourceS.readUint32LE();
	uint16 objectCount = sourceS.readUint16LE();
	uint32 gameStateOffs = sourceS.readUint32LE();
	_gameStateSize = sourceS.readUint32LE();
	uint32 objectsOffs = sourceS.readUint32LE();
	uint32 objectsSize = sourceS.readUint32LE();
	_mainCodeObjectIndex = sourceS.readUint16LE();

	debug(2, "objectIndexOffs = %08X; objectCount = %d; gameStateOffs = %08X; gameStateSize = %d; objectsOffs = %08X; objectsSize = %d\n", objectIndexOffs, objectCount, gameStateOffs, _gameStateSize, objectsOffs, objectsSize);

	_gameState = new byte[_gameStateSize];
	sourceS.seek(gameStateOffs);
	sourceS.read(_gameState, _gameStateSize);

	Common::Array<uint32> objectOffsets;
	sourceS.seek(objectIndexOffs);
	for (uint32 i = 0; i < objectCount; i++)
		objectOffsets.push_back(sourceS.readUint32LE());

	for (uint32 i = 0; i < objectCount; i++) {
		Object *obj = new Object();

		// The LSB indicates if it's a constant or variable object.
		// Constant objects are loaded from disk, while variable objects exist
		// in the _gameState buffer.

		//debug(2, "obj(%04X) ofs = %08X\n", i, objectOffsets[i]);

		if (objectOffsets[i] & 1) {
			//debug(2, "-> const %08X\n", objectsOffs + objectOffsets[i] - 1);
			sourceS.seek(objectsOffs + objectOffsets[i] - 1);
			obj->loadVersion3(sourceS);
		} else {
			//debug(2, "-> var\n");
			obj->loadVersion3(_gameState + objectOffsets[i]);
		}
		_objects.push_back(obj);
	}

}

bool GameDatabase::getSavegameDescription(const char *filename, Common::String &description) {

	Common::InSaveFile *in;

	if (!(in = g_system->getSavefileManager()->openForLoading(filename))) {
		return false;
	}

	char desc[64];

	in->skip(4); // TODO: Verify marker 'SGAM'
	in->skip(4); // TODO: Verify size
	in->skip(2); // TODO: Verify version
	in->read(desc, 64);
	description = desc;

	delete in;
	
	return true;
	
}

int16 GameDatabase::savegame(const char *filename, const char *description, int16 version) {

	Common::OutSaveFile *out;
	int16 result = 0;

	if (!(out = g_system->getSavefileManager()->openForSaving(filename))) {
		warning("Can't create file '%s', game not saved", filename);
		return 6;
	}
	
	switch (_vm->_engineVersion) {
	case 2:
		result = savegameV2(out, description, version);
		break;
	case 3:
		result = savegameV3(out, description, version);
		break;
	default:
		error("GameDatabase::savegame() Unknown engine version");
	}

	delete out;

	return result;

}

int16 GameDatabase::loadgame(const char *filename, int16 version) {

	Common::InSaveFile *in;
	int16 result = 0;

	if (!(in = g_system->getSavefileManager()->openForLoading(filename))) {
		warning("Can't open file '%s', game not loaded", filename);
		return 1;
	}

	switch (_vm->_engineVersion) {
	case 2:
		result = loadgameV2(in, version);
		break;
	case 3:
		result = loadgameV3(in, version);
		break;
	default:
		error("GameDatabase::loadgame() Unknown engine version");
	}

	delete in;

	return result;

}

int16 GameDatabase::savegameV2(Common::OutSaveFile *out, const char *description, int16 version) {
	// Variable 0 is not saved
	out->write(_gameState + 2, _gameStateSize - 2);
	for (uint i = 0; i < _objects.size(); i++)
		_objects[i]->saveVersion2(*out);
	return 0;
}

int16 GameDatabase::loadgameV2(Common::InSaveFile *in, int16 version) {
	// Variable 0 is not loaded
	in->read(_gameState + 2, _gameStateSize - 2);
	for (uint i = 0; i < _objects.size(); i++)
		_objects[i]->loadVersion2(*in);
	return 0;
}

int16 GameDatabase::savegameV3(Common::OutSaveFile *out, const char *description, int16 version) {
	uint32 size = 4 + 4 + 2 + _gameStateSize;
	char desc[64];
	strncpy(desc, description, 64);
	out->writeUint32BE(MKID_BE('SGAM'));
	out->writeUint32LE(size);
	out->writeUint16LE(version);
	out->write(desc, 64);
	out->write(_gameState, _gameStateSize);
	return 0;
}

int16 GameDatabase::loadgameV3(Common::InSaveFile *in, int16 version) {
	//uint32 expectedSize = 4 + 4 + 2 + _gameStateSize;
	in->skip(4); // TODO: Verify marker 'SGAM'
	in->skip(4); // TODO: Verify size
	in->skip(2); // TODO: Verify version
	in->skip(64); // skip savegame description
	in->read(_gameState, _gameStateSize);
	return 0;
}

int16 GameDatabase::getVar(int16 index) {
	return (int16)READ_LE_UINT16(_gameState + index * 2);
}

void GameDatabase::setVar(int16 index, int16 value) {
	WRITE_LE_UINT16(_gameState + index * 2, value);
}

int16 *GameDatabase::getObjectPropertyPtrV2(int16 objectIndex, int16 propertyId, int16 &propertyFlag) {
	Object *obj = getObject(objectIndex);

	int16 *prop = (int16*)obj->getData();
	byte count1 = obj->getCount1();
	byte count2 = obj->getCount2();

	int16 *propPtr1 = prop + count1;
	int16 *propPtr2 = prop + count2;

	// First see if the property exists in the given object
	while (count2-- > 0) {
		if ((READ_LE_UINT16(prop) & 0x7FFF) == propertyId) {
			propertyFlag = obj->getFlags() & 1;
			return propPtr1;
		}
		prop++;
		propPtr1++;
	}

	// Now check in the object hierarchy of the given object
	int16 parentObjectIndex = obj->getClass();
	if (parentObjectIndex == 0) {
		debug(2, "GameDatabase::getObjectPropertyPtrV2() NULL(1)");
		return NULL;
	}

	while (parentObjectIndex != 0) {

		//debug(2, "parentObjectIndex = %04X\n", parentObjectIndex);

		obj = getObject(parentObjectIndex);

		prop = (int16*)obj->getData();
		count1 = obj->getCount1();
		count2 = obj->getCount2();

		propPtr1 = propPtr2 + count1 - count2;
		int16 *propertyPtr = prop + count1;

		while (count2-- > 0) {
			if (!(READ_LE_UINT16(prop) & 0x8000)) {
				if ((READ_LE_UINT16(prop) & 0x7FFF) == propertyId) {
					propertyFlag = obj->getFlags() & 1;
					return propPtr1;
				} else {
					propPtr1++;
				}
			} else {
				if ((READ_LE_UINT16(prop) & 0x7FFF) == propertyId) {
					propertyFlag = obj->getFlags() & 1;
					return propertyPtr;
				}
			}
			prop++;
			propertyPtr++;
		}

		parentObjectIndex = obj->getClass();

	}

 	debug(2, "GameDatabase::getObjectPropertyPtrV2() NULL(2)");
	return NULL;

}

int16 *GameDatabase::getObjectPropertyPtrV3(int16 objectIndex, int16 propertyId, int16 &propertyFlag) {
	Object *obj = getObject(objectIndex);

	int16 *prop = (int16*)obj->getData();
	byte count1 = obj->getCount1();
	byte count2 = obj->getCount2();

	int16 *propPtr1 = prop + count1;
	int16 *propPtr2 = prop + count2;

	// First see if the property exists in the given object
	while (count2-- > 0) {
		if ((READ_LE_UINT16(prop) & 0x3FFF) == propertyId) {
			if (READ_LE_UINT16(prop) & 0x4000) {
				propertyFlag = 1;
				return (int16*)_gameState + READ_LE_UINT16(propPtr1);
			} else {
				propertyFlag = obj->getFlags() & 1;
				return propPtr1;
			}
		}
		prop++;
		propPtr1++;
	}

	// Now check in the object hierarchy of the given object
	int16 parentObjectIndex = obj->getClass();
	if (parentObjectIndex == 0) {
		//debug(2, "! NULL(np)\n");
		return NULL;
	}

	while (parentObjectIndex != 0) {

		//debug(2, "parentObjectIndex = %04X\n", parentObjectIndex);

		obj = getObject(parentObjectIndex);

		prop = (int16*)obj->getData();
		count1 = obj->getCount1();
		count2 = obj->getCount2();

		propPtr1 = propPtr2 + count1 - count2;
		int16 *propertyPtr = prop + count1;

		while (count2-- > 0) {
			if (!(READ_LE_UINT16(prop) & 0x8000)) {
				if ((READ_LE_UINT16(prop) & 0x3FFF) == propertyId) {
					if (READ_LE_UINT16(prop) & 0x4000) {
						propertyFlag = 1;
						return (int16*)_gameState + READ_LE_UINT16(propPtr1);
					} else {
						propertyFlag = obj->getFlags() & 1;
						return propPtr1;
					}
				} else {
					propPtr1++;
				}
			} else {
				if ((READ_LE_UINT16(prop) & 0x3FFF) == propertyId) {
					if (READ_LE_UINT16(prop) & 0x4000) {
						propertyFlag = 1;
						return (int16*)_gameState + READ_LE_UINT16(propertyPtr);
					} else {
						propertyFlag = obj->getFlags() & 1;
						return propertyPtr;
					}
				}
			}
			prop++;
			propertyPtr++;
		}

		parentObjectIndex = obj->getClass();

	}

	//debug(2, "! NULL(nf)\n");
	return NULL;

}

int16 *GameDatabase::getObjectPropertyPtr(int16 objectIndex, int16 propertyId, int16 &propertyFlag) {
	switch (_vm->_engineVersion) {
	case 2:
		return getObjectPropertyPtrV2(objectIndex, propertyId, propertyFlag);
	case 3:
		return getObjectPropertyPtrV3(objectIndex, propertyId, propertyFlag);
	default:
		error("GameDatabase::getObjectPropertyPtr() Unknown engine version");
		return NULL;
	}
}

int16 GameDatabase::getObjectProperty(int16 objectIndex, int16 propertyId) {

	if (objectIndex == 0)
		return 0;

	int16 propertyFlag;
	int16 *property = getObjectPropertyPtr(objectIndex, propertyId, propertyFlag);
	
	if (property) {
		return (int16)READ_LE_UINT16(property);
	} else {
		return 0;
	}

}

int16 GameDatabase::setObjectProperty(int16 objectIndex, int16 propertyId, int16 value) {

	if (objectIndex == 0)
		return 0;

	int16 propertyFlag;
	int16 *property = getObjectPropertyPtr(objectIndex, propertyId, propertyFlag);
	
	if (property) {
		if (propertyFlag == 1) {
			WRITE_LE_UINT16(property, value);
		} else {
			warning("GameDatabase::setObjectProperty(%04X, %04X, %04X) Trying to set constant property\n",
				objectIndex, propertyId, value);
		}
		return value;
	} else {
		return 0;
	}
	
}

const char *GameDatabase::getString(uint16 offset) {
	return (const char*)&_gameText[offset * 4];
}

void GameDatabase::dumpObject(int16 index) {
	Object *obj = getObject(index);
	char fn[512];
	sprintf(fn, "obj%04X.0", index);
	obj->dump(fn);
}

} // End of namespace Made
