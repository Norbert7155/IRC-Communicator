#include <cstdio>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    char bufor[256];
    const char powitanie[] = "Witaj na serwerze!\r\n";


    if (sock == -1) {
        std::perror("socket");
        return 1;
    }

    sockaddr_in adresSerwera{};
    adresSerwera.sin_family = AF_INET;
    adresSerwera.sin_port = htons(1335);
    adresSerwera.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (sockaddr*)&adresSerwera, sizeof(adresSerwera)) == -1) {
        std::perror("bind");
        close(sock);
        return 1;
    }

    if (listen(sock, 10) == -1) {
        std::perror("listen");
        close(sock);
        return 1;
    }

    int klient = accept(sock, nullptr, nullptr);
    if (klient == -1) {
        std::perror("accept");
        close(sock);
        return 1;
    }

    std::puts("Klient sie polaczyl");
    if (send(klient, powitanie, sizeof(powitanie) - 1, MSG_NOSIGNAL) == -1) {
        std::perror("send");
    }

    std::string daneOczekujace;
    bool bladWysylania = false;
    while (!bladWysylania) {
        ssize_t odebrano = recv(klient, bufor, sizeof(bufor), 0);
        if (odebrano == 0) {
            break;
        }
        if (odebrano == -1) {
            std::perror("recv");
            break;
        }

        daneOczekujace.append(bufor, static_cast<std::size_t>(odebrano));
        while (true) {
            const auto koniec = daneOczekujace.find("\r\n");
            if (koniec == std::string::npos) {
                break;
            }

            const auto dlugoscWiadomosci = koniec + 2;
            if (send(klient, daneOczekujace.data(), dlugoscWiadomosci, MSG_NOSIGNAL) == -1) {
                std::perror("send");
                bladWysylania = true;
                break;
            }
            daneOczekujace.erase(0, dlugoscWiadomosci);
        }
    }

    close(klient);
    close(sock);
    return 0;
}
