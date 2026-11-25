#include "../common/linia.hpp"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    char bufor[256];
    const char powitanie[] = "Witaj na serwerze!\r\n";


    if (sock == -1) {
        perror("socket");
        return 1;
    }

    sockaddr_in adresSerwera{};
    adresSerwera.sin_family = AF_INET;
    adresSerwera.sin_port = htons(1335);
    adresSerwera.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&adresSerwera, sizeof(adresSerwera)) == -1) {
        perror("bind");
        close(sock);
        return 1;
    }

    if (listen(sock, 10) == -1) {
        perror("listen");
        close(sock);
        return 1;
    }

    int klient = accept(sock, nullptr, nullptr);
    if (klient == -1) {
        perror("accept");
        close(sock);
        return 1;
    }

    puts("Klient sie polaczyl");
    if (send(klient, powitanie, sizeof(powitanie) - 1, MSG_NOSIGNAL) == -1) {
        perror("send");
    }

    char daneOczekujace[pojemnoscLinii];
    size_t liczbaBajtow = 0;
    bool bladWysylania = false;
    while (!bladWysylania) {
        const size_t wolneMiejsce = sizeof(daneOczekujace) - liczbaBajtow;
        if (wolneMiejsce == 0) {
            fputs("Wiadomosc klienta jest za dluga.\n", stderr);
            break;
        }
        const size_t doOdczytania = wolneMiejsce < sizeof(bufor) ? wolneMiejsce : sizeof(bufor);
        const ssize_t odebrano = recv(klient, bufor, doOdczytania, 0);
        if (odebrano == 0) {
            break;
        }
        if (odebrano == -1) {
            perror("recv");
            break;
        }

        memcpy(daneOczekujace + liczbaBajtow, bufor, odebrano);
        liczbaBajtow += odebrano;
        while (true) {
            const size_t dlugoscWiadomosci = znajdzDlugoscLinii(daneOczekujace, liczbaBajtow);
            if (dlugoscWiadomosci == 0) {
                break;
            }

            if (send(klient, daneOczekujace, dlugoscWiadomosci, MSG_NOSIGNAL) == -1) {
                perror("send");
                bladWysylania = true;
                break;
            }
            liczbaBajtow -= dlugoscWiadomosci;
            memmove(daneOczekujace, daneOczekujace + dlugoscWiadomosci, liczbaBajtow);
        }
    }

    close(klient);
    close(sock);
    return 0;
}
