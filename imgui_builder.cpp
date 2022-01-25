 /* GENERATED WITH IMGUI BUILDER :) HAS 10 Objs & 1 forms */


void ToggleButton(const char* str_id, bool* v)
{ 
ImVec2 p = ImGui::GetCursorScreenPos(); 
ImDrawList* draw_list = ImGui::GetWindowDrawList();
float height = ImGui::GetFrameHeight();
float width = height * 1.55f;
float radius = height * 0.50f;
 if (ImGui::InvisibleButton(str_id, ImVec2(width, height)))
   *v = !*v;
ImU32 col_bg;
if (ImGui::IsItemHovered())
	  col_bg = *v ? IM_COL32(145 + 20, 211, 68 + 20, 255) : IM_COL32(218 - 20, 218 - 20, 218 - 20, 255);
 else
	   col_bg = *v ? IM_COL32(145, 211, 68, 255) : IM_COL32(218, 218, 218, 255);
  draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
   draw_list->AddCircleFilled(ImVec2(*v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}


void gui_builder0()
{
ImGui::SetNextWindowSize({500.f,525.f});

ImGui::Begin( "Settings");
ImGui::SetCursorPos({30.f,30.f});
if(ImGui::Button("Browse...", {90.f,20.f}))
{

}
ImGui::SetCursorPos({130.f,30.f});
ImGui::PushItemWidth(340.000000);
ImGui::InputText("", buffer, 255);
ImGui::PopItemWidth( );

ImGui::SetCursorPos({30.f,70.f});

ImGui::BeginChild("child0",{440.f,160.f},true );

ImGui::SetCursorPos({50.f,59.f});
	ImGui::PushItemWidth(340.000000);
	ImGui::InputText("Number of frames", buffer, 255);
	ImGui::PopItemWidth( );

ImGui::SetCursorPos({50.f,20.f});
	ImGui::PushItemWidth(340.000000);
	ImGui::InputText("Name of entity  ", buffer, 255);
	ImGui::PopItemWidth( );

ImGui::SetCursorPos({50.f,120.f});
	if(ImGui::Button("<", {30.f,19.f }))
{

	}
ImGui::SetCursorPos({245.f,120.f});
	if(ImGui::Button(">", {30.f,19.f }))
{

	}
ImGui::SetCursorPos({138.f,123.f});
	ImGui::PushItemWidth(49.000000);
	ImGui::Text("ID : 00");
	ImGui::PopItemWidth( );


ImGui::EndChild();

ImGui::SetCursorPos({30.f,250.f});

ImGui::BeginChild("child1",{440.f,212.f},true );

ImGui::SetCursorPos({30.f,160.f});
	if(ImGui::Button("Save", {57.f,19.f }))
{

	}
ImGui::SetCursorPos({30.f,30.f});
	ToggleButton("hithurt", the_bool);
ImGui::SetCursorPos({70.f,32.f});
	ImGui::PushItemWidth(42.000000);
	ImGui::Text("Hitbox");
	ImGui::PopItemWidth( );



ImGui::End();
}


