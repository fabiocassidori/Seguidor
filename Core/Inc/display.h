/*
 * display.h
 *
 *  Created on: Sep 24, 2025
 *      Author: jlour
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "stm32f4xx_hal.h"


// Enum para fontes, facilitando a escolha
typedef enum {
    FONTE_PEQUENA = 0, 	// Font_7x10
    FONTE_MEDIA,    	// Font_11x18
    FONTE_GRANDE    	// Font_16x26
} TamanhoFonte;


void inicializarDisplay(void);
void limparDisplay(void);

void mostrarMensagemUnica(char* mensagem);
/**
 * @brief Mostra na tela um título e mensagem. Ideal para debug.
 * @param title Título a ser exibido na primeira linha (fonte média).
 * @param message Mensagem a ser exibida abaixo do título (fonte pequena).
 * @param delay_ms Tempo em milissegundos que a mensagem ficará na tela.
 */
void mostrarMensagemComTitulo(char* titulo, char* mensagem, uint32_t delay_ms);

/**
 * @brief Escreve um texto simples em uma posição específica.
 * @param text Texto a ser escrito.
 * @param x Posição X inicial.
 * @param y Posição Y inicial.
 * @param font Tamanho da fonte (FONTE_PEQUENA, FONTE_MEDIA, FONTE_GRANDE).
 */
void mostraMensagemNaPosicaoAt(char* texto, uint8_t x, uint8_t y, TamanhoFonte font);

/**
 * @brief Mostra uma mensagem longa, quebrando em linhas e páginas automaticamente.
 * @param titulo Título a ser exibido no topo de cada página.
 * @param mensagem A string longa a ser exibida.
 * @param delay_ms Tempo em milissegundos que a mensagem ficará em cada página.
 */
void mostrarMensagemPaginada(char* titulo, char* mensagem, uint32_t delay_ms);

#endif /* INC_DISPLAY_H_ */
