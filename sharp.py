import rp2
import machine
from machine import Pin

machine.freq()          # get the current frequency of the CPU
machine.freq(240000000)  # set the CPU frequency to 240 MHz
pinout_PICO = {
    'pin_CNTRL1_VCC': '3V3',
    'pin_FSR1_Pin_2': 'GPIO26_ADC0',
    'pin_CNTRL1_CAL': 'GPIO21',
    'pin_CNTRL1_ZERO': 'GPIO20',

}

pinout_PICO_num = {
    'pin_PICO_VSYS': 39,
    'pin_PICO_VSYS': 38,
    'pin_CNTRL1_VCC': 36,
    'pin_FSR1_Pin_2': 31,
    'pin_CNTRL1_CAL': 27,
    'pin_CNTRL1_ZERO': 26,
    'pin_PICO_GPIO13': 17,
    'pin_PICO_GPIO12': 16,
    'pin_PICO_GPIO11': 15,
    'pin_PICO_GPIO10': 14,
    'pin_PICO_GPIO9': 12,
    'pin_PICO_GPIO8': 11,
    'pin_PICO_GPIO1': 2,
    'pin_PICO_GPIO0': 1,
}


pinout_hx711 = {
    'pin_hx711_AP': 'A+',
    'pin_hx711_AN': 'A-',
    'pin_hx711_EN': 'E-',
    'pin_hx711_EP': 'E+',
    'pin_PICO_VSYS': 'VCC',
    'pin_PICO_GPIO1': 'SCK',
    'pin_PICO_GPIO0': 'DT',
    'pin_PICO_VSYS': 'GND',
}

pinout_hx711_num = {
    'pin_hx711_AP': 3,
    'pin_hx711_AN': 4,
    'pin_hx711_EN': 5,
    'pin_hx711_EP': 6,
    'pin_PICO_VSYS': 7,
    'pin_PICO_GPIO1': 8,
    'pin_PICO_GPIO0': 9,
    'pin_PICO_VSYS': 10,
}

pinout_CNTRL1_num = {
    'pin_CNTRL1_VCC': 1,
    'pin_CNTRL1_CAL': 3,
    'pin_CNTRL1_ZERO': 4,
}

pinout_CNTRL1 = {
    'pin_CNTRL1_VCC': 'VCC',
    'pin_CNTRL1_CAL': 'CAL',
    'pin_CNTRL1_ZERO': 'ZERO',
}
pinout_disp1 = {
    'pin_PICO_GPIO13': 8,
    'pin_PICO_GPIO12': 7,
    'pin_PICO_GPIO8': 6,
    'pin_PICO_GPIO9': 5,
    'pin_PICO_GPIO10': 4,
    'pin_PICO_GPIO11': 3,
    'pin_PICO_VSYS': 2,
    'pin_PICO_VSYS': 1,
}
pinout_disp = {
    'pin_PICO_GPIO13': 'BL',
    'pin_PICO_GPIO12': 'RST',
    'pin_PICO_GPIO8': 'DC',
    'pin_PICO_GPIO9': 'CS',
    'pin_PICO_GPIO10': 'CLK',
    'pin_PICO_GPIO11': 'DIN',
    'pin_PICO_VSYS': 'GND',
    'pin_PICO_VSYS': 'VCC',
}
