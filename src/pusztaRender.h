#pragma once

#include "raylib.h"
#include <string>
#include <vector>
#include "style.h"

std::vector<std::string> BLOCK_ELEMENTS = {
    "html", "body", "article", "section", "nav", "aside",
    "h1", "h2", "h3", "h4", "h5", "h6", "hgroup", "header",
    "footer", "address", "p", "hr", "pre", "blockquote",
    "ol", "ul", "menu", "li", "dl", "dt", "dd", "figure",
    "figcaption", "main", "div", "table", "form", "fieldset",
    "legend", "details", "summary"
};

class HTMLRenderer
{
	private:
		Font normal;
		Font bold;
		Font italic;
		Font bold_italic;

	public:

		HTMLRenderer(std::string normalPath, std::string boldPath, std::string italicPath, std::string boldItalicPath)
		{
			this->normal = LoadFont(normalPath.c_str());
			this->bold = LoadFont(boldPath.c_str());
			this->italic = LoadFont(italicPath.c_str());
			this->bold_italic = LoadFont(boldItalicPath.c_str());
		}

		HTMLRenderer(Font normal, Font bold, Font italic, Font bold_italic)
		{
			this->normal = normal;
			this->bold = bold;
			this->italic = italic;
			this->bold_italic = bold_italic;
		}


		// Setter
		void setFont(std::string normalPath, std::string boldPath, std::string italicPath, std::string boldItalicPath)
		{
			this->normal = LoadFont(normalPath.c_str());
			this->bold = LoadFont(boldPath.c_str());
			this->italic = LoadFont(italicPath.c_str());
			this->bold_italic = LoadFont(boldItalicPath.c_str());
		}

		void setFont(Font normal, Font bold, Font italic, Font bold_italic)
		{
			this->normal = normal;
			this->bold = bold;
			this->italic = italic;
			this->bold_italic = bold_italic;
		}


		// draw text
		void drawElement(std::string text, float x, float y, Style style)
		{
			switch(style.fontFlag)
			{
				case 0: // normal
					DrawTextEx(this->normal, text.c_str(), {x, y}, style.fontSize, 1, style.textColor);
					break;

				case 1: // bold
					DrawTextEx(this->bold, text.c_str(), {x, y}, style.fontSize, 1, style.textColor);
					break;

				case 2: // italic
					DrawTextEx(this->italic, text.c_str(), {x, y}, style.fontSize, 1, style.textColor);
					break;

				case 3: // bold italic
					DrawTextEx(this->bold_italic, text.c_str(), {x, y}, style.fontSize, 1, style.textColor);
					break;
			}
		}
};


// HTML Layout Tree
struct LayoutBox
{
	float x, y, width, height;
	HTMLElement *element;
	Style style;
	std::vector<LayoutBox*> children;
};

void build_layout_tree(HTMLElement *elem, LayoutBox *parentBox)
{
	for(HTMLElement *child : elem->children)
	{
		// Skip non-displayed tags
		if (child->tag == "head" || child->tag == "meta" || child->tag == "script")
		{
            continue;
        }
	}
}
