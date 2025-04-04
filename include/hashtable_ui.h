#pragma once

class HashTable;
extern HashTable ht;

#include "raylib.h"
#include "../include/HashTable.h"
#include "../lib/tinyfiledialogs.h"
#include <string>
#include <fstream>
#include <locale>   
#include <codecvt>

extern HashTable ht;

void DrawHashTable();
void LoadFromFile();

