#include <iostream>
#include <memory>
#include <fstream>
#include <vector>

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

class Shape {
public:
    std::string m_name      {""};
    std::string m_shape     {""};
    float m_x               {100.f};
    float m_y               {100.f};
    float m_speedX          {2.f};
    float m_speedY          {4.f};
    int m_red               {255};
    int m_green             {0};
    int m_blue              {0};
    float m_radius          {50.f};
    float m_w               {100.f}; // will be used for rectangles
    float m_h               {100.f}; // will be used for rectangles
    bool m_isAlive          {true};
    bool m_isTextAlive      {true};

    Shape(const std::string& name, const std:: string& shape, float x, float y, float speedX, float speedY, int red, int green, int blue, float rad)
        : m_name        (name)  
        , m_shape       (shape)
        , m_x           (x)
        , m_y           (y)
        , m_speedX      (speedX)
        , m_speedY      (speedY)
        , m_red         (red)
        , m_green       (green)
        , m_blue        (blue)
        , m_radius      (rad) 
        , m_isAlive     (true)
        , m_isTextAlive (true)
    {}

    Shape(const std::string& name, const std:: string& shape, float x, float y, float speedX, float speedY, int red, int green, int blue, float w, float h)
        : m_name        (name)  
        , m_shape       (shape)
        , m_x           (x)
        , m_y           (y)
        , m_speedX      (speedX)
        , m_speedY      (speedY)
        , m_red         (red)
        , m_green       (green)
        , m_blue        (blue)
        , m_w           (w)
        , m_h           (h)
        , m_isAlive     (true)
        , m_isTextAlive (true)
    {}

    void printCircle() const {
        std::cout << "name: " << m_name << ", shape: " << m_shape << ", x: " << m_x << ", y: " << m_y << ", sx: " << m_speedX << ", sy: " << m_speedY << ", r: " << m_red << ", g: " << m_green << ", b: " << m_blue<< ", rad: " << m_radius << '\n';
    }

    void printRectangle() const {
        std::cout << "name: " << m_name << ", shape: " << m_shape << ", x: " << m_x << ", y: " << m_y << ", sx: " << m_speedX << ", sy: " << m_speedY << ", r: " << m_red << ", g: " << m_green << ", b: " << m_blue<< ", w: " << m_w << ", h: " << m_h << '\n';
    }    
};


int main(int argc, char* argv[])
{
    const int SCALE_FACTOR  = 50; 
    float circleRadius      = 50;
    int circleSegments      = 32;
    float circleSpeedX      = 1.0f;
    float circleSpeedY      = 0.5f;
    bool drawShape          = true;
    bool drawText           = true;
    float rectW             = 100.f;
    float rectH             = 100.f;
    int shapeScale          = 0;
    unsigned int wWidth     = 1280;
    unsigned int wHeight    = 720;
    std::string fontStr     = "";
    int textSize            = 24;
    float red               = 0;
    float green             = 0;
    float blue              = 0;
    int circleCount         = 0;
    int velocityX           = 0;
    int velocityY           = 0;

    // vector to store all shapes
    std::vector<Shape> shapes;
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
                shapes.push_back(Shape(name, shape, x, y, speedX, speedY, r, g, b, rad));
                shapes.back().printCircle();
                circleCount++;
            }
            else {
                float w, h;
                fin >> w >> h;
                shapes.push_back(Shape(name, shape, x, y, speedX, speedY, r, g, b, w, h));
                shapes.back().printRectangle();
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
    text.setPosition({0.f, static_cast<float>(wHeight - text.getCharacterSize())});
  
    // set up a character array to set the text
    char displayString[255] = "Sample Text";

    for (const auto& shape : shapes) {
        if (shape.m_shape == "Circle") {
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
        else {
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
    }

    // Keep these variables persistent (static, class members, etc.)
    static std::vector<std::string> comboItems;
    static int selected_idx = 0; 

    for (size_t i = 0; i < shapes.size(); i++) {
        comboItems.push_back(shapes[i].m_name);
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

        // Get the text preview string of the currently chosen item
        const char* combo_preview_value = comboItems[selected_idx].c_str();

        if (ImGui::BeginCombo("Select shape", combo_preview_value)) 
        {
            for (int n = 0; n < comboItems.size(); n++) 
            {
                const bool is_selected = (selected_idx == n);
                
                // Render each selectable item in the dropdown
                if (ImGui::Selectable(comboItems[n].c_str(), is_selected)) 
                {
                    selected_idx = n; // Update selection when clicked
                    std::cout << "Selected idx is " << selected_idx << '\n';

                    // set displayString to selected shape's name
                    size_t i = 0;                
                    while (displayString[i] = shapes[selected_idx].m_name[i]) {
                        i++;
                    }
                }

                // Set the initial focus when opening the combo box
                if (is_selected) 
                {
                    ImGui::SetItemDefaultFocus(); 
                }
            }
            ImGui::EndCombo(); // Must always match BeginCombo() if it returns true
        }

        ImGui::Text("Toggle visibilty");
        if (ImGui::Checkbox("Draw Shapes", &drawShape)) {
            shapes[selected_idx].m_isAlive = drawShape;
            std::cout << shapes[selected_idx].m_name << "\'s visibilty is set to " << shapes[selected_idx].m_isAlive << '\n';
        }

        ImGui::SameLine();
        if (ImGui::Checkbox("Draw Text", &drawText)) {
            shapes[selected_idx].m_isTextAlive = drawText;    
            std::cout << shapes[selected_idx].m_name << "\'s text visibilty is set to " << shapes[selected_idx].m_isTextAlive << '\n'; 
        }

        ImGui::Text("Velocity");
        if (ImGui::SliderInt("x velocity", &velocityX, -8, 8)) {
            shapes[selected_idx].m_speedX = velocityX; 
        }
        if (ImGui::SliderInt("y velocity", &velocityY, -8, 8)) {
            shapes[selected_idx].m_speedY = velocityY; 
        }

        if (ImGui::SliderInt("Scale", &shapeScale, 0, 4)) {
            if (shapes[selected_idx].m_shape == "Circle") {
                float newRadius = circleRadius + shapeScale * SCALE_FACTOR;
                sfCircleShapes[selected_idx].setRadius(newRadius);
                shapes[selected_idx].m_radius = newRadius;

                // update text's position to keep text in centre
                sfCircleTexts[selected_idx].setPosition({sfCircleShapes[selected_idx].getPosition().x + shapes[selected_idx].m_radius - textSize, sfCircleShapes[selected_idx].getPosition().y + shapes[selected_idx].m_radius - 10});
            }
            else {
                float newWidth = rectW + shapeScale * SCALE_FACTOR;
                float newHeight = rectH + shapeScale * SCALE_FACTOR;
                sfRectShapes[selected_idx - circleCount].setSize(sf::Vector2f(newWidth, newHeight)); 
                shapes[selected_idx].m_w = newWidth;   
                shapes[selected_idx].m_h = newHeight;

                // update text's position to keep text in centre
                sfRectTexts[selected_idx - circleCount].setPosition({sfRectShapes[selected_idx - circleCount].getPosition().x + (shapes[selected_idx].m_w / 2.f) - textSize, sfRectShapes[selected_idx - circleCount].getPosition().y + (shapes[selected_idx].m_h / 2.f) - 10});                   
            }
        }

        if (shapes[selected_idx].m_shape == "Circle" && ImGui::SliderFloat("Radius", &circleRadius, 0.0f, 300.0f)) {
            if (shapes[selected_idx].m_shape == "Circle") {
                sfCircleShapes[selected_idx].setRadius(circleRadius);
            }
        }
        
        if (shapes[selected_idx].m_shape == "Rectangle" && ImGui::SliderFloat("Width", &rectW, 0.0f, 300.0f) || ImGui::SliderFloat("Height", &rectH, 0.0f, 300.0f)) {
            if (shapes[selected_idx].m_shape == "Rectangle")
                sfRectShapes[selected_idx - circleCount].setSize(sf::Vector2f(rectW, rectH));
        }
        
        if (shapes[selected_idx].m_shape == "Circle" && ImGui::SliderInt("Point count", &circleSegments, 3, 64)) {
            if (shapes[selected_idx].m_shape == "Circle") {
                sfCircleShapes[selected_idx].setPointCount(circleSegments);
            }
        }

        if (ImGui::ColorEdit3("Color Shape", c)) {
            if (shapes[selected_idx].m_shape == "Circle") {    
                sfCircleShapes[selected_idx].setFillColor(sf::Color((c[0] * 255),(c[1] * 255),(c[2] * 255)));
                
                // Make the text color the complement of shape color
                sfCircleTexts[selected_idx].setFillColor(sf::Color(255 - (c[0] * 255), 255 - (c[1] * 255), 255 - (c[2] * 255)));
            }
            else {
                sfRectShapes[selected_idx - circleCount].setFillColor(sf::Color((c[0] * 255),(c[1] * 255),(c[2] * 255)));

                // Make the text color the complement of shape color
                sfRectTexts[selected_idx - circleCount].setFillColor(sf::Color(255 - (c[0] * 255), 255 - (c[1] * 255), 255 - (c[2] * 255)));
            }
        }

        ImGui::InputText("Text", displayString, 255);

        if (ImGui::Button("Set Text"))
        {
            if (shapes[selected_idx].m_shape == "Circle") {
                sfCircleTexts[selected_idx].setString(displayString);
            }
            else {
                sfRectTexts[selected_idx - circleCount].setString(displayString);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            for (size_t i = 0; i < sfCircleShapes.size(); i++) {
                if (shapes[selected_idx].m_shape == "Circle") {
                    sfCircleTexts[selected_idx].setString(shapes[selected_idx].m_name);
                }

                sfCircleShapes[i].setPosition({shapes[i].m_x, shapes[i].m_y});
                sfCircleTexts[i].setPosition({shapes[i].m_x + shapes[i].m_radius - textSize, shapes[i].m_y + shapes[i].m_radius - 10});
            }
            for (size_t i = 0; i < sfRectShapes.size(); i++) {
                if (shapes[selected_idx].m_shape == "Rectangle") {
                    sfRectTexts[selected_idx - circleCount].setString(shapes[selected_idx].m_name);
                }

                sfRectShapes[i].setPosition({shapes[i + circleCount].m_x, shapes[i + circleCount].m_y});
                sfRectTexts[i].setPosition({shapes[i + circleCount].m_x + (shapes[i + circleCount].m_w / 2.f) - textSize, shapes[i + circleCount].m_y + (shapes[i + circleCount].m_h / 2.f) - 10});
            }
        }

        ImGui::End();

        // basic rendering function calls
        window.clear();     // clear the window of anything previously drawn

        // window.draw(circle);
        
        for (size_t i = 0; i < sfCircleShapes.size(); i++) {
            sf::Vector2f position = sfCircleShapes[i].getPosition();
            float currentX = position.x;
            float currentY = position.y;
            
            if (currentX < 0 || currentX + 2 * shapes[i].m_radius > wWidth) { 
                shapes[i].m_speedX *= -1.f;
            }
            if (currentY < 0 || currentY + 2 * shapes[i].m_radius > wHeight) {
                shapes[i].m_speedY *= -1.f;
            }
            sfCircleShapes[i].move({shapes[i].m_speedX, shapes[i].m_speedY});
            if (shapes[i].m_isAlive) {
                window.draw(sfCircleShapes[i]);
            }

            sfCircleTexts[i].move({shapes[i].m_speedX, shapes[i].m_speedY});
            if (shapes[i].m_isTextAlive) {
                window.draw(sfCircleTexts[i]);
            }
        }

        for (size_t i = 0; i < sfRectShapes.size(); i++) {
            sf::Vector2f position = sfRectShapes[i].getPosition();
            float currentX = position.x;
            float currentY = position.y;
            
            if (currentX < 0 || currentX + shapes[i + circleCount].m_w > wWidth) { 
                shapes[i + circleCount].m_speedX *= -1.f;
            }
            if (currentY < 0 || currentY + shapes[i + circleCount].m_h > wHeight) {
                shapes[i + circleCount].m_speedY *= -1.f;
            }
            sfRectShapes[i].move({shapes[i + circleCount].m_speedX, shapes[i + circleCount].m_speedY});
            if (shapes[i + circleCount].m_isAlive) {
                window.draw(sfRectShapes[i]);
            }

            sfRectTexts[i].move({shapes[i + circleCount].m_speedX, shapes[i + circleCount].m_speedY});
            if (shapes[i + circleCount].m_isTextAlive) {
                window.draw(sfRectTexts[i]);
            }
        }

        ImGui::SFML::Render(window);    // draw the ui last so it's on top
        window.display();               // call the window display function
    }

    ImGui::SFML::Shutdown();
    
    return 0;
}