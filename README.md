# projeto_psis

Este projeto consiste no desenvolvimento de uma área de transferência de dados num sistema distribuído de servidores, onde os dados são partilhados por todos os servidores. Os dois constituintes principais do processo são os servidores de armazenamento de dados numa estrutura de dados com 10 regiões e os clientes que têm a possibilidade de interagir com o servidor local.

Os clientes têm a possibilidade de realizar 3 ações:
- Copiar: copiar dados para uma região de armazenamento do servidor.
- Colar: colar os dados de uma região.
- Esperar: esperar a alteração dos dados de uma região e recebê-los.

Para correr o programa, na pasta corre-se o comando:

```make```

para gerar os ficheiros executáveis. De seguida, inicia-se um servidor em modo singular, através de:

```./clipboard```


Pode-se correr uma aplicação, abre-se um novo terminal na mesma diretoria, e executa-se a aplicação através de:

```./app```


Para iniciar um servidor em modo conectado, tem-se as 2 opções:

1. Numa máquina distinta, no terminal, gerar os executáveis e correr o comando:

```./clipboard -c <IP_address> <port>```

em que, <ip_address> corresponde ao endereço IP do servidor ao qual nos queremos conectar e <port> é o porto no qual esse servidor está a receber as ligações, que foi a primeira linha impressa no terminal, quando se iniciou o servidor.

2. Na mesma máquina, noutro terminal, na diretoria do projeto, correr o comando:

```./clipboard -c 127.0.0.1 <port>```

em que <port> é o porto no qual o servidor ao qual nos queremos conectar está à espera de receber ligações, que foi a primeira linha impressa no terminal, quando se iniciou o servidor

A partir daí pode-se ligar outra aplicação ao novo servidor e começar a interagir com a área de transferência distribuída.
