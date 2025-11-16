#pragma once

#include "raylib.h"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <functional>
#include "pusztaHTML.h"
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
	{"h1", {48, BLACK, Color{255, 255, 255, 0}, 10, 10, 0, 0, 15, 15, 15, 15, 1}},
	{"h2", {38, BLACK, Color{255, 255, 255, 0}, 8, 8, 0, 0, 12, 12, 12, 12,  1}},
	{"h3", {32, BLACK, Color{255, 255, 255, 0}, 6, 6, 0, 0, 10, 10, 10, 10,  1}},
	{"p", {24, BLACK, Color{255, 255, 255, 0}, 4, 4, 0, 0, 8, 8, 8, 8,  0}},
	{"b", {24, BLACK, Color{255, 255, 255, 0}, 0, 0, 0, 0, 0, 0, 0, 0,  1}},
	{"strong", {24, BLACK, Color{255, 255, 255, 0}, 0, 0, 0, 0, 0, 0, 0, 0,  1}},
	{"em", {24, BLACK, Color{255, 255, 255, 0}, 0, 0, 0, 0, 0, 0, 0, 0,  2}},
	{"i", {24, BLACK, Color{255, 255, 255, 0}, 0, 0, 0, 0, 0, 0, 0, 0,  2}},
	{"span", {24, BLACK, Color{255, 255, 255, 0}, 0, 0, 0, 0, 0, 0, 0, 0,  0}},
	{"div", {24, BLACK, Color{255, 255, 255, 0}, 4, 4, 0, 0, 0, 0, 0, 0,  0}},
};

struct LayoutBox;
class HTMLRenderer;

struct LayoutBox
{
	float x, y, width, height;
	HTMLElement *element;
	Style style;
	std::vector<LayoutBox*> children;
	LayoutBox *parent = nullptr;

	std::string text;
};

std::vector<std::string> wrap_and_measure(HTMLRenderer &renderer, const std::string &text, Font font, int fontSize, float maxWidth);

static Style computeStyleForTag(const std::string &tag, const std::map<std::string, Style> &stylesheet)
{
	auto it = stylesheet.find(tag);
	if (it != stylesheet.end()) return it->second;

	// default style
	Style s;
	s.fontSize = 24;
	s.textColor = BLACK;
	s.backgroundColor = (Color){0,0,0,0};
	s.paddingTop = s.paddingBottom = s.paddingLeft = s.paddingRight = 0;
	s.marginTop = s.marginBottom = s.marginLeft = s.marginRight = 0;
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

		void paintLayout(LayoutBox *root)
		{
			if(root->style.backgroundColor.a > 0)
			{
				DrawRectangle((int)root->x, (int)root->y, (int)root->width, (int)root->height, root->style.backgroundColor);
			}

			if(!root->text.empty())
			{
				Font font = this->getFontForStyle(root->style);

				float contentX = root->x + root->style.paddingLeft;
				float contentY = root->y + root->style.paddingTop;

				float usableWidth = root->width - root->style.paddingLeft - root->style.paddingRight;

				auto lines = wrap_and_measure(*this, root->text, font, root->style.fontSize, usableWidth);
				float lineHeight = (float)root->style.fontSize + 2;

				for(const auto &line : lines)
				{
					drawElement(line, contentX, contentY, root->style);
					contentY += lineHeight;
				}
			}

			// paint children
			for(auto child : root->children)
			{
				paintLayout(child);
			}
		}
};


// HTML Layout Tree

static inline std::string trim_copy(const std::string &s)
{
	size_t a = 0;
	while (a < s.size() && isspace((unsigned char)s[a])) ++a;
	size_t b = s.size();
	while (b > a && isspace((unsigned char)s[b-1])) --b;
	return s.substr(a, b - a);
}

static LayoutBox* build_layout_tree(HTMLElement *elem, const std::map<std::string, Style> &stylesheet)
{
    if (!elem) return nullptr;

    // create a box for this element (caller's expectation: call with <body> and get a root box)
    LayoutBox *box = new LayoutBox();
    box->element = elem;
    box->style = computeStyleForTag(elem->tag, stylesheet);
    if (!elem->content.empty()) box->text = trim_copy(elem->content);

    for (auto child : elem->children)
	{
        // skip non-visual tags
        if (child->tag == "head" || child->tag == "meta" || child->tag == "script" || child->tag == "style" || child->tag == "link") continue;

        bool isBlock = (std::find(BLOCK_ELEMENTS.begin(), BLOCK_ELEMENTS.end(), child->tag) != BLOCK_ELEMENTS.end());

        if (isBlock)
		{
            LayoutBox *childBox = build_layout_tree(child, stylesheet);

            if (childBox)
			{
                childBox->parent = box;
                box->children.push_back(childBox);
            }
        }
		
		else
		{
            // inline: collect text into this box
            if (!child->content.empty())
			{
                std::string trimmed = trim_copy(child->content);
                if (!trimmed.empty()) // But only if there's actually any content
				{
                    if (!box->text.empty()) box->text += " ";
                    box->text += trimmed;
                }
            }

            // recurse and flatten grandchildren
            for (auto grand : child->children) {

                bool grandIsBlock = (std::find(BLOCK_ELEMENTS.begin(), BLOCK_ELEMENTS.end(), grand->tag) != BLOCK_ELEMENTS.end());
                if (grandIsBlock)
				{
                    LayoutBox *gbox = build_layout_tree(grand, stylesheet);
                    if (gbox)
					{
                        gbox->parent = box;
                        box->children.push_back(gbox);
                    }

                } 
				else
				{
                    // inline grandchild
                    std::function<void(HTMLElement*, LayoutBox*)> collectInline;
                    collectInline = [&](HTMLElement* el, LayoutBox* dest)
					{
                        if (!el->content.empty())
						{
                            std::string trimmed = trim_copy(el->content);
                            if (!trimmed.empty())
							{
                                if (!dest->text.empty()) dest->text += " ";
                                dest->text += trimmed;
                            }
                        }
                        for (auto e : el->children) collectInline(e, dest);
                    };
                    collectInline(grand, box);
                }
            }
        }
    }

    return box;
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
			child->parent = root;

			// Calculate children first
			for(auto child : root->children)
			{
				child->parent = root;

				// Set child Y to cursorY for now
				child->y = cursorY + child->style.marginTop;

				compute_layout(child, renderer, root->x + child->style.marginLeft, child->y, root->width  - child->style.marginLeft - child->style.marginRight);

				cursorY = child->y + child->height + child->style.marginBottom;
			}

			root->height = (cursorY - root->y) + root->style.paddingBottom;
		}
	}
}


void freeLayoutTree(LayoutBox *root) {
	if (!root) return;
	for (auto c : root->children) freeLayoutTree(c);
	delete root;
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