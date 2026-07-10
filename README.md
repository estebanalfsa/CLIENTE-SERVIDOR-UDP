# Transferência de Arquivos via UDP

Trabalho prático de Redes de Computadores.
**Aluno:** José Esteban Alfaro  

## Compilação

```bash
gcc servidor.c -o servidor
gcc cliente.c  -o cliente
```

## Como usar

**Terminal 1:**
```bash
./servidor
```

**Terminal 2:**
```bash
./cliente <arquivo>    # envia um arquivo
./cliente SAIR         # encerra o servidor
```

## Observações

- Funciona com qualquer formato (pdf, png, txt...).
- O arquivo recebido é salvo com o prefixo `recebido_`.
- O servidor deve estar rodando antes de executar o cliente.

## Problemas comuns
 
Se o servidor travar ou a porta 8080 ficar ocupada, execute:
 
```bash
pkill -f servidor
```
 
Depois inicie o servidor normalmente.