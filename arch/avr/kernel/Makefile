obj-n += dummy.o
obj-y = spin.o
obj-$(CONFIG_EXT_MEM) += mem.o
obj-$(CONFIG_DELAY_US) += delay.o
obj-$(CONFIG_ATMEGA_ANALOG:m=y) += adc.o
obj-$(CONFIG_I2C_ATMEGA:m=y) += i2c.o
obj-$(CONFIG_ATMEGA_SPI:m=y) += spi.o
obj-$(CONFIG_ATMEGA_USART:m=y) += usart.o
obj-$(CONFIG_SIMUL_AVR) += simulavr.o
obj-$(CONFIG_IRQ_SUPPORT) += irq.o irq-entry.o
obj-$(CONFIG_TIMER) += timer.o timer-irq.o
obj-$(CONFIG_SCHED) += sched.o context.o asm-sched.o
obj-$(CONFIG_ARCH_POWER_SAVE) += power.o
obj-$(CONFIG_HRTIMER) += hrtimer.o
obj-y += spinlock.o cpu.o
extra-y += start.o devinit.o init.o
