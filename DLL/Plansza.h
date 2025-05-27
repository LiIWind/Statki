#pragma once
#include "StatkiDef.h"
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>

namespace Statki {

    class STATKILIB_API Plansza {
    public:
        Plansza(int rozmiar, const std::vector<int>& dlugosciStatkow = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 });
        ~Plansza();

        int getRozmiar() const;
        Pole getPole(int x, int y) const;

        bool umiescStatek(int x, int y, int dlugosc, Orientacja orientacja);
        bool atakuj(int x, int y);
        bool czyWszystkieStatkiZatopione() const;

        void wyswietlPlansze() const;

        void zapiszPlansze(const std::string& nazwaPliku) const;
        void wczytajPlansze(const std::string& nazwaPliku);
        void zapiszPlansze(std::ostream& plik) const;
        void wczytajPlansze(std::istream& plik);

        bool operator==(const Plansza& other) const;

        int policzZatopioneStatki() const;
        int policzWszystkieStatki() const;
        bool czyStatekZatopiony(int x, int y, std::vector<std::vector<bool>>& odwiedzone) const;

        bool czyMoznaUmiescicStatek(int x, int y, int dlugosc, Orientacja orientacja) const;
        const std::vector<int>& getDlugosciStatkow() const;
        std::vector<std::pair<int, int>> znajdzCalyStatek(int x, int y) const;
        bool czyPoleAtakowane(int x, int y) const;
        bool czyPoprawneWspolrzedne(int x, int y) const;

    private:
        int rozmiar;
        Pole** pola;
        std::vector<int> dlugosciStatkow;
        bool czyMaSasiedztwo(int x, int y) const;
    };

}