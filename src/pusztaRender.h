#pragma once

#include "raylib.h"
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include "style.h"

std::vector<std::string> wrap_and_measure(HTMLRenderer &renderer, LayoutBox *box, float maxWidth);

std::vector<std::string> BLOCK_ELEMENTS = {
    "html", "body", "article", "section", "nav", "aside",
    "h1", "h2", "h3", "h4", "h5", "h6", "hgroup", "header",
    "footer", "address", "p", "hr", "pre", "blockquote",
    "ol", "ul", "menu", "li", "dl", "dt", "dd", "figure",
    "figcaption", "main", "div", "table", "form", "fieldset",
    "legend", "details", "summary"
};

// Styles
std::map<std::string, Style> tagStyles = {
    {"h1", {32, BLACK, Color{255, 255, 255, 0}, 10, 10, 0, 0, 15, 15, 1}},
    {"h2", {28, BLACK, Color{255, 255, 255, 0}, 8, 8, 0, 0, 12, 12, 1}},
    {"h3", {24, BLACK, Color{255, 255, 255, 0}, 6, 6, 0, 0, 10, 10, 1}},
    {"p", {16, BLACK, Color{255, 255, 255, 0}, 4, 4, 0, 0, 8, 8, 0}},
    {"b", {16, BLACK, Color{255, 255, 255, 0}, 0, 0, 0, 0, 0, 0, 1}},
    {"strong", {16, BLACK, Color{255, 255, 255, 0}, 0, 0, 0, 0, 0, 0, 1}},
    {"em", {16, BLACK, Color{255, 255, 255, 0}, 0, 0, 0, 0, 0, 0, 2}},
    {"i", {16, BLACK, Color{255, 255, 255, 0}, 0, 0, 0, 0, 0, 0, 2}},
    {"span", {16, BLACK, Color{255, 255, 255, 0}, 0, 0, 0, 0, 0, 0, 0}},
    {"div", {16, BLACK, Color{255, 255, 255, 0}, 4, 4, 0, 0, 0, 0, 0}},
};

static Style computeStyleForTag(const std::string &tag, const std::map<std::string, Style> &stylesheet) {
    auto it = stylesheet.find(tag);
    if (it != stylesheet.end()) return it->second;
    // default style
    Style s;
    s.fontSize = 20;
    s.textColor = BLACK;
    s.backgroundColor = (Color){0,0,0,0};
    s.paddingTop = s.paddingBottom = s.paddingLeft = s.paddingRight = 0;
    s.marginTop = s.marginBottom = 0;
    s.fontFlag = 0;
    return s;
}

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
			DrawTextEx(getFontForStyle(style), text.c_str(), {x, y}, style.fontSize, 1, style.textColor);
		}

		Font getFontForStyle(const Style &s)
		{
			switch(s.fontFlag)
			{
				case 0: return this->normal;
				case 1: return this->bold;
				case 2: return this->italic;
				case 3: return this->bold_italic;
				default: return this->normal;
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

	std::vector<std::string> lines;
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

		bool isBlock = (std::find(BLOCK_ELEMENTS.begin(), BLOCK_ELEMENTS.end(), child->tag) != BLOCK_ELEMENTS.end());

		if(isBlock)
		{
			// Create new layout box and continue with the element's children
			LayoutBox *box = new LayoutBox{0, 0, 0, 0, child, Style{}, {}};

			if(elem->tag == "h1") box->style.fontSize = 30;
			else box->style.fontSize = 20;

			parentBox->children.push_back(box);
			build_layout_tree(child, box);
		}
		else
		{
			// Inline element -> flatten into parent
			if(!child->content.empty())
			{
				parentBox->element->content += child->content;
			}

			build_layout_tree(child, parentBox);
		}
	}
}


std::vector<std::string> wrap_and_measure(HTMLRenderer &renderer, LayoutBox *box, float maxWidth)
{
	std::vector<std::string> lines;
	std::istringstream words(box->element->content);
	std::string word;
	std::string line;
	Font fontToUse = renderer.getFontForStyle(box->style);
	int fontSize = box->style.fontSize;
}