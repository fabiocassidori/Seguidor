/*
 * sd.c
 *
 * Created on: Sep 24, 2025
 * Author: jlour
 */

#include "sd.h"
#include <string.h> // Para strlen
#include "display.h"
#include <stdio.h> // Para usar sprintf

// As variáveis do FatFs são estáticas, ou seja, privadas a este arquivo.
static FATFS fs;
static FIL fil;

static uint32_t total_kb, free_kb;
char buffer[64];

void inicializaMontarCartao(void){
	if (montarCartaoSD() == HAL_OK) {
		mostrarMensagemComTitulo("CARTAO SD", "Montado com Sucesso!", 1500);
	} else {
		mostrarMensagemComTitulo("ERRO FATAL", "Falha ao montar o SD", 0); // 0 = para sempre
		while(1); // Trava o sistema
	}
}

void inicializaMostrarDetalhesCartao(void){
	if (mostraDetalhesCartaoSD(&total_kb, &free_kb) == HAL_OK) {
	  sprintf(buffer, "Total: %lu KB\nLivre: %lu KB", total_kb, free_kb);
	  mostrarMensagemComTitulo("Info Cartao", buffer, 3000);
	} else {
	  mostrarMensagemComTitulo("ERRO", "Nao foi possivel ler info", 2000);
	}
}

HAL_StatusTypeDef montarCartaoSD(void) {
    FRESULT fresult = f_mount(&fs, "", 1); // 1 = montar imediatamente
    if (fresult != FR_OK) {
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef desmontarCartaoSD(void) {
    FRESULT fresult = f_mount(NULL, "", 1);
    if (fresult != FR_OK) {
        return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef mostraDetalhesCartaoSD(uint32_t* total, uint32_t* free_space) {
    FATFS *pfs;
    DWORD fre_clust;
    FRESULT fresult = f_getfree("", &fre_clust, &pfs);

    if (fresult != FR_OK) {
        return HAL_ERROR;
    }

    *total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5); // KB
    *free_space = (uint32_t)(fre_clust * pfs->csize * 0.5); // KB

    return HAL_OK;
}

HAL_StatusTypeDef adicionaStringAoArquivo(const char* filename, const char* text) {
    // Abre o arquivo com a flag FA_OPEN_APPEND para adicionar ao final
    FRESULT fresult = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE | FA_CREATE_ALWAYS);
    if (fresult != FR_OK) {
        return HAL_ERROR;
    }

    // Escreve a string
    UINT bytes_written;
    fresult = f_write(&fil, text, strlen(text), &bytes_written);
    if (fresult != FR_OK) {
        f_close(&fil); // Fecha o arquivo mesmo em caso de erro
        return HAL_ERROR;
    }

    // Fecha o arquivo (muito importante para salvar os dados!)
    f_close(&fil);

    return HAL_OK;
}
