/*
 * sd.h
 *
 *  Created on: Sep 24, 2025
 *      Author: jlour
 */

#ifndef INC_SD_H_
#define INC_SD_H_

#include "fatfs.h"
#include <stdint.h>


void inicializaMontarCartao(void);
void inicializaMostrarDetalhesCartao(void);

/**
 * @brief Monta o sistema de arquivos do cartão SD.
 * @return HAL_OK se sucesso, HAL_ERROR se falha.
 */
HAL_StatusTypeDef montarCartaoSD(void);

/**
 * @brief Desmonta o sistema de arquivos do cartão SD.
 * @return HAL_OK se sucesso, HAL_ERROR se falha.
 */
HAL_StatusTypeDef desmontarCartaoSD(void);

/**
 * @brief Obtém o espaço total e livre do cartão SD.
 * @param total Ponteiro para armazenar o espaço total em KB.
 * @param free Ponteiro para armazenar o espaço livre em KB.
 * @return HAL_OK se sucesso, HAL_ERROR se falha.
 */
HAL_StatusTypeDef mostraDetalhesCartaoSD(uint32_t* total, uint32_t* free_space);

/**
 * @brief Adiciona uma string ao final de um arquivo. Cria o arquivo se não existir.
 * @param filename Nome do arquivo (ex: "log.txt").
 * @param text String a ser adicionada.
 * @return HAL_OK se sucesso, HAL_ERROR se falha.
 */
HAL_StatusTypeDef adicionaStringAoArquivo(const char* filename, const char* text);

#endif /* INC_SD_H_ */
