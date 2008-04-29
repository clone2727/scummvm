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

#include "kyra/kyra_v3.h"
#include "kyra/timer.h"

namespace Kyra {

void KyraEngine_v3::resetItem(int index) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::resetItem(%d)", index);
	_itemList[index].id = 0xFFFF;
	_itemList[index].sceneId = 0xFFFF;
	_itemList[index].x = 0;
	_itemList[index].y = 0;
	_itemList[index].unk8 = 0;
}

void KyraEngine_v3::resetItemList() {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::resetItemList()");
	for (int i = 0; i < 50; ++i)
		resetItem(i);
}

void KyraEngine_v3::removeTrashItems() {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::removeTrashItems()");
	for (int i = 0; _trashItemList[i] != 0xFF; ++i) {
		for (int item = findItem(_trashItemList[i]); item != -1; item = findItem(_trashItemList[i])) {
			if (_itemList[item].sceneId != _mainCharacter.sceneId)
				resetItem(item);
			else
				break;
		}
	}
}

int KyraEngine_v3::findFreeItem() {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::findFreeItem()");
	for (int i = 0; i < 50; ++i) {
		if (_itemList[i].id == 0xFFFF)
			return i;
	}
	return -1;
}

int KyraEngine_v3::findFreeInventorySlot() {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::findFreeInventorySlot()");
	for (int i = 0; i < 10; ++i) {
		if (_mainCharacter.inventory[i] == 0xFFFF)
			return i;
	}
	return -1;
}

int KyraEngine_v3::findItem(uint16 sceneId, uint16 id) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::findItem(%u, %u)", sceneId, id);
	for (int i = 0; i < 50; ++i) {
		if (_itemList[i].id == id && _itemList[i].sceneId == sceneId)
			return i;
	}
	return -1;
}

int KyraEngine_v3::findItem(uint16 item) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::findItem(%u)", item);
	for (int i = 0; i < 50; ++i) {
		if (_itemList[i].id == item)
			return i;
	}
	return -1;
}

int KyraEngine_v3::countAllItems() {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::countAllItems()");
	int count = 0;

	for (int i = 0; i < 50; ++i) {
		if (_itemList[i].id != 0xFFFF)
			++count;
	}

	return count;
}

int KyraEngine_v3::checkItemCollision(int x, int y) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::checkItemCollision(%d, %d)", x, y);
	int itemIndex = -1;
	int maxItemY = -1;

	for (int i = 0; i < 50; ++i) {
		if (_itemList[i].id == 0xFFFF || _itemList[i].sceneId != _mainCharacter.sceneId)
			continue;

		const int x1 = _itemList[i].x - 11;
		const int x2 = _itemList[i].x + 10;

		if (x < x1 || x > x2)
			continue;

		const int y1 = _itemList[i].y - _itemBuffer1[_itemList[i].id] - 3;
		const int y2 = _itemList[i].y + 3;

		if (y < y1 || y > y2)
			continue;

		if (_itemList[i].y >= maxItemY) {
			itemIndex = i;
			maxItemY = _itemList[i].y;
		}
	}

	return itemIndex;
}

void KyraEngine_v3::setMouseCursor(uint16 item) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::setMouseCursor(%u)", item);
	int shape = 0;
	int hotX = 1;
	int hotY = 1;

	if (item != 0xFFFF) {
		hotX = 12;
		hotY = 19;
		shape = item+248;
	}

	if ((int16)item != _itemInHand)
		_screen->setMouseCursor(hotX, hotY, getShapePtr(shape));
}

void KyraEngine_v3::setItemMouseCursor() {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::setItemMouseCursor()");
	_handItemSet = _itemInHand;
	if (_itemInHand == -1)
		_screen->setMouseCursor(0, 0, _gameShapes[0]);
	else
		_screen->setMouseCursor(12, 19, _gameShapes[_itemInHand+248]);
}

void KyraEngine_v3::setHandItem(uint16 item) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::setHandItem(%u)", item);
	_screen->hideMouse();

	if (item == 0xFFFF) {
		removeHandItem();
	} else {
		setMouseCursor(item);
		_itemInHand = item;
	}

	_screen->showMouse();
}

void KyraEngine_v3::removeHandItem() {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::removeHandItem()");
	_screen->hideMouse();
	_screen->setMouseCursor(0, 0, _gameShapes[0]);
	_itemInHand = -1;
	_handItemSet = -1;
	_screen->showMouse();
}

bool KyraEngine_v3::dropItem(int unk1, uint16 item, int x, int y, int unk2) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::dropItem(%d, %d, %d, %d, %d)", unk1, item, x, y, unk2);

	if (_handItemSet <= -1)
		return false;

	if (processItemDrop(_mainCharacter.sceneId, item, x, y, unk1, unk2))
		return true;

	snd_playSoundEffect(13, 200);

	if (countAllItems() >= 50) {
		removeTrashItems();
		if (processItemDrop(_mainCharacter.sceneId, item, x, y, unk1, unk2))
			return true;

		if (countAllItems() >= 50)
			showMessageFromCCode(14, 0xB3, 0);
	}

	if (!_chatText)
		snd_playSoundEffect(13, 200);
	return false;
}

bool KyraEngine_v3::processItemDrop(uint16 sceneId, uint16 item, int x, int y, int unk1, int unk2) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::processItemDrop(%d, %d, %d, %d, %d, %d)", sceneId, item, x, y, unk1, unk2);

	int itemPos = checkItemCollision(x, y);

	if (unk1)
		itemPos = -1;

	if (itemPos >= 0) {
		exchangeMouseItem(itemPos, 1);	
		return true;
	}

	int freeItemSlot = -1;

	if (unk2 != 3) {
		for (int i = 0; i < 50; ++i) {
			if (_itemList[i].id == 0xFFFF) {
				freeItemSlot = i;
				break;
			}
		}
	}

	if (freeItemSlot < 0)
		return false;

	if (_mainCharacter.sceneId != sceneId) {
		_itemList[freeItemSlot].x = x;
		_itemList[freeItemSlot].y = y;
		_itemList[freeItemSlot].id = item;
		_itemList[freeItemSlot].unk8 = 1;
		_itemList[freeItemSlot].sceneId = sceneId;
		return true;
	}

	int itemHeight = _itemBuffer1[item];

	// no idea why it's '&&' here and not single checks for x and y
	if (x == -1 && y == -1) {
		x = _rnd.getRandomNumberRng(0x18, 0x128);
		y = _rnd.getRandomNumberRng(0x14, 0x87);
	}

	int posX = x, posY = y;
	int itemX = -1, itemY = -1;
	bool needRepositioning = true;

	while (needRepositioning) {
		if ((_screen->getDrawLayer(posX, posY) <= 1 && _screen->getDrawLayer2(posX, posY, itemHeight) <= 1 && isDropable(posX, posY)) || posY == 187) {
			int posX2 = posX, posX3 = posX;
			bool repositioning = true;

			while (repositioning) {
				if (isDropable(posX3, posY) && _screen->getDrawLayer2(posX3, posY, itemHeight) < 7 && checkItemCollision(posX3, posY) == -1) {
					itemX = posX3;
					itemY = posY;
					needRepositioning = false;
					repositioning = false;
				}

				if (isDropable(posX2, posY) && _screen->getDrawLayer2(posX2, posY, itemHeight) < 7 && checkItemCollision(posX2, posY) == -1) {
					itemX = posX2;
					itemY = posY;
					needRepositioning = false;
					repositioning = false;
				}

				if (repositioning) {
					posX3 = MAX(posX3 - 2, 24);
					posX2 = MIN(posX2 + 2, 296);

					if (posX3 <= 24 && posX2 >= 296)
						repositioning = false;
				}
			}
		}

		if (posY == 187)
			needRepositioning = false;
		else
			posY = MIN(posY + 2, 187);
	}

	if (itemX == -1 || itemY == -1)
		return false;

	if (unk1 == 3) {
		_itemList[freeItemSlot].x = itemX;
		_itemList[freeItemSlot].y = itemY;
		return true;
	} else if (unk1 == 2) {
		itemDropDown(x, y, itemX, itemY, freeItemSlot, item, 0);
	}

	itemDropDown(x, y, itemX, itemY, freeItemSlot, item, (unk1 == 0) ? 1 : 0);

	if (!unk1 && unk2) {
		int itemStr = 1;
		if (_lang == 1)
			itemStr = getItemCommandStringDrop(item);
		updateItemCommand(item, itemStr, 0xFF);
	}

	return true;
}

void KyraEngine_v3::itemDropDown(int startX, int startY, int dstX, int dstY, int itemSlot, uint16 item, int remove) {
	debugC(9, kDebugLevelMain, "KyraEngine_v2::itemDropDown(%d, %d, %d, %d, %d, %u, %d)", startX, startY, dstX, dstY, itemSlot, item, remove);
	if (startX == dstX && startY == dstY) {
		_itemList[itemSlot].x = dstX;
		_itemList[itemSlot].y = dstY;
		_itemList[itemSlot].id = item;
		_itemList[itemSlot].sceneId = _mainCharacter.sceneId;
		snd_playSoundEffect(0x0C, 0xC8);
		addItemToAnimList(itemSlot);
	} else {
		uint8 *itemShape = getShapePtr(item + 248);
		_screen->hideMouse();

		if (startY <= dstY) {
			int speed = 2;
			int curY = startY;
			int curX = startX - 12;

			backUpGfxRect32x32(curX, curY-16);
			while (curY < dstY) {
				restoreGfxRect32x32(curX, curY-16);

				curY = MIN(curY + speed, dstY);
				++speed;

				backUpGfxRect32x32(curX, curY-16);
				uint32 endDelay = _system->getMillis() + _tickLength;

				_screen->drawShape(0, itemShape, curX, curY-16, 0, 0);
				_screen->updateScreen();

				delayUntil(endDelay);
			}
			restoreGfxRect32x32(curX, curY-16);

			if (dstX != dstY || (dstY - startY > 16)) {
				snd_playSoundEffect(0x11, 0xC8);
				speed = MAX(speed, 6);
				int speedX = ((dstX - startX) << 4) / speed;
				int origSpeed = speed;
				speed >>= 1;

				if (dstY - startY <= 8)
					speed >>= 1;

				speed = -speed;

				curX = startX << 4;

				int x = 0, y = 0;
				while (--origSpeed) {
					curY = MIN(curY + speed, dstY);
					curX += speedX;
					++speed;

					x = (curX >> 4) - 8;
					y = curY - 16;
					backUpGfxRect32x32(x, y);

					uint16 endDelay = _system->getMillis() + _tickLength;
					_screen->drawShape(0, itemShape, x, y, 0, 0);
					_screen->updateScreen();

					restoreGfxRect32x32(x, y);

					delayUntil(endDelay);
				}

				restoreGfxRect32x32(x, y);
			} 
		}

		_itemList[itemSlot].x = dstX;
		_itemList[itemSlot].y = dstY;
		_itemList[itemSlot].id = item;
		_itemList[itemSlot].sceneId = _mainCharacter.sceneId;
		snd_playSoundEffect(0x0C, 0xC8);
		addItemToAnimList(itemSlot);
		_screen->showMouse();
	}

	if (remove)
		removeHandItem();
}

void KyraEngine_v3::exchangeMouseItem(int itemPos, int runScript) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::exchangeMouseItem(%d, %d)", itemPos, runScript);

	if (itemListMagic(_itemInHand, itemPos))
		return;

	if (_itemInHand == 43) {
		removeHandItem();
		return;
	}

	_screen->hideMouse();
	deleteItemAnimEntry(itemPos);

	int itemId = _itemList[itemPos].id;
	_itemList[itemPos].id = _itemInHand;
	_itemInHand = itemId;

	addItemToAnimList(itemPos);
	snd_playSoundEffect(0x0B, 0xC8);
	setMouseCursor(_itemInHand);
	int str2 = 0;

	if (_lang == 1)
		str2 = getItemCommandStringPickUp(itemId);

	updateItemCommand(itemId, str2, 0xFF);
	_screen->showMouse();

	if (runScript)
		runSceneScript6();
}

bool KyraEngine_v3::pickUpItem(int x, int y, int runScript) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::pickUpItem(%d, %d, %d)", x, y, runScript);
	int itemPos = checkItemCollision(x, y);

	if (itemPos <= -1)
		return false;

	if (_itemInHand >= 0) {
		exchangeMouseItem(itemPos, runScript);
	} else {
		_screen->hideMouse();
		deleteItemAnimEntry(itemPos);
		int itemId = _itemList[itemPos].id;
		_itemList[itemPos].id = 0xFFFF;
		snd_playSoundEffect(0x0B, 0xC8);
		setMouseCursor(itemId);
		int itemString = 0;

		if (_lang == 1)
			itemString = getItemCommandStringPickUp(itemId);

		updateItemCommand(itemId, itemString, 0xFF);
		_itemInHand = itemId;
		_screen->showMouse();

		if (runScript)
			runSceneScript6();
	}

	return true;
}

bool KyraEngine_v3::isDropable(int x, int y) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::isDropable(%d, %d)", x, y);
	if (y < 14 || y > 187)
		return false;

	x -= 12;

	for (int xpos = x; xpos < x + 24; ++xpos) {
		if (_screen->getShapeFlag1(xpos, y) == 0)
			return false;
	}

	return true;
}

bool KyraEngine_v3::itemListMagic(int handItem, int itemSlot) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::itemListMagic(%d, %d)", handItem, itemSlot);

	uint16 item = _itemList[itemSlot].id;
	if (_currentChapter == 1 && handItem == 3 && item == 3 && queryGameFlag(0x76)) {
		eelScript();
		return true;
	} else if ((handItem == 6 || handItem == 7) && item == 2) {
		int animObjIndex = -1;
		for (int i = 17; i <= 66; ++i) {
			if (_animObjects[i].shapeIndex2 == 250)
				animObjIndex = i;
		}

		assert(animObjIndex != -1);

		_screen->hideMouse();
		snd_playSoundEffect(0x93, 0xC8);
		for (int i = 109; i <= 141; ++i) {
			_animObjects[animObjIndex].shapeIndex = i+248;
			_animObjects[animObjIndex].needRefresh = true;
			delay(1*_tickLength, true);
		}

		deleteItemAnimEntry(itemSlot);
		_itemList[itemSlot].id = 0xFFFF;
		_screen->showMouse();
		return true;
	}

	if (_mainCharacter.sceneId == 51 && queryGameFlag(0x19B) && !queryGameFlag(0x19C)
		&& ((item == 63 && handItem == 56) || (item == 56 && handItem == 63))) {

		if (queryGameFlag(0x1AC)) {
			setGameFlag(0x19C);
			setGameFlag(0x1AD);
		} else {
			setGameFlag(0x1AE);
		}
		
		_timer->setCountdown(12, 1);
		_timer->enable(12);
	}

	for (int i = 0; _itemMagicTable[i] != 0xFF; i += 4) {
		if (_itemMagicTable[i+0] != handItem || _itemMagicTable[i+1] != item)
			continue;

		uint8 resItem = _itemMagicTable[i+2];
		uint8 newItem = _itemMagicTable[i+3];

		snd_playSoundEffect(0x0F, 0xC8);

		_itemList[itemSlot].id = (resItem == 0xFF) ? 0xFFFF : resItem;

		_screen->hideMouse();
		deleteItemAnimEntry(itemSlot);
		addItemToAnimList(itemSlot);
		
		if (newItem == 0xFE)
			removeHandItem();
		else if (newItem != 0xFF)
			setHandItem(newItem);
		_screen->showMouse();

		if (_lang != 1)
			updateItemCommand(resItem, 3, 0xFF);

		// Unlike the original we give points for when combining with scene items 
		if (resItem == 7) {
			updateScore(35, 100);
			delay(60*_tickLength, true);
		}

		return true;
	}

	return false;
}

bool KyraEngine_v3::itemInventoryMagic(int handItem, int invSlot) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::itemInventoryMagic(%d, %d)", handItem, invSlot);

	uint16 item = _mainCharacter.inventory[invSlot];
	if (_currentChapter == 1 && handItem == 3 && item == 3 && queryGameFlag(0x76)) {
		eelScript();
		return true;
	} else if ((handItem == 6 || handItem == 7) && item == 2) {
		_screen->hideMouse();
		snd_playSoundEffect(0x93, 0xC8);
		for (int i = 109; i <= 141; ++i) {
			_mainCharacter.inventory[invSlot] = i;
			_screen->drawShape(2, getShapePtr(invSlot+422), 0, 144, 0, 0);
			_screen->drawShape(2, getShapePtr(i+248), 0, 144, 0, 0);
			_screen->copyRegion(0, 144, _inventoryX[invSlot], _inventoryY[invSlot], 24, 20, 2, 0, Screen::CR_NO_P_CHECK);
			_screen->updateScreen();
			delay(1*_tickLength, true);
		}

		_mainCharacter.inventory[invSlot] = 0xFFFF;
		clearInventorySlot(invSlot, 0);
		_screen->showMouse();
		return true;
	}

	for (int i = 0; _itemMagicTable[i] != 0xFF; i += 4) {
		if (_itemMagicTable[i+0] != handItem || _itemMagicTable[i+1] != item)
			continue;

		uint8 resItem = _itemMagicTable[i+2];
		uint8 newItem = _itemMagicTable[i+3];

		snd_playSoundEffect(0x0F, 0xC8);

		_mainCharacter.inventory[invSlot] = (resItem == 0xFF) ? 0xFFFF : resItem;

		_screen->hideMouse();
		clearInventorySlot(invSlot, 0);
		drawInventorySlot(0, resItem, invSlot);
		
		if (newItem == 0xFE)
			removeHandItem();
		else if (newItem != 0xFF)
			setHandItem(newItem);
		_screen->showMouse();

		if (_lang != 1)
			updateItemCommand(resItem, 3, 0xFF);

		// Unlike the original we give points for every language
		if (resItem == 7) {
			updateScore(35, 100);
			delay(60*_tickLength, true);
		}

		return true;
	}

	return false;
}

int KyraEngine_v3::getItemCommandStringDrop(uint16 item) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::getItemCommandStringDrop(%u)", item);
	assert(item < _itemStringMapSize);
	int stringId = _itemStringMap[item];
	return _itemStringDrop[stringId];
}

int KyraEngine_v3::getItemCommandStringPickUp(uint16 item) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::getItemCommandStringPickUp(%u)", item);
	assert(item < _itemStringMapSize);
	int stringId = _itemStringMap[item];
	return _itemStringPickUp[stringId];
}

int KyraEngine_v3::getItemCommandStringInv(uint16 item) {
	debugC(9, kDebugLevelMain, "KyraEngine_v3::getItemCommandStringInv(%u)", item);
	assert(item < _itemStringMapSize);
	int stringId = _itemStringMap[item];
	return _itemStringInv[stringId];
}

} // end of namespace Kyra

