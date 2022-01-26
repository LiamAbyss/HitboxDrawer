#include <SFML/Graphics.hpp>
#include <sstream>
#include "libs/SFGL/Game.h"
#include "imgui.h"
#include "libs/IMGUI/imgui_stdlib.h"
#include "imgui-SFML.h"
#include "libs/ImGuiFileDialog/ImGuiFileDialog.h"
#include "ConcavePolygon.h"

#define CANVAS_SIZE 900.F
#define SPRITEX 470.F
#define SPRITEY 20.F

class Hitbox
{
public:
    sf::VertexArray vertices;
    bool isHurtbox = false;
};

class Frame
{
public:
    std::vector<Hitbox> boxes;

};

float dist2(sf::Vector2f& a, sf::Vector2f& b)
{
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

class MainActivity : public Scene
{
    std::string filename;
    int nFrames = 1;
    std::string entity;
    bool isHurtbox = false;
    bool drawing = false;
    int id = 0;
    
    int selectedBox = -1;
    int selectedPoint = -1;

    float factor = 1;
    sf::Texture texture;
    sf::Sprite sprite;
    sf::RectangleShape canvas;

    std::vector<Frame> frames;
    sf::CircleShape circle;
    sf::CircleShape pointShape;

    sf::VertexArray crosshairx;
    sf::VertexArray crosshairy;

    // Hérité via Scene
    virtual void initialize() override
    {
        ImGui::SFML::Init(window());
        canvas.setSize({ CANVAS_SIZE, CANVAS_SIZE });
        canvas.setPosition(460, 10);
        canvas.setFillColor(sf::Color::Black);
        canvas.setOutlineThickness(2);
        
        circle.setRadius(5);
        circle.setFillColor(sf::Color::Blue);
        circle.setPosition(-20, -20);
        circle.setOrigin(5, 5);

        pointShape.setRadius(4);
        pointShape.setFillColor(sf::Color::Blue);
        pointShape.setPosition(-20, -20);
        pointShape.setOrigin(4, 4);

        frames.resize(1);
        frames[0].boxes.resize(1);

        crosshairx.setPrimitiveType(sf::PrimitiveType::Lines);
        crosshairy.setPrimitiveType(sf::PrimitiveType::Lines);

        for (float i = 0; i < 100; i++)
        {
            crosshairx.append(sf::Vertex({ canvas.getPosition().x, canvas.getPosition().y + canvas.getSize().y * i / 100 }, sf::Color::Blue));
            crosshairy.append(sf::Vertex({ canvas.getPosition().x + canvas.getSize().x * i / 100, canvas.getPosition().y }, sf::Color::Blue));
        }
    }

    sf::Vector2f getMousePosRelativeToFactor()
    {
        sf::Vector2f pos = (sf::Vector2f)sf::Mouse::getPosition(window());

        //std::cout << "Real pos : " << pos.x << " " << pos.y << std::endl;

        // To export
        pos.x = static_cast<float>(static_cast<int>((pos.x - SPRITEX) / factor));
        pos.y = static_cast<float>(static_cast<int>((pos.y - SPRITEY) / factor));

        // To import
        pos.x = SPRITEX + (pos.x * factor);
        pos.y = SPRITEY + (pos.y * factor);

        return pos;
    }

    sf::Vector2f getAltMajPos(const sf::Vector2f& lastPos, const sf::Vector2f& pos)
    {
        sf::Vector2f res = pos;
        if (abs(lastPos.x - pos.x) > abs(lastPos.y - pos.y))
            res.y = lastPos.y;
        else
            res.x = lastPos.x;
        return res;
    }

    virtual void update(sf::Time dt, sf::Event& ev) override
    {
        ImGui::SFML::ProcessEvent(ev);

        if (ev.type == sf::Event::MouseMoved || ev.type == sf::Event::MouseButtonReleased)
        {
            sf::Vector2f pos = getMousePosRelativeToFactor();
            if (!canvas.getGlobalBounds().contains(pos)) return;

            auto& boxes = frames[id].boxes;
            if (boxes.empty()) return;

            sf::VertexArray& v = boxes[boxes.size() - 1].vertices;
            if (drawing)
            {
                // Vertical or horizontal
                if (v.getVertexCount() > 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                {
                    sf::Vector2f lastPos = v[v.getVertexCount() - 2].position;
                    pos = getAltMajPos(lastPos, pos);
                }

                for (int i = 0; i < crosshairx.getVertexCount(); i++)
                {
                    crosshairx[i].position.x = pos.x;
                    crosshairy[i].position.y = pos.y;                            
                }

                circle.setPosition(pos);
                if (v.getVertexCount() > 0)
                {
                    v[v.getVertexCount() - 1].position = pos;
                    v[v.getVertexCount() - 1].color = circle.getFillColor();
                }

                if (ev.type == sf::Event::MouseButtonReleased)
                {
                    if (v.getVertexCount() == 0)
                    {
                        v.append(sf::Vertex(pos, circle.getFillColor()));
                    }
                    v.append(sf::Vertex(pos, circle.getFillColor()));
                }
            }
            else
            {
                // Can move points
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                {
                    sf::VertexArray v2;
                    if (selectedPoint == -1)
                    {
                        bool stop = false;
                        selectedBox = 0;
                        selectedPoint = 0;
                        for (auto& b : boxes)
                        {
                            v2 = b.vertices;

                            for (int i = 0; i < v2.getVertexCount(); i++)
                            {
                                if (dist2(pos, v2[i].position) <= 16)
                                {
                                    //// Vertical or horizontal
                                    if (v2.getVertexCount() > 1 && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                                    {
                                        sf::Vector2f lastPos;
                                        if (i == 0)
                                            lastPos = v2[v2.getVertexCount() - 2].position;
                                        else
                                            lastPos = v2[i - 1].position;
                                        pos = getAltMajPos(lastPos, pos);
                                    }
                                    if (v2.getVertexCount() > 1 && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                                    {
                                        sf::Vector2f nextPos;
                                        if (i == v2.getVertexCount() - 1)
                                            nextPos = v2[1].position;
                                        else
                                            nextPos = v2[i + 1].position;
                                        pos = getAltMajPos(nextPos, pos);
                                    }

                                    b.vertices[i].position = pos;

                                    if (i == v2.getVertexCount() - 1)
                                        b.vertices[0].position = pos;
                                    else if (i == 0)
                                        b.vertices[v2.getVertexCount() - 1].position = pos;
                                    stop = true;
                                    break;
                                }
                                selectedPoint++;
                            }
                            if (stop) break;
                            selectedBox++;
                            selectedPoint = 0;
                        }
                        if (!stop)
                        {
                            selectedBox = -1;
                            selectedPoint = -1;
                        }
                    }
                    else
                    {
                        v2 = boxes[selectedBox].vertices;

                        //// Vertical or horizontal
                        if (v2.getVertexCount() > 1 && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                        {
                            sf::Vector2f lastPos;
                            if (selectedPoint == 0)
                                lastPos = v2[v2.getVertexCount() - 2].position;
                            else
                                lastPos = v2[selectedPoint - 1].position;
                            pos = getAltMajPos(lastPos, pos);
                        }
                        if (v2.getVertexCount() > 1 && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                        {
                            sf::Vector2f nextPos;
                            if (selectedPoint == v2.getVertexCount() - 1)
                                nextPos = v2[1].position;
                            else
                                nextPos = v2[selectedPoint + 1].position;
                            pos = getAltMajPos(nextPos, pos);
                        }

                        boxes[selectedBox].vertices[selectedPoint].position = pos;

                        if (selectedPoint == v2.getVertexCount() - 1)
                            boxes[selectedBox].vertices[0].position = pos;
                        else if (selectedPoint == 0)
                            boxes[selectedBox].vertices[v2.getVertexCount() - 1].position = pos;
                    }

                    for (int i = 0; i < crosshairx.getVertexCount(); i++)
                    {
                        crosshairx[i].position.x = pos.x;
                        crosshairy[i].position.y = pos.y;
                    }
                }
                else
                {
                    selectedBox = -1;
                    selectedPoint = -1;
                }
            }
        }
    }

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

    void drawGui()
    {
        auto& boxes = frames[id].boxes;
        ImGui::SetNextWindowSize({ 450,920 }, ImGuiCond_::ImGuiCond_Always);
        ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_::ImGuiCond_Always);
        ImGui::Begin("Settings", NULL,
            ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_::ImGuiWindowFlags_NoMove
        );
        ImGui::SetCursorPos({ 20,30.f });

        // open Dialog Simple
        if (ImGui::Button("Browse...", { 90.f,20.f }))
            ImGuiFileDialog::Instance()->OpenDialog("browse", "Choose File", ".png,.jpg,.jpeg,.bmp,.gif,.tga", ".");

        // display
        if (ImGuiFileDialog::Instance()->Display("browse", 32, ImVec2(500, 400)))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                texture.loadFromFile(filePathName);
                sprite.setTexture(texture);
                updateTexture();
                filename = filePathName;
                entity = filename.substr(filename.find_last_of("\\")+1, filename.find_last_of(".") - filename.find_last_of("\\") - 1);
                // action
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::SetCursorPos({ 130.f,30.f });
        ImGui::PushItemWidth(300);
        ImGui::InputText(" ", &filename, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
        ImGui::PopItemWidth();

        ImGui::SetCursorPos({ 20,70.f });

        ImGui::BeginChild("child0", { 410,160.f }, true);

        ImGui::SetCursorPos({ 50.f,20.f });
        ImGui::PushItemWidth(210);
        ImGui::InputText("Name of entity  ", &entity);
        ImGui::PopItemWidth();

        ImGui::SetCursorPos({ 50.f,59 });
        if (ImGui::Button("<##0", { 30.f,19.f }))
        {
            nFrames--;
            if (nFrames <= 0) nFrames = 1;
            updateTexture();
        }
        ImGui::SetCursorPos({ 335, 59 });
        if (ImGui::Button(">##0", { 30.f,19.f }))
        {
            nFrames++;
            updateTexture();
        }
        ImGui::SetCursorPos({ 150,61 });
        ImGui::PushItemWidth(45);
        ImGui::Text((std::string("Number of frames : ") + std::to_string(nFrames)).c_str());
        ImGui::PopItemWidth();

        ImGui::SetCursorPos({ 50.f,120.f });
        if (ImGui::Button("<", { 30.f,19.f }))
        {
            if (id <= 0) id = nFrames - 1;
            else id--;
            updateTexture();
        }
        ImGui::SetCursorPos({ 335,120.f });
        if (ImGui::Button(">", { 30.f,19.f }))
        {
            if (id >= nFrames - 1) id = 0;
            else id++;
            updateTexture();
        }
        ImGui::SetCursorPos({ 200,123.f });
        ImGui::PushItemWidth(49.000000);
        ImGui::Text((std::string("ID : ") + std::to_string(id)).c_str());
        ImGui::PopItemWidth();


        ImGui::EndChild();

        ImGui::SetCursorPos({ 20,250.f });

        ImGui::BeginChild("child1", { 410,212.f }, true);

        ImGui::SetCursorPos({ 30,160.f });
        if (ImGui::Button("New", { 57.f,19.f }))
        {
            if (drawing && boxes.size() > 0)
            {
                sf::VertexArray& v = boxes[boxes.size() - 1].vertices;
                if (v.getVertexCount() < 4)
                    throw std::runtime_error("Hitbox should have at least 3 vertices.");
                v[v.getVertexCount() - 1] = v[0];
            }

            Hitbox h;
            h.vertices = sf::VertexArray(sf::PrimitiveType::LineStrip);
            h.isHurtbox = isHurtbox;
            if (drawing || boxes.size() == 0 || boxes[boxes.size() - 1].vertices.getVertexCount() != 0)
                boxes.push_back(h);
            else
                boxes[boxes.size() - 1] = h;

            if (isHurtbox) circle.setFillColor(sf::Color::Red);
            else circle.setFillColor(sf::Color::Blue);
            for (int i = 0; i < crosshairx.getVertexCount(); i++)
            {
                crosshairx[i].color = circle.getFillColor();
                crosshairy[i].color = circle.getFillColor();
            }
            drawing = true;
        }

        ImGui::SetCursorPos({ 100,160.f });
        if (ImGui::Button("Finish", { 57.f,19.f }))
        {
            if (drawing)
            {
                if (boxes.size() > 0)
                {
                    sf::VertexArray& v = boxes[boxes.size() - 1].vertices;
                    if (v.getVertexCount() < 4)
                        throw std::runtime_error("Hitbox should have at least 3 vertices.");
                    v[v.getVertexCount() - 1] = v[0];
                }

                Hitbox h;
                h.vertices = sf::VertexArray(sf::PrimitiveType::LineStrip);
                h.isHurtbox = isHurtbox;
                boxes.push_back(h);

                if (isHurtbox) circle.setFillColor(sf::Color::Red);
                else circle.setFillColor(sf::Color::Blue);

                drawing = false;
            }
        }

        ImGui::SetCursorPos({ 300,130 });
        // open Dialog Simple
        if (ImGui::Button("Import...", { 90.f,20.f }))
            ImGuiFileDialog::Instance()->OpenDialog("import", "Choose file", ".json", ".");

        // display
        if (ImGuiFileDialog::Instance()->Display("import", 32, ImVec2(500, 400)))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filename = ImGuiFileDialog::Instance()->GetCurrentFileName();
                importHitboxes(filename);
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
        ImGui::SetCursorPos({ 300,160.f });
        // open Dialog Simple
        if (ImGui::Button("Export...", { 90.f,20.f }))
            ImGuiFileDialog::Instance()->OpenDialog("export", "Choose Folder", NULL, ".");

        // display
        if (ImGuiFileDialog::Instance()->Display("export", 32, ImVec2(500, 400)))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                exportHitboxes(filePath + "\\" + entity + ".json");
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::SetCursorPos({ 30.f,30.f });
        ToggleButton("hithurt", &isHurtbox);
        ImGui::SetCursorPos({ 70.f,32.f });
        ImGui::PushItemWidth(42.000000);
        ImGui::Text(isHurtbox ? "Hurtbox" : "Hitbox");
        ImGui::PopItemWidth();
        ImGui::EndChild();

        ImGui::End();
    }

    void updateTexture()
    {
        if (frames.size() != nFrames)
        {
            id = 0;
            frames.resize(nFrames);

            for (auto& f : frames)
            {
                f.boxes.clear();
                f.boxes.resize(1);
            }
        }

        int frameWidth = texture.getSize().x / nFrames;
        int height = static_cast<int>(texture.getSize().y);
        int offset = frameWidth * id;

        sprite.setTextureRect(sf::IntRect({ offset, 0 }, { frameWidth, height }));
        
        factor = 1;
        float limit = CANVAS_SIZE - 20;
        /*if (frameWidth > 830 || height > 800)
        {*/
            if (frameWidth / limit > height / limit)
            {
                factor = floor(limit / frameWidth);
            }
            else factor = floor(limit / height);
            if (factor == 0) factor = limit / frameWidth;
        //}

        sprite.setScale(factor, factor);
        sprite.setPosition(SPRITEX, SPRITEY);//470, 20);
    }

    virtual void update(sf::Time dt) override
    {
        ImGui::SFML::Update(window(), dt);
        drawGui();
    }

    virtual void render() override
    {
        window().draw(canvas);
        ImGui::SFML::Render(window());
        window().draw(sprite);

        const auto& boxes = frames[id].boxes;
        for (const auto& b : boxes)
        {
            window().draw(b.vertices);
            if (b.vertices.getVertexCount() > 0)
            {
                pointShape.setFillColor(b.vertices[0].color);
                for (int i = 0; i < b.vertices.getVertexCount(); i++)
                {
                    pointShape.setPosition(b.vertices[i].position);
                    window().draw(pointShape);
                }
            }
        }
        window().draw(crosshairx);
        window().draw(crosshairy);
        if (drawing)
            window().draw(circle);
    }

    void importHitboxes(const std::string& filename)
    {
        using namespace nlohmann; 
        
        std::ifstream file;
        file.open(filename);
        if (file.is_open())
        {
            std::stringstream stream;
            stream << file.rdbuf();
            file.close();

            std::vector<Frame> newFrames;
            json j = json::parse(stream.str());

            try
            {
                json jEntity = j["entity"];

                entity = jEntity["name"];

                json jFrames = jEntity["frames"];

                nFrames = jEntity["frames"].size();
                id = 0;

                std::string originPath = jEntity["path"];

                texture.loadFromFile(originPath);
                sprite.setTexture(texture);
                updateTexture();
                entity = originPath.substr(originPath.find_last_of("\\") + 1, originPath.find_last_of(".") - originPath.find_last_of("\\") - 1);
                this->filename = originPath;

                for (auto& jFrame : jFrames)
                {
                    Frame tmpFrame;
                    std::vector<Hitbox> tmpBoxes;
                    for (auto& jBox : jFrame)
                    {
                        Hitbox box;
                        box.isHurtbox = jBox["type"] == "hurtbox";
                        box.vertices.setPrimitiveType(sf::PrimitiveType::LineStrip);
                        
                        json jPoints = jBox["vertices"];
                        for (auto& jPoint : jPoints)
                        {
                            sf::Vertex vertex;
                            vertex.color = box.isHurtbox ? sf::Color::Red : sf::Color::Blue;
                            vertex.position = sf::Vector2f(SPRITEX + (jPoint["x"] * factor), SPRITEY + (jPoint["y"] * factor));
                            box.vertices.append(vertex);
                        }

                        tmpBoxes.push_back(box);
                    }
                    tmpFrame.boxes = tmpBoxes;
                    newFrames.push_back(tmpFrame);
                }

                frames = newFrames;
            }
            catch (const std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
        }
    }

    void exportHitboxes(const std::string& filepath)
    {
        /*
         {
            entity : {
                name : "name",
                frames : [
                    [
                        {
                            type : "type",
                            vertices : [
                                {
                                    x : 0,
                                    y : 0
                                }
                            ]
                        }
                    ]
                ]
            }
         }        
         */

        using namespace nlohmann;
        json j = json::object();

        json jEntity = json::object();
        jEntity["name"] = entity;
        jEntity["path"] = filename;
        json jFrames = json::array();

        auto convexFrames = getConcaveFrames();

        for (const auto& f : convexFrames)
        {
            json jFrameBoxes = json::array();
            for (int j = 0; j < f.boxes.size(); j++)
            {
                const auto& b = f.boxes[j];
                if (b.vertices.getVertexCount() > 0)
                {
                    json box = json::object();
                    json points = json::array();
                    for (int i = 0; i < b.vertices.getVertexCount(); i++)
                    {
                        const auto& p = b.vertices[i];
                        json point = json::object();
                        point["x"] = static_cast<int>((p.position.x - SPRITEX) / factor);
                        point["y"] = static_cast<int>((p.position.y - SPRITEY) / factor);
                        points.push_back(point);
                    }
                    box["vertices"] = points;
                    box["type"] = b.isHurtbox ? "hurtbox" : "hitbox";
                    jFrameBoxes.push_back(box);
                }
            }
            jFrames.push_back(jFrameBoxes);
        }
        jEntity["frames"] = jFrames;

        j["entity"] = jEntity;

        std::cout << j.dump(4) << std::endl;
        std::ofstream file;
        file.open(filepath);
        if (file.is_open())
        {
            file << j.dump(4) << std::endl;
            file.close();
        }
    }

    std::vector<cxd::ConcavePolygon> convexDecomp(const sf::VertexArray& vert)
    {
        // Create vertices
        std::vector<cxd::Vertex> vertices;
        for (int i = 0; i < vert.getVertexCount(); i++)
        {
            const auto& v = vert[i];
            vertices.push_back(cxd::Vec2({ v.position.x, v.position.y }));
        }

        // Create polygon from these vertices
        cxd::ConcavePolygon concavePoly(vertices);
        // Perform convex decomposition on polygon
        concavePoly.convexDecomp();

        // Create a vector and retrieve all convex subpolygons
        // as a single list
        std::vector<cxd::ConcavePolygon> subPolygonList;
        concavePoly.returnLowestLevelPolys(subPolygonList);

        return subPolygonList;
    }

    /*std::vector<Hitbox> convexDecomp2(const sf::VertexArray& vert)
    {
        cPolygon poly;
        for (int i = 0; i < vert.getVertexCount(); i++)
            poly.push(Point(vert[i].position.x, vert[i].position.y));

        poly.makeCCW();
        
        EdgeList diags = poly.decomp();
        std::vector<cPolygon> remaining = { poly };
        std::vector<cPolygon> polys;

        for (auto& diag : diags)
        {
            Point p1 = diag.first;
            Point p2 = diag.second;

            int remainingId = 0;
            while (std::find(remaining[remainingId].tp.begin(), remaining[remainingId].tp.end(), p1) == remaining[remainingId].tp.end())
                remainingId++;

            cPolygon& currPoly = remaining[remainingId];

            int pId = std::distance(
                currPoly.tp.begin(),
                std::find(currPoly.tp.begin(), currPoly.tp.end(), p1)
            ) + 1;

            cPolygon builder;
            builder.push(p1);
            do
            {
                if (pId > currPoly.tp.size() - 1)
                    pId = 0;
                builder.push(currPoly.tp[pId]);
                pId++;
            } while (currPoly.tp[pId].x != p1.x && currPoly.tp[pId].y != p1.y || currPoly.tp[pId].x != p2.x && currPoly.tp[pId].y != p2.y);

        }

    }*/

    std::vector<Frame> getConcaveFrames()
    {
        std::vector<Frame> resultFrames;

        for (const auto& f : frames)
        {
            std::vector<Hitbox> resultBoxes;
            for (const auto& b : f.boxes)
            {
                Hitbox h;
                h.isHurtbox = b.isHurtbox;

                if (b.vertices.getVertexCount() > 0)
                {
                    auto subPolygonList = convexDecomp(b.vertices);

                    for (const auto& p : subPolygonList)
                    {
                        h.vertices.clear();
                        for (const auto& v : p.getVertices())
                        {
                            h.vertices.append(sf::Vertex(sf::Vector2f((float)v.position.x, (float)v.position.y)));
                        }

                        if (h.vertices[h.vertices.getVertexCount() - 1].position != h.vertices[0].position)
                            h.vertices.append(h.vertices[0]);
                        resultBoxes.push_back(h);
                    }
                }
            }
            resultFrames.push_back(Frame({resultBoxes}));
        }

        return resultFrames;
    }
};

int main()
{
    /*// Create a vector of vertices
    std::vector<cxd::Vertex > vertices =
    {
        cxd::Vec2({10, 5}),
        cxd::Vec2({20, 10.0f}),
        cxd::Vec2({30.0f, 8}),
        cxd::Vec2({35, 22.0f}),
        cxd::Vec2({22, 27.0f}),
        cxd::Vec2({12, 21.0f}),
    };

    // Create polygon from these vertices
    cxd::ConcavePolygon concavePoly(vertices);

    // Perform convex decomposition on polygon
    concavePoly.convexDecomp();

    // Retrieve a decomposed convex subpolygon by index
    // We still use the concave poly type here
    cxd::ConcavePolygon subPolygon = concavePoly.getSubPolygon(0);

    // Retrieve vertices from the subpolygon (also applies to the
    // concave polygon we defined earlier)
    std::vector<cxd::Vertex > subPolyVerts = subPolygon.getVertices();

    // Create a vector and retrieve all convex subpolygons
    // as a single list
    std::vector<cxd::ConcavePolygon > subPolygonList;
    concavePoly.returnLowestLevelPolys(subPolygonList);*/

    Game game("SFGL - Hitbox Drawer", sf::VideoMode(CANVAS_SIZE + 470, CANVAS_SIZE + 20));
    game.addScene("Main", new MainActivity());
    game.setCurrentScene("Main");
    game.launch();
    ImGui::SFML::Shutdown();

    return 0;
}