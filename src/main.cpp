#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDI
#define NOUSER

#include "BearHttpsClientOne.c"
#include "raylib.h"
#include <string>
#include <iostream>
#include <cassert>
#include <map>

#include "pusztaHTML.h"
#include "style.h"
#include "pusztaRender.h"


#define TITLE "miniPuszta"
#define VERSION_NUM "0.0.1"

using namespace std;

const int screenWidth = 800;
const int screenHeight = 600;

// Default font (Poppins)
std::string font = "Poppins";

/*Font normal = LoadFont(("assets/fonts/" + font + "/normal.ttf").c_str());
Font italic = LoadFont(("assets/fonts/" + font + "/normal.ttf").c_str());
Font bold = LoadFont(("assets/fonts/" + font + "/normal.ttf").c_str());
Font bold_italic = LoadFont(("assets/fonts/" + font + "/normal.ttf").c_str());

HTMLRenderer renderer = HTMLRenderer(normal, italic, bold, bold_italic);*/



string fetch_page(string url)
{

	BeginDrawing();
	ClearBackground(RAYWHITE);
	DrawText("Loading...", (screenWidth - MeasureText("Loading...", 20)) / 2, screenHeight / 2, 20, BLACK);
	EndDrawing();

	BearHttpsRequest *request = newBearHttpsRequest(url.c_str());
	BearHttpsResponse *response = BearHttpsRequest_fetch(request);

	if(BearHttpsResponse_error(response))
	{
		printf("Error: %s\n", BearHttpsResponse_get_error_msg(response));
		BearHttpsRequest_free(request);
		BearHttpsResponse_free(response);
		return "";
	}

	const char *body = BearHttpsResponse_read_body_str(response);

	if(BearHttpsResponse_error(response))
	{
		printf("Error: %s\n", BearHttpsResponse_get_error_msg(response));
		BearHttpsRequest_free(request);
		BearHttpsResponse_free(response); 
		return "";
	}

	//printf("Response body: %s\n", body);

	string body_html = string(body);

	BearHttpsRequest_free(request);
	BearHttpsResponse_free(response);

	return body_html;
}

void renderHTMLtree(HTMLElement *elem, int depth = 0)
{

}

int main() 
{

	InitWindow(screenWidth, screenHeight, (string(TITLE) + " " + string(VERSION_NUM)).c_str());
	SetTargetFPS(165);

	HTMLRenderer renderer = HTMLRenderer(("assets/fonts/" + font + "/normal.ttf"), ("assets/fonts/" + font + "/italic.ttf"), ("assets/fonts/" + font + "/bold.ttf"), ("assets/fonts/" + font + "/bold_italic.ttf"));

	string html = fetch_page("https://example.com");

	cout << "Fetched HTML: " << html << endl;

	//HTMLElement *el = HTMLParser("<h1>Hello World!</h1>");
	HTMLElement *el = HTMLParser(html);

	//assert(el->children.size() == 1);
	cout << "Parsed HTML Tree: \n" << HTMLTreeToString(el) << endl;

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText((string(TITLE) + " " + string(VERSION_NUM) + " - " + to_string(GetFPS()) + " FPS").c_str(), 20, 20, 20, BLACK);
		//DrawText(html.c_str(), 20, 50, 20, DARKGRAY);

		/*renderer.drawElement("Normal", 20, 50, tagStyles["p"]);
		renderer.drawElement("Italic", 20, 100, tagStyles["i"]);
		renderer.drawElement("Bold", 20, 150, tagStyles["b"]);
		renderer.drawElement("Bold italic", 20, 200, tagStyles["bi"]);*/

		renderer.drawElement(html.c_str(), 20, 50, tagStyles["p"]);

		renderHTMLtree(el);

		EndDrawing();
	}
	CloseWindow();
	return 0;
}
