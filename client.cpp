#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>

#define PORT 8080
#define MESSAGE_SIZE (1024) 
#define BUFFER_SIZE 1024
#define DEFAULT_INCREASED_SIZE 200000  // Size for -i flag

void print_buffer_sizes(int sock) {
    int recv_buf, send_buf;
    socklen_t optlen = sizeof(int);
    
    getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recv_buf, &optlen);
    getsockopt(sock, SOL_SOCKET, SO_SNDBUF, &send_buf, &optlen);
    
    std::cout << "TCP Buffer Sizes:\n";
    std::cout << "Receive Buffer: " << recv_buf << " bytes\n";
    std::cout << "Send Buffer: " << send_buf << " bytes\n";
}

void set_buffer_sizes(int sock, int buffer_size) {
    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size));
    setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &buffer_size, sizeof(buffer_size));
}

int main(int argc, char *argv[]) {
    int buffer_size = 0;  // Default: No change

    if (argc == 3 && std::string(argv[1]) == "-n") {
        buffer_size = std::atoi(argv[2]);
    } else if (argc == 2 && std::string(argv[1]) == "-i") {
        buffer_size = DEFAULT_INCREASED_SIZE;
    } else if (argc > 1) {
        std::cerr << "Usage: " << argv[0] << " [-n buffer_size] or [-i]\n";
        return EXIT_FAILURE;
    }

    int sock = 0;
    struct sockaddr_in serv_addr;
    char message[MESSAGE_SIZE];
    memset(message, 'A', sizeof(message) - 1);
    message[sizeof(message) - 1] = '\0';  // Null-terminate the string

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    std::cout << "Default ";
    print_buffer_sizes(sock);

    if (buffer_size > 0) {
        set_buffer_sizes(sock, buffer_size);
        std::cout << "Updated ";
        print_buffer_sizes(sock);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected to server.\n";

    while (true) {
        send(sock, message, sizeof(message), 0);
        std::cout << "Sent 16384 bytes data packet.\n";

        char buffer[BUFFER_SIZE] = {0};
        int bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            std::cout << "Server disconnected.\n";
            break;
        }
        std::cout << "Received echo (" << bytes_received << " bytes)\n";
    }

    close(sock);
    return 0;
}
