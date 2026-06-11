#include <iostream>

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({1280, 720}),
        "SFML 3"
    );
    window.setFramerateLimit(60); // limit frame rate to 60 fps
    sf::Clock deltaClock;
    sf::CircleShape circle1(50.f);
    circle1.setFillColor(sf::Color(100, 250, 50));
    
    sf::CircleShape circle2(30.f);
    circle2.setFillColor(sf::Color(0, 250, 50));

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        
        circle1.move({1.2f, 2.5f});  
        circle2.move({2.2f, 1.5f});  
        window.draw(circle1);
        window.draw(circle2);
        window.display();
    }
}