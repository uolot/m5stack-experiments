### RTC

# TZs / timezones
my_timezone = "CET-1CEST"
# found in second field, text before the coma, in
# https://github.com/loboris/MicroPython_ESP32_psRAM_LoBo/blob/master/MicroPython_BUILD/components/micropython/docs/zones.csv


### WLAN

import network

wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect(ssid, password)
# workspace:
wlan.connect("3sTK Hyperoptic 1Gb Fibre 2.4Ghz", "25sMPZwLGwTRZ")
wlan.isconnected()


### FTP

from network import ftp

ftp.start()
ftp.start(user="micro", password="python", buffsize=1024, timeout=300)
ftp.status()
ftp.stop()


### Uptime

from m5stack import lcd
import time
import machine

lcd.set_fg(lcd.BLACK)
lcd.set_bg(lcd.WHITE)
lcd.font(lcd.FONT_DejaVu24)
lcd.clear()

t0 = time.time()
while True:
    t = time.time()
    m, s = map(int, divmod(t - t0, 60))
    lcd.text(0, 0, "{}:{:02}".format(m, s))
