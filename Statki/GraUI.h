#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "StatkiDef.h"
#include "Plansza.h"

void rysujPlansze(sf::RenderWindow& window, const Statki::Plansza& plansza, int przesuniecieX, int przesuniecieY,
    int rozmiarPola, sf::Texture& textureStatek, sf::Texture& textureTrafiony,
    sf::Texture& texturePudlo, sf::Texture& textureZatopiony, bool pokazStatki);

bool czyStatekZatopiony(const Statki::Plansza& plansza, int x, int y, std::set<std::pair<int, int>>& odwiedzone);