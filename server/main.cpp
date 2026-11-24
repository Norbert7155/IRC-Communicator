#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        std::perror("socket");
        return 1;
    }

    sockaddr_in adresSerwera{};
    adresSerwera.sin_family = AF_INET;
    adresSerwera.sin_port = htons(1335);
    adresSerwera.sin_addr.s_addr = INADDR_ANY;

    

    if(bind(sock, (sockaddr*)&adresSerwera, sizeof(adresSerwera)) == -1) {
        std::perror("bind");
        close(sock);
        return 1;
    }

    if(listen(sock, 10) == -1){
        std::perror("listen");
        close(sock);
        return 1;
    }

    int klient = accept(sock, nullptr, nullptr);
    if(klient == -1) {
        std:perror("akcept");
        close(sock);
        return 1;
    }

    std::puts("Klient sie polaczyl");
    close(klient);
    close(sock)
    return 0;

    const char powitanie[] = "Witaj na serwerze!\r\n";

    
    if (send(klient, powitanie, sizeof(powitanie) - 1, MSG_NOSIGNAL) == -1) {
        std::perror("send");
    }

}