#include "Plansza.h"
#include <queue>
#include <set>
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace Statki {

    Plansza::Plansza(int rozmiar, const std::vector<int>& dlugosciStatkow)
        : rozmiar(rozmiar), dlugosciStatkow(dlugosciStatkow)
    {
        if (rozmiar <= 0) {
            throw std::invalid_argument("Rozmiar planszy musi byæ wiêkszy ni¿ 0.");
        }

        pola = new Pole * [rozmiar];
        for (int i = 0; i < rozmiar; ++i) {
            pola[i] = new Pole[rozmiar];
            for (int j = 0; j < rozmiar; ++j) {
                pola[i][j] = Pole::Puste;
            }
        }
    }

    Plansza::~Plansza() {
        for (int i = 0; i < rozmiar; ++i) {
            delete[] pola[i];
        }
        delete[] pola;
    }

    int Plansza::getRozmiar() const {
        return rozmiar;
    }

    Pole Plansza::getPole(int x, int y) const {
        if (!czyPoprawneWspolrzedne(x, y)) {
            throw std::out_of_range("Wspolrzêdne poza zakresem planszy.");
        }
        return pola[x][y];
    }

    bool Plansza::czyMaSasiedztwo(int x, int y) const {
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx, ny = y + dy;
                if (czyPoprawneWspolrzedne(nx, ny) && pola[nx][ny] == Pole::Statek) {
                    return true;
                }
            }
        }
        return false;
    }

    bool Plansza::umiescStatek(int x, int y, int dlugosc, Orientacja orientacja) {
        if (!czyMoznaUmiescicStatek(x, y, dlugosc, orientacja)) {
            return false;
        }

        if (orientacja == Orientacja::Pozioma) {
            for (int i = 0; i < dlugosc; ++i) {
                pola[x][y + i] = Pole::Statek;
            }
        }
        else {
            for (int i = 0; i < dlugosc; ++i) {
                pola[x + i][y] = Pole::Statek;
            }
        }
        return true;
    }

    bool Plansza::atakuj(int x, int y) {
        if (!czyPoprawneWspolrzedne(x, y)) {
            throw std::out_of_range("Wspolrzêdne poza zakresem planszy.");
        }

        if (pola[x][y] == Pole::Statek) {
            pola[x][y] = Pole::Trafiony;
            return true;
        }
        else if (pola[x][y] == Pole::Puste) {
            pola[x][y] = Pole::Pudlo;
            return false;
        }
        return false;
    }

    bool Plansza::czyWszystkieStatkiZatopione() const {
        return policzZatopioneStatki() == policzWszystkieStatki();
    }

    void Plansza::wyswietlPlansze() const {
        for (int i = 0; i < rozmiar; ++i) {
            for (int j = 0; j < rozmiar; ++j) {
                std::cout << static_cast<char>(pola[i][j]) << " ";
            }
            std::cout << std::endl;
        }
    }

    bool Plansza::czyPoprawneWspolrzedne(int x, int y) const {
        return (x >= 0 && x < rozmiar && y >= 0 && y < rozmiar);
    }

    bool Plansza::czyMoznaUmiescicStatek(int x, int y, int dlugosc, Orientacja orientacja) const {
        if (orientacja == Orientacja::Pozioma) {
            if (y + dlugosc > rozmiar) return false;
            for (int i = 0; i < dlugosc; ++i) {
                if (!czyPoprawneWspolrzedne(x, y + i) || pola[x][y + i] != Pole::Puste) return false;
                if (czyMaSasiedztwo(x, y + i)) return false;
            }
        }
        else {
            if (x + dlugosc > rozmiar) return false;
            for (int i = 0; i < dlugosc; ++i) {
                if (!czyPoprawneWspolrzedne(x + i, y) || pola[x + i][y] != Pole::Puste) return false;
                if (czyMaSasiedztwo(x + i, y)) return false;
            }
        }
        return true;
    }

    void Plansza::zapiszPlansze(const std::string& nazwaPliku) const {
        std::ofstream plik(nazwaPliku, std::ios::binary);
        if (!plik.is_open()) throw std::runtime_error("Nie mo¿na otworzyæ pliku do zapisu (binarnie).");
        zapiszPlansze(plik);
        plik.close();
    }
    void Plansza::wczytajPlansze(const std::string& nazwaPliku) {
        std::ifstream plik(nazwaPliku, std::ios::binary);
        if (!plik.is_open()) throw std::runtime_error("Nie mo¿na otworzyæ pliku do odczytu (binarnie).");
        wczytajPlansze(plik);
        plik.close();
    }
    void Plansza::zapiszPlansze(std::ostream& plik) const {
        plik.write(reinterpret_cast<const char*>(&rozmiar), sizeof(rozmiar));
        for (int i = 0; i < rozmiar; ++i)
            for (int j = 0; j < rozmiar; ++j) {
                char pole = static_cast<char>(pola[i][j]);
                plik.write(&pole, sizeof(pole));
            }
        int liczbaStatkow = static_cast<int>(dlugosciStatkow.size());
        plik.write(reinterpret_cast<const char*>(&liczbaStatkow), sizeof(liczbaStatkow));
        for (int dl : dlugosciStatkow)
            plik.write(reinterpret_cast<const char*>(&dl), sizeof(dl));
    }
    void Plansza::wczytajPlansze(std::istream& plik) {
        for (int i = 0; i < rozmiar; ++i) delete[] pola[i];
        delete[] pola;
        plik.read(reinterpret_cast<char*>(&rozmiar), sizeof(rozmiar));
        pola = new Pole * [rozmiar];
        for (int i = 0; i < rozmiar; ++i) {
            pola[i] = new Pole[rozmiar];
            for (int j = 0; j < rozmiar; ++j) {
                char pole;
                plik.read(&pole, sizeof(pole));
                pola[i][j] = static_cast<Pole>(pole);
            }
        }
        int liczbaStatkow;
        plik.read(reinterpret_cast<char*>(&liczbaStatkow), sizeof(liczbaStatkow));
        dlugosciStatkow.clear();
        for (int i = 0; i < liczbaStatkow; ++i) {
            int dl;
            plik.read(reinterpret_cast<char*>(&dl), sizeof(dl));
            dlugosciStatkow.push_back(dl);
        }
    }

    bool Plansza::operator==(const Plansza& other) const {
        if (rozmiar != other.rozmiar) return false;
        for (int i = 0; i < rozmiar; ++i) {
            for (int j = 0; j < rozmiar; ++j) {
                if (pola[i][j] != other.pola[i][j]) return false;
            }
        }
        return true;
    }

    bool Plansza::czyStatekZatopiony(int x, int y, std::vector<std::vector<bool>>& odwiedzone) const {
        if (!czyPoprawneWspolrzedne(x, y) || odwiedzone[x][y])
            return true;
        if (pola[x][y] == Pole::Statek)
            return false;
        if (pola[x][y] != Pole::Trafiony)
            return true;
        odwiedzone[x][y] = true;
        return czyStatekZatopiony(x + 1, y, odwiedzone) &&
            czyStatekZatopiony(x - 1, y, odwiedzone) &&
            czyStatekZatopiony(x, y + 1, odwiedzone) &&
            czyStatekZatopiony(x, y - 1, odwiedzone);
    }

    int Plansza::policzWszystkieStatki() const {
        return static_cast<int>(dlugosciStatkow.size());
    }

    int Plansza::policzZatopioneStatki() const {
        int liczba = 0;
        std::vector<std::vector<bool>> odwiedzone(rozmiar, std::vector<bool>(rozmiar, false));
        for (int i = 0; i < rozmiar; ++i) {
            for (int j = 0; j < rozmiar; ++j) {
                if ((pola[i][j] == Pole::Trafiony || pola[i][j] == Pole::Statek) && !odwiedzone[i][j]) {
                    std::vector<std::vector<bool>> lokalneOdwiedzone = odwiedzone;
                    if (czyStatekZatopiony(i, j, lokalneOdwiedzone)) {
                        ++liczba;
                        std::vector<std::pair<int, int>> stack = { {i, j} };
                        while (!stack.empty()) {
                            auto pos = stack.back(); stack.pop_back();
                            int x = pos.first, y = pos.second;
                            if (!czyPoprawneWspolrzedne(x, y) || odwiedzone[x][y]) continue;
                            if (pola[x][y] == Pole::Trafiony) {
                                odwiedzone[x][y] = true;
                                stack.push_back({ x + 1, y });
                                stack.push_back({ x - 1, y });
                                stack.push_back({ x, y + 1 });
                                stack.push_back({ x, y - 1 });
                            }
                        }
                    }
                }
            }
        }
        return liczba;
    }

    const std::vector<int>& Plansza::getDlugosciStatkow() const {
        return dlugosciStatkow;
    }

    std::vector<std::pair<int, int>> Plansza::znajdzCalyStatek(int x, int y) const {
        std::vector<std::pair<int, int>> statek;
        if (!czyPoprawneWspolrzedne(x, y)) return statek;
        Pole pole = pola[x][y];
        if (pole != Pole::Trafiony && pole != Pole::Statek) return statek;
        std::vector<std::vector<bool>> odwiedzone(rozmiar, std::vector<bool>(rozmiar, false));
        std::queue<std::pair<int, int>> q;
        q.push({ x, y });
        odwiedzone[x][y] = true;
        while (!q.empty()) {
            auto [cx, cy] = q.front();
            q.pop();
            statek.push_back({ cx, cy });
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    if ((dx == 0) == (dy == 0)) continue;
                    int nx = cx + dx, ny = cy + dy;
                    if (czyPoprawneWspolrzedne(nx, ny) && !odwiedzone[nx][ny] &&
                        (pola[nx][ny] == Pole::Trafiony || pola[nx][ny] == Pole::Statek)) {
                        odwiedzone[nx][ny] = true;
                        q.push({ nx, ny });
                    }
                }
            }
        }
        return statek;
    }

    bool Plansza::czyPoleAtakowane(int x, int y) const {
        if (!czyPoprawneWspolrzedne(x, y)) return false;
        Pole p = pola[x][y];
        return (p == Pole::Trafiony || p == Pole::Pudlo);
    }

}