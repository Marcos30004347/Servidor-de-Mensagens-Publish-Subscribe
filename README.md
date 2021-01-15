<center>
<p style="font-family: arial; font-size:10pt">
<h2>Universidade Federal de Minas Gerais</h2>
<h3>Departamento de Ciência da Computação</h3>
<h3>Curso de Redes 2020/2º</h3>
</p>
</center>

</br>

### **Aluno:** Marcos Vinicius Moreira Santos

</br>


# Relatório: Trabalho Prático - Aplicação Publish/Subscribe
## Introdução
<p style="font-family: arial; font-size:10pt">
O código foi dividido em 2 modulos, uma biblioteca de código que está em localizada em src/netowrk que abstrai um servidor e cliente TCP. Essa abstração foi usada para a implementação da aplicação Publish/Subscribe permitindo que as regras de negócio da aplicação fiquem mais limpas e de facil entendimento.

As regras de negócio da aplicação Publish/Subscribe está definida nos arquivos *.c e *.h localizadas dentro do diretório 'src/'.
</p>

### Compatibilidade:
<p style="font-family:arial; font-size:10pt">
A aplicação foi testada somente em sistemas **UNIX**, e portanto a compatibilidade com outros sistemas não é garantida.
</p>

# Network:

### Introdução:

<p style="font-family:arial; font-size:10pt">
Os arquivos localizados em src/network são os arquivos que abstraem a implementação de uma estrutura de servidor e cliente para comunicação sobre a network utilizando o protocolo TCP/IP. Além disso, também foi implementado estruturas de dados para programação assíncrona como threads e mutexes.
</p>
<p style="font-family:arial; font-size:10pt">
A biblioteca foi construida utilizando UNIX sockets e POSIX threads além da biblioteca padrão do C99.
</p>

### Servidor TCP:

<p style="font-family:arial; font-size:10pt">
Em src/network/tcp_server.c foi definida uma estrutura de dados e metodos que abstraem um servidor TCP. A abstração em questão utiliza a thread principal da aplicação para esperar por conexões, e quando uma solicitação de conexão é feita por um cliente, essa mesma thread é responsável por criar uma nova thread que irá ser responsável por manter a conexão com o cliente ativa, qualquer mensagem enviada pelo cliente portanto será recebida e tratada dentro dessa thread.
</p>

<p style="font-family:arial; font-size:10pt">
Em caso de aplicações em que a ordem que o servidor irá processar as requisições de diferentes clientes importa, é possivel instanciar o servidor tcp com a flag **TCP_SERVER_SYNC**, essa flag fará com o que servidor execute a primeira requisição que chegar antes que a thread de um outro cliente com requisição pendente comece sua execução.
</p>

<p style="font-family:arial; font-size:10pt">
O comportamento do servidor é definido utilizando-se o método:
</p>

    void tcp_server_t_set_request_handler(struct tcp_server_t* server, tcp_server_t_request_handler handler);

<p style="font-family:arial; font-size:10pt">
Este método irá definir como o servidor deverá processar uma mensagem qualquer recebida de algum cliente. Uma mensagem so é repassada ao handler quando o servidor detectar um '\n', se essa condição não ocorrerm o server continuará esperando uma nova mensagem que será tratada como a continuação da mensagem anterior.
</p>

<p style="font-family:arial; font-size:10pt">
Estes metodos precisam seguir a interface:
</p>

    void(*)(struct request_t, struct reply_t);

<p style="font-family:arial; font-size:10pt">
As estruturas request_t e reply_t são estruturas que armazenam informações uteis como o cliente que fez a requisição, o servidor que está processando a requisição, a mensagem de payload e o tamanho da mensagem enviada pelo cliente.
</p>


### Cliente TCP:
<p style="font-family:arial; font-size:10pt">
Foi implementado em src/network/tcp_client.c uma estrutura que abstrai a funcionalidade de um client TCP.
</p>

<p style="font-family:arial; font-size:10pt">
No cliente não foi necessário nenhuma thread extra, foi escolhido a implementação de um cliente que utiliza *'non blocking sockets'*. Essa escolha foi tomada para permitir que o programador utilizando o cliente possa fazer o uso dos recursos computacionais da forma que ele achar melhor baseado na aplicação sendo desenvolvida.
</p>

### Detalhes de implementação:

<p style="font-family:arial; font-size:10pt">
Para a implementação das estruturas do servidor, foram utilizadas estruturas de dados assincronas, como threads e mutexes, ambas essas estruturas foram abstraidas utilizando a biblioteca pthreads em src/network/async.c.
</p>

<p style="font-family:arial; font-size:10pt">
Foi feito o uso de UNIX sockets para a implementação do servidor e cliente assim como outras classes da biblioteca padrão como strings, stdlib e stdio. No caso do cliente foi utilizado um *'non blocking socket'*  atualizando suas flags para incluir a flag ***O_NONBLOCK*** após sua inicialização.
</p>

<p style="font-family:arial; font-size:10pt">
O uso de threads foi necessário para garantir que um servidor possa servir a mais de um cliente por vez, portanto para cada cliente o servidor cria uma thread unica que será responsável por escutar as mensagens enviadas por esse cliente.
</p>

<p style="font-family:arial; font-size:10pt">
Para armazenar as conexoes ativas com o servidor, foi utilizado uma estrutura de lista encadeada que pode ser utilizada simultaneamente por multiplas threads, sincronizada através do uso de mutexes, onde cada nó armazena as informações references à uma conexão/cliente.
</p>

# Aplicação Publish/Subscribe:

<p style="font-family:arial; font-size:10pt">
Toda a lógica relacionada à aplicação de Publish/Subscribe está definida sobre os arquivos *.c e *.h dentro do diretório src/.
</p>
<p style="font-family:arial; font-size:10pt">
A aplicação solicitada define quatro ações de usuário:
</p>

1. Cadastrar o cliente em uma tag enviando uma mengagem iniciada com o caractere '+'.
2. Descadastar o cliente de uma tag enviando uma mengagem iniciada com o caractere '-'.
3. Encerrar a execução do servidor e todas as suas conexoes enviando uma mensagem "##kill".
4. Enviar uma mensagem de texto para o servidor que irá redirecionar a mesma mensagem para qualquer cliente que esteja interessado utilizando a sintaxe *"mensagem #tag"*. Essa ação pode ser realizada enviando uma mensagem que não se encaixa em nenhuma das condições das ações anteriores.

### Observaçõs:

<p style="font-family:arial; font-size:10pt">
A descrição do trabalho prático define:
</p>

    Servidores e clientes trocam mensagens curtas de até 500 bytes usando o protocolo TCP.  Mensagens carregam texto codificado segundo a tabela ASCII. Apenas letras, números, os caracteres de pontuação ,.?!:;+-*/=@#$%()[]{} e espaços podem ser transmitidos. (Caracteres acentuados não podem ser transmitidos.)

<p style="font-family:arial; font-size:10pt">
A segunda sentença define que mensagens dever ser codificadas de acordo com a tabela ASCII. A partir disso, entendi que o range de characteres deveria estar entre (48, 57) que abrage os numeros de 0 a 9, (65, 90) que abrange os caracteres maiusculos, (97,122) que abrange os caracteres minusculos e o conjunto de caracteres ,.?!:;+-*/=@#$%()[]{}. Se uma mensagem for enviada e não obedecer esses parametros o servidor irá desconectar o cliente e o cliente abortará sua execução.
</p>

<p style="font-family:arial; font-size:10pt">
Outra definição que a descrição do trabalho define é:
</p>

    1) As mensagens de interesse (+tag) e desinteresse (-tag) devem ter o sinal (+ ou -) no primeiro caractere e apenas um tag, sem nenhum texto adicional.

    2) Tags e especificações de interesse e desinteresse devem estar precedidas e sucedidas por espaço, início da mensagem, ou término da mensagem. Em outras palavras, um string como “#dota#overwatch” não será considerado como tags.

<p style="font-family:arial; font-size:10pt">
A primeira sentença define que mensagens de interesse e desinteresse devem conter (+|-) no primeiro caractere. Porém, a segunda sentença diz que as especificações de interesse e desinteresse devem estar precedidas e sucedidas por expaço, ou seja, o primeiro caractere poderia ser um espaço seguido de um (+|-), portanto temos duas declarações conflitantes. Foi escolhido a primeira sobre a segunda por ser mais especifica.
</p>

<p style="font-family:arial; font-size:10pt">
Por consequencia, a segunda sentença também define que tags de broadcast (geralmente iniciadas com #) precisam estar entre espaços ou se encontrar após início de mensagem(entendido como pelo menos uma sequencia de caracteres de tamanho maior que 1) ou termino da mensagem. Sendo assim, as seguintes mensagems seriam:
</p>

1. "#dota#overwatch" - Não seria uma tag válida por não inicial em um espaço ou palavra.
2. " #dota#overwatch" - Não seria uma tag válida pois apesar de de iniciar em espaços, ela não termina em um (*'devem estar precedidas e sucedidas por espaço'*). O mesmo raciocínio serve para "#dota#overwatch ".
3. "perdeu eh culpa do suporte #dota #overwatch" - #dota seria uma tag válida pois é cercada por espaços, porém #overwatch não seria já que não é terminado em espaços.

<p style="font-family:arial; font-size:10pt">
Para melhor experiencia de usuário, foi optado uma gramatica para o protocolo que segue:
</p>

S -> (M*( \#'P)\*)\* | +P | -P | P;

Q -> , | . | ? | ! | : | ; | + | - | * | / | = | @ | # | $ | $ | % | ( | ) | { | };

L -> [a-z] | [A-z] | [0-9];

P -> L* | QL | LPL | LP;

M -> P( P)*;

<p style="font-family:arial; font-size:10pt">
Onde S equivale ao Protocolo, Q aos possiveis sinais, L as possiveis letras e P a regra de formação de palavras e M a regra para formação de frases.
</p>


### O Servidor:

<p style="font-family:arial; font-size:10pt">
O servidor é responsável por tratar as requisições feitas pelos clientes. Foi definido um handler, adicionado ao servidor através do método: 
</p>

    void tcp_server_t_set_request_handler(struct tcp_server_t* server, tcp_server_t_request_handler handler);

<p style="font-family:arial; font-size:10pt">
Por sua vez, esse handler verifica a mensagem enviada pelo cliente e, baseado nas quatro condições descritas acima, decide alguma ação que será executado sobre a mensagem.
</p>

<p style="font-family:arial; font-size:10pt">
O servidor utiliza apenas duas estruturas de dados auxiliares, um mapa do tipo 'string'->'lista de inteiros' que irá armazenar em qual canal('string') um determinado grupo de clientes('lista de inteiros') estão interessados, chamaremos essa estrutura de tabela de canais, e uma estrutura de tabela hash que armazenará inteiros e que será utilizada para armazenar quais os clientes que já receberam uma dada mensagem para evitar que clientes cadastrados em dois canais diferentes recebam uma mensagem duplicada em caso de essa mesma mensagem ter sido enviada para ambos os canais.
</p>

### O Cliente:
<p style="font-family:arial; font-size:10pt">
O cliente é responsável por 2 coisas:
</p>
    
1. Receber a entrada do usuário 
2. Enviar a entrada do usuário para o servidor.
3. Receber as mensagens do servidor

<p style="font-family:arial; font-size:10pt">
Para receber a entrada do usuário, foi implementada em src/terminal.h algumas funções uteis no manejo de entradas de usuário pelo terminal, como funções não bloqueantes que verificam se existem alguma entrada pendente. A partir disso, a lógica do cliente passa a ser bastante simples, basta testar se existe entrada pendente do usuário, caso exista, a mensagem é enviada ao servidor.
</p>

<p style="font-family:arial; font-size:10pt">
Em caso de nenhuma mensagem pendente, o cliente tenta receber mensagens do servidor utilizando o método:
</p>

    int tcp_client_t_receive(struct tcp_client_t* client, char* message, int length);

<p style="font-family:arial; font-size:10pt">
Este método retorna -1 quando não existem mensagems pendentes e 1 quando existe pelo menos uma mensagem pendente do servidor. No segundo caso, é salva no 2º parametro chamado *message* uma mensagem com tamanho igual ao indicado pelo 3º parametro *length*.
</p>

### Conclusão:

<p style="font-family:arial; font-size:10pt">
Durante a implementação desse Trabalho foi implementado um Servidor e Cliente que utilizam o protocolo TCP/IP, o que possibilitou entender com mais detalhes o funcionamento deste Protocolo.
</p>

<p style="font-family:arial; font-size:10pt">
Também foi implementado uma aplicação por cima do servidor e cliente TCP abstraidos dentro de src/network que se resume a uma aplicação Publish/Subscribe. Durante a implementação desta aplicação foi possível o melhor entendimento das aplicações que utilizam comunicação sobre a netowrk, e em especial, foi possivel obter "insights" sobre o funcionamento de sistemas como Apache Kafka e RabbitMQ.
</p>
