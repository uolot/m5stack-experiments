from m5stack import lcd, btnA, btnB, btnC, speaker
import machine
import time

IDLE = 0
RUNNING = 1
FINISHED = 2

BG_COLOR = lcd.BLACK
FG_COLOR = lcd.GREEN


def set_colors(invert=False, clear=True):
    if invert:
        lcd.set_bg(FG_COLOR)
        lcd.set_fg(BG_COLOR)
    else:
        lcd.set_bg(BG_COLOR)
        lcd.set_fg(FG_COLOR)
    if clear:
        lcd.clear()


def draw(state):
    # draw time
    lcd.font(lcd.FONT_DejaVu40)
    m, s = divmod(state.time, 60)
    lcd.text(lcd.CENTER, 100, "{:02}:{:02}".format(m, s))
    # draw menu
    lcd.font(lcd.FONT_DejaVu24)
    if state.state == IDLE:
        lcd.print("inc", 50, lcd.BOTTOM)
        lcd.print("dec", 140, lcd.BOTTOM)
        lcd.print("start", 225, lcd.BOTTOM)
    elif state.state == RUNNING:
        lcd.print("     ", 50, lcd.BOTTOM)
        lcd.print("     ", 140, lcd.BOTTOM)
        lcd.print("stop ", 227, lcd.BOTTOM)


def play_alarm(state):
    set_colors(invert=True)
    for _x in range(3):
        for _y in range(4):
            speaker.tone(1000, 60, 1)
            draw(state)
            time.sleep_ms(120)
        time.sleep_ms(300)
    set_colors(invert=False)
    draw(state)


class State:
    def __init__(self):
        self.state = IDLE
        self.time = 0

    def idle(self):
        return self.state == IDLE

    def running(self):
        return self.state == RUNNING

    def finished(self):
        return self.state == FINISHED

    def inc_time(self, dt=60):
        self.time += dt

    def dec_time(self, dt=60):
        self.time = max(self.time - dt, 0)

    def tick(self):
        self.dec_time(1)
        if self.time == 0:
            self.state = FINISHED

    def start(self):
        self.state = RUNNING

    def stop(self):
        self.state = IDLE
        self.time = 0


state = State()
state.inc_time(300)
set_colors()
draw(state)
last_time = 0
last_press = 0

while True:
    if state.idle():
        if btnA.wasPressed():
            state.inc_time()
        elif btnB.wasPressed():
            state.dec_time()
        elif btnC.wasPressed():
            state.start()
            last_time = time.time()
    elif state.running():
        if btnC.wasPressed():
            state.stop()
        else:
            t = time.time()
            if t > last_time:
                state.tick()
                last_time = t
    elif state.finished():
        play_alarm(state)
        state.stop()
        set_colors(invert=False)
        draw(state)
        state.stop()
    draw(state)
    machine.idle()
