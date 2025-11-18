#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDI
#define NOUSER

#include <string>
#include <iostream>
#include <cassert>
#include <map>

#include "BearHttpsClientOne.c"
#include "raylib.h"
#include "imgui.h"
#include "rlImGui.h"

#include "pusztaHTML.h"
#include "style.h"
#include "pusztaRender.h"
#include "ui_theme.h"


#define TITLE "csillagPuszta"
#define VERSION_NUM "0.5.0"

using namespace std;

const int screenWidth = 800;
const int screenHeight = 600;

// Default font (Poppins)
std::string font = "Poppins";
bool show_settings = false;

/*Font normal = LoadFont(("assets/fonts/" + font + "/normal.ttf").c_str());
Font italic = LoadFont(("assets/fonts/" + font + "/normal.ttf").c_str());
Font bold = LoadFont(("assets/fonts/" + font + "/normal.ttf").c_str());
Font bold_italic = LoadFont(("assets/fonts/" + font + "/normal.ttf").c_str());

HTMLRenderer renderer = HTMLRenderer(normal, italic, bold, bold_italic);*/


// DPI scaling functions
float ScaleToDPIF(float value)
{
    return GetWindowScaleDPI().x * value;
}

int ScaleToDPII(int value)
{
    return int(GetWindowScaleDPI().x * value);
}

string html;
HTMLElement *htmlTree;
void browser_ui();

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

void renderHTMLtree(HTMLRenderer &renderer, HTMLElement *elem, float docY)
{
	HTMLElement *body = findBody(elem); // only render the contents of body
	if (!body) return;

	LayoutBox *rootBox = build_layout_tree(body, tagStyles);

	float docX = 20; // left margin // top margin
	float docWidth = (float)GetScreenWidth() - docX * 2;

	rootBox->x = docX;
	rootBox->y = docY;
	rootBox->width = docWidth;
	compute_layout(rootBox, renderer, rootBox->x, rootBox->y, rootBox->width);

	renderer.paintLayout(rootBox);

	freeLayoutTree(rootBox);
}

int main() 
{

	InitWindow(screenWidth, screenHeight, (string(TITLE) + " " + string(VERSION_NUM)).c_str());
	SetTargetFPS(165);
	rlImGuiSetup(true);

	HTMLRenderer renderer = HTMLRenderer(("assets/fonts/" + font + "/normal.ttf"), ("assets/fonts/" + font + "/italic.ttf"), ("assets/fonts/" + font + "/bold.ttf"), ("assets/fonts/" + font + "/bold_italic.ttf"));

	string html = fetch_page("https://laky2k8.hu/blog");

	cout << "Fetched HTML: " << html << endl;

	//HTMLElement *el = HTMLParser("<h1>Hello World!</h1>");
	HTMLElement *el = HTMLParser(html);

	bool successfulFetch = el->children.size() > 0;

	//assert(el->children.size() == 1);
	if(successfulFetch)
	{
		cout << "Parsed HTML Tree: \n" << HTMLTreeToString(el) << endl;
	}
	else
	{
		cout << "Fetching failed: Site is empty!" << endl;
	}


	float docY = 50; // initial Y position to start rendering

	while (!WindowShouldClose())
	{
		// Scrolling
		docY += GetMouseWheelMove() * 20;

		BeginDrawing();
		ClearBackground(RAYWHITE);



		DrawText((string(TITLE) + " " + string(VERSION_NUM) + " - " + to_string(GetFPS()) + " FPS").c_str(), 20, 20, 20, BLACK);
		//DrawText(html.c_str(), 20, 50, 20, DARKGRAY);

		/*renderer.drawElement("Normal", 20, 50, tagStyles["p"]);
		renderer.drawElement("Italic", 20, 100, tagStyles["i"]);
		renderer.drawElement("Bold", 20, 150, tagStyles["b"]);
		renderer.drawElement("Bold italic", 20, 200, tagStyles["bi"]);*/

		//renderer.drawElement(html.c_str(), 20, 50, tagStyles["p"]);

		if(successfulFetch)
		{
			renderHTMLtree(renderer, el, docY);
		}
		else
		{
			DrawText("Something went wrong, please try again!", (screenWidth - MeasureText("Something went wrong, please try again!", 20)) / 2, screenHeight / 2, 20, BLACK);
		}
		

		// start ImGui Conent
		rlImGuiBegin();

		// end ImGui Content
		rlImGuiEnd();


		EndDrawing();
	}

	freeHTMLTree(el);

	CloseWindow();
	return 0;
}

void browser_ui()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 60));
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	if (ImGui::Begin("StatusBar", nullptr, window_flags)) {
		/*if (ImGui::BeginMenuBar()) {
			ImGui::Text((TITLE + " v" + VERSION).c_str());
			ImGui::EndMenuBar();
		}*/

		// empty space looks better
		float cursor_y_default = ImGui::GetCursorPosY();
		float cursor_y = cursor_y_default;
		ImGui::SetCursorPosY(cursor_y + 5.0f); 

		// Control buttons
		bool back_pressed = ImGui::ArrowButton("##left", ImGuiDir_Left);
		ImGui::SameLine();
		bool forward_pressed = ImGui::ArrowButton("##right", ImGuiDir_Right);
		ImGui::SameLine();

		bool reload_pressed = ImGui::Button(ICON_FA_ARROW_ROTATE_RIGHT);


		ImGui::SameLine();

		// URL input
		static char url_input[256];
		bool should_search = ImGui::InputTextWithHint("##URL", "URL", url_input, IM_ARRAYSIZE(url_input), ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		//if (ImGui::Button("Go") || (ImGui::IsItemActive() && ImGui::IsKeyPressed(ImGuiKey_Enter)))
		if(should_search)
		{
			// Check if the url_input is not empty
			if (strlen(url_input) > 0) 
			{

				// Search the URL
				#ifdef DEBUG
					std::cout << "Searching: " << url_input << std::endl;
				#endif
				//search(url_input, url_input); // Pass the URL to the search function

				//cursor_y = cursor_y_default;

			} 
			else 
			{
				//site_content = "Please enter a valid URL.";
			}
		}


		if(back_pressed)
		{
			/*if(history_index > 0)
			{
				history_index--;
				std::string url = history[history_index];
				#ifdef DEBUG
					std::cout << "Searching: " << url << std::endl;
				#endif
				search(url, url_input); // Pass the URL to the search function

				cursor_y = cursor_y_default;
			}*/
		}

		if(forward_pressed)
		{
			/*if(history_index < history.size() - 1)
			{
				history_index++;
				std::string url = history[history_index];
				#ifdef DEBUG
					std::cout << "Searching: " << url << std::endl;
				#endif
				search(url, url_input); // Pass the URL to the search function

				cursor_y = cursor_y_default;
			}*/
		}

		if(reload_pressed)
		{
			/*if(history_index >= 0 && history_index < history.size())
			{
				std::string url = history[history_index];
				#ifdef DEBUG
					std::cout << "Reloading: " << url << std::endl;
				#endif
				search(url, url_input); 

				cursor_y = cursor_y_default;
			}*/
		}

		ImGui::SameLine();


		// Calculate available space and position at the right edge
		float avail = ImGui::GetContentRegionAvail().x;
		float button_width = ImGui::CalcTextSize(ICON_FA_GEAR).x + ImGui::GetStyle().FramePadding.x * 2.0f;
		ImGui::SameLine(ImGui::GetCursorPosX() + avail - button_width);

		if (ImGui::Button(ICON_FA_GEAR))
			show_settings = true;

		if(show_settings)
		{
			// Get the main viewport and calculate the center from it
			ImGuiViewport* main_viewport = ImGui::GetMainViewport(); 
			ImVec2 viewport_center = ImVec2(
				main_viewport->Pos.x + main_viewport->Size.x * 0.5f,
				main_viewport->Pos.y + main_viewport->Size.y * 0.5f
			);
			
			// Center the settings window
			ImVec2 window_size = ImVec2(400, 300);
			ImGui::SetNextWindowPos(ImVec2(
				viewport_center.x - window_size.x * 0.5f,
				viewport_center.y - window_size.y * 0.5f
			), ImGuiCond_Always);
			
			ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
			ImGui::SetNextWindowBgAlpha(0.95f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);

			ImGui::SetNextWindowFocus();

			if (ImGui::Begin("Settings", &show_settings, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
			{
				ImGui::Text("Settings");
				ImGui::Separator();
				
				ImGui::Text("Theme:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(150.0f);

				// Dropdown, no theme selection yet just an empty list
				static int selected_theme = 0;
				const char* themes[] = { "Dark", "Light" };

				// ImGui::ListBox
				if(ImGui::ListBox("##theme", &selected_theme, themes, IM_ARRAYSIZE(themes), 2))
				{
					if(selected_theme == 0)
					{
						DarkMode();
					}
					else if(selected_theme == 1)
					{
						LightMode();
					}
				}

			}
			ImGui::End();
			ImGui::PopStyleVar(2);
		}


		ImGui::End();
	}
	ImGui::PopStyleVar();
}