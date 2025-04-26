#pragma once

class HashTable;
extern HashTable ht;
extern bool open;

#include "raylib.h"
#include "../../header/Hash Table/HashTable.h"
#include "../../resource/tinyfiledialogs.h"
#include "../Font.h"
#include <string>
#include <fstream>
#include <locale>   
#include <codecvt>

extern HashTable ht;
extern bool runAtOnce;

void DrawHashTable();
void LoadFromFile();

