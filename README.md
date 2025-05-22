# Terminal Invaders

## Descrição

Terminal Invaders é um jogo de tiro inspirado em Space Invaders, executado no terminal. O jogador controla uma "nave" (
`^`
) posicionada na parte inferior da tela e deve eliminar todos os invasores (`W`) antes que eles alcancem sua linha. O jogo roda a 60 FPS e oferece três níveis de dificuldade, ajustando apenas a velocidade de movimento dos invasores.

## Funcionalidades

* Menu inicial com seleção de dificuldade (Fácil, Médio, Difícil)
* Movimento da nave com as teclas `A` (esquerda) e `D` (direita)
* Disparo de projéteis com a barra de espaço (` `)
* Sistema de colisão entre projéteis e invasores
* Contador de pontuação para cada invasor abatido
* Armazenamento de top scores em arquivo de texto (`scores.txt`)
* Tela de vitória ou game over com exibição da pontuação final

## Conceitos e Requisitos

Este projeto atende aos seguintes requisitos e conceitos:

* Uso de **structs** para representar invasores e balas
* Manipulação de **ponteiros** e **alocação dinâmica de memória** (`malloc`/`free`) para lista de balas
* Implementação de **lista encadeada** para gerenciar balas ativas
* Uso de **matrizes** para posicionamento dos invasores (array bidimensional)
* **Leitura e escrita em arquivo** para armazenar e exibir os top scores

### Dependências

* Compilador C compatível (GCC recomendado)
* Headers:

  * `keyboard.h` (gerenciamento de entrada sem bloquear)
  * `screen.h` (controle de cursor e atualização da tela)
  * `timer.h` (controle de temporização)

## Compilação e Execução

1. Clone este repositório:

   ```bash
   git clone https://github.com/Artur-Antunes-1/Terminal_Invaders.git
   cd TerminalInvaders
   ```
2. Compile usando `make`:

   ```bash
   make
   ```
3. Execute o jogo:

   ```bash
   ./terminal_invaders
   ```

## Estrutura de Arquivos

```
TerminalInvaders/
├── include/
│   ├── keyboard.h
│   ├── screen.h
│   └── timer.h
├── src/
│   ├── main.c
│   └── ... (implementações auxiliares)
├── Makefile
├── scores.txt        # Armazena scores em formato "nome pontuação"
└── README.md         # Este documento
```

## Contato

* **Autor**: Artur Antunes
* **Email**: [aas4@cesar.school](mailto:aas4@cesar.school)
