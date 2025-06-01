#include "Gra.h"
#include <random>
#include <fstream>
#include <sstream>
#include <queue>
#include <set>
#include <algorithm>

namespace Statki {

    Gra::Gra(int rozmiarPlanszy, const std::vector<int>& dlugosciStatkow)
        : planszaGracza(rozmiarPlanszy, dlugosciStatkow),
        planszaKomputera(rozmiarPlanszy, dlugosciStatkow),
        dlugosciStatkow(dlugosciStatkow)
    {
        celeDoZatopienia.clear();
        trafioneAleNieZatopione.clear();
        historiaAI.clear();
        umiescStatkiKomputera();
    }

    Gra::~Gra() {}

    Plansza& Gra::getPlanszaGracza() {
        return planszaGracza;
    }

    Plansza& Gra::getPlanszaKomputera() {
        return planszaKomputera;
    }

    bool Gra::atakujKomputer(int x, int y) {
        bool trafiony = planszaKomputera.atakuj(x, y);
        return trafiony;
    }

    std::pair<int, int> Gra::losujWspolrzedneAtaku() const {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dyst(0, planszaGracza.getRozmiar() - 1);
        return { dyst(gen), dyst(gen) };
    }

    void Gra::umiescStatkiKomputera() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dyst(0, 1);

        for (int dlugosc : dlugosciStatkow) {
            bool umieszczono = false;
            while (!umieszczono) {
                int x = losujWspolrzedneAtaku().first;
                int y = losujWspolrzedneAtaku().second;
                Orientacja orientacja = (dyst(gen) == 0) ? Orientacja::Pozioma : Orientacja::Pionowa;
                try {
                    umieszczono = planszaKomputera.umiescStatek(x, y, dlugosc, orientacja);
                }
                catch (...) {}
            }
        }
    }

    std::vector<std::pair<int, int>> sasiedniePolaWokolStatku(const Plansza& plansza, const std::vector<std::pair<int, int>>& statek) {
        std::set<std::pair<int, int>> sasiedzi;
        for (auto [x, y] : statek) {
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx, ny = y + dy;
                    if (plansza.czyPoprawneWspolrzedne(nx, ny))
                        sasiedzi.insert({ nx, ny });
                }
            }
        }
        for (auto pole : statek) sasiedzi.erase(pole);
        return std::vector<std::pair<int, int>>(sasiedzi.begin(), sasiedzi.end());
    }

    bool Gra::atakujGracza(int x, int y) {
        bool trafiony = planszaGracza.atakuj(x, y);
        if (trafiony) {
            trafioneAleNieZatopione.insert({ x, y });
            dodajCeleObok(x, y);

            std::vector<std::vector<bool>> odwiedzone(planszaGracza.getRozmiar(), std::vector<bool>(planszaGracza.getRozmiar(), false));
            if (planszaGracza.czyStatekZatopiony(x, y, odwiedzone)) {
                auto wspolrzedneStatku = planszaGracza.znajdzCalyStatek(x, y);
                auto sasiedzi = sasiedniePolaWokolStatku(planszaGracza, wspolrzedneStatku);
                for (const auto& pole : wspolrzedneStatku) {
                    trafioneAleNieZatopione.erase(pole);
                    celeDoZatopienia.erase(std::remove(celeDoZatopienia.begin(), celeDoZatopienia.end(), pole), celeDoZatopienia.end());
                }
                for (const auto& pole : sasiedzi) {
                    celeDoZatopienia.erase(std::remove(celeDoZatopienia.begin(), celeDoZatopienia.end(), pole), celeDoZatopienia.end());
                }
            }
        }
        return trafiony;
    }

    bool Gra::czyKoniecGry() const {
        return planszaGracza.czyWszystkieStatkiZatopione() || planszaKomputera.czyWszystkieStatkiZatopione();
    }

    std::string Gra::getWynik() const {
        if (planszaGracza.czyWszystkieStatkiZatopione()) {
            return "Komputer wygral!";
        }
        else if (planszaKomputera.czyWszystkieStatkiZatopione()) {
            return "Gracz wygral!";
        }
        return "Gra w toku.";
    }

    std::pair<int, int> Gra::getRuchKomputera() {
        if (!celeDoZatopienia.empty()) {
            auto ruch = wybierzRuchZatopienia();
            historiaAI.push_back(ruch);
            return ruch;
        }
        else {
            std::pair<int, int> ruch;
            do {
                ruch = losujWspolrzedneAtaku();
            } while (planszaGracza.czyPoleAtakowane(ruch.first, ruch.second));
            historiaAI.push_back(ruch);
            return ruch;
        }
    }

    void Gra::dodajCeleObok(int x, int y) const {
        static const int dx[] = { 1, -1, 0, 0 };
        static const int dy[] = { 0, 0, 1, -1 };

        for (int d = 0; d < 4; ++d) {
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (planszaGracza.czyPoprawneWspolrzedne(nx, ny)
                && !planszaGracza.czyPoleAtakowane(nx, ny)) {
                if (std::find(celeDoZatopienia.begin(), celeDoZatopienia.end(), std::make_pair(nx, ny)) == celeDoZatopienia.end())
                    celeDoZatopienia.push_back({ nx, ny });
            }
        }
    }

    std::pair<int, int> Gra::wybierzRuchZatopienia() const {
        while (!celeDoZatopienia.empty()) {
            auto pos = celeDoZatopienia.front();
            celeDoZatopienia.erase(celeDoZatopienia.begin());

            int x = pos.first, y = pos.second;

            if (!planszaGracza.czyPoprawneWspolrzedne(x, y) || planszaGracza.czyPoleAtakowane(x, y))
                continue;

            return { x, y };
        }
        return losujWspolrzedneAtaku();
    }

	void Gra::zapiszGre(const std::string& nazwaPliku) const { //Obs³uga plików binarnych
        std::ofstream plik(nazwaPliku, std::ios::binary);
        if (!plik.is_open())
            throw std::runtime_error("Nie mo¿na otworzyæ pliku do zapisu (binarnie).");

        planszaGracza.zapiszPlansze(plik);
        planszaKomputera.zapiszPlansze(plik);

        size_t celeSize = celeDoZatopienia.size();
        plik.write(reinterpret_cast<const char*>(&celeSize), sizeof(celeSize));
        for (const auto& cel : celeDoZatopienia) {
            plik.write(reinterpret_cast<const char*>(&cel.first), sizeof(int));
            plik.write(reinterpret_cast<const char*>(&cel.second), sizeof(int));
        }
        size_t trafySize = trafioneAleNieZatopione.size();
        plik.write(reinterpret_cast<const char*>(&trafySize), sizeof(trafySize));
        for (const auto& t : trafioneAleNieZatopione) {
            plik.write(reinterpret_cast<const char*>(&t.first), sizeof(int));
            plik.write(reinterpret_cast<const char*>(&t.second), sizeof(int));
        }
        plik.close();
    }

    void Gra::wczytajGre(const std::string& nazwaPliku) {  //Obs³uga plików binarnych
        std::ifstream plik(nazwaPliku, std::ios::binary);
        if (!plik.is_open())
            throw std::runtime_error("Nie mo¿na otworzyæ pliku do odczytu (binarnie).");

        planszaGracza.wczytajPlansze(plik);
        planszaKomputera.wczytajPlansze(plik);

        size_t celeSize = 0;
        plik.read(reinterpret_cast<char*>(&celeSize), sizeof(celeSize));
        celeDoZatopienia.clear();
        for (size_t i = 0; i < celeSize; ++i) {
            int x, y;
            plik.read(reinterpret_cast<char*>(&x), sizeof(int));
            plik.read(reinterpret_cast<char*>(&y), sizeof(int));
            celeDoZatopienia.push_back({ x, y });
        }
        size_t trafySize = 0;
        plik.read(reinterpret_cast<char*>(&trafySize), sizeof(trafySize));
        trafioneAleNieZatopione.clear();
        for (size_t i = 0; i < trafySize; ++i) {
            int x, y;
            plik.read(reinterpret_cast<char*>(&x), sizeof(int));
            plik.read(reinterpret_cast<char*>(&y), sizeof(int));
            trafioneAleNieZatopione.insert({ x, y });
        }
        plik.close();

    }

}