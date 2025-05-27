#include "Menu.h"

AkcjaMenu rysujIMenu(sf::RenderWindow& window, sf::Font& font, sf::Texture& menuBgTexture,
    sf::Texture& btnNowaTexture, sf::Texture& btnWczytajTexture, sf::Texture& btnWyjdzTexture) {
    window.clear();

    sf::Sprite bg(menuBgTexture);
    bg.setScale(
        float(window.getSize().x) / menuBgTexture.getSize().x,
        float(window.getSize().y) / menuBgTexture.getSize().y
    );
    window.draw(bg);

    sf::Sprite btnNowa(btnNowaTexture);
    sf::Sprite btnWczytaj(btnWczytajTexture);
    sf::Sprite btnWyjdz(btnWyjdzTexture);

    float btnWidth = btnNowaTexture.getSize().x;
    float btnHeight = btnNowaTexture.getSize().y;
    float scaleX = 360.f / btnWidth;
    float scaleY = 60.f / btnHeight;
    btnNowa.setScale(scaleX, scaleY);
    btnWczytaj.setScale(scaleX, scaleY);
    btnWyjdz.setScale(scaleX, scaleY);

    int oknoSzer = window.getSize().x;
    btnNowa.setPosition(oknoSzer / 2.f - 180, 250);
    btnWczytaj.setPosition(oknoSzer / 2.f - 180, 350);
    btnWyjdz.setPosition(oknoSzer / 2.f - 180, 450);

    window.draw(btnNowa);
    window.draw(btnWczytaj);
    window.draw(btnWyjdz);

    window.display();

    sf::Event event;
    while (window.waitEvent(event)) {
        if (event.type == sf::Event::Closed)
            return AkcjaMenu::Zamknij;

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mouse(event.mouseButton.x, event.mouseButton.y);
            if (btnNowa.getGlobalBounds().contains(mouse))
                return AkcjaMenu::NowaGra;
            if (btnWczytaj.getGlobalBounds().contains(mouse))
                return AkcjaMenu::WczytajGre;
            if (btnWyjdz.getGlobalBounds().contains(mouse))
                return AkcjaMenu::Zamknij;
        }
    }
    return AkcjaMenu::Nic;
}