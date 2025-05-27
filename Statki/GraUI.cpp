#include "GraUI.h"

bool czyStatekZatopiony(const Statki::Plansza& plansza, int x, int y, std::set<std::pair<int, int>>& odwiedzone) {
    int rozmiar = plansza.getRozmiar();
    if (odwiedzone.count({ x, y }) || x < 0 || x >= rozmiar || y < 0 || y >= rozmiar) {
        return true;
    }

    if (plansza.getPole(x, y) == Statki::Pole::Statek) {
        return false;
    }

    if (plansza.getPole(x, y) != Statki::Pole::Trafiony) {
        return true;
    }

    odwiedzone.insert({ x, y });

    return czyStatekZatopiony(plansza, x + 1, y, odwiedzone) &&
        czyStatekZatopiony(plansza, x - 1, y, odwiedzone) &&
        czyStatekZatopiony(plansza, x, y + 1, odwiedzone) &&
        czyStatekZatopiony(plansza, x, y - 1, odwiedzone);
}

void rysujPlansze(sf::RenderWindow& window, const Statki::Plansza& plansza, int przesuniecieX, int przesuniecieY,
    int rozmiarPola, sf::Texture& textureStatek, sf::Texture& textureTrafiony,
    sf::Texture& texturePudlo, sf::Texture& textureZatopiony, bool pokazStatki) {
    int rozmiar = plansza.getRozmiar();
    std::set<std::pair<int, int>> odwiedzone;
    for (int i = 0; i < rozmiar; ++i) {
        for (int j = 0; j < rozmiar; ++j) {
            sf::RectangleShape pole(sf::Vector2f(rozmiarPola, rozmiarPola));
            pole.setPosition(static_cast<float>(przesuniecieX + j * rozmiarPola),
                static_cast<float>(przesuniecieY + i * rozmiarPola));
            pole.setOutlineThickness(1);
            pole.setOutlineColor(sf::Color::Black);
            pole.setFillColor(sf::Color(200, 200, 255, 180));
            window.draw(pole);

            switch (plansza.getPole(i, j)) {
            case Statki::Pole::Statek:
                if (pokazStatki) {
                    sf::Sprite sprite(textureStatek);
                    sprite.setPosition(static_cast<float>(przesuniecieX + j * rozmiarPola),
                        static_cast<float>(przesuniecieY + i * rozmiarPola));
                    window.draw(sprite);
                }
                break;
            case Statki::Pole::Trafiony: {
                std::set<std::pair<int, int>> lokalneOdwiedzone;
                if (czyStatekZatopiony(plansza, i, j, lokalneOdwiedzone)) {
                    sf::Sprite sprite(textureZatopiony);
                    sprite.setPosition(static_cast<float>(przesuniecieX + j * rozmiarPola),
                        static_cast<float>(przesuniecieY + i * rozmiarPola));
                    sprite.setColor(sf::Color(255, 255, 255, 255));
                    window.draw(sprite);
                }
                else {
                    sf::Sprite sprite(textureTrafiony);
                    sprite.setPosition(static_cast<float>(przesuniecieX + j * rozmiarPola),
                        static_cast<float>(przesuniecieY + i * rozmiarPola));
                    window.draw(sprite);
                }
                break;
            }
            case Statki::Pole::Pudlo: {
                sf::Sprite sprite(texturePudlo);
                sprite.setPosition(static_cast<float>(przesuniecieX + j * rozmiarPola),
                    static_cast<float>(przesuniecieY + i * rozmiarPola));
                window.draw(sprite);
                break;
            }
            }
        }
    }
}