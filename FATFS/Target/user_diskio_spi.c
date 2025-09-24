#include "user_diskio_spi.h"
#include "main.h" // Onde hspi3 e SD_CS_Pin são definidos
#include <stdio.h>

// Referência externa ao handle do SPI3 gerado pelo CubeIDE em main.c
extern SPI_HandleTypeDef hspi3;

/* Definições de comandos do cartão SD */
#define CMD0    (0x40+0)    // GO_IDLE_STATE
#define CMD8    (0x40+8)    // SEND_IF_COND
#define CMD17   (0x40+17)   // READ_SINGLE_BLOCK
#define CMD24   (0x40+24)   // WRITE_BLOCK
#define ACMD41  (0xC0+41)   // SD_SEND_OP_COND (ACMD)
#define CMD55   (0x40+55)   // APP_CMD
#define CMD58   (0x40+58)   // READ_OCR

static volatile DSTATUS Stat = STA_NOINIT;
static BYTE CardType;

// Funções auxiliares de baixo nível
static void SD_CS_Select() {
    HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);
}

static void SD_CS_Deselect() {
    HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);
}

static uint8_t SPI_TxRx(uint8_t data) {
    uint8_t rx_data = 0;
    HAL_SPI_TransmitReceive(&hspi3, &data, &rx_data, 1, HAL_MAX_DELAY);
    return rx_data;
}

static void SPI_Tx(uint8_t data) {
    SPI_TxRx(data);
}

static uint8_t SPI_Rx() {
    return SPI_TxRx(0xFF);
}

// Aguarda o cartão ficar pronto
static uint8_t wait_ready(void) {
    uint8_t res;
    // Timeout de 500ms
    for (int i = 0; i < 5000 && (res = SPI_Rx()) != 0xFF; i++) {
         HAL_Delay(1); // Espera
    }
    return res;
}

// Envia um comando para o cartão SD
static BYTE send_cmd(BYTE cmd, DWORD arg) {
    BYTE res;

    if (cmd & 0x80) { // Se for um ACMD
        cmd &= 0x7F;
        res = send_cmd(CMD55, 0);
        if (res > 1) return res;
    }

    // Espera o cartão ficar pronto
    wait_ready();

    // Envia o pacote de comando
    SPI_Tx(cmd);
    SPI_Tx((BYTE)(arg >> 24));
    SPI_Tx((BYTE)(arg >> 16));
    SPI_Tx((BYTE)(arg >> 8));
    SPI_Tx((BYTE)arg);

    // CRC (dummy para SPI)
    BYTE crc = 0x01;
    if (cmd == CMD0) crc = 0x95;
    if (cmd == CMD8) crc = 0x87;
    SPI_Tx(crc);

    // Recebe a resposta
    for (int n = 10; n; n--) {
        res = SPI_Rx();
        if (!(res & 0x80)) break;
    }

    return res;
}

// Implementação das funções do driver
DSTATUS USER_SPI_initialize(BYTE pdrv) {
    BYTE ty, ocr[4];

    if (pdrv != 0) return STA_NOINIT;
    if (Stat & STA_NODISK) return Stat;

    // Envia 80 pulsos de clock com CS alto
    SD_CS_Deselect();
    for(int i = 0; i < 10; i++) SPI_Tx(0xFF);

    SD_CS_Select();
    ty = 0;
    if (send_cmd(CMD0, 0) == 1) { // Entra em modo Idle
        if (send_cmd(CMD8, 0x1AA) == 1) { // SDv2
            for (int i = 0; i < 4; i++) ocr[i] = SPI_Rx();
            if (ocr[2] == 0x01 && ocr[3] == 0xAA) { // Tensão compatível
                while(send_cmd(ACMD41, 1UL << 30)); // Espera sair do idle
                if (send_cmd(CMD58, 0) == 0) { // Checa o status do cartão
                    for (int i = 0; i < 4; i++) ocr[i] = SPI_Rx();
                    ty = (ocr[0] & 0x40) ? 6 : 2; // SDHC/XC ou SDSC
                }
            }
        }
    }
    CardType = ty;
    SD_CS_Deselect();

    Stat &= ~STA_NOINIT;
    return Stat;
}

DSTATUS USER_SPI_status(BYTE pdrv) {
    return Stat;
}

DRESULT USER_SPI_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count) {
    if (pdrv != 0 || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    if (!(CardType & 4)) sector *= 512; // Converte para byte address se não for SDHC

    SD_CS_Select();
    while(count--) {
        if (send_cmd(CMD17, sector) != 0) return RES_ERROR;

        // Espera pelo token de início de dados
        while(SPI_Rx() != 0xFE);

        for (int i = 0; i < 512; i++) buff[i] = SPI_Rx();

        // Ignora o CRC
        SPI_Rx();
        SPI_Rx();
        sector++;
    }
    SD_CS_Deselect();

    return RES_OK;
}

DRESULT USER_SPI_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count) {
     if (pdrv != 0 || !count) return RES_PARERR;
     if (Stat & STA_NOINIT) return RES_NOTRDY;
     if (!(CardType & 4)) sector *= 512;

     SD_CS_Select();
     while(count--) {
         if (send_cmd(CMD24, sector) != 0) return RES_ERROR;

         SPI_Tx(0xFE); // Token de início
         for (int i = 0; i < 512; i++) SPI_Tx(buff[i]);

         // Dummy CRC
         SPI_Tx(0xFF);
         SPI_Tx(0xFF);

         if ((SPI_Rx() & 0x1F) != 0x05) return RES_ERROR; // Verifica se os dados foram aceitos
         if (wait_ready() != 0xFF) return RES_ERROR; // Espera o fim da escrita

         sector++;
     }
     SD_CS_Deselect();

     return RES_OK;
}

DRESULT USER_SPI_ioctl(BYTE pdrv, BYTE cmd, void* buff) {
     return RES_OK; // Pode ser implementado se necessário
}
