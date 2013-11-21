/** @file lis3dh_regs.h
 * @author david.siorpaes@st.com
 * LIS3DH register description macros
 */

#define SPI_MULTI_ACCESS (1 << 6)
#define SPI_READ         (1 << 7)

#define STATUS_REG_AUX   0x07
#define OUT_ADC1_L       0x08
#define OUT_ADC1_H       0x09
#define OUT_ADC2_L       0x0A
#define OUT_ADC2_H       0x0B
#define OUT_ADC3_L       0x0C
#define OUT_ADC3_H       0x0D
#define INT_COUNTER_REG  0x0E

#define WHO_AM_I         0X0F
#define WHO_AM_I_VALUE   0x33

#define TEMP_CFG_REG     0x1F

#define CTRL_REG1        0x20
#define CTRL_REG1_DEF    0x07
#define CTRL_REG1_XEN    (1<<0)
#define CTRL_REG1_YEN    (1<<1)
#define CTRL_REG1_ZEN    (1<<2)
#define CTRL_REG1_LPEN   (1<<3)
#define CTRL_REG1_ODR_MASK  0xF0
#define CTRL_REG1_ODR_0     (0<<4)
#define CTRL_REG1_ODR_1     (1<<4)
#define CTRL_REG1_ODR_10    (2<<4)
#define CTRL_REG1_ODR_25    (3<<4)
#define CTRL_REG1_ODR_50    (4<<4)
#define CTRL_REG1_ODR_100   (5<<4)
#define CTRL_REG1_ODR_200   (6<<4)
#define CTRL_REG1_ODR_400   (7<<4)
#define CTRL_REG1_ODR_1_6K  (8<<4)
#define CTRL_REG1_ODR_5_0K  (9<<4)

#define CTRL_REG2        0x21

#define CTRL_REG3        0x22
#define CTRL_REG3_DEF    0
#define CTRL_REG3_I1_CLICK   (1<<7)
#define CTRL_REG3_I1_AOI1    (1<<6)
#define CTRL_REG3_I1_AOI2    (1<<5)
#define CTRL_REG3_I1_DRDY1   (1<<4)
#define CTRL_REG3_I1_DRDY2   (1<<3)
#define CTRL_REG3_I1_WTM     (1<<2)
#define CTRL_REG3_I1_OVERRUN (1<<1)

#define CTRL_REG4             0x23
#define CTRL_REG4_DEF         0x00
#define CTRL_REG_BDU         (1<<7)
#define CTRL_REG_BLE         (1<<6)
#define CTRL_REG4_FS2G       (0<<4)
#define CTRL_REG4_FS4G       (1<<4)
#define CTRL_REG4_FS8G       (2<<4)
#define CTRL_REG4_FS16G      (3<<4)

#define CTRL_REG5        0x24

#define CTRL_REG6        0x25
#define CTRL_REG6_DEF    0
#define CTRL_REG6_I2_CLICKEN  (1<<7)

#define REFERENCE        0x26
#define STATUS_REG2      0x27
#define OUT_X_L          0x28
#define OUT_X_H          0x29
#define OUT_Y_L          0x2A
#define OUT_Y_H          0x2B
#define OUT_Z_L          0x2C
#define OUT_Z_H          0x2D
#define FIFO_CTRL_REG    0x2E
#define FIFO_SRC_REG     0x3F
#define INT1_CFG         0x30
#define INT1_SOURCE      0x31
#define INT1_THS         0x32
#define INT1_DURATION    0x33

#define CLICK_CFG        0x38
#define CLICK_CFG_DEF    0
#define CLICK_CFG_ZD     (1<<5)
#define CLICK_CFG_ZS     (1<<4)
#define CLICK_CFG_YD     (1<<3)
#define CLICK_CFG_YS     (1<<2)
#define CLICK_CFG_XD     (1<<1)
#define CLICK_CFG_XS     (1<<0)

#define CLICK_SRC        0x39
#define CLICK_THS        0x3A
#define TIME_LIMIT       0x3B
#define TIME_LATENCY     0x3C
#define TIME_WINDOW      0x3D

