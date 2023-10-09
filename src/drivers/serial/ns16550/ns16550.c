/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    12.10.2012
 */
#include <assert.h>
#include <stdint.h>

#include <drivers/common/memory.h>
#include <drivers/serial/diag_serial.h>
#include <drivers/serial/uart_dev.h>
#include <framework/mod/options.h>
#include <hal/reg.h>

#define REG_WIDTH    OPTION_GET(NUMBER, reg_width)
#define MEM32_ACCESS OPTION_GET(BOOLEAN, mem32_access)

/**
 * General Register Set
 * 
 * (R) - read only
 * (W) - write only
 * (R/W) - read/write
 */
#define UART_RHR(base) (base + REG_WIDTH * 0) /* (R) Receiver Holding Reg */
#define UART_THR(base) (base + REG_WIDTH * 0) /* (W) Transmitter Holding Reg */
#define UART_IER(base) (base + REG_WIDTH * 1) /* (R/W) Interrupt Enable Reg */
#define UART_ISR(base) (base + REG_WIDTH * 2) /* (R) Interrupt Status Reg */
#define UART_FCR(base) (base + REG_WIDTH * 2) /* (W) FIFO Control Reg */
#define UART_LCR(base) (base + REG_WIDTH * 3) /* (R/W) Line Control Reg */
#define UART_MCR(base) (base + REG_WIDTH * 4) /* (R/W) Modem Control Reg */
#define UART_LSR(base) (base + REG_WIDTH * 5) /* (R) Line Status Reg */
#define UART_MSR(base) (base + REG_WIDTH * 6) /* (R) Modem Status Reg */
#define UART_SPR(base) (base + REG_WIDTH * 7) /* (R/W) Scratch Pad Reg */

#define UART_IER_DR   (1U << 0) /* Data Ready */
#define UART_IER_THRE (1U << 1) /* Transmit-hold-register empty */
#define UART_IER_RLS  (1U << 2) /* Receiver Line Status */
#define UART_IER_MS   (1U << 3) /* Modem Status */
#define UART_IER_TE   (1U << 6) /* DMA Rx End */
#define UART_IER_RE   (1U << 7) /* DMA Tx End */

#define UART_LSR_DR   (1U << 0) /* Data Ready */
#define UART_LSR_OE   (1U << 1) /* Overrun Error */
#define UART_LSR_PE   (1U << 2) /* Parity Error */
#define UART_LSR_FE   (1U << 3) /* Framing Error */
#define UART_LSR_BI   (1U << 4) /* Break Interrupt */
#define UART_LSR_THRE (1U << 5) /* Transmit-hold-register empty */
#define UART_LSR_TE   (1U << 6) /* Transmitter empty */
#define UART_LSR_FDE  (1U << 7) /* FIFO data Error */

/**
 * Registers accesible only when DLAB = 1
 * 
 * (R) - read only
 * (W) - write only
 * (R/W) - read/write
 */
/* (R/W) Baudrate Divisor’s Constant Least Significant Byte */
#define UART_DLL(base) (base + REG_WIDTH * 0)
/* (R/W) Baudrate Divisor’s Constant Most Significant Byte */
#define UART_DLM(base) (base + REG_WIDTH * 1)
#define UART_PSD(base) (base + REG_WIDTH * 5) /* (W) Prescaler Division */

#if MEM32_ACCESS
static_assert(REG_WIDTH == 4, "");

#define UART_REG_LOAD(addr)        REG32_LOAD(addr)
#define UART_REG_STORE(addr, val)  REG32_STORE(addr, val)
#define UART_REG_ORIN(addr, mask)  REG32_ORIN(addr, mask)
#define UART_REG_CLEAR(addr, mask) REG32_CLEAR(addr, mask)
#define UART_REG_ANDIN(addr, mask) REG32_ANDIN(addr, mask)
#else
static_assert(REG_WIDTH > 0, "");

#define UART_REG_LOAD(addr)        REG8_LOAD(addr)
#define UART_REG_STORE(addr, val)  REG8_STORE(addr, val)
#define UART_REG_ORIN(addr, mask)  REG8_ORIN(addr, mask)
#define UART_REG_CLEAR(addr, mask) REG8_CLEAR(addr, mask)
#define UART_REG_ANDIN(addr, mask) REG8_ANDIN(addr, mask)
#endif

static int ns16550_setup(struct uart *dev, const struct uart_params *params) {
	if (params->uart_param_flags & UART_PARAM_FLAGS_USE_IRQ) {
		UART_REG_ORIN(UART_IER(dev->base_addr), UART_IER_DR);
	}

	return 0;
}

static int ns16550_irq_en(struct uart *dev, const struct uart_params *params) {
	UART_REG_ORIN(UART_IER(dev->base_addr), UART_IER_DR);

	return 0;
}

static int ns16550_irq_dis(struct uart *dev, const struct uart_params *params) {
	UART_REG_CLEAR(UART_IER(dev->base_addr), UART_IER_DR);

	return 0;
}

static int ns16550_putc(struct uart *dev, int ch) {
	while (!(UART_REG_LOAD(UART_LSR(dev->base_addr)) & UART_LSR_THRE)) {}

	UART_REG_STORE(UART_THR(dev->base_addr), ch);

	return 0;
}

static int ns16550_getc(struct uart *dev) {
	return UART_REG_LOAD(UART_RHR(dev->base_addr));
}

static int ns16550_has_symbol(struct uart *dev) {
	return UART_REG_LOAD(UART_LSR(dev->base_addr)) & UART_LSR_DR;
}

const struct uart_ops ns16550_uart_ops = {
    .uart_getc = ns16550_getc,
    .uart_putc = ns16550_putc,
    .uart_hasrx = ns16550_has_symbol,
    .uart_setup = ns16550_setup,
    .uart_irq_en = ns16550_irq_en,
    .uart_irq_dis = ns16550_irq_dis,
};
