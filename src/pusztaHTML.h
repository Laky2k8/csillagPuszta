#pragma once
#include <string>
#include <vector>
#include <set>

// PusztaHTML - based off of https://devtails.xyz/@adam/how-to-build-an-html-parser-in-c++

class HTMLElement
{
	public:
		std::vector<struct HTMLElement *> children;
		struct HTMLElement *parentElem;

		std::string tag;
		std::string content;

		/*HTMLElement(const std::string &tag_name, const std::string &textContent)
		{
			tag = tag_name;
			content = textContent;
			parent = nullptr;
		}*/
};

// HTML parsing state machine
enum State
{
	STATE_INIT, // init
	STATE_START_TAG, // start a new tag
	STATE_READING_TAG, // reading the name of the tag itself
	STATE_READING_ATTRIBUTES, // reading the attributes of the tag
	STATE_END_TAG, // tag ended
	STATE_BEGIN_CLOSING_TAG // the current tag is a closing tag
};

bool isWhitespace(char c)
{
	return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

bool isSelfClosingTag(const std::string& tag)
{
	static const std::set<std::string> selfClosing = {
		"meta", "link", "br", "hr", "img", "input", 
		"area", "base", "col", "embed", "param", "source", "track", "wbr", "!doctype"
	};
	return selfClosing.find(tag) != selfClosing.end();
}

HTMLElement *HTMLParser(std::string input)
{
	HTMLElement *root = new HTMLElement(); // The root element, all elements are children of this

	State state = STATE_INIT;
	HTMLElement *lastParent = root;
	std::string tagName = "";

	for (size_t i = 0; i < input.size(); ++i)
	{
		char c = input[i];

		if(state == STATE_INIT && c == '<')
		{
			state = STATE_START_TAG;
		}

		else if(state == STATE_START_TAG)
		{
			if(c == '/') // Closing tag
			{
				state = STATE_BEGIN_CLOSING_TAG;
			}

			else if(c == '!') // Comment or doctype, skip for now
			{
				// Skip until ">"
				while (i + 1 < input.size() && input[i] != '>')
				{
					++i;
				}
				
				state = STATE_END_TAG;
				continue;
			}
			
			else if(!isWhitespace(c))
			{
				state = STATE_READING_TAG;
				tagName = c;
			}
		}

		else if(state == STATE_READING_TAG)
		{
			if(isWhitespace(c)) // Space -> start reading the attributes
			{
				state = STATE_READING_ATTRIBUTES;
			}

			else if(c == '>') // Tag ended
			{
				state = STATE_END_TAG;

				auto node = new HTMLElement();
				node->tag = tagName;
				node->parentElem = lastParent;

				lastParent->children.push_back(node);

				// If it's a self-closing tag, don't make it the parent
				if(!isSelfClosingTag(tagName))
				{
					lastParent = node;
				}

			}
			
			else // Read the tag
			{
				tagName += c;
			}
		}

		else if(state == STATE_READING_ATTRIBUTES)
		{
			// no attribute reading for now, we just move to STATE_END_TAG once a ">" is detected
			if(c == '>')
			{
				state = STATE_END_TAG;

				auto node = new HTMLElement();
				node->tag = tagName;
				node->parentElem = lastParent;

				lastParent->children.push_back(node);
				
				// If it's a self-closing tag, don't make it the parent
				if(!isSelfClosingTag(tagName))
				{
					lastParent = node;
				}

			}
		}

		else if(state == STATE_END_TAG)
		{
			if(c == '<')  // New tag starting, don't add '<' to content
			{
				state = STATE_START_TAG;
			}
			else
			{
				lastParent->content += c;
			}	
		}

		else if(state == STATE_BEGIN_CLOSING_TAG)
		{
			if (c == '>')
			{
				state = STATE_INIT;
				if(lastParent->parentElem != nullptr)
				{
					lastParent = lastParent->parentElem;
				}
			}
		}
	}

	return root;
}

// HTML tree as string (for debugging)
std::string HTMLTreeToString(HTMLElement *elem, int depth = 0)
{
	std::string result = "";

	for(int i = 0; i < depth; i++)
	{
		result += "  ";
	}

	result += "<" + elem->tag + ">\n";

	if(!elem->content.empty())
	{
		for(int i = 0; i < depth + 1; i++)
		{
			result += "  ";
		}
		result += elem->content + "\n";
	}

	for(auto child : elem->children)
	{
		result += HTMLTreeToString(child, depth + 1);
	}

	for(int i = 0; i < depth; i++)
	{
		result += "  ";
	}

	result += "</" + elem->tag + ">\n";

	return result;
}

HTMLElement* findBody(HTMLElement* node)
{
    if (node->tag == "body") return node;
    for (auto child : node->children)
	{
        if (HTMLElement* found = findBody(child)) return found;
    }
    return nullptr;
}