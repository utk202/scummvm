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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "groovie/groovie.h"
#include "groovie/logic/triangle.h"

namespace Groovie {

namespace {
extern const int8 triangleLookup1[12];
extern const int8 triangleLookup2[12];
extern const int8 triangleLookup3[12];
extern const int8 triangleLogicTable[924];
}

TriangleGame::TriangleGame() : _random("TriangleGame") {
	init();
}

void TriangleGame::run(byte *scriptVariables) {
	byte op = scriptVariables[3];
	int8 move;

	switch (op) {
	case 3:
		init();
		scriptVariables[3] = 0;
		return;

	case 4:
		move = sub03(2);
		break;

	case 5:
		move = sub03(1);
		break;

	default:
		setCell(scriptVariables[1] + 10 * scriptVariables[0], 2);
		scriptVariables[3] = sub02();

		if (scriptVariables[3] == 0) {
			move = sub03(1);
		} else {
			return;
		}
	}

	scriptVariables[0] = move / 10;
	scriptVariables[1] = move % 10;
	scriptVariables[3] = sub02();
}

void TriangleGame::init() {
	_triangleCellCount = 0;
	memset(_triangleCells, 0, 66);
}

int8 TriangleGame::sub02() {
	int8 v6[132];
	int8 v7[68];

	sub05(_triangleCells, v6, v7);

	for (int i = 0; v6[i] != 66; i++) {
		bool v1 = false;
		bool v2 = false;
		bool pl = false;

		// There could be several sections, each one
		// ends with 66. And the overall list ends with 66 too
		// Hence, two loops
		for (; v6[i] != 66; i++) {
			if (!triangleLogicTable[14 * v6[i] + 6])
				pl = true;
			if (!triangleLogicTable[14 * v6[i] + 7])
				v2 = true;
			if (!triangleLogicTable[14 * v6[i] + 8])
				v1 = true;
		}

		if (pl && v2 && v1)
			return _triangleCells[v6[i - 1]];
	}

	return 0;
}

int8 TriangleGame::sub03(int8 player) {
	int8 pickedMoves[4];
	int8 tempTriangle1[68];
	int8 tempTriangle2[68];
	int8 a6a[132];
	int8 tempTriangle3[68];
	int8 tempMoves[132];
	int8 pos;

	if (_triangleCellCount >= 2) {
		sub05(_triangleCells, tempMoves, tempTriangle3);
		sub07(tempMoves, _triangleCells, tempTriangle3, tempTriangle2, tempTriangle1, a6a);

		// Find move until valid one
		(pos = sub09(player, tempTriangle2, tempTriangle1, a6a, _triangleCells)) != 66 ||
		(pos = sub10(player, tempTriangle1, _triangleCells)) != 66 ||
		(pos = sub12(player, a6a, _triangleCells, tempTriangle1)) != 66 ||
		(pos = sub09(3 - player, tempTriangle2, tempTriangle1, a6a, _triangleCells));

		if (pos == 66) {
			pos = _random.getRandomNumber(65);

			int8 oldPos = pos;
			while (_triangleCells[pos]) {
				if (++pos > 65)
					pos = 0;
				if (oldPos == pos) {
					pos = 66;
					break;
				}
			}
		}
	} else {
		int8 max = 0;
		if (!_triangleCells[24]) {
			pickedMoves[0] = 24;
			max = 1;
		}

		if (!_triangleCells[31])
			pickedMoves[max++] = 31;
		if (!_triangleCells[32])
			pickedMoves[max++] = 32;
		if (max)
			pos = pickedMoves[_random.getRandomNumber(max - 1)];
		else
			pos = tempMoves[0]; // This is uninitalized in this branch
	}

	if (pos != 66)
		setCell(pos, player);

	return pos;
}

void TriangleGame::sub05(int8 *triangleCells, int8 *tempMoves, int8 *tempTriangle) {
	int8 dest[4];

	for (int i = 0; i < 66; i++)
		tempTriangle[i] = triangleCells[i];

	int v16 = 3;

	for (int j = 0; j < 66; ++j) {
		if (triangleCells[j]) {
			bool flag = false;

			copyLogicRow(j, triangleCells[j], dest);
			for (int8 *k = dest; *k != 66; k++) {
				if (j > *k) {
					if (flag) {
						if (tempTriangle[j] != tempTriangle[*k])
							replaceCells(tempTriangle, j, tempTriangle[j], tempTriangle[*k]);
					} else {
						flag = true;
						tempTriangle[j] = tempTriangle[*k];
					}
				}
			}
			if (!flag)
				tempTriangle[j] = v16++;
		}
	}

	int v11 = 0;

	if (v16 > 3) {
		for (int v12 = 3; v12 < v16; v12++) {
			int v14 = v11;

			for (int m = 0; m < 66; m++) {
				if (tempTriangle[m] == v12)
					tempMoves[v11++] = m;
			}

			if (v11 != v14)
				tempMoves[v11++] = 66;
		}
	}

	tempMoves[v11] = 66;
}

void TriangleGame::sub07(int8 *a1, int8 *triangleCells, int8 *a3, int8 *a4, int8 *a5, int8 *a6) {
}

int8 TriangleGame::sub09(int8 player, int8 *tempTriangle2, int8 *tempTriangle1, int8 *a4, int8 *triangleCells) {
	int8 movesTable[280];

	int numDir1 = 0;
	int numDir2 = 0;
	int numDir3 = 0;
	int numDir4 = 0;
	int row = 0;

	for (const int8 *tPtr = &triangleLogicTable[6]; tPtr < &triangleLogicTable[924]; tPtr += 14, row++) {
		if (!triangleCells[row] && (tempTriangle1[row] & (player == 1 ? 1 : 64)) != 0) {
			int c1 = 0, c2 = 0, c3 = 0;
			int mask = 0;

			copyLogicRow(row, player, movesTable);

			for (int8 *movPtr = movesTable; *movPtr != 66; ++movPtr) {
				int row2 = 0;

				mask |= tempTriangle1[*movPtr];

				for (const int8 *tPtr2 = &triangleLogicTable[6]; tPtr2 < &triangleLogicTable[924]; tPtr2 += 14, row2++) {
					if (tempTriangle2[row2] == tempTriangle2[*movPtr]) {
						c1 += (tPtr2[0] == 0) ? 1 : 0;
						c2 += (tPtr2[1] == 0) ? 1 : 0;
						c3 += (tPtr2[2] == 0) ? 1 : 0;
					}
				}
			}

			if (c1)
				mask &= ~4u;
			if (c2)
				mask &= ~8u;
			if (c3)
				mask &= ~2u;

			if (tPtr[0] || c1) {
				if (tPtr[1] || c2) {
					if (tPtr[2] || c3) {
						if (mask) {
							if (mask == 0xe) {
								movesTable[numDir2 + 76] = row;
								numDir2++;
							} else if (mask == 2 || mask == 8 || mask == 4) {
								movesTable[numDir4 + 212] = row;
								numDir4++;
							} else {
								movesTable[numDir3 + 144] = row;
								numDir3++;
							}
						}
					} else {
						movesTable[numDir1 + 8] = row;
						numDir1++;
					}
				} else {
					movesTable[numDir1 + 8] = row;
					numDir1++;
				}
			} else {
				movesTable[numDir1 + 8] = row;
				numDir1++;
			}
		}
	}

	if (numDir1)
		return movesTable[_random.getRandomNumber(numDir1 - 1) + 8];
	if (numDir2)
		return movesTable[_random.getRandomNumber(numDir2 - 1) + 76];
	if (numDir3)
		return movesTable[_random.getRandomNumber(numDir3 - 1) + 144];
	if (numDir4)
		return movesTable[_random.getRandomNumber(numDir4 - 1) + 212];

	return 66;
}

int8 TriangleGame::sub10(int8 player, int8 *a2, int8 *triangleCells) {
	int8 *destPtr;
	byte mask;
	int counter;
	int8 dest[76];

	mask = 0;
	counter = 0;

	if (player == 1)
		mask = 16;
	else if (player == 2)
		mask = 32;

	for (int i = 0; i < 66; ++i) {
		if (!triangleCells[i] && (mask & (byte)a2[i]) != 0) {
			copyLogicRow(i, player, dest);

			destPtr = dest;

			while (*destPtr != 66) {
				if ((a2[*destPtr] & 0xE) == 0xE) {
					counter++;
					dest[counter + 8] = i;
					break;
				}

				destPtr++;
			}
		}
	}

	if (counter)
		return dest[_random.getRandomNumber(counter - 1) + 8];

	return 66;
}

int8 TriangleGame::sub12(int8 player, int8 *a2, int8 *triangleCells, int8 *a4) {
	return 0;
}

int TriangleGame::sub13(int8 row, int8 *triangleCells, int8 *moves) {
	int pos = 0;

	for (int i = 0; i < 6; i++) {
		int8 v6 = triangleLogicTable[14 * row + i];

		if (v6 != -1 && !triangleCells[v6]) {
			int v7 = (i + 1) % 6;
			int8 v8 = triangleLogicTable[14 * row + v7];

			if (v8 != -1 && !triangleCells[v8]) {
				int8 v9 = triangleLogicTable[14 * v6 + v7];

				if (v9 != -1 && !triangleCells[v9])
					moves[pos++] = v9;
			}
		}
	}

	moves[pos] = 66;

	return pos;
}

void TriangleGame::setCell(int8 cellnum, int8 val) {
	if (cellnum >= 0 && cellnum < 66) {
		++_triangleCellCount;
		_triangleCells[cellnum] = val;
	}
}

void TriangleGame::copyLogicRow(int row, int8 key, int8 *dest) {
	int pos = 0;

	for (int i = 0; i < 6; i++) {
		int8 val = triangleLogicTable[14 * row + i];
		if (val != -1 && _triangleCells[val] == key)
			dest[pos++] = val;
	}

	dest[pos] = 66;
}

void TriangleGame::replaceCells(int8 *tempTriangle, int limit, int8 from, int8 to) {
	for (int i = 0; i <= limit; ++i) {
		if (tempTriangle[i] == from)
			tempTriangle[i] = to;
	}
}

int copyLookup(int8 *lookup, int8 *from, int8 *dest){
	int counter = 0;

	if (*lookup == 66) {
		*dest = 66;
		return counter;
	}

	for (; *lookup != 66; lookup++) {
		for (int8 *ptr = from; *ptr != 66; ptr++) {
			if (*ptr == *lookup )
				dest[counter++] = *lookup;
		}
	}

	dest[counter] = 66;

	return counter;
}

namespace {

const int8 triangleLookup1[12] = {
	0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66
};

const int8 triangleLookup2[12] = {
	0, 2, 5, 9, 14, 20, 27, 35, 44, 54, 65, 66
};

const int8 triangleLookup3[12] = {
	55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66
};

const int8 triangleLogicTable[924] = {
	-1, -1,  2,  1, -1, -1,  0,  0, 10, 10,  6,  0, 10, 10,
	 0,  2,  4,  3, -1, -1,  0,  1,  9,  9,  5,  1,  9,  9,
	-1, -1,  5,  4,  1,  0,  1,  0,  9, 11,  5,  1,  9,  9,
	 1,  4,  7,  6, -1, -1,  0,  2,  8,  8,  4,  2,  8,  8,
	 2,  5,  8,  7,  3,  1,  1,  1,  8, 10,  4,  1,  8,  8,
	-1, -1,  9,  8,  4,  2,  2,  0,  8, 12,  4,  2,  8,  8,
	 3,  7, 11, 10, -1, -1,  0,  3,  7,  7,  3,  3,  7,  7,
	 4,  8, 12, 11,  6,  3,  1,  2,  7,  9,  3,  2,  7,  7,
	 5,  9, 13, 12,  7,  4,  2,  1,  7, 11,  3,  2,  7,  7,
	-1, -1, 14, 13,  8,  5,  3,  0,  7, 13,  3,  3,  7,  7,
	 6, 11, 16, 15, -1, -1,  0,  4,  6,  6,  3,  4,  6,  6,
	 7, 12, 17, 16, 10,  6,  1,  3,  6,  8,  2,  3,  6,  6,
	 8, 13, 18, 17, 11,  7,  2,  2,  6, 10,  2,  2,  6,  6,
	 9, 14, 19, 18, 12,  8,  3,  1,  6, 12,  2,  3,  6,  6,
	-1, -1, 20, 19, 13,  9,  4,  0,  6, 14,  3,  4,  6,  6,
	10, 16, 22, 21, -1, -1,  0,  5,  5,  5,  3,  5,  5,  5,
	11, 17, 23, 22, 15, 10,  1,  4,  5,  7,  2,  4,  5,  5,
	12, 18, 24, 23, 16, 11,  2,  3,  5,  9,  1,  3,  5,  5,
	13, 19, 25, 24, 17, 12,  3,  2,  5, 11,  1,  3,  5,  5,
	14, 20, 26, 25, 18, 13,  4,  1,  5, 13,  2,  4,  5,  5,
	-1, -1, 27, 26, 19, 14,  5,  0,  5, 15,  3,  5,  5,  5,
	15, 22, 29, 28, -1, -1,  0,  6,  4,  4,  3,  6,  6,  4,
	16, 23, 30, 29, 21, 15,  1,  5,  4,  6,  2,  5,  5,  4,
	17, 24, 31, 30, 22, 16,  2,  4,  4,  8,  1,  4,  4,  4,
	18, 25, 32, 31, 23, 17,  3,  3,  4, 10,  0,  3,  4,  4,
	19, 26, 33, 32, 24, 18,  4,  2,  4, 12,  1,  4,  4,  4,
	20, 27, 34, 33, 25, 19,  5,  1,  4, 14,  2,  5,  4,  5,
	-1, -1, 35, 34, 26, 20,  6,  0,  4, 16,  3,  6,  4,  6,
	21, 29, 37, 36, -1, -1,  0,  7,  3,  3,  3,  7,  7,  3,
	22, 30, 38, 37, 28, 21,  1,  6,  3,  5,  2,  6,  6,  3,
	23, 31, 39, 38, 29, 22,  2,  5,  3,  7,  1,  5,  5,  3,
	24, 32, 40, 39, 30, 23,  3,  4,  3,  9,  0,  4,  4,  3,
	25, 33, 41, 40, 31, 24,  4,  3,  3, 11,  0,  4,  3,  4,
	26, 34, 42, 41, 32, 25,  5,  2,  3, 13,  1,  5,  3,  5,
	27, 35, 43, 42, 33, 26,  6,  1,  3, 15,  2,  6,  3,  6,
	-1, -1, 44, 43, 34, 27,  7,  0,  3, 17,  3,  7,  3,  7,
	28, 37, 46, 45, -1, -1,  0,  8,  2,  2,  4,  8,  8,  2,
	29, 38, 47, 46, 36, 28,  1,  7,  2,  4,  3,  7,  7,  2,
	30, 39, 48, 47, 37, 29,  2,  6,  2,  6,  2,  6,  6,  2,
	31, 40, 49, 48, 38, 30,  3,  5,  2,  8,  1,  5,  5,  3,
	32, 41, 50, 49, 39, 31,  4,  4,  2, 10,  1,  4,  4,  4,
	33, 42, 51, 50, 40, 32,  5,  3,  2, 12,  1,  5,  3,  5,
	34, 43, 52, 51, 41, 33,  6,  2,  2, 14,  2,  6,  2,  6,
	35, 44, 53, 52, 42, 34,  7,  1,  2, 16,  3,  7,  2,  7,
	-1, -1, 54, 53, 43, 35,  8,  0,  2, 18,  4,  8,  2,  8,
	36, 46, 56, 55, -1, -1,  0,  9,  1,  1,  5,  9,  9,  1,
	37, 47, 57, 56, 45, 36,  1,  8,  1,  3,  4,  8,  8,  1,
	38, 48, 58, 57, 46, 37,  2,  7,  1,  5,  3,  7,  7,  2,
	39, 49, 59, 58, 47, 38,  3,  6,  1,  7,  2,  6,  6,  3,
	40, 50, 60, 59, 48, 39,  4,  5,  1,  9,  2,  5,  5,  4,
	41, 51, 61, 60, 49, 40,  5,  4,  1, 11,  2,  5,  4,  5,
	42, 52, 62, 61, 50, 41,  6,  3,  1, 13,  2,  6,  3,  6,
	43, 53, 63, 62, 51, 42,  7,  2,  1, 15,  3,  7,  2,  7,
	44, 54, 64, 63, 52, 43,  8,  1,  1, 17,  4,  8,  1,  8,
	-1, -1, 65, 64, 53, 44,  9,  0,  1, 19,  5,  9,  1,  9,
	45, 56, -1, -1, -1, -1,  0, 10,  0,  0,  6, 10, 10,  0,
	46, 57, -1, -1, 55, 45,  1,  9,  0,  2,  5,  9,  9,  1,
	47, 58, -1, -1, 56, 46,  2,  8,  0,  4,  4,  8,  8,  2,
	48, 59, -1, -1, 57, 47,  3,  7,  0,  6,  3,  7,  7,  3,
	49, 60, -1, -1, 58, 48,  4,  6,  0,  8,  3,  6,  6,  4,
	50, 61, -1, -1, 59, 49,  5,  5,  0, 10,  3,  5,  5,  5,
	51, 62, -1, -1, 60, 50,  6,  4,  0, 12,  3,  6,  4,  6,
	52, 63, -1, -1, 61, 51,  7,  3,  0, 14,  3,  7,  3,  7,
	53, 64, -1, -1, 62, 52,  8,  2,  0, 16,  4,  8,  2,  8,
	54, 65, -1, -1, 63, 53,  9,  1,  0, 18,  5,  9,  1,  9,
	-1, -1, -1, -1, 64, 54, 10,  0,  0, 20,  6, 10,  0, 10
};

} // End of anonymous namespace


} // End of Groovie namespace