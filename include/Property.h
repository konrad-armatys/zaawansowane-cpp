#ifndef PROPERTY_H
#define PROPERTY_H

#include <functional>
#include <utility>

/**
 * @brief Szablon klasy opakowującej wartość z możliwością śledzenia zmian
 * @tparam T Typ przechowywanej wartości
 */
template <typename T>
class Property {
private:
    T value_;
    std::function<void(T)> onChange_;

public:
    /**
     * @brief Konstruktor domyślny z opcjonalną wartością początkową
     * @param initialValue Początkowa wartość (domyślnie T{})
     */
    explicit Property(T initialValue = T{})
        : value_(std::move(initialValue)), onChange_(nullptr) {}

    /**
     * @brief Ustawia funkcję callback wywoływaną przy zmianie wartości
     * @param callback Funkcja przyjmująca nową wartość jako argument
     */
    void setOnChange(std::function<void(T)> callback) {
        onChange_ = std::move(callback);
    }

    /**
     * @brief Pobiera aktualną wartość
     * @return Stała referencja do przechowywanej wartości
     */
    [[nodiscard]] const T& get() const noexcept {
        return value_;
    }

    /**
     * @brief Ustawia nową wartość i wywołuje callback jeśli został zdefiniowany
     * @param newValue Nowa wartość do ustawienia
     */
    void set(T newValue) {
        value_ = std::move(newValue);
        if (onChange_) {
            onChange_(value_);
        }
    }

    /**
     * @brief Operator przypisania
     * @param newValue Nowa wartość
     * @return Referencja do tego obiektu
     */
    Property<T>& operator=(const T& newValue) {
        set(newValue);
        return *this;
    }

    /**
     * @brief Operator konwersji na typ T
     * @return Wartość przechowywana w Property
     */
    [[nodiscard]] operator T() const noexcept {
        return value_;
    }

    /**
     * @brief Operator pre-inkrementacji
     * @return Referencja do tego obiektu po inkrementacji
     */
    Property<T>& operator++() {
        set(value_ + 1);
        return *this;
    }

    /**
     * @brief Operator post-inkrementacji
     * @return Wartość przed inkrementacją
     */
    T operator++(int) {
        T oldValue = value_;
        set(value_ + 1);
        return oldValue;
    }

    /**
     * @brief Operator dodawania i przypisania
     * @param delta Wartość do dodania
     * @return Referencja do tego obiektu
     */
    Property<T>& operator+=(const T& delta) {
        set(value_ + delta);
        return *this;
    }

    /**
     * @brief Resetuje wartość do wartości domyślnej typu T
     */
    void reset() {
        set(T{});
    }
};

#endif
