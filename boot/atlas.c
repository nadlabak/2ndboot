#include "cspi.h"
#include "types.h"
#include "atlas.h"

#define ATLAS_CSPI_MODULE 1

int atlas_init() {
  struct cspi_config cfg;
  cfg.chip_select = 2;
  cfg.data_rate = 4200000;
  cfg.bit_count = 32;
  cfg.active_high_ss_polarity = 1;
  cfg.ss_waveform = 1;
  cfg.phase = 0;
  cfg.active_high_clk_polarity = 1;
  return cspi_init(ATLAS_CSPI_MODULE, &cfg);
}  

int atlas_reg_io(int write, atlas_register reg, uint32_t *p) {
  uint32_t outdata, indata;

  outdata = (reg & 0x3f) << 0x19;
  if (write) {
    outdata |= *p & 0xffffff;
    outdata |= 1 << 31;
  }
  if (cspi_send(ATLAS_CSPI_MODULE, &outdata, 1) != 1) {
    return -1;
  }
  if (cspi_recv(ATLAS_CSPI_MODULE, &indata, 1) != 1) {
    return -2;
  }
  if ((indata >> 0x18) != 0) {
    return -3;
  }
  if (!write) {
    *p = indata & 0xffffff;
  }
  return 0;
}

int atlas_reg_read(atlas_register reg, uint32_t *v) {
  return atlas_reg_io(0, reg, v);
}

int atlas_reg_modify(atlas_register reg, uint32_t mask, uint32_t v) {
  uint32_t foo;
  int ret;

  mask &= 0xffffff;
  v &= 0xffffff;

  ret = atlas_reg_read(reg, &foo); // force read;
  if (ret != 0) {
    return ret;
  }
  foo = (foo & (~mask)) | v;
  return atlas_reg_io(1, reg, &foo);
}

int atlas_reg_write(atlas_register reg, uint32_t v) {
  return atlas_reg_modify(reg, 0, v);
}

int atlas_reg_partial(atlas_register reg, uint32_t mask, uint32_t data) {
  mask &= 0xffffff;
  data &= 0xffffff;
  return atlas_reg_modify(reg, ~mask, data);
}
