#pragma once

/**
 * @brief Tryb wejścia użytkownika używany przez prezenter i widok
 */
enum class InputMode {
    None,  ///< Brak aktywnego trybu wejścia (normalna gra)
    Save,  ///< Tryb wpisywania ścieżki do zapisu
    Load   ///< Tryb wpisywania ścieżki do wczytania
};
