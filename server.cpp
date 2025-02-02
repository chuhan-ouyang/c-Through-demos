#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

#define PORT 8080
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

    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Default ";
    print_buffer_sizes(server_fd);

    if (buffer_size > 0) {
        set_buffer_sizes(server_fd, buffer_size);
        std::cout << "Updated ";
        print_buffer_sizes(server_fd);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    socklen_t addrlen = sizeof(address);
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Client connected.\n";
    print_buffer_sizes(new_socket);

    while (true) {
        int bytes_received = read(new_socket, buffer, BUFFER_SIZE);
        if (bytes_received <= 0) {
            std::cout << "Client disconnected.\n";
            break;
        }
        std::cout << "Received (" << bytes_received << " bytes), echoing back...\n";
        send(new_socket, buffer, bytes_received, 0);  // Echo back
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
