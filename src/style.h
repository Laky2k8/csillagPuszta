#pragma once

#include "raylib.h"

struct Style
{
	int fontSize;
	
	Color textColor;
	Color backgroundColor;

	int paddingTop;
	int paddingBottom;
	int paddingLeft;
	int paddingRight;

	int marginTop;
	int marginBottom;

	int fontFlag; // 0 = normal, 1 = bold, 2 = italic, 3 = bold+italic
};