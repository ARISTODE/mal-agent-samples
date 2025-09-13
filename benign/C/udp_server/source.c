#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define close(s) closesocket(s)
#define SOCKET_ERROR_VAL WSAGetLastError()
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SOCKET_ERROR_VAL errno
#endif

#define BUFFER_SIZE 4096
#define DEFAULT_PORT 8888
#define MAX_CLIENTS 100

typedef struct {
    struct sockaddr_in address;
    time_t last_seen;
    int message_count;
    size_t bytes_received;
} ClientInfo;

typedef struct {
    SOCKET socket;
    int port;
    int running;
    time_t start_time;
    int total_messages;
    size_t total_bytes;
    ClientInfo clients[MAX_CLIENTS];
    int client_count;
} UDPServer;

void print_usage() {
    printf("Usage: binary.exe [OPTIONS]\n");
    printf("  OPTIONS:\n");
    printf("    --help, -h        Show this help message\n");
    printf("    --test            Run UDP server tests\n");
    printf("    --serve PORT      Start UDP server on port\n");
    printf("    --echo PORT       Start UDP echo server\n");
    printf("    --client HOST PORT Test as UDP client\n");
    printf("    --broadcast       Test UDP broadcast\n");
    printf("\nUDP server and client demonstration\n");
    printf("Examples:\n");
    printf("  binary.exe --serve 8888\n");
    printf("  binary.exe --echo 9999\n");
    printf("  binary.exe --client localhost 8888\n");
}

#ifdef _WIN32
int initialize_winsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return 0;
    }
    return 1;
}

void cleanup_winsock() {
    WSACleanup();
}
#else
int initialize_winsock() { return 1; }
void cleanup_winsock() {}
#endif

void udp_server_init(UDPServer *server) {
    memset(server, 0, sizeof(UDPServer));
    server->socket = INVALID_SOCKET;
    server->port = DEFAULT_PORT;
}

int udp_server_start(UDPServer *server, int port) {
    printf("Starting UDP server on port %d...\n", port);
    
    server->port = port;
    server->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server->socket == INVALID_SOCKET) {
        printf("Error: Socket creation failed: %d\n", (int)SOCKET_ERROR_VAL);
        return 0;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server->socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Error: Bind failed: %d\n", (int)SOCKET_ERROR_VAL);
        close(server->socket);
        server->socket = INVALID_SOCKET;
        return 0;
    }
    
    server->running = 1;
    server->start_time = time(NULL);
    printf("UDP server started successfully on port %d\n", port);
    return 1;
}

ClientInfo* find_or_add_client(UDPServer *server, const struct sockaddr_in *client_addr) {
    for (int i = 0; i < server->client_count; i++) {
        if (server->clients[i].address.sin_addr.s_addr == client_addr->sin_addr.s_addr &&
            server->clients[i].address.sin_port == client_addr->sin_port) {
            return &server->clients[i];
        }
    }
    
    if (server->client_count < MAX_CLIENTS) {
        ClientInfo *new_client = &server->clients[server->client_count];
        memcpy(&new_client->address, client_addr, sizeof(struct sockaddr_in));
        new_client->last_seen = time(NULL);
        new_client->message_count = 0;
        new_client->bytes_received = 0;
        server->client_count++;
        
        printf("New client: %s:%d\n", 
               inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
        return new_client;
    }
    
    return NULL;
}

void udp_server_run_echo(UDPServer *server) {
    printf("UDP Echo Server running on port %d\n", server->port);
    printf("Press Ctrl+C to stop (or send 'quit' message)\n\n");
    
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    while (server->running) {
        int received = recvfrom(server->socket, buffer, sizeof(buffer) - 1, 0,
                               (struct sockaddr*)&client_addr, &client_len);
        
        if (received == SOCKET_ERROR) {
            printf("Error receiving data: %d\n", (int)SOCKET_ERROR_VAL);
            break;
        }
        
        if (received > 0) {
            buffer[received] = '\0';
            server->total_messages++;
            server->total_bytes += received;
            
            printf("Received from %s:%d (%d bytes): '%s'\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),
                   received, buffer);
            
            ClientInfo *client = find_or_add_client(server, &client_addr);
            if (client) {
                client->message_count++;
                client->bytes_received += received;
                client->last_seen = time(NULL);
            }
            
            if (strcmp(buffer, "quit") == 0) {
                printf("Quit command received, stopping server...\n");
                server->running = 0;
                break;
            }
            
            int sent = sendto(server->socket, buffer, received, 0,
                             (struct sockaddr*)&client_addr, client_len);
            
            if (sent == SOCKET_ERROR) {
                printf("Error sending echo: %d\n", (int)SOCKET_ERROR_VAL);
            } else {
                printf("Echoed %d bytes back to client\n", sent);
            }
        }
    }
}

void udp_server_run_simple(UDPServer *server) {
    printf("UDP Server running on port %d\n", server->port);
    printf("Accepting messages (send 'quit' to stop)...\n\n");
    
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    while (server->running) {
        int received = recvfrom(server->socket, buffer, sizeof(buffer) - 1, 0,
                               (struct sockaddr*)&client_addr, &client_len);
        
        if (received == SOCKET_ERROR) {
            printf("Error receiving data: %d\n", (int)SOCKET_ERROR_VAL);
            break;
        }
        
        if (received > 0) {
            buffer[received] = '\0';
            server->total_messages++;
            server->total_bytes += received;
            
            printf("[%s:%d] %s\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
            
            ClientInfo *client = find_or_add_client(server, &client_addr);
            if (client) {
                client->message_count++;
                client->bytes_received += received;
                client->last_seen = time(NULL);
            }
            
            if (strcmp(buffer, "quit") == 0) {
                printf("Quit command received, stopping server...\n");
                
                const char *goodbye = "Server shutting down. Goodbye!";
                sendto(server->socket, goodbye, (int)strlen(goodbye), 0,
                       (struct sockaddr*)&client_addr, client_len);
                
                server->running = 0;
                break;
            }
            
            const char *ack = "Message received";
            sendto(server->socket, ack, (int)strlen(ack), 0,
                   (struct sockaddr*)&client_addr, client_len);
        }
    }
}

void udp_server_stop(UDPServer *server) {
    if (server->socket != INVALID_SOCKET) {
        printf("\nStopping UDP server on port %d\n", server->port);
        close(server->socket);
        server->socket = INVALID_SOCKET;
    }
    server->running = 0;
}

void udp_server_print_stats(const UDPServer *server) {
    printf("\n=== UDP Server Statistics ===\n");
    printf("Port: %d\n", server->port);
    printf("Running time: %d seconds\n", (int)(time(NULL) - server->start_time));
    printf("Total messages: %d\n", server->total_messages);
    printf("Total bytes: %zu\n", server->total_bytes);
    printf("Unique clients: %d\n", server->client_count);
    
    if (server->client_count > 0) {
        printf("\nClient Details:\n");
        for (int i = 0; i < server->client_count; i++) {
            const ClientInfo *client = &server->clients[i];
            char *time_str = ctime(&client->last_seen);
            time_str[strlen(time_str) - 1] = '\0';
            
            printf("  %d. %s:%d - %d messages, %zu bytes (last seen: %s)\n",
                   i + 1,
                   inet_ntoa(client->address.sin_addr),
                   ntohs(client->address.sin_port),
                   client->message_count,
                   client->bytes_received,
                   time_str);
        }
    }
}

int udp_client_test(const char *host, int port) {
    printf("UDP Client Test\n");
    printf("===============\n");
    printf("Target: %s:%d\n\n", host, port);
    
    SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_socket == INVALID_SOCKET) {
        printf("Error: Client socket creation failed: %d\n", (int)SOCKET_ERROR_VAL);
        return 0;
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
#ifdef _WIN32
    server_addr.sin_addr.s_addr = inet_addr(host);
    if (server_addr.sin_addr.s_addr == INADDR_NONE) {
        struct hostent *host_entry = gethostbyname(host);
        if (host_entry == NULL) {
            printf("Error: Cannot resolve hostname '%s'\n", host);
            close(client_socket);
            return 0;
        }
        memcpy(&server_addr.sin_addr, host_entry->h_addr, host_entry->h_length);
    }
#else
    if (inet_aton(host, &server_addr.sin_addr) == 0) {
        struct hostent *host_entry = gethostbyname(host);
        if (host_entry == NULL) {
            printf("Error: Cannot resolve hostname '%s'\n", host);
            close(client_socket);
            return 0;
        }
        memcpy(&server_addr.sin_addr, host_entry->h_addr, host_entry->h_length);
    }
#endif
    
    const char *test_messages[] = {
        "Hello UDP Server!",
        "Test message 1",
        "Test message 2",
        "This is a longer test message with more content",
        "Final test message"
    };
    
    const int num_messages = sizeof(test_messages) / sizeof(test_messages[0]);
    
    for (int i = 0; i < num_messages; i++) {
        printf("Sending: '%s'\n", test_messages[i]);
        
        int sent = sendto(client_socket, test_messages[i], (int)strlen(test_messages[i]), 0,
                         (struct sockaddr*)&server_addr, sizeof(server_addr));
        
        if (sent == SOCKET_ERROR) {
            printf("Error sending message: %d\n", (int)SOCKET_ERROR_VAL);
            continue;
        }
        
        char response[BUFFER_SIZE];
        struct sockaddr_in from_addr;
        socklen_t from_len = sizeof(from_addr);
        
        int received = recvfrom(client_socket, response, sizeof(response) - 1, 0,
                               (struct sockaddr*)&from_addr, &from_len);
        
        if (received > 0) {
            response[received] = '\0';
            printf("Response: '%s'\n", response);
        } else {
            printf("No response received\n");
        }
        
        printf("\n");
    }
    
    close(client_socket);
    return 1;
}

void test_udp_broadcast() {
    printf("UDP Broadcast Test\n");
    printf("==================\n\n");
    
    SOCKET broadcast_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (broadcast_socket == INVALID_SOCKET) {
        printf("Error: Broadcast socket creation failed: %d\n", (int)SOCKET_ERROR_VAL);
        return;
    }
    
    int broadcast_enable = 1;
    if (setsockopt(broadcast_socket, SOL_SOCKET, SO_BROADCAST,
                   (char*)&broadcast_enable, sizeof(broadcast_enable)) == SOCKET_ERROR) {
        printf("Error: Cannot enable broadcast: %d\n", (int)SOCKET_ERROR_VAL);
        close(broadcast_socket);
        return;
    }
    
    struct sockaddr_in broadcast_addr;
    memset(&broadcast_addr, 0, sizeof(broadcast_addr));
    broadcast_addr.sin_family = AF_INET;
    broadcast_addr.sin_addr.s_addr = INADDR_BROADCAST;
    broadcast_addr.sin_port = htons(8888);
    
    const char *broadcast_message = "UDP Broadcast Test Message";
    printf("Broadcasting message: '%s'\n", broadcast_message);
    
    int sent = sendto(broadcast_socket, broadcast_message, (int)strlen(broadcast_message), 0,
                     (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
    
    if (sent == SOCKET_ERROR) {
        printf("Error sending broadcast: %d\n", (int)SOCKET_ERROR_VAL);
    } else {
        printf("Broadcast sent successfully (%d bytes)\n", sent);
    }
    
    close(broadcast_socket);
}

void run_udp_tests() {
    printf("UDP Test Suite\n");
    printf("==============\n\n");
    
    printf("--- Test 1: UDP Broadcast ---\n");
    test_udp_broadcast();
    
    printf("\n--- Test 2: Localhost Client Test ---\n");
    printf("Note: This test requires a UDP server running on localhost:8888\n");
    printf("You can start one with: binary.exe --echo 8888\n");
    
    printf("\nUDP tests completed!\n");
}

int main(int argc, char *argv[]) {
    if (!initialize_winsock()) {
        return 1;
    }
    
    if (argc < 2) {
        printf("UDP Server/Client Demo\n");
        printf("======================\n\n");
        print_usage();
        cleanup_winsock();
        return 1;
    }
    
    int result = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage();
            break;
        } else if (strcmp(argv[i], "--test") == 0) {
            run_udp_tests();
            break;
        } else if (strcmp(argv[i], "--serve") == 0) {
            if (i + 1 < argc) {
                int port = atoi(argv[i + 1]);
                if (port > 0 && port <= 65535) {
                    UDPServer server;
                    udp_server_init(&server);
                    
                    if (udp_server_start(&server, port)) {
                        udp_server_run_simple(&server);
                        udp_server_print_stats(&server);
                        udp_server_stop(&server);
                    }
                } else {
                    printf("Error: Invalid port number\n");
                    result = 1;
                }
                i++; // Skip the port argument
            } else {
                printf("Error: --serve requires a port argument\n");
                result = 1;
            }
            break;
        } else if (strcmp(argv[i], "--echo") == 0) {
            if (i + 1 < argc) {
                int port = atoi(argv[i + 1]);
                if (port > 0 && port <= 65535) {
                    UDPServer server;
                    udp_server_init(&server);
                    
                    if (udp_server_start(&server, port)) {
                        udp_server_run_echo(&server);
                        udp_server_print_stats(&server);
                        udp_server_stop(&server);
                    }
                } else {
                    printf("Error: Invalid port number\n");
                    result = 1;
                }
                i++; // Skip the port argument
            } else {
                printf("Error: --echo requires a port argument\n");
                result = 1;
            }
            break;
        } else if (strcmp(argv[i], "--client") == 0) {
            if (i + 2 < argc) {
                int port = atoi(argv[i + 2]);
                if (port > 0 && port <= 65535) {
                    udp_client_test(argv[i + 1], port);
                } else {
                    printf("Error: Invalid port number\n");
                    result = 1;
                }
                i += 2; // Skip the host and port arguments
            } else {
                printf("Error: --client requires host and port arguments\n");
                result = 1;
            }
            break;
        } else if (strcmp(argv[i], "--broadcast") == 0) {
            test_udp_broadcast();
            break;
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage();
            result = 1;
            break;
        }
    }
    
    cleanup_winsock();
    return result;
}