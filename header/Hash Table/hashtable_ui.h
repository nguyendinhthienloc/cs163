#pragma once

class HashTable;
extern HashTable ht;

#include "raylib.h"
#include "../../header/Hash Table/HashTable.h"
#include "../../resource/tinyfiledialogs.h"
#include <string>
#include <fstream>
#include <locale>   
#include <codecvt>

extern HashTable ht;

void DrawHashTable();
void LoadFromFile();

