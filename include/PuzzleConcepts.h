#ifndef PUZZLE_CONCEPTS_H
#define PUZZLE_CONCEPTS_H

#include <concepts>

/**
 * @brief Concept definiujący bazowe wymagania dla typu elementu planszy
 *
 * Typ spełniający PuzzleTile concept musi:
 * - być kopiowalna (std::copyable)
 * - być porównywalna (std::equality_comparable)
 * - być całkowicie usporządkowana (std::totally_ordered)
 * - być domyślnie inicjalizowalna (std::default_initializable)
 *
 * @tparam T Typ do sprawdzenia
 */
template<typename T>
concept PuzzleTile =
    std::copyable<T> &&
    std::equality_comparable<T> &&
    std::totally_ordered<T> &&
    std::default_initializable<T>;

/**
 * @brief Concept rozszerzający PuzzleTile o wymagania numeryczne
 *
 * Typ spełniający NumericTile concept musi:
 * - spełniać PuzzleTile concept
 * - obsługiwać operację odejmowania z wynikiem konwertowalnym do int
 * - być konwertowalny do int
 * - być konstruowalny z int
 *
 * Używany przez heurystyki wymagające operacji numerycznych (np. Manhattan Distance).
 *
 * @tparam T Typ do sprawdzenia
 */
template<typename T>
concept NumericTile =
    PuzzleTile<T> &&
    requires(T a, T b) {
        { a - b } -> std::convertible_to<int>;
        { static_cast<int>(a) } -> std::same_as<int>;
        requires std::constructible_from<T, int>;
    };

#endif
