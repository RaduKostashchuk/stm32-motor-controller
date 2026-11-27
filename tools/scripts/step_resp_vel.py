import serial
import time
import os
from pathlib import Path

# ==== НАСТРОЙКИ ====
PORT = 'COM7'
BAUD = 115200
DURATION = 15          # сколько секунд писать
TARGET_RPM = 50

BASE_DIR = Path(__file__).resolve().parent
OUTPUT_DIR = BASE_DIR / "out"
OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
OUTPUT_FILE = OUTPUT_DIR / "vel_step_resp_telemetry.csv"

# ==== ОТКРЫТИЕ ПОРТА ====
ser = serial.Serial(PORT, BAUD, timeout=1)
print(f"Открыт порт {PORT} на скорости {BAUD}")
print("Жду 2 секунды пока Arduino стабилизируется")
time.sleep(2)

# ==== СТАРТ МОТОРА ====
print(f"Отправляю VEL 1 {TARGET_RPM}")
ser.write(f'VEL 1 {TARGET_RPM}\n'.encode())

# ==== СБОР ДАННЫХ ====
print(f"Собираю телеметрию {DURATION} секунд")
start = time.time()
lines = 0
skipped = 0

with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
    # Заголовок
    f.write("t_ms,desired,current,error,p,i,d,uff,pwm,vbat\n")
    
    while time.time() - start < DURATION:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if not line:
            continue
        if not line.startswith("T:"):
            skipped += 1
            continue
	# Фильтр: только строки, которые парсятся как 10 чисел
        values = []
        try:
            values = [float(v) for v in line[2:].split(',')]
        except ValueError:
            skipped +=1
            continue
        if len(values) != 10:
            skipped += 1
            continue
        f.write(f"{line[2:]}\n")
        lines += 1

# ==== ОСТАНОВКА МОТОРА ====
print("Отправляю STOP 1")
ser.write(b'STOP 1\n')

ser.close()
print(f"Готово. Записано {lines} строк в {OUTPUT_FILE}")
print(f"Пропущено {skipped} строк")
