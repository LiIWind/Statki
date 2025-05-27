#pragma once
#include "StatkiDef.h"
#include "Plansza.h"
#include <vector>
#include <set>
#include <string>
#include <utility>

namespace Statki {

    class STATKILIB_API Gra {
    public:
        Gra(int rozmiarPlanszy, const std::vector<int>& dlugosciStatkow = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 });
        ~Gra();

        Plansza& getPlanszaGracza();
        Plansza& getPlanszaKomputera();

        bool atakujKomputer(int x, int y);
        bool atakujGracza(int x, int y);

        bool czyKoniecGry() const;
        std::string getWynik() const;
        std::pair<int, int> getRuchKomputera();

        void zapiszGre(const std::string& nazwaPliku) const;
        void wczytajGre(const std::string& nazwaPliku);

        void zapiszStanAI(const std::string& nazwaPliku) const;
        void wczytajStanAI(const std::string& nazwaPliku);

    private:
        Plansza planszaGracza;
        Plansza planszaKomputera;
        std::vector<int> dlugosciStatkow;

        mutable std::vector<std::pair<int, int>> celeDoZatopienia;
        mutable std::set<std::pair<int, int>> trafioneAleNieZatopione;
        mutable std::vector<std::pair<int, int>> historiaAI;

        std::pair<int, int> losujWspolrzedneAtaku() const;
        void umiescStatkiKomputera();
        std::pair<int, int> wybierzRuchZatopienia() const;
        void dodajCeleObok(int x, int y) const;
    };

}