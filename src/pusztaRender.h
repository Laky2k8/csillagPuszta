#pragma once

#include "raylib.h"
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include "style.h"

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

static Style computeStyleForTag(const std::string &tag, const std::map<std::string, Style> &stylesheet)
{
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

std::vector<std::string> wrap_and_measure(HTMLRenderer &renderer, const std::string &text, Font font, int fontSize, float maxWidth);

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
	LayoutBox *parent = nullptr;

	std::string text;
};

static inline std::string trim_copy(const std::string &s)
{
    size_t a = 0;
    while (a < s.size() && isspace((unsigned char)s[a])) ++a;
    size_t b = s.size();
    while (b > a && isspace((unsigned char)s[b-1])) --b;
    return s.substr(a, b - a);
}

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
			LayoutBox *box = new LayoutBox();
			box->element = child;
			box->parent = parentBox;

			box->style = computeStyleForTag(child->tag, tagStyles);

			if (!child->content.empty()) 
			{
                box->text = trim_copy(child->content);
            }

			parentBox->children.push_back(box);
			
			build_layout_tree(child, box);
		}
		else
		{
			// Inline element -> flatten into parent
			if(!child->content.empty())
			{
				std::string trimmed = trim_copy(child->content);
				if(!trimmed.empty()) // only flatten if there's actually any content
				{
					if(!parentBox->text.empty()) parentBox->text += " ";
					parentBox->element->content += trimmed;
				}

				build_layout_tree(child, parentBox);
			}
		}
	}
}

void compute_layout(LayoutBox *root, HTMLRenderer &renderer, float parentX, float parentY, float parentWidth)
{
	root->x = parentX + root->style.paddingLeft;
	root->width = parentWidth - (root->style.paddingLeft + root->style.paddingRight);

	// Set the height for the root element, the rest will be calculated accordingly
	if(root->parent == nullptr)
	{
		root->y = parentY + root->style.paddingTop;
	}

	if(root->children.empty()) // If leaf (text only)
	{
		Font font = renderer.getFontForStyle(root->style);

		auto lines = wrap_and_measure(renderer, root->text, font, root->style.fontSize, root->width - root->style.paddingLeft - root->style.paddingRight);
		float lineHeight = (float)root->style.fontSize + 2; // Line spacing

		root->height = ( (float)lines.size() * lineHeight ) + root->style.paddingTop + root->style.paddingBottom;
	}

	else // Block with children
	{
		float cursorY = root->y + root->style.paddingTop;

		for(auto child : root->children)
		{
			
		}
	}
}


std::vector<std::string> wrap_and_measure(HTMLRenderer &renderer, const std::string &text, Font font, int fontSize, float maxWidth)
{
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string word;
    std::string current;

	while(iss >> word)
	{
		std::string test = current.empty() ? word : current + " " + word;
		Vector2 m = MeasureTextEx(font, test.c_str(), fontSize, 1);

		if(m.x > maxWidth && !current.empty())
		{
			lines.push_back(current);
			current = word;
		}
		else
		{
			current = test;
		}
	}

	if(!current.empty())
	{
		lines.push_back(current);
	}

	if(lines.empty())
	{
		lines.push_back(""); // Empty line for height measurement
	}
}