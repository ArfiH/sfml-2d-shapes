#include <iostream>
#include <memory>
#include <fstream>
#include <vector>

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

class CircleShape {
public:
    std::string m_name  {"CRed"};
    std::string m_shape {"Circle"};
    float m_x           {100.f};
    float m_y           {100.f};
    float m_speedX      {2.f};
    float m_speedY      {4.f};
    int m_red           {255};
    int m_green         {0};
    int m_blue          {0};
    float m_radius      {50.f};

    CircleShape(const std::string& name, const std:: string& shape, float x, float y, float speedX, float speedY, int red, int green, int blue, float rad)
        : m_name    (name)  
        , m_shape   (shape)
        , m_x       (x)
        , m_y       (y)
        , m_speedX  (speedX)
        , m_speedY  (speedY)
        , m_red     (red)
        , m_green   (green)
        , m_blue    (blue)
        , m_radius  (rad) 
    {}

    void print() const {
        std::cout << "name: " << m_name << ", shape: " << m_shape << ", x: " << m_x << ", y: " << m_y << ", sx: " << m_speedX << ", sy: " << m_speedY << ", r: " << m_red << ", g: " << m_green << ", b: " << m_blue<< ", rad: " << m_radius << '\n';
    }
};

class RectShape {
public:
    std::string m_name  {"RGrey"};
    std::string m_shape {"Rectangle"};
    float m_x           {100.f};
    float m_y           {100.f};
    float m_speedX      {2.f};
    float m_speedY      {4.f};
    int m_red           {255};
    int m_green         {0};
    int m_blue          {0};
    float m_w           {100.f};
    float m_h           {100.f};

    RectShape(const std::string& name, const std:: string& shape, float x, float y, float speedX, float speedY, int red, int green, int blue, float w, float h)
        : m_name    (name)  
        , m_shape   (shape)
        , m_x       (x)
        , m_y       (y)
        , m_speedX  (speedX)
        , m_speedY  (speedY)
        , m_red     (red)
        , m_green   (green)
        , m_blue    (blue)
        , m_w       (w)
        , m_h       (h)
    {}

    void print() const {
        std::cout << "name: " << m_name << ", shape: " << m_shape << ", x: " << m_x << ", y: " << m_y << ", sx: " << m_speedX << ", sy: " << m_speedY << ", r: " << m_red << ", g: " << m_green << ", b: " << m_blue<< ", w: " << m_w << ", h: " << m_h << '\n';
    }
};


int main(int argc, char* argv[])
{
    // let's make a shape that we will draw to the screen
    float circleRadius = 50;    // radius to draw the circle
    int circleSegments = 32;    // number of segments to draw the circle with
    float circleSpeedX = 1.0f;  // we will use this to move the circle later
    float circleSpeedY = 0.5f;  // you will read these values from the file
    bool drawCircle = true;     // whether to draw the circle
    bool drawText = true;       // whether to draw the text
    float rectW = 100.f;
    float rectH = 100.f;
    unsigned int wWidth = 1280;
    unsigned int wHeight = 720;
    std::string fontStr = "";
    int textSize = 24;
    float red, green, blue;
    int circleCount = 0;

    // vector to store all shapes
    std::vector<CircleShape> cShapes;
    std::vector<RectShape> rShapes;
    std::vector<sf::CircleShape> sfCircleShapes;
    std::vector<sf::RectangleShape> sfRectShapes;
    std::vector<sf::Text> sfCircleTexts;
    std::vector<sf::Text> sfRectTexts;

    const std::string FILENAME = "../assets/config/config.txt";
    std::ifstream fin(FILENAME);
    if (!fin.is_open()) {
        std::cerr << "Could not open " << FILENAME << " file.\n";
        return 1;
    }

    std::cout << "Reading contents from " << FILENAME << " file.\n";
    std::string configStr = "";
    int configInt;

    // create a new window of size w*h pixels
    // top-left of the window is (0,0) and bottom-right is (w,h)
    // you will have to read these from the config file
    
    while (fin >> configStr) { // inputs Window string
        if (configStr == "Window") {
            fin >> configInt; 
            wWidth = configInt;
            fin >> configInt;
            wHeight = configInt;

            std::cout << "Window size: " << wWidth << ' ' << wHeight << '\n';
        }
        else if (configStr == "Font") {
            fin >> fontStr;
            fin >> textSize;
            fin >> red >> green >> blue;
            std::cout << red << ' ' << green << ' ' << blue << '\n';
        }
        else {
            std::string shape = configStr;
            std::string name;
            fin >> name;

            float x, y, speedX, speedY;
            fin >> x >> y >> speedX >> speedY;

            int r, g, b;
            fin >> r >> g >> b;

            if (shape == "Circle") {
                float rad;
                fin >> rad;
                cShapes.push_back(CircleShape(name, shape, x, y, speedX, speedY, r, g, b, rad));
                cShapes.back().print();
                circleCount++;
            }
            else {
                float w, h;
                fin >> w >> h;
                rShapes.push_back(RectShape(name, shape, x, y, speedX, speedY, r, g, b, w, h));
                rShapes.back().print();
            }
        }
    }

    sf::RenderWindow window(sf::VideoMode({wWidth, wHeight}), "Assignment 1");
    window.setFramerateLimit(60); // limit frame rate to 60 fps

    // initialize IMGUI and create a clock used for its internal timing
    if (!ImGui::SFML::Init(window))
    {
        std::cerr << "Failed to initialize ImGui-SFML\n";
        return -1;
    }
    sf::Clock deltaClock;

    // scale the imgui ui by a given factor, does not affect text size
    ImGui::GetStyle().ScaleAllSizes(2.0f);
    // ImGui::GetIO().FontGlobalScale = 2.0f; // this line affect text size

    // the imgui color {r, g, b} wheel requires floats from 0-1 instead of ints from 0-255
    float c[3] = {0.0f, 1.0f, 1.0f};

    // let's load a font, so we can display some text
    sf::Font myFont;

    // attempt to load the font from a file
    if (!myFont.openFromFile(fontStr))
    {
        // if we can't load the font, print an error to the error console and exit
        std::cerr << "Could not load font!\n";
        exit(-1);
    }

    // set up the text object that will be drawn to the screen
    sf::Text text(myFont, "Sample Text", textSize);

    // set the color
    text.setFillColor(sf::Color(red, green, blue, 255)); 

    // position the top-left corner of the text so that the text aligns on the bottom
    // text character size is in pixels, so move the text up from the bottom by its height
    text.setPosition({
        0.f,
        static_cast<float>(wHeight - text.getCharacterSize())
    });
    // set up a character array to set the text
    char displayString[255] = "Sample Text";

    for (const auto& shape : cShapes) {
        // create the sfml circle shape based on our parameters
        sf::CircleShape circle(shape.m_radius, 32); // create a circle shape with radius 50
        circle.setPosition({shape.m_x, shape.m_y}); // set the top-left position of the circle
        circle.setFillColor(sf::Color(shape.m_red, shape.m_green, shape.m_blue));
        circle.setRadius(shape.m_radius);

        // basic animation - move each shape if it's still in frame
        sfCircleShapes.push_back(circle);

        // set up the text object that will be drawn to the screen
        sf::Text shapeText(myFont, shape.m_name, textSize);

        // set the color
        shapeText.setFillColor(sf::Color(255 - shape.m_red, 255 - shape.m_green, 255 - shape.m_blue)); 

        shapeText.setPosition({shape.m_x + shape.m_radius - textSize, shape.m_y + shape.m_radius - 10});
        sfCircleTexts.push_back(shapeText);
        std::cout << "Circle pushed\n";
    }

    for (const auto& shape : rShapes) {    
        sf::RectangleShape rectangle({shape.m_w, shape.m_h});
        rectangle.setPosition({shape.m_x, shape.m_y}); // set the top-left position of the rectangle
        rectangle.setFillColor(sf::Color(shape.m_red, shape.m_green, shape.m_blue));

        // basic animation - move each shape if it's still in frame
        sfRectShapes.push_back(rectangle);

        // set up the text object that will be drawn to the screen
        sf::Text shapeText(myFont, shape.m_name, textSize);

        // set the color
        shapeText.setFillColor(sf::Color(255 - shape.m_red, 255 - shape.m_green, 255 - shape.m_blue)); 

        shapeText.setPosition({shape.m_x + (shape.m_w / 2.f) - textSize, shape.m_y + (shape.m_h / 2.f) - 10});
        sfRectTexts.push_back(shapeText);
        std::cout << "rectangle pushed\n";
    }

    // Keep these variables persistent (static, class members, etc.)
    static std::vector<std::string> comboItems;
    static int selected_idx = 0; 

    for (size_t i = 0; i < cShapes.size(); i++) {
        comboItems.push_back(cShapes[i].m_name);
    }
    for (size_t i = 0; i < rShapes.size(); i++) {
        comboItems.push_back(rShapes[i].m_name);
    }


    // main loop - continues for each frame while window is open
    while (window.isOpen())
    {
        // event handling
        while (const auto event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event);

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (const auto* keyPressed =
                    event->getIf<sf::Event::KeyPressed>())
            {
                std::cout << "Key pressed\n";

                if (keyPressed->code == sf::Keyboard::Key::X)
                {
                    circleSpeedX *= -1.0f;
                }
            }
        }

        // update imgui for this frame with the time that the last frame took
        ImGui::SFML::Update(window, deltaClock.restart());
        // ImGui::ShowDemoWindow();

        // draw the UI
        ImGui::Begin("Debug Window");
        ImGui::Text("Toggle visibilty");
        ImGui::Checkbox("Draw Shapes", &drawCircle);
        ImGui::SameLine();
        ImGui::Checkbox("Draw Text", &drawText);

        // Get the text preview string of the currently chosen item
        const char* combo_preview_value = comboItems[selected_idx].c_str();

        if (ImGui::BeginCombo("Select circle", combo_preview_value)) 
        {
            for (int n = 0; n < comboItems.size(); n++) 
            {
                const bool is_selected = (selected_idx == n);
                
                // Render each selectable item in the dropdown
                if (ImGui::Selectable(comboItems[n].c_str(), is_selected)) 
                {
                    selected_idx = n; // Update selection when clicked
                    std::cout << "Selected idx is " << selected_idx << '\n'; 
                }

                // Set the initial focus when opening the combo box
                if (is_selected) 
                {
                    ImGui::SetItemDefaultFocus(); 
                }
            }
            ImGui::EndCombo(); // Must always match BeginCombo() if it returns true
        }

        if (ImGui::SliderFloat("Radius", &circleRadius, 0.0f, 300.0f)) {
            if (selected_idx < circleCount) {
                sfCircleShapes[selected_idx].setRadius(circleRadius);
            }
        }
        
        if (ImGui::SliderFloat("Width", &rectW, 0.0f, 300.0f) || ImGui::SliderFloat("Height", &rectH, 0.0f, 300.0f)) {
            sfRectShapes[selected_idx - circleCount].setSize(sf::Vector2f(rectW, rectH));
        }
        
        if (ImGui::SliderInt("Point count", &circleSegments, 3, 64)) {
            if (selected_idx < circleCount) {
                sfCircleShapes[selected_idx].setPointCount(circleSegments);
            }
        }

        if (ImGui::ColorEdit3("Color Shape", c)) {
            if (selected_idx < circleCount) {    
                sfCircleShapes[selected_idx].setFillColor(
                    sf::Color(
                        static_cast<std::uint8_t>(c[0] * 255),
                        static_cast<std::uint8_t>(c[1] * 255),
                        static_cast<std::uint8_t>(c[2] * 255)
                    )
                );
                
                // Make the text color the complement of shape color
                sfCircleTexts[selected_idx].setFillColor(sf::Color(255 - (c[0] * 255), 255 - (c[1] * 255), 255 - (c[2] * 255)));
            }
            else {
                sfRectShapes[selected_idx - circleCount].setFillColor(
                    sf::Color(
                        static_cast<std::uint8_t>(c[0] * 255),
                        static_cast<std::uint8_t>(c[1] * 255),
                        static_cast<std::uint8_t>(c[2] * 255)
                    )
                );

                // Make the text color the complement of shape color
                sfRectTexts[selected_idx - circleCount].setFillColor(sf::Color(255 - (c[0] * 255), 255 - (c[1] * 255), 255 - (c[2] * 255)));
            }
        }

        ImGui::InputText("Text", displayString, 255);

        if (ImGui::Button("Set Text"))
        {
            text.setString(displayString);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            for (size_t i = 0; i < sfRectShapes.size(); i++) {
                sfRectShapes[i].setPosition({rShapes[i].m_x, rShapes[i].m_y});
            }
            for (size_t i = 0; i < sfCircleShapes.size(); i++) {
                sfCircleShapes[i].setPosition({rShapes[i].m_x, rShapes[i].m_y});
            }
        }

        ImGui::End();

        // basic rendering function calls
        window.clear();     // clear the window of anything previously drawn

        // window.draw(circle);
        if (drawCircle)     // draw the circle if the boolean is true
        {
            for (size_t i = 0; i < sfCircleShapes.size(); i++) {
                sf::Vector2f position = sfCircleShapes[i].getPosition();
                float currentX = position.x;
                float currentY = position.y;
                
                if (currentX < 0 || currentX + 2 * cShapes[i].m_radius > wWidth) { 
                    cShapes[i].m_speedX *= -1.f;
                }
                if (currentY < 0 || currentY + 2 * cShapes[i].m_radius > wHeight) {
                    cShapes[i].m_speedY *= -1.f;
                }
                sfCircleShapes[i].move({cShapes[i].m_speedX, cShapes[i].m_speedY});
                window.draw(sfCircleShapes[i]);

                sfCircleTexts[i].move({cShapes[i].m_speedX, cShapes[i].m_speedY});
                window.draw(sfCircleTexts[i]);
            }

            for (size_t i = 0; i < sfRectShapes.size(); i++) {
                sf::Vector2f position = sfRectShapes[i].getPosition();
                float currentX = position.x;
                float currentY = position.y;
                
                if (currentX < 0 || currentX + rShapes[i].m_w > wWidth) { 
                    rShapes[i].m_speedX *= -1.f;
                }
                if (currentY < 0 || currentY + rShapes[i].m_h > wHeight) {
                    rShapes[i].m_speedY *= -1.f;
                }
                sfRectShapes[i].move({rShapes[i].m_speedX, rShapes[i].m_speedY});
                window.draw(sfRectShapes[i]);

                sfRectTexts[i].move({rShapes[i].m_speedX, rShapes[i].m_speedY});
                window.draw(sfRectTexts[i]);
            }
        }

        if (drawText)       // draw the text if the boolean is true
        {
            window.draw(text);
        }
        ImGui::SFML::Render(window);    // draw the ui last so it's on top
        window.display();               // call the window display function
    }

    ImGui::SFML::Shutdown();
    
    return 0;
}