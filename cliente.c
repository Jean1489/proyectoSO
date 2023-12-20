#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 5555
#define SERVER_IP "127.0.0.1"

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char command[1024];
    char buffer[1024];

    // Crear el socket del cliente
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la estructura del servidor
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error al conectar al servidor");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Leer el comando desde el usuario
        printf("Ingrese un comando (o 'salida' para salir): ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0'; // Eliminar el carácter de nueva línea

        // Salir si el usuario ingresa 'salida'
        if (strcmp(command, "salida") == 0) {
            // Enviar un mensaje especial al servidor indicando que el cliente se va a desconectar
            send(client_socket, "salida", strlen("salida"), 0);
            break;
        }

        // Enviar el comando al servidor
        ssize_t bytes_sent = send(client_socket, command, sizeof(command), 0);
        if (bytes_sent == -1){
            perror("Error al enviar el comando al servidor");
            break;
        
        }

        // Recibir la respuesta del servidor
        recv(client_socket, buffer, sizeof(buffer), 0);

        // Imprimir la respuesta
        printf("Respuesta del servidor: %s\n", buffer);
    }

    // Cerrar el socket del cliente
    close(client_socket);

    return 0;
}

