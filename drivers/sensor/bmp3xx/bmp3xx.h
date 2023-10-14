/* Bosch BMP388 and BMP390 pressure sensors
 *
 * Copyright (c) 2020 Facebook, Inc. and its affiliates
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Datasheet:
 * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp388-ds001.pdf
 * https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp390-ds002.pdf
 */

#ifndef __BMP3XX_H
#define __BMP3XX_H

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/util.h>

#undef DT_DRV_COMPAT
#define DT_DRV_COMPAT bosch_bmp388
#define BMP3XX_BUS_SPI_BMP388 DT_ANY_INST_ON_BUS_STATUS_OKAY(spi)
#define BMP3XX_BUS_I2C_BMP388 DT_ANY_INST_ON_BUS_STATUS_OKAY(i2c)
#undef DT_DRV_COMPAT
#define DT_DRV_COMPAT bosch_bmp390
#define BMP3XX_BUS_SPI_BMP390 DT_ANY_INST_ON_BUS_STATUS_OKAY(spi)
#define BMP3XX_BUS_I2C_BMP390 DT_ANY_INST_ON_BUS_STATUS_OKAY(i2c)
#undef DT_DRV_COMPAT

union bmp3xx_bus {
#if BMP3XX_BUS_SPI_BMP388 || BMP3XX_BUS_SPI_BMP390
	struct spi_dt_spec spi;
#endif
#if BMP3XX_BUS_I2C_BMP388 || BMP3XX_BUS_I2C_BMP390
	struct i2c_dt_spec i2c;
#endif
};

typedef int (*bmp3xx_bus_check_fn)(const union bmp3xx_bus *bus);
typedef int (*bmp3xx_reg_read_fn)(const union bmp3xx_bus *bus,
				  uint8_t start, uint8_t *buf, int size);
typedef int (*bmp3xx_reg_write_fn)(const union bmp3xx_bus *bus,
				   uint8_t reg, uint8_t val);

struct bmp3xx_bus_io {
	bmp3xx_bus_check_fn check;
	bmp3xx_reg_read_fn read;
	bmp3xx_reg_write_fn write;
};

#if BMP3XX_BUS_SPI
#define BMP3XX_SPI_OPERATION (SPI_WORD_SET(8) | SPI_TRANSFER_MSB |	\
			      SPI_MODE_CPOL | SPI_MODE_CPHA)
extern const struct bmp3xx_bus_io bmp3xx_bus_io_spi;
#endif

#if BMP3XX_BUS_I2C
extern const struct bmp3xx_bus_io bmp3xx_bus_io_i2c;
#endif

/* registers */
#define BMP3XX_REG_CHIPID       0x00
#define BMP3XX_REG_ERR_REG      0x02
#define BMP3XX_REG_STATUS       0x03
#define BMP3XX_REG_DATA0        0x04
#define BMP3XX_REG_DATA1        0x05
#define BMP3XX_REG_DATA2        0x06
#define BMP3XX_REG_DATA3        0x07
#define BMP3XX_REG_DATA4        0x08
#define BMP3XX_REG_DATA5        0x09
#define BMP3XX_REG_SENSORTIME0  0x0C
#define BMP3XX_REG_SENSORTIME1  0x0D
#define BMP3XX_REG_SENSORTIME2  0x0E
#define BMP3XX_REG_SENSORTIME3  0x0F
#define BMP3XX_REG_EVENT        0x10
#define BMP3XX_REG_INT_STATUS   0x11
#define BMP3XX_REG_FIFO_LENGTH0 0x12
#define BMP3XX_REG_FIFO_LENGTH1 0x13
#define BMP3XX_REG_FIFO_DATA    0x14
#define BMP3XX_REG_FIFO_WTM0    0x15
#define BMP3XX_REG_FIFO_WTM1    0x16
#define BMP3XX_REG_FIFO_CONFIG1 0x17
#define BMP3XX_REG_FIFO_CONFIG2 0x18
#define BMP3XX_REG_INT_CTRL     0x19
#define BMP3XX_REG_IF_CONF      0x1A
#define BMP3XX_REG_PWR_CTRL     0x1B
#define BMP3XX_REG_OSR          0x1C
#define BMP3XX_REG_ODR          0x1D
#define BMP3XX_REG_CONFIG       0x1F
#define BMP3XX_REG_CALIB0       0x31
#define BMP3XX_REG_CMD          0x7E

/* BMP3XX_REG_CHIPID */
#define BMP3XX_CHIP_ID_BMP388 0x50
#define BMP3XX_CHIP_ID_BMP390 0x60

/* BMP3XX_REG_STATUS */
#define BMP3XX_STATUS_FATAL_ERR  BIT(0)
#define BMP3XX_STATUS_CMD_ERR    BIT(1)
#define BMP3XX_STATUS_CONF_ERR   BIT(2)
#define BMP3XX_STATUS_CMD_RDY    BIT(4)
#define BMP3XX_STATUS_DRDY_PRESS BIT(5)
#define BMP3XX_STATUS_DRDY_TEMP  BIT(6)

/* BMP3XX_REG_INT_CTRL */
#define BMP3XX_INT_CTRL_DRDY_EN_POS  6
#define BMP3XX_INT_CTRL_DRDY_EN_MASK BIT(6)

/* BMP3XX_REG_PWR_CTRL */
#define BMP3XX_PWR_CTRL_PRESS_EN    BIT(0)
#define BMP3XX_PWR_CTRL_TEMP_EN     BIT(1)
#define BMP3XX_PWR_CTRL_MODE_POS    4
#define BMP3XX_PWR_CTRL_MODE_MASK   (0x03 << BMP3XX_PWR_CTRL_MODE_POS)
#define BMP3XX_PWR_CTRL_MODE_SLEEP  (0x00 << BMP3XX_PWR_CTRL_MODE_POS)
#define BMP3XX_PWR_CTRL_MODE_FORCED (0x01 << BMP3XX_PWR_CTRL_MODE_POS)
#define BMP3XX_PWR_CTRL_MODE_NORMAL (0x03 << BMP3XX_PWR_CTRL_MODE_POS)

/* BMP3XX_REG_OSR */
#define BMP3XX_ODR_POS  0
#define BMP3XX_ODR_MASK 0x1F

/* BMP3XX_REG_ODR */
#define BMP3XX_OSR_PRESSURE_POS  0
#define BMP3XX_OSR_PRESSURE_MASK (0x07 << BMP3XX_OSR_PRESSURE_POS)
#define BMP3XX_OSR_TEMP_POS      3
#define BMP3XX_OSR_TEMP_MASK     (0x07 << BMP3XX_OSR_TEMP_POS)

/* BMP3XX_REG_CONFIG */
#define BMP3XX_IIR_FILTER_POS  1
#define BMP3XX_IIR_FILTER_MASK (0x7 << BMP3XX_IIR_FILTER_POS)

/* BMP3XX_REG_CMD */
#define BMP3XX_CMD_FIFO_FLUSH 0xB0
#define BMP3XX_CMD_SOFT_RESET 0xB6

/* default PWR_CTRL settings */
#define BMP3XX_PWR_CTRL_ON	    \
	(BMP3XX_PWR_CTRL_PRESS_EN | \
	 BMP3XX_PWR_CTRL_TEMP_EN |  \
	 BMP3XX_PWR_CTRL_MODE_NORMAL)
#define BMP3XX_PWR_CTRL_OFF 0

#define BMP3XX_SAMPLE_BUFFER_SIZE (6)

struct bmp3xx_cal_data {
	uint16_t t1;
	uint16_t t2;
	int8_t t3;
	int16_t p1;
	int16_t p2;
	int8_t p3;
	int8_t p4;
	uint16_t p5;
	uint16_t p6;
	int8_t p7;
	int8_t p8;
	int16_t p9;
	int8_t p10;
	int8_t p11;
} __packed;

struct bmp3xx_sample {
	uint32_t press;
	uint32_t raw_temp;
	int64_t comp_temp;
};

struct bmp3xx_config {
	union bmp3xx_bus bus;
	const struct bmp3xx_bus_io *bus_io;

#ifdef CONFIG_BMP3XX_TRIGGER
	struct gpio_dt_spec gpio_int;
#endif

	uint8_t iir_filter;
};

struct bmp3xx_data {
	uint8_t odr;
	uint8_t osr_pressure;
	uint8_t osr_temp;
	struct bmp3xx_cal_data cal;

#if defined(CONFIG_BMP3XX_TRIGGER)
	struct gpio_callback gpio_cb;
#endif

	struct bmp3xx_sample sample;

#ifdef CONFIG_BMP3XX_TRIGGER_OWN_THREAD
	struct k_sem sem;
#endif

#ifdef CONFIG_BMP3XX_TRIGGER_GLOBAL_THREAD
	struct k_work work;
#endif

#if defined(CONFIG_BMP3XX_TRIGGER_GLOBAL_THREAD) || \
	defined(CONFIG_BMP3XX_TRIGGER_DIRECT)
	const struct device *dev;
#endif

#ifdef CONFIG_BMP3XX_TRIGGER
	sensor_trigger_handler_t handler_drdy;
	const struct sensor_trigger *trig_drdy;
#endif /* CONFIG_BMP3XX_TRIGGER */
};

int bmp3xx_trigger_mode_init(const struct device *dev);
int bmp3xx_trigger_set(const struct device *dev,
		       const struct sensor_trigger *trig,
		       sensor_trigger_handler_t handler);
int bmp3xx_reg_field_update(const struct device *dev,
			    uint8_t reg,
			    uint8_t mask,
			    uint8_t val);

#endif /* __BMP3XX_H */
