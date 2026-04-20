#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <stdexcept>
#include <concepts>
#include <memory>

/**
 * @brief Szablon klasy reprezentującej planszę do gry N-Puzzle
 * @tparam T Typ elementów przechowywanych na planszy (musi być kopiowalna)
 */
template <typename T>
requires std::copyable<T>
class Board {
private:
    int size_;
    std::vector<T> data_;

    /**
     * @brief Konwertuje współrzędne 2D na indeks 1D
     * @param x Współrzędna x (kolumna)
     * @param y Współrzędna y (wiersz)
     * @return Indeks w spłaszczonym wektorze
     */
    [[nodiscard]] constexpr int toIndex(int x, int y) const noexcept {
        return y * size_ + x;
    }

    /**
     * @brief Waliduje współrzędne
     * @param x Współrzędna x
     * @param y Współrzędna y
     * @throws std::out_of_range Gdy współrzędne są poza zakresem
     */
    void validateCoordinates(int x, int y) const {
        if (x < 0 || x >= size_ || y < 0 || y >= size_) {
            throw std::out_of_range("Coordinates out of bounds: (" +
                std::to_string(x) + ", " + std::to_string(y) +
                ") for board size " + std::to_string(size_));
        }
    }

public:
    /**
     * @brief Konstruktor tworzący planszę o zadanym rozmiarze
     * @param size Rozmiar planszy (N x N)
     * @param defaultValue Domyślna wartość dla wszystkich komórek
     * @throws std::invalid_argument Gdy rozmiar jest mniejszy lub równy 0
     */
    explicit Board(int size, const T& defaultValue = T{})
        : size_(size), data_(size * size, defaultValue) {
        if (size <= 0) {
            throw std::invalid_argument("Board size must be positive, got: " +
                std::to_string(size));
        }
    }

    /**
     * @brief Konstruktor tworzący planszę z wektora danych
     * @param size Rozmiar planszy (N x N)
     * @param data Wektor danych o rozmiarze size*size
     * @throws std::invalid_argument Gdy rozmiar wektora nie odpowiada size*size
     */
    Board(int size, std::vector<T> data)
        : size_(size), data_(std::move(data)) {
        if (size <= 0) {
            throw std::invalid_argument("Board size must be positive");
        }
        if (static_cast<int>(data_.size()) != size * size) {
            throw std::invalid_argument("Data size (" + std::to_string(data_.size()) +
                ") does not match board dimensions (" + std::to_string(size * size) + ")");
        }
    }

    /**
     * @brief Dostęp do elementu planszy (tylko odczyt)
     * @param x Współrzędna x (kolumna)
     * @param y Współrzędna y (wiersz)
     * @return Stała referencja do elementu
     * @throws std::out_of_range Gdy współrzędne są poza zakresem
     */
    [[nodiscard]] const T& at(int x, int y) const {
        validateCoordinates(x, y);
        return data_[toIndex(x, y)];
    }

    /**
     * @brief Dostęp do elementu planszy (odczyt i zapis)
     * @param x Współrzędna x (kolumna)
     * @param y Współrzędna y (wiersz)
     * @return Referencja do elementu
     * @throws std::out_of_range Gdy współrzędne są poza zakresem
     */
    T& at(int x, int y) {
        validateCoordinates(x, y);
        return data_[toIndex(x, y)];
    }

    /**
     * @brief Zamienia miejscami dwa elementy na planszy
     * @param x1 Współrzędna x pierwszego elementu
     * @param y1 Współrzędna y pierwszego elementu
     * @param x2 Współrzędna x drugiego elementu
     * @param y2 Współrzędna y drugiego elementu
     * @throws std::out_of_range Gdy którekolwiek współrzędne są poza zakresem
     */
    void swap(int x1, int y1, int x2, int y2) {
        validateCoordinates(x1, y1);
        validateCoordinates(x2, y2);
        std::swap(data_[toIndex(x1, y1)], data_[toIndex(x2, y2)]);
    }

    /**
     * @brief Pobiera rozmiar planszy
     * @return Rozmiar planszy (N dla planszy N x N)
     */
    [[nodiscard]] constexpr int getSize() const noexcept {
        return size_;
    }

    /**
     * @brief Tworzy głęboką kopię obiektu planszy
     * @return Nowy obiekt Board będący kopią aktualnej planszy
     */
    [[nodiscard]] Board<T> getCopy() const {
        return Board<T>(size_, data_);
    }

    /**
     * @brief Dostęp do surowych danych planszy (tylko odczyt)
     * @return Stała referencja do wektora danych
     */
    [[nodiscard]] const std::vector<T>& getData() const noexcept {
        return data_;
    }

    /**
     * @brief Operator porównania
     * @param other Inna plansza do porównania
     * @return true jeśli plansze są identyczne, false w przeciwnym wypadku
     */
    [[nodiscard]] bool operator==(const Board<T>& other) const noexcept {
        return size_ == other.size_ && data_ == other.data_;
    }

    /**
     * @brief Operator nierówności
     * @param other Inna plansza do porównania
     * @return true jeśli plansze są różne, false w przeciwnym wypadku
     */
    [[nodiscard]] bool operator!=(const Board<T>& other) const noexcept {
        return !(*this == other);
    }
};

#endif
