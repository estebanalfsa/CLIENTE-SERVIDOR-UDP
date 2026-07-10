// servidor
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORTA      8080
#define TAM_BUFFER 1024

int main() {

    //cria socket UDP
    int socket_servidor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_servidor < 0) {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    //configura endereço do servidor
    struct sockaddr_in endereco_servidor;
    memset(&endereco_servidor, 0, sizeof(endereco_servidor));
    endereco_servidor.sin_family      = AF_INET;
    endereco_servidor.sin_addr.s_addr = INADDR_ANY;
    endereco_servidor.sin_port        = htons(PORTA);

    if (bind(socket_servidor,
             (struct sockaddr *)&endereco_servidor,
             sizeof(endereco_servidor)) < 0) {
        perror("Erro no bind");
        close(socket_servidor);
        exit(EXIT_FAILURE);
    }

    printf("Servidor UDP aguardando na porta %d...\n", PORTA);

    struct sockaddr_in endereco_cliente;
    socklen_t tamanho_cliente = sizeof(endereco_cliente);
    char buffer[TAM_BUFFER];
    char nome_arquivo[256];

    while (1) {

        //recebe o nome do arquivo (ou comando SAIR)
        int bytes = recvfrom(socket_servidor, nome_arquivo, sizeof(nome_arquivo) - 1, 0,
            (struct sockaddr *)&endereco_cliente, &tamanho_cliente);
        if (bytes <= 0) continue;
        nome_arquivo[bytes] = '\0';

        //verifica se é o comando de encerramento
        if (strcmp(nome_arquivo, "SAIR") == 0) {
            printf("Comando SAIR recebido. Encerrando servidor.\n");
            break;
        }

        //prrefixo para não sobrescrever arquivos locais: receibido_nome_arquivo
        char nome_saida[300];
        snprintf(nome_saida, sizeof(nome_saida), "recebido_%s", nome_arquivo);

        printf("Recebendo '%s' de %s:%d...\n",
               nome_arquivo,
               inet_ntoa(endereco_cliente.sin_addr),
               ntohs(endereco_cliente.sin_port));

        FILE *arq = fopen(nome_saida, "wb");
        if (!arq) {
            perror("Erro ao criar arquivo");
            continue;
        }

        int total = 0;

        //recebe os dados até chegar um datagrama vazio 
        while (1) {
            bytes = recvfrom(socket_servidor, buffer, TAM_BUFFER, 0,
                (struct sockaddr *)&endereco_cliente, &tamanho_cliente);
            if (bytes == 3 && memcmp(buffer, "FIM", 3) == 0) break; //datagrama vazio = fim

            fwrite(buffer, 1, bytes, arq);
            total += bytes;
        }

        fclose(arq);
        printf("Arquivo salvo como '%s' (%d bytes)\n\n", nome_saida, total);
    }

    close(socket_servidor);
    return 0;
}