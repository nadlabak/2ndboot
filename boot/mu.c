#include "mxc91231.h"
#include "error.h"
#include "common.h"
#include "gpt.h"

#define MU_MSR_ADDR (MU_BASE_ADDR+0x20)
#define MU_MCR_ADDR (MU_BASE_ADDR+0x24)

#define DSP_RUN_TRIES 100

static void mu_nmi(void) {
  modify_register32(MU_MCR_ADDR, 0, 1 << 3);
}

static int mu_dsp_power_state(void) {
  return (read32(MU_MSR_ADDR) >> 5) & 0x3;
}

static void mu_dsp_reset_wait() {
  while (read32(MU_MSR_ADDR) & (1 << 7)) {
  }
}

static void mu_assert_dsp_running() {
  int cnt = DSP_RUN_TRIES;
  if (mu_dsp_power_state() != 0) {
    mu_nmi();
  }
  while (cnt--) {
    if (mu_dsp_power_state() == 0) {
      return;
    }
    gpt_wait(1);
  }
  critical_error(DSP_IS_NOT_RUNNING);
}

int mu_reset(void) {
  mu_assert_dsp_running();
  modify_register32(MU_MCR_ADDR, 0, 1 << 5);
  mu_dsp_reset_wait();
  return 0;
}

int mu_dsp_reset(void) {
  mu_assert_dsp_running();
  modify_register32(MU_MCR_ADDR, 0, 1 << 4);
  modify_register32(MU_MCR_ADDR, 1 << 4, 0);
  mu_dsp_reset_wait();
  printf("DSP reset completed\n");
  return 0;
}

