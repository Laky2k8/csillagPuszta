/*
	csillagPuszta Character Decoder
	A small thingamajig to decode HTML entities and other encodings.
	I put it in a seperate header because it's neater :]
*/

#include <string>
#include <map>
#include <stdexcept>

static const std::map<std::string, std::string> htmlEntities = {
	{"amp", "&"}, {"lt", "<"}, {"gt", ">"}, {"quot", "\""}, {"apos", "'"},
	{"nbsp", "\u00A0"}, {"copy", "©"}, {"reg", "®"}, {"deg", "°"}, {"times", "×"},
	{"euro", "€"}, {"pound", "£"}, {"yen", "¥"}, {"cent", "¢"}, {"sect", "§"},

	// Accented characters
	{"aacute", "á"}, {"acirc", "â"}, {"agrave", "à"}, {"aring", "å"},
	{"atilde", "ã"}, {"auml", "ä"}, {"eacute", "é"}, {"ecirc", "ê"},
	{"egrave", "è"}, {"euml", "ë"}, {"iacute", "í"}, {"icirc", "î"},
	{"igrave", "ì"}, {"iuml", "ï"}, {"oacute", "ó"}, {"ocirc", "ô"},
	{"ograve", "ò"}, {"otilde", "õ"}, {"ouml", "ö"}, {"uacute", "ú"},
	{"ucirc", "û"}, {"ugrave", "ù"}, {"uuml", "ü"}, {"szlig", "ß"},
	{"ccedil", "ç"}, {"ntilde", "ñ"},

	// Symbols
	{"ndash", "–"}, {"mdash", "—"}, {"hellip", "…"}, {"bull", "•"},
	{"prime", "′"}, {"Prime", "″"}, {"larr", "←"}, {"rarr", "→"}
};

std::string codepoint_to_utf8(uint32_t cp)
{
    // Validate
    if (cp > 0x10FFFF) 
        throw std::out_of_range("codepoint out of Unicode range");
    if (cp >= 0xD800 && cp <= 0xDFFF) 
        throw std::invalid_argument("codepoint is a UTF-16 surrogate");

    std::string out;
    if (cp < 0x80)
	{
        out.push_back(static_cast<char>(cp));
    } 

	else if (cp < 0x800)
	{
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } 

	else if (cp < 0x10000)
	{
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }

	else
	{ // cp <= 0x10FFFF
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }

    return out;
}

static std::string decodeEntity(const std::string& entity)
{
	// Return HTML entity if in the database
	auto it = htmlEntities.find(entity);
	if(it != htmlEntities.end())
	{
		return it->second;
	}

	// Numeric entities
	if(entity[0] == '#')
	{
		try
		{
			int codepoint = 0;

			if(entity[1] == 'x' || entity[1] == 'X') // Hex
			{
				codepoint = std::stoi(entity.substr(2), nullptr, 16);
			}
			else
			{
				// Decimal
				codepoint = std::stoi(entity.substr(1));
			}

			return codepoint_to_utf8(static_cast<uint32_t>(codepoint));
		}
		catch(...)
		{
			return "&" + entity + ";"; 
		}		
	}

	return "&" + entity + ";"; // If all else fails lmao
}

static std::string decodeHTMLString(const std::string& text)
{
	std::string result;
	size_t i = 0;

	while(i < text.length())
	{
		if(text[i] == '&')
		{
			size_t end = text.find(';', i);

			if(end != std::string::npos)
			{
				std::string htmlEntity = text.substr(i + 1, end - i - 1);
				result += decodeEntity(htmlEntity);
				i = end + 1;
			}
			else
			{
				result += text[i];
				++i;
			}
		}
		else
		{
			result += text[i];
			++i;
		}
	}

	return result;
}
