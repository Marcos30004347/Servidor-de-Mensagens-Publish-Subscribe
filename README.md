**Universidade Federal de Minas Gerais - DCC - Curso de Redes 2020/2**

Aluno: Marcos Vinicius Moreira Santos

# 1º Trabalho Prático - Servidor de Mensagens Publish/Subscribe
## Introdução
Ao longo dessa documentação discutiremos a implementação do Primeiro Trabalho Prático do Curso de Redes do Segundo Semestre de 2020.

Para a implementação, o código foi dividido em 2 modulos, uma biblioteca de código que está em src/netowrk com fim de servir como uma espécie de framework para o desenvolvimento da aplicação que visa facilitar a impementação do cliente e servidor de forma a deixar a lógica da aplicação separada dos detalhes de network e concorrência.

A lógica de programação da aplicação Publish/Subscribe está definida nos arquivos *.c e *.h localizadas dentro do diretório "src/".

A aplicação é compativel somente com sistemas UNIX.

## Network:
### Introdução:
Os arquivos em src/network são os arquivos que abstraem a implementação de uma estrutura de servidor e cliente para comunicação sobre a network além de algoritmos de hash para strings e estruturas de dados para programação assincrona.

### Servidor:
O Servidor cria uma thread que é responsável por esperar conexoes e quando uma solicitação de conexão é feita por um cliente, essa mesma thread é responsável por criar uma nova thread que irá ser responsável por manter a conexão com o cliente em questão.

As requisições são feitas pelos clientes ao se enviar mensagens ao servidor utilizando-se o seguinte formato:
*  endpoint + \r\n + message + \r\n" - ex: "/say-hello-to/\r\njonny\r\n"

O endpoint diz ao servidor qual função implementada pelo servidor deverá ser utilizada para cuidar da requisição em questão e o campo message diz qual sequência de caracteres deverá ser fornecida como entrada ao handler selecionado.

### Cliente:
Foi implementado em src/network/client.c uma estrutura que abstrai a funcionalidade de um client para o servidor implementado em src/network/servidor.c.

Foi feito o uso de unix sockets para a implementação do cliente e abtrações de estruturas de dados assincronas(encontradas em src/network/async.h).

O cliente faz uso de um unico thread responsável por processar as mensagens enviadas pelo servidor cujo cliente está atualmente conectado.

### Detalhes de implementação:
Para a implementação das estruturas de server e de cliente dentro de network foram utilizadas estruturas de dados assincronas, como threads e mutexes, ambas essas estruturas foram abstraidas atravez da biblioteca pthreads em src/network/async.h e src/network/async.c.

Foi feito o uso de unix sockets para a implementação do servidor e cliente assim como outras classes da standard library como strings, stdlib e stdio.

O uso de threads foi necessário para garantir que um servidor possa servir a mais de um cliente por vez, portanto para cada cliente o servidor cria uma thread unica que será responsável por escutar as requisições feitas por esse cliente, o servidor tambem possui uma mutex responsável por sincronizar a execução dos endpoints no servidor.

Pela natureza assincrona que o cliente pode receber as mensagens do servidor, também foi utilizado uma thread para tal.

Para armazenar as conexoes ativas com o servidor, foi utilizado uma estrutura de lista encadeada que pode ser utilizada simultaneamente por multiplas threads, sincronizada através do uso de mutexes, onde cada nó armazena as informações references à uma conexão/cliente.

## Publish/Subscribe:
Toda a lógica relacionada à aplicação de Publish/Subscribe está definida sobre os arquivos *.c e *.h dentro do diretório src/.

A aplicação solicitada defini quatro ações de usuário:

1. Cadastrar o cliente em um canal enviando uma mengagem iniciada com o caractere '+'.

2. Descadastar o cliente de um canal enviando uma mengagem iniciada com o caractere '-'.

3. Encerrar a execução do servidor e todas as suas conexoes enviando uma mensagem "##kill".

4. Enviar uma mensagem de texto para o servidor que irá enviar a mesma mensagem para qualquer cliente que esteja interessado, enviando uma mensagem como caractere inicial arbitrário, processo que nomearemos broadcast.

### Servidor:
O servidor é responsável por tratar as requisições feitas por clientes. Cada uma das ações recebeu um *"endpoint"* especializado no servidor. Como discutido na seção Network.Server dessa documentação, um endpoint é a primeira parte da mensagem, separada por '\r\n', enviada por qualquer cliente ao servidor, e seu papel é dizer ao servidor qual o método deverá ser executado sobre os dados enviados.

O servidor utiliza apenas uma estrutura de dados auxiliar, um mapa do tipo 'string'->'lista de inteiros' que irá armazenar em qual canal('string') um determinado grupo de clientes('lista de inteiros') estão interessados, chamaremos essa estrutura de tabela de canais.

A partir disso, podemos definir os quatro endpoints necessários:

1. "/channel/listen/" - Esse endpoint é responsável por extrair da entrada o canal que o cliente está tentando se cadastrar e adicionar à tabela de canais o cliente solicitante usando como chave o canal solicitado.

2. "/channel/mute/" - Esse endpoint é responsável por extrair da entrada o canal que o cliente está tentando se descadastrar e remover da tabela de canais o cliente solicitante usando como chave o canal extraido.

3. "/kill/" - Esse endpoint é responsavél por enviar a todos os clientes a mensagem "##kill" e finalizar de forma correta a execução do servidor, o que inclui terminar todas as conexões, threads e estruturas de dados utilizadas.

4. "/broadcast/" - Esse endpoint é responsavel por extrair da mensagem recebida os canais que o cliente deseja postar uma mensagem e enviar a mesma mensagem para todos os clientes cadastrados nesses canais.

Além destes endpoints também foi adicionado um endpoint extra "/disconnect/" responsável por desconectar o cliente de um servidor destruindo todas as estruturas e memória ocupadas pelo cliente se desconectando.

### Cliente:
O cliente é responsável por 2 coisas:
    
1. Receber a entrada do usuário 

2. Converter a entrada do usuário em chamadas para o servidor.

O passo 1 é feito por um thread especializado que fica esperando por uma entrada do usuário com o metodo 'fgets'. Após isso, nossa aplicação define as quatro possiveis ações que o usuário pode tomar:

1. Para cadastrar o cliente em um canal, basta verificar o primeiro caractere da entrada do usuário, caso este seja igual à '+', enviaremos a entrada do usuário para o servidor utilizando o endpoint '/channel/listen/'.

2. Para descadastrar o cliente em um canal, basta verificar o primeiro caractere da entrada do usuário, caso este seja igual à '-', enviaremos a entrada do usuário para o servidor utilizando o endpoint '/channel/mute/'.

3. Para encerrar a execução do servidor e todas as suas conexoes, basta verificar se a entrada é igual à sequência "##kill", caso o resultado seja verdadeiro, uma mensagem para o servidor através do endpoint "/kill/". Vale lembrar que o encerramento do cliente não acontece em um procedimento sincrono logo após o envio da requisição para o servidor, o que esse endpoint no servidor faz é enviar para todos os clientes conectados a mensagem "##kill", e somente após ler essa mensagem é que os clientes são propriamente finalizados.

4. Para executar o broadcast de mensagens, basta enviar ao servidor uma mensagem que não se encaixe dentre nenhuma das condições para as ações anteriores, se essa condição for verdadeira, a mensagem é enviada ao servidor pelo endpoint "/broadcast/" e será responsabilidade do servidor descobrir quais os canais cuja a mensagem será enviada.

