#include "../common/linia.hpp"

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

static bool odbierzLinie(int sock, char* daneOczekujace, size_t* liczbaBajtow) {
    char bufor[256];
    size_t dlugoscLinii = znajdzDlugoscLinii(daneOczekujace, *liczbaBajtow);

    while (dlugoscLinii == 0) {
        const size_t wolneMiejsce = pojemnoscLinii - *liczbaBajtow;
        if (wolneMiejsce == 0) {
            fputs("Odpowiedz serwera jest za dluga.\n", stderr);
            return false;
        }
        const size_t doOdczytania = wolneMiejsce < sizeof(bufor) ? wolneMiejsce : sizeof(bufor);
        const ssize_t odebraneBajty = ::recv(sock, bufor, doOdczytania, 0);
        if (odebraneBajty == -1) {
            perror("recv");
            return false;
        }
        if (odebraneBajty == 0) {
            fputs("Serwer zamknal polaczenie przed koncem odpowiedzi.\n", stderr);
            return false;
        }

        memcpy(daneOczekujace + *liczbaBajtow, bufor, odebraneBajty);
        *liczbaBajtow += odebraneBajty;
        dlugoscLinii = znajdzDlugoscLinii(daneOczekujace, *liczbaBajtow);
    }

    fwrite(daneOczekujace, 1, dlugoscLinii, stdout);
    *liczbaBajtow -= dlugoscLinii;
    memmove(daneOczekujace, daneOczekujace + dlugoscLinii, *liczbaBajtow);
    return true;
}

int main(){
    int sock = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1){
        perror("socket");
        return 1;
    }

    sockaddr_in adresSerwera{};
    adresSerwera.sin_family = AF_INET;
    adresSerwera.sin_port = htons(1335);
    if(::inet_pton(AF_INET, "127.0.0.1", &adresSerwera.sin_addr) != 1){
        fputs("Nie udalo sie ustawic adresu IPv4 serwera.\n", stderr);
        ::close(sock);
        return 1;
    }

    if (::connect(sock, reinterpret_cast<const sockaddr*>(&adresSerwera),
        sizeof(adresSerwera)) == -1) {
        perror("connect");
        ::close(sock);
        return 1;
    }

    puts("Polaczono z serwerem");

    char daneOczekujace[pojemnoscLinii];
    size_t liczbaBajtow = 0;
    if (!odbierzLinie(sock, daneOczekujace, &liczbaBajtow)) {
        ::close(sock);
        return 1;
    }

    char wiadomosc[pojemnoscLinii + 1];
    while (true) {
        if (!fgets(wiadomosc, sizeof(wiadomosc), stdin)) {
            if (feof(stdin) && !ferror(stdin)) {
                break;
            }
            fputs("Nie udalo sie odczytac wiadomosci.\n", stderr);
            ::close(sock);
            return 1;
        }
        size_t dlugosc = strlen(wiadomosc);
        if (dlugosc > 0 && wiadomosc[dlugosc - 1] == '\n') {
            --dlugosc;
            if (dlugosc > 0 && wiadomosc[dlugosc - 1] == '\r') {
                --dlugosc;
            }
        }
        if (dlugosc > maksDlugoscTekstu) {
            fputs("Wiadomosc jest za dluga.\n", stderr);
            ::close(sock);
            return 1;
        }
        wiadomosc[dlugosc++] = '\r';
        wiadomosc[dlugosc++] = '\n';

        const ssize_t wyslaneBajty = ::send(sock, wiadomosc, dlugosc, MSG_NOSIGNAL);
        if (wyslaneBajty == -1) {
            perror("send");
            ::close(sock);
            return 1;
        }
        if (static_cast<size_t>(wyslaneBajty) != dlugosc) {
            fputs("Wyslano tylko czesc wiadomosci.\n", stderr);
            ::close(sock);
            return 1;
        }

        if (!odbierzLinie(sock, daneOczekujace, &liczbaBajtow)) {
            ::close(sock);
            return 1;
        }
    }

    ::close(sock);
    return 0;

}
