// cliente
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORTA      8080
#define TAM_BUFFER 1024

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo>\n", argv[0]);
        fprintf(stderr, "     %s SAIR        (encerra o servidor)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //Cria socket UDP
    int socket_cliente = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_cliente < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    //configura endereço do servidor
    struct sockaddr_in endereco_servidor;
    memset(&endereco_servidor, 0, sizeof(endereco_servidor));
    endereco_servidor.sin_family = AF_INET;
    endereco_servidor.sin_port   = htons(PORTA);

    if (inet_pton(AF_INET, "127.0.0.1", &endereco_servidor.sin_addr) <= 0) {
        perror("Endereço IP inválido");
        close(socket_cliente);
        exit(EXIT_FAILURE);
    }

    socklen_t tamanho_servidor = sizeof(endereco_servidor);

    //comando SAIR: avisa o servidor e encerra
    if (strcmp(argv[1], "SAIR") == 0) {
        sendto(socket_cliente, "SAIR", 4, 0,
               (struct sockaddr *)&endereco_servidor, tamanho_servidor);
        printf("Comando SAIR enviado ao servidor.\n");
        close(socket_cliente);
        return 0;
    }

    //abre o arquivo em modo binario
    FILE *arq = fopen(argv[1], "rb");
    if (!arq) {
        perror("Erro ao abrir arquivo");
        close(socket_cliente);
        exit(EXIT_FAILURE);
    }

    //envia o nome do arquivo primeiro
    const char *nome = strrchr(argv[1], '/');
    nome = nome ? nome + 1 : argv[1];

    sendto(socket_cliente, nome, strlen(nome) + 1, 0,
           (struct sockaddr *)&endereco_servidor, tamanho_servidor);

    printf("Enviando arquivo: %s\n", nome);

    //envia o conteúdo do arquivo em partes
    char buffer[TAM_BUFFER];
    int total = 0;
    int bytes_lidos;

    while ((bytes_lidos = fread(buffer, 1, TAM_BUFFER, arq)) > 0) {
        int bytes_enviados = sendto(socket_cliente, buffer, bytes_lidos, 0,
                                    (struct sockaddr *)&endereco_servidor,
                                    tamanho_servidor);
        if (bytes_enviados < 0) {
            perror("Erro ao enviar dados");
            break;
        }
        total += bytes_enviados;
    }

    //envia sinal de fim
    sendto(socket_cliente, "FIM", 3, 0,
       (struct sockaddr *)&endereco_servidor, tamanho_servidor);

    printf("Arquivo enviado: %d bytes\n", total);

    fclose(arq);
    close(socket_cliente);
    return 0;
}