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
#define DEFAULT_PORT 80
#define DEFAULT_HOST "httpbin.org"

typedef struct {
    SOCKET socket;
    char host[256];
    int port;
    char last_error[512];
    int connected;
    time_t connect_time;
    size_t bytes_sent;
    size_t bytes_received;
} TCPClient;

void print_usage() {
    printf("Usage: binary.exe [OPTIONS]\n");
    printf("  OPTIONS:\n");
    printf("    --help, -h          Show this help message\n");
    printf("    --test              Run TCP client tests\n");
    printf("    --connect HOST PORT Connect to specific host and port\n");
    printf("    --http HOST         Make HTTP GET request to host\n");
    printf("    --echo HOST PORT    Connect to echo server\n");
    printf("    --benchmark         Run connection benchmark\n");
    printf("\nTCP client demonstration and testing\n");
    printf("Examples:\n");
    printf("  binary.exe --http httpbin.org\n");
    printf("  binary.exe --connect localhost 8080\n");
    printf("  binary.exe --echo echo.websocket.org 80\n");
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

void tcp_client_init(TCPClient *client) {
    memset(client, 0, sizeof(TCPClient));
    client->socket = INVALID_SOCKET;
    client->port = DEFAULT_PORT;
    strcpy(client->host, DEFAULT_HOST);
}

int tcp_client_connect(TCPClient *client, const char *host, int port) {
    printf("Connecting to %s:%d...\n", host, port);
    
    strncpy(client->host, host, sizeof(client->host) - 1);
    client->host[sizeof(client->host) - 1] = '\0';
    client->port = port;
    
    struct addrinfo hints, *result = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%d", port);
    
    int res = getaddrinfo(host, port_str, &hints, &result);
    if (res != 0) {
        snprintf(client->last_error, sizeof(client->last_error),
                "getaddrinfo failed: %d", res);
        printf("Error: %s\n", client->last_error);
        return 0;
    }
    
    client->socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (client->socket == INVALID_SOCKET) {
        snprintf(client->last_error, sizeof(client->last_error),
                "Socket creation failed: %d", (int)SOCKET_ERROR_VAL);
        printf("Error: %s\n", client->last_error);
        freeaddrinfo(result);
        return 0;
    }
    
    client->connect_time = time(NULL);
    res = connect(client->socket, result->ai_addr, (int)result->ai_addrlen);
    if (res == SOCKET_ERROR) {
        snprintf(client->last_error, sizeof(client->last_error),
                "Connect failed: %d", (int)SOCKET_ERROR_VAL);
        printf("Error: %s\n", client->last_error);
        close(client->socket);
        client->socket = INVALID_SOCKET;
        freeaddrinfo(result);
        return 0;
    }
    
    freeaddrinfo(result);
    client->connected = 1;
    printf("Successfully connected to %s:%d\n", host, port);
    return 1;
}

int tcp_client_send(TCPClient *client, const char *data, size_t length) {
    if (!client->connected || client->socket == INVALID_SOCKET) {
        strcpy(client->last_error, "Not connected");
        return 0;
    }
    
    printf("Sending %zu bytes...\n", length);
    int sent = send(client->socket, data, (int)length, 0);
    if (sent == SOCKET_ERROR) {
        snprintf(client->last_error, sizeof(client->last_error),
                "Send failed: %d", (int)SOCKET_ERROR_VAL);
        printf("Error: %s\n", client->last_error);
        return 0;
    }
    
    client->bytes_sent += sent;
    printf("Sent %d bytes\n", sent);
    return sent;
}

int tcp_client_receive(TCPClient *client, char *buffer, size_t buffer_size) {
    if (!client->connected || client->socket == INVALID_SOCKET) {
        strcpy(client->last_error, "Not connected");
        return 0;
    }
    
    int received = recv(client->socket, buffer, (int)(buffer_size - 1), 0);
    if (received == SOCKET_ERROR) {
        snprintf(client->last_error, sizeof(client->last_error),
                "Receive failed: %d", (int)SOCKET_ERROR_VAL);
        printf("Error: %s\n", client->last_error);
        return 0;
    }
    
    if (received == 0) {
        printf("Connection closed by server\n");
        client->connected = 0;
        return 0;
    }
    
    buffer[received] = '\0';
    client->bytes_received += received;
    printf("Received %d bytes\n", received);
    return received;
}

void tcp_client_disconnect(TCPClient *client) {
    if (client->socket != INVALID_SOCKET) {
        printf("Disconnecting from %s:%d\n", client->host, client->port);
        close(client->socket);
        client->socket = INVALID_SOCKET;
    }
    client->connected = 0;
}

void tcp_client_print_stats(const TCPClient *client) {
    printf("\n=== Connection Statistics ===\n");
    printf("Host: %s:%d\n", client->host, client->port);
    printf("Connected: %s\n", client->connected ? "Yes" : "No");
    
    if (client->connect_time > 0) {
        char *time_str = ctime(&client->connect_time);
        time_str[strlen(time_str) - 1] = '\0';
        printf("Connect time: %s\n", time_str);
        printf("Duration: %d seconds\n", (int)(time(NULL) - client->connect_time));
    }
    
    printf("Bytes sent: %zu\n", client->bytes_sent);
    printf("Bytes received: %zu\n", client->bytes_received);
    
    if (strlen(client->last_error) > 0) {
        printf("Last error: %s\n", client->last_error);
    }
}

void run_http_test(const char *host) {
    printf("HTTP GET Request Test\n");
    printf("====================\n");
    printf("Target: %s\n\n", host);
    
    TCPClient client;
    tcp_client_init(&client);
    
    if (!tcp_client_connect(&client, host, 80)) {
        tcp_client_print_stats(&client);
        return;
    }
    
    char request[1024];
    snprintf(request, sizeof(request),
            "GET /get HTTP/1.1\r\n"
            "Host: %s\r\n"
            "User-Agent: TCPClientDemo/1.0\r\n"
            "Accept: application/json\r\n"
            "Connection: close\r\n"
            "\r\n", host);
    
    printf("Sending HTTP request:\n");
    printf("---\n%s---\n", request);
    
    if (tcp_client_send(&client, request, strlen(request))) {
        char response[BUFFER_SIZE];
        
        printf("\nReceiving HTTP response:\n");
        printf("---\n");
        
        int total_received = 0;
        while (client.connected) {
            int received = tcp_client_receive(&client, response, sizeof(response));
            if (received > 0) {
                printf("%s", response);
                total_received += received;
                
                if (total_received > 10000) {
                    printf("\n[Response truncated - received %d bytes total]\n", total_received);
                    break;
                }
            } else {
                break;
            }
        }
        printf("\n---\n");
    }
    
    tcp_client_disconnect(&client);
    tcp_client_print_stats(&client);
}

void run_echo_test(const char *host, int port) {
    printf("Echo Server Test\n");
    printf("===============\n");
    printf("Target: %s:%d\n\n", host, port);
    
    TCPClient client;
    tcp_client_init(&client);
    
    if (!tcp_client_connect(&client, host, port)) {
        tcp_client_print_stats(&client);
        return;
    }
    
    const char *test_messages[] = {
        "Hello, Echo Server!",
        "This is a test message",
        "12345 numbers test",
        "Special chars: !@#$%^&*()",
        "Final test message"
    };
    
    const int num_messages = sizeof(test_messages) / sizeof(test_messages[0]);
    
    for (int i = 0; i < num_messages; i++) {
        printf("\n--- Test message %d ---\n", i + 1);
        printf("Sending: '%s'\n", test_messages[i]);
        
        if (tcp_client_send(&client, test_messages[i], strlen(test_messages[i]))) {
            char response[BUFFER_SIZE];
            int received = tcp_client_receive(&client, response, sizeof(response));
            
            if (received > 0) {
                printf("Echo received: '%s'\n", response);
                
                if (strcmp(test_messages[i], response) == 0) {
                    printf("Echo test PASSED\n");
                } else {
                    printf("Echo test FAILED (mismatch)\n");
                }
            }
        }
    }
    
    tcp_client_disconnect(&client);
    tcp_client_print_stats(&client);
}

void run_benchmark_test() {
    printf("Connection Benchmark Test\n");
    printf("========================\n\n");
    
    const char *test_hosts[] = {
        "httpbin.org",
        "www.google.com",
        "www.github.com"
    };
    
    const int num_hosts = sizeof(test_hosts) / sizeof(test_hosts[0]);
    
    for (int i = 0; i < num_hosts; i++) {
        printf("--- Benchmark %d: %s ---\n", i + 1, test_hosts[i]);
        
        TCPClient client;
        tcp_client_init(&client);
        
        clock_t start = clock();
        int success = tcp_client_connect(&client, test_hosts[i], 80);
        clock_t connect_time = clock();
        
        if (success) {
            const char *quick_request = "HEAD / HTTP/1.1\r\nHost: ";
            char request[256];
            snprintf(request, sizeof(request), "%s%s\r\n\r\n", quick_request, test_hosts[i]);
            
            tcp_client_send(&client, request, strlen(request));
            
            char response[1024];
            tcp_client_receive(&client, response, sizeof(response));
            
            clock_t total_time = clock();
            
            double connect_ms = ((double)(connect_time - start)) / CLOCKS_PER_SEC * 1000;
            double total_ms = ((double)(total_time - start)) / CLOCKS_PER_SEC * 1000;
            
            printf("  Connection time: %.2f ms\n", connect_ms);
            printf("  Total time: %.2f ms\n", total_ms);
            printf("  Status: SUCCESS\n");
            
            tcp_client_disconnect(&client);
        } else {
            printf("  Status: FAILED (%s)\n", client.last_error);
        }
        
        printf("\n");
    }
}

void run_basic_tests() {
    printf("TCP Client Test Suite\n");
    printf("====================\n\n");
    
    printf("--- Test 1: HTTP GET Request ---\n");
    run_http_test("httpbin.org");
    
    printf("\n\n--- Test 2: Connection Benchmark ---\n");
    run_benchmark_test();
    
    printf("All TCP client tests completed!\n");
}

int main(int argc, char *argv[]) {
    if (!initialize_winsock()) {
        return 1;
    }
    
    if (argc < 2) {
        printf("TCP Client Demo\n");
        printf("===============\n\n");
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
            run_basic_tests();
            break;
        } else if (strcmp(argv[i], "--http") == 0) {
            if (i + 1 < argc) {
                run_http_test(argv[i + 1]);
                i++; // Skip the host argument
            } else {
                printf("Error: --http requires a host argument\n");
                result = 1;
            }
            break;
        } else if (strcmp(argv[i], "--echo") == 0) {
            if (i + 2 < argc) {
                int port = atoi(argv[i + 2]);
                if (port > 0 && port <= 65535) {
                    run_echo_test(argv[i + 1], port);
                } else {
                    printf("Error: Invalid port number\n");
                    result = 1;
                }
                i += 2; // Skip the host and port arguments
            } else {
                printf("Error: --echo requires host and port arguments\n");
                result = 1;
            }
            break;
        } else if (strcmp(argv[i], "--connect") == 0) {
            if (i + 2 < argc) {
                int port = atoi(argv[i + 2]);
                if (port > 0 && port <= 65535) {
                    TCPClient client;
                    tcp_client_init(&client);
                    
                    if (tcp_client_connect(&client, argv[i + 1], port)) {
                        printf("Connection established successfully!\n");
                        printf("You can now send data (not implemented in this demo)\n");
                        tcp_client_disconnect(&client);
                    }
                    tcp_client_print_stats(&client);
                } else {
                    printf("Error: Invalid port number\n");
                    result = 1;
                }
                i += 2; // Skip the host and port arguments
            } else {
                printf("Error: --connect requires host and port arguments\n");
                result = 1;
            }
            break;
        } else if (strcmp(argv[i], "--benchmark") == 0) {
            run_benchmark_test();
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