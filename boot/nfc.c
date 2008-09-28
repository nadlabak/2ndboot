#include "nfc.h"
#include "common.h"
#include "mxc91231.h"

#define NFC_ADDR_REG_ADDR (NFC_BASE_ADDR + 0xe06)
#define NFC_CMD_REG_ADDR (NFC_BASE_ADDR + 0xe08)
#define NFC_CFG_REG_ADDR (NFC_BASE_ADDR + 0xe0a)
#define NFC_WP_REG_ADDR (NFC_BASE_ADDR + 0xe12)
#define NFC_CFG1_REG_ADDR (NFC_BASE_ADDR + 0xe1a)
#define NFC_CFG2_REG_ADDR (NFC_BASE_ADDR + 0xe1c)

static void nfc_enable_clock() {
  modify_register32(CRM_AP_BASE_ADDR+0xc, 0, 1 << 20);
}

static void nfc_preset() {
  write16(0x1, NFC_CFG_REG_ADDR); // Lock RAMbuffer.
  write16(0x2, NFC_WP_REG_ADDR); // Lock all NAND blocks.
  write16(0x10, NFC_CFG1_REG_ADDR); // Mask inerrupt.
}

int nfc_wait() {
  while ((read16(NFC_CFG2_REG_ADDR) & (1 << 15)) == 0) {
  }
  modify_register16(NFC_CFG2_REG_ADDR, 0x80, 0);
  return 0;
}

int nfc_send_command(uint16_t cmd) {
  write16(cmd, NFC_CMD_REG_ADDR);
  write16(0x1, NFC_CFG2_REG_ADDR);
  nfc_wait();
  return 0;
}

int nfc_send_address(uint16_t addr) {
  write16(addr, NFC_ADDR_REG_ADDR);
  write16(0x2, NFC_CFG2_REG_ADDR);
  nfc_wait();
  return 0;
}

int nfc_reset() {
  nfc_preset();
  return nfc_send_command(0xff);
}

int nfc_init() {
  nfc_enable_clock();
  return nfc_reset();
}
