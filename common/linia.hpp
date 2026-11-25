#pragma once

#include <stddef.h>

constexpr size_t maksDlugoscTekstu = 4096;
constexpr size_t pojemnoscLinii = maksDlugoscTekstu + 2;

// Zwraca długość pierwszej pełnej linii razem z CRLF albo 0.
inline size_t znajdzDlugoscLinii(const char* dane, size_t liczbaBajtow) {
    for (size_t i = 1; i < liczbaBajtow; ++i) {
        if (dane[i - 1] == '\r' && dane[i] == '\n') {
            return i + 1;
        }
    }
    return 0;
}
