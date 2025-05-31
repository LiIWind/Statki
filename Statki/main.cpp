#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include "Menu.h"
#include "GraUI.h"
#include "Gra.h"

const int ROZMIAR_PLANSZY = 10;
const int ROZMIAR_POLA = 32;
const int ODSTEP = 50;
const int OKNO_SZEROKOSC = 2 * ROZMIAR_PLANSZY * ROZMIAR_POLA + ODSTEP + 200;
const int OKNO_WYSOKOSC = ROZMIAR_PLANSZY * ROZMIAR_POLA + 250;
const int NAGLOWEK_WYS = 80;
const int STOPKA_WYS = 60;

enum class StanAplikacji {
    Menu,
    Gra
};

int main() {
    sf::RenderWindow window(sf::VideoMode(OKNO_SZEROKOSC, OKNO_WYSOKOSC), "Statki");

    sf::Music muzyka;
    if (!muzyka.openFromFile("muzyka.wav")) {
        std::cerr << "Nie mo¿na za³adowaæ muzyki (muzyka.wav)!" << std::endl;
    }
    muzyka.setLoop(true);
    muzyka.setVolume(20.f);
    muzyka.play();
    

    sf::Texture textureMenuBg;
    if (!textureMenuBg.loadFromFile("menu_bg.png")) {
        std::cerr << "Nie mo¿na za³adowaæ t³a menu (menu_bg.png)!" << std::endl;
        return -1;
    }

    sf::Texture btnNowaTexture, btnWczytajTexture, btnWyjdzTexture;
    if (!btnNowaTexture.loadFromFile("btn_nowa.png") ||
        !btnWczytajTexture.loadFromFile("btn_wczytaj.png") ||
        !btnWyjdzTexture.loadFromFile("btn_wyjscie.png")) {
        std::cerr << "Nie mo¿na za³adowaæ tekstur przycisków!" << std::endl;
        return -1;
    }

    sf::Texture textureBitwa;
    if (!textureBitwa.loadFromFile("bitwa.png")) {
        std::cerr << "Nie mo¿na za³adowaæ obrazka nag³ówka (bitwa.png)!" << std::endl;
        return -1;
    }
    sf::Sprite spriteBitwa(textureBitwa);

    sf::Texture textureWoda;
    if (!textureWoda.loadFromFile("woda.png")) {
        std::cerr << "Nie mo¿na za³adowaæ tekstury t³a (woda.png)!" << std::endl;
        return -1;
    }
    sf::Sprite spriteWoda(textureWoda);

    sf::Texture textureStatek, textureTrafiony, texturePudlo, textureZatopiony;
    if (!textureStatek.loadFromFile("statek.png") ||
        !textureTrafiony.loadFromFile("trafiony.png") ||
        !texturePudlo.loadFromFile("pudlo.png") ||
        !textureZatopiony.loadFromFile("zatopiony.png")) {
        std::cerr << "Nie mo¿na za³adowaæ tekstur statków/trafieñ/pude³/zatopionych!" << std::endl;
        return -1;
    }

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Nie mozna za³adowac czcionki!" << std::endl;
        return -1;
    }

    StanAplikacji stan = StanAplikacji::Menu;
    std::vector<int> statkiDoUmieszczenia = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
    bool graWczytana = false;
    Statki::Gra* gra = nullptr;

    while (window.isOpen()) {
        if (stan == StanAplikacji::Menu) {
            AkcjaMenu akcja = rysujIMenu(window, font, textureMenuBg, btnNowaTexture, btnWczytajTexture, btnWyjdzTexture);
            if (akcja == AkcjaMenu::Zamknij) {
                window.close();
                return 0;
            }
            else if (akcja == AkcjaMenu::NowaGra) {
                if (gra) delete gra;
                gra = new Statki::Gra(ROZMIAR_PLANSZY, statkiDoUmieszczenia);
                graWczytana = false;
                stan = StanAplikacji::Gra;
            }
            else if (akcja == AkcjaMenu::WczytajGre) {
                if (gra) delete gra;
                gra = new Statki::Gra(ROZMIAR_PLANSZY, statkiDoUmieszczenia);
                try {
                    gra->wczytajGre("zapis_gry.bin");
                    graWczytana = true;
                    stan = StanAplikacji::Gra;
                }
                catch (const std::exception& e) { //Obs³uga wyj¹tków
                    sf::Text blad("B³ad wczytywania gry!", font, 32);
                    blad.setFillColor(sf::Color::Red);
                    blad.setPosition(OKNO_SZEROKOSC / 2.f - blad.getLocalBounds().width / 2, 560);
                    window.draw(blad);
                    window.display();
                    sf::sleep(sf::seconds(2));
                }
            }
        }
        else if (stan == StanAplikacji::Gra && gra) {
            float maxNaglowekSzer = OKNO_SZEROKOSC * 0.8f;
            float maxNaglowekWys = float(NAGLOWEK_WYS);

            float scaleBitwa = std::min(
                maxNaglowekSzer / textureBitwa.getSize().x,
                maxNaglowekWys / textureBitwa.getSize().y
            );
            spriteBitwa.setScale(scaleBitwa, scaleBitwa);

            float spriteBitwaWidth = textureBitwa.getSize().x * scaleBitwa;
            float spriteBitwaHeight = textureBitwa.getSize().y * scaleBitwa;
            spriteBitwa.setPosition(
                (OKNO_SZEROKOSC - spriteBitwaWidth) / 2.f,
                (NAGLOWEK_WYS - spriteBitwaHeight + 20) / 2.f
            );

            sf::Text komunikat;
            komunikat.setFont(font);
            komunikat.setCharacterSize(20);
            komunikat.setFillColor(sf::Color::White);
            komunikat.setStyle(sf::Text::Bold);
            komunikat.setOrigin(0, 0);

            size_t aktualnyStatek = 0;
            Statki::Orientacja orientacja = Statki::Orientacja::Pozioma;
            bool statkiUmieszczone = graWczytana;

            sf::RectangleShape podgladStatku(sf::Vector2f(ROZMIAR_POLA, ROZMIAR_POLA));
            podgladStatku.setFillColor(sf::Color(0, 255, 0, 150));

            sf::CircleShape podgladStrzalu(ROZMIAR_POLA / 3);
            podgladStrzalu.setFillColor(sf::Color(255, 255, 0, 150));
            podgladStrzalu.setOrigin(ROZMIAR_POLA / 3, ROZMIAR_POLA / 3);

            int szerokoscPlansz = 2 * ROZMIAR_PLANSZY * ROZMIAR_POLA + ODSTEP;
            int przesuniecieXPlansz = (OKNO_SZEROKOSC - szerokoscPlansz) / 2;
            int przesuniecieYPlansz = NAGLOWEK_WYS + 25;

            komunikat.setString("");
            komunikat.setOrigin(0, 0);

            while (window.isOpen() && stan == StanAplikacji::Gra) {
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                        break;
                    }
                    if (event.type == sf::Event::KeyPressed) {
                        if (event.key.code == sf::Keyboard::Escape) {
                            stan = StanAplikacji::Menu;
                            break;
                        }
                        else if (event.key.code == sf::Keyboard::F1) {
                            try {
                                gra->zapiszGre("zapis_gry.bin");
                                komunikat.setString("Gra zapisana!");
                            }
                            catch (const std::exception& e) {
                                komunikat.setString("Blad zapisu: " + std::string(e.what()));
                            }
                        }
                        else if (event.key.code == sf::Keyboard::F2) {
                            try {
                                gra->wczytajGre("zapis_gry.bin");
                                statkiUmieszczone = true;
                                graWczytana = true;
                                komunikat.setString("Gra wczytana! Kontynuuj gre.");
                            }
                            catch (const std::exception& e) {
                                komunikat.setString("B³ad wczytywania: " + std::string(e.what()));
                            }
                        }
                        else if (!statkiUmieszczone && !graWczytana && event.key.code == sf::Keyboard::R) {
                            orientacja = (orientacja == Statki::Orientacja::Pozioma) ?
                                Statki::Orientacja::Pionowa : Statki::Orientacja::Pozioma;
                        }
                    }

                    if (!statkiUmieszczone && !graWczytana) {
                        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                            int x = (event.mouseButton.y - przesuniecieYPlansz) / ROZMIAR_POLA;
                            int y = (event.mouseButton.x - przesuniecieXPlansz) / ROZMIAR_POLA;

                            if (x >= 0 && x < ROZMIAR_PLANSZY && y >= 0 && y < ROZMIAR_PLANSZY) {
                                if (gra->getPlanszaGracza().umiescStatek(x, y, statkiDoUmieszczenia[aktualnyStatek], orientacja)) {
                                    aktualnyStatek++;
                                    if (aktualnyStatek >= statkiDoUmieszczenia.size()) {
                                        statkiUmieszczone = true;
                                        komunikat.setString("Rozpocznij gre! Kliknij na plansze przeciwnika.");
                                    }
                                }
                            }
                        }
                    }
                    else if (!gra->czyKoniecGry()) {
                        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                            int x = event.mouseButton.x;
                            int y = event.mouseButton.y;

                            int planszaKompX = przesuniecieXPlansz + ROZMIAR_PLANSZY * ROZMIAR_POLA + ODSTEP;
                            if (x >= planszaKompX &&
                                x < planszaKompX + ROZMIAR_PLANSZY * ROZMIAR_POLA &&
                                y >= przesuniecieYPlansz && y < przesuniecieYPlansz + ROZMIAR_PLANSZY * ROZMIAR_POLA) {

                                int kliknieteX = (y - przesuniecieYPlansz) / ROZMIAR_POLA;
                                int kliknieteY = (x - planszaKompX) / ROZMIAR_POLA;

                                auto& planszaKomputera = gra->getPlanszaKomputera();
                                if (planszaKomputera.getPole(kliknieteX, kliknieteY) == Statki::Pole::Puste ||
                                    planszaKomputera.getPole(kliknieteX, kliknieteY) == Statki::Pole::Statek) {

                                    bool trafiony = gra->atakujKomputer(kliknieteX, kliknieteY);

                                    if (trafiony) {
                                        std::set<std::pair<int, int>> odwiedzone;
                                        if (czyStatekZatopiony(planszaKomputera, kliknieteX, kliknieteY, odwiedzone)) {
                                            komunikat.setString("Statek zatopiony!");
                                        }
                                        else {
                                            komunikat.setString("Trafiony!");
                                        }
                                    }
                                    else {
                                        komunikat.setString("Pudlo!");
                                    }

                                    if (!gra->czyKoniecGry()) {
                                        auto ruch = gra->getRuchKomputera();
                                        gra->atakujGracza(ruch.first, ruch.second);
                                    }
                                }
                            }
                        }
                    }
                }

                for (int y = 0; y < OKNO_WYSOKOSC; y += textureWoda.getSize().y) {
                    for (int x = 0; x < OKNO_SZEROKOSC; x += textureWoda.getSize().x) {
                        spriteWoda.setPosition(static_cast<float>(x), static_cast<float>(y));
                        window.draw(spriteWoda);
                    }
                }

                window.draw(spriteBitwa);

                rysujPlansze(window, gra->getPlanszaGracza(), przesuniecieXPlansz, przesuniecieYPlansz, ROZMIAR_POLA,
                    textureStatek, textureTrafiony, texturePudlo, textureZatopiony, true);
                rysujPlansze(window, gra->getPlanszaKomputera(), przesuniecieXPlansz + ROZMIAR_PLANSZY * ROZMIAR_POLA + ODSTEP,
                    przesuniecieYPlansz, ROZMIAR_POLA, textureStatek, textureTrafiony, texturePudlo, textureZatopiony, false);//zmiana widocznosci statków

                if (!statkiUmieszczone) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    int gridX = (mousePos.y - przesuniecieYPlansz) / ROZMIAR_POLA;
                    int gridY = (mousePos.x - przesuniecieXPlansz) / ROZMIAR_POLA;

                    if (gridX >= 0 && gridX < ROZMIAR_PLANSZY && gridY >= 0 && gridY < ROZMIAR_PLANSZY) {
                        int dlugosc = statkiDoUmieszczenia[aktualnyStatek];
                        bool moznaUmiescic = gra->getPlanszaGracza().czyMoznaUmiescicStatek(gridX, gridY, dlugosc, orientacja);

                        podgladStatku.setFillColor(moznaUmiescic ? sf::Color(0, 255, 0, 150) : sf::Color(255, 0, 0, 150));

                        if (orientacja == Statki::Orientacja::Pozioma) {
                            for (int i = 0; i < dlugosc; ++i) {
                                if (gridY + i < ROZMIAR_PLANSZY) {
                                    podgladStatku.setPosition(static_cast<float>(przesuniecieXPlansz + (gridY + i) * ROZMIAR_POLA),
                                        static_cast<float>(przesuniecieYPlansz + gridX * ROZMIAR_POLA));
                                    window.draw(podgladStatku);
                                }
                            }
                        }
                        else {
                            for (int i = 0; i < dlugosc; ++i) {
                                if (gridX + i < ROZMIAR_PLANSZY) {
                                    podgladStatku.setPosition(static_cast<float>(przesuniecieXPlansz + gridY * ROZMIAR_POLA),
                                        static_cast<float>(przesuniecieYPlansz + (gridX + i) * ROZMIAR_POLA));
                                    window.draw(podgladStatku);
                                }
                            }
                        }
                    }
                }

                if (statkiUmieszczone && !gra->czyKoniecGry()) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    int planszaKompX = przesuniecieXPlansz + ROZMIAR_PLANSZY * ROZMIAR_POLA + ODSTEP;
                    if (mousePos.x >= planszaKompX &&
                        mousePos.x < planszaKompX + ROZMIAR_PLANSZY * ROZMIAR_POLA &&
                        mousePos.y >= przesuniecieYPlansz && mousePos.y < przesuniecieYPlansz + ROZMIAR_PLANSZY * ROZMIAR_POLA) {

                        int gridX = (mousePos.y - przesuniecieYPlansz) / ROZMIAR_POLA;
                        int gridY = (mousePos.x - planszaKompX) / ROZMIAR_POLA;

                        auto& planszaKomputera = gra->getPlanszaKomputera();
                        if (planszaKomputera.getPole(gridX, gridY) == Statki::Pole::Puste ||
                            planszaKomputera.getPole(gridX, gridY) == Statki::Pole::Statek) {

                            podgladStrzalu.setPosition(
                                static_cast<float>(planszaKompX + gridY * ROZMIAR_POLA + ROZMIAR_POLA / 2),
                                static_cast<float>(przesuniecieYPlansz + gridX * ROZMIAR_POLA + ROZMIAR_POLA / 2)
                            );
                            window.draw(podgladStrzalu);
                        }
                    }
                }

                if (!statkiUmieszczone) {
                    komunikat.setString("Umiesc statek (" + std::to_string(statkiDoUmieszczenia[aktualnyStatek]) +
                        " masztowy). R - obroc");
                }
                else if (gra->czyKoniecGry()) {
                    komunikat.setString(gra->getWynik());
                }
                sf::FloatRect komunikatBounds = komunikat.getLocalBounds();
                komunikat.setOrigin(komunikatBounds.width / 2, 0);
                komunikat.setPosition(OKNO_SZEROKOSC / 2, OKNO_WYSOKOSC - STOPKA_WYS + 5);
                window.draw(komunikat);

                sf::Text pomoc;
                pomoc.setFont(font);
                pomoc.setCharacterSize(18);
                pomoc.setFillColor(sf::Color::White);
                pomoc.setString("F1 - Zapisz gre   |   F2 - Wczytaj gre   |   R - Obroc statek   |   ESC - Menu");
                sf::FloatRect pomocBounds = pomoc.getLocalBounds();
                pomoc.setOrigin(pomocBounds.width / 2, 0);
                pomoc.setPosition(OKNO_SZEROKOSC / 2, OKNO_WYSOKOSC - STOPKA_WYS + 35);
                window.draw(pomoc);

                window.display();

                if (gra->czyKoniecGry()) {
                    sf::sleep(sf::seconds(3));
                    stan = StanAplikacji::Menu;
                }
            }
        }
    }
    if (gra) delete gra;

    return 0;
}