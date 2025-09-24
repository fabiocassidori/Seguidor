/*
 * Display.c
 *
 *  Created on: Sep 24, 2025
 *      Author: jlour
 */

#include "display.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <string.h> // Necessário para as funções strlen() e strcpy()
#include <stdio.h>  // Necessário para a função sprintf()

// Mapeia o enum para as fontes reais da biblioteca
static SSD1306_Font_t get_font(TamanhoFonte font) {
    switch (font) {
        case FONTE_PEQUENA:
            return Font_6x8;
        case FONTE_MEDIA:
            return Font_7x10;
        case FONTE_GRANDE:
            return Font_11x18;
        default:
            return Font_7x10;
    }
}

void inicializarDisplay(void){
	ssd1306_Init();
}

void limparDisplay(void) {
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
}

void mostrarMensagemUnica(char* mensagem){
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString(mensagem, Font_7x10, White);
	ssd1306_UpdateScreen();
}

void mostrarMensagemComTitulo(char* titulo, char* mensagem, uint32_t delay_ms) {
    ssd1306_Fill(Black);

    // Escreve o título centralizado
    ssd1306_SetCursor((128 - strlen(titulo) * 11) / 2, 5); // Centraliza fonte 11x18
    ssd1306_WriteString(titulo, get_font(FONTE_MEDIA), White);

    // Escreve a mensagem abaixo
    ssd1306_SetCursor(0, 30);
    ssd1306_WriteString(mensagem, get_font(FONTE_MEDIA), White);

    ssd1306_UpdateScreen();

    if (delay_ms > 0) {
        HAL_Delay(delay_ms);
    }
}

void mostraMensagemNaPosicaoAt(char* texto, uint8_t x, uint8_t y, TamanhoFonte fonte) {
    ssd1306_SetCursor(x, y);
    ssd1306_WriteString(texto, get_font(fonte), White);
    ssd1306_UpdateScreen(); // Atualiza a tela após escrever
}

/**
 * @brief Mostra uma mensagem longa, quebrando em linhas e páginas automaticamente.
 * - Limite de 19 caracteres por linha (baseado na sua observação).
 * - Limite de 4 linhas de mensagem por página.
 * - Pausa de 2 segundos entre as páginas.
 * @param titulo Título a ser exibido no topo de cada página.
 * @param mensagem A string longa a ser exibida.
 */
void mostrarMensagemPaginada(char* titulo, char* mensagem, uint32_t delay_ms) {
    // --- Constantes de layout ---
    const int chars_por_linha = 19;     // Máximo de caracteres por linha com FONTE_MEDIA
    const int linhas_por_pagina = 5;    // Máximo de linhas de mensagem por tela
    const int altura_linha = 12;        // Altura da fonte 7x10 + espaçamento
    const int y_inicial = 0;           // Posição Y onde a primeira linha da mensagem começa

    const char* ptr_texto = mensagem;   // Ponteiro para a posição atual na string da mensagem
    char buffer_linha[chars_por_linha + 1]; // Buffer para armazenar a string de uma linha (+1 para '\0')

    // Loop principal: continua enquanto houver texto para mostrar
    while (*ptr_texto) {
        // 1. Prepara uma nova página
        ssd1306_Fill(Black);

        // Escreve o título (usando fonte grande, como na sua função original)
        ssd1306_SetCursor((128 - strlen(titulo) * 11) / 2, 5);
        ssd1306_WriteString(titulo, get_font(FONTE_MEDIA), White);

        // 2. Preenche a página com até 4 linhas
        for (int i = 0; i < linhas_por_pagina; ++i) {
            // Se o texto acabou, para de preencher a página
            if (!*ptr_texto) {
                break;
            }

            // Copia o segmento de texto para a linha atual
            strncpy(buffer_linha, ptr_texto, chars_por_linha);
            buffer_linha[chars_por_linha] = '\0'; // Garante que a string termine corretamente

            // Encontra o tamanho real do segmento que será escrito
            size_t tamanho_segmento = strlen(buffer_linha);

            // Se o texto original for maior, verifica se não cortamos uma palavra no meio
            if (strlen(ptr_texto) > chars_por_linha) {
                 // Esta é uma lógica simples, pode ser aprimorada para quebrar palavras
                 // de forma mais inteligente se necessário.
            }

            // Escreve a linha no display
            ssd1306_SetCursor(0, y_inicial + (i * altura_linha));
            ssd1306_WriteString(buffer_linha, get_font(FONTE_MEDIA), White);

            // Avança o ponteiro principal pelo tanto que foi copiado
            ptr_texto += tamanho_segmento;
        }

        // 3. Atualiza a tela
        ssd1306_UpdateScreen();

        // 4. Se ainda houver texto, espera 2 segundos antes de mostrar a próxima página
        if (*ptr_texto) {
            HAL_Delay(delay_ms);
        }
    }
}
