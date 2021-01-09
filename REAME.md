# Servidor de Mensagens Publish/Subscribe

TODO: terminar documentação e testar código

## Introdução
A aplicação foi dividida em 2 partes:

Os arquivos src/*c e src/*h contem toda a lógica de programação necessária para a implementação do servidor e cliente Publish/Subscribe.

Os arquivos src/network/*c e src/network/*h são os arquivos que abstraem a implementação de uma estrutura de servidor e cliente além de algoritmos de hash para strings e estruturas de dados para programação assincrona.

## Network:
Para detalhes de implementação mains profundos, favor checar os códigos de cabeçalho (src/network/*.h) que documenta as estruturas e métodos utilizados nessa biblioteca.

Foi implementado em src/network/servidor.c uma estrutura que abstrai a funcionalidade de um servidor.
Foi feito o uso de unix sockets para a implementação do servidor e abtrações de estruturas de dados assincronas(encontradas em src/network/async.h).

### Servidor
O Servidor cria 1 thread que é responsável por esperar conexoes e quando uma solicitação de conexão é feita por um cliente, essa mesma thread é responsável por criar uma nova thread que irá ser responsável por manter a conexão com o cliente em questão.

As requisições são feitas pelos clientes ao se enviar mensagens ao servidor utilizando-se o seguinte formato:
*  endpoint\r\nmessage\r\n" - ex: "/say-hello-to/\r\njonny\r\n"

O "endpoint" diz ao servidor qual handler cadastrado pelo servidor deverá ser utilizado para cuidar da requisição e o campo message diz qual sequência de caracteres deverá ser fornecida como entrada ao handler selecionado.

### Cliente
Foi implementado em src/network/client.c uma estrutura que abstrai a funcionalidade de um client para o servidor implementado em src/network/servidor.c.
Foi feito o uso de unix sockets para a implementação do cliente e abtrações de estruturas de dados assincronas(encontradas em src/network/async.h).

O cliente faz uso de um unico thread responsável por processar as mensagens enviadas pelo servidor cujo cliente está atualmente conectado.

## Publish/Subscribe:
Toda a lógica relacionada à aplicação de Publish/Subscribe