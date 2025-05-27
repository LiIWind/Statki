#pragma once
#include <SFML/Graphics.hpp>

enum class AkcjaMenu {
    Nic,
    NowaGra,
    WczytajGre,
    Zamknij
};

AkcjaMenu rysujIMenu(sf::RenderWindow& window, sf::Font& font, sf::Texture& menuBgTexture,
    sf::Texture& btnNowaTexture, sf::Texture& btnWczytajTexture, sf::Texture& btnWyjdzTexture);