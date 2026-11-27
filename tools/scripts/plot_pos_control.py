import csv
import matplotlib.pyplot as plt
import os
from pathlib import Path

# ==== Settings ====
BASE_DIR = Path(__file__).resolve().parent
INPUT_DIR = BASE_DIR / "out"
INPUT_FILE = INPUT_DIR / "pos_control_telemetry.csv"
OUTPUT_DIR = BASE_DIR / "images"
OUTPUT_FILE = OUTPUT_DIR / "pos_control_telemetry.png"
MAX_RPM = -50 

# ==== Read CSV ====
t = []
target_rpm = []
current_rpm = []
error_rpm = []
p = []
i = []
d = []
target_pos = []
current_pos = []
error_pos = []
uff = []
pwm = []
vbat = []

with open(INPUT_FILE, 'r', encoding='utf-8') as f:
    reader = csv.DictReader(f)
    for row in reader:
        t.append(float(row['t_ms']) / 1000.0)  # в секунды
        target_rpm.append(float(row['target_rpm']))
        current_rpm.append(float(row['current_rpm']))
        error_rpm.append(float(row['error_rpm']))
        p.append(float(row['p']))
        i.append(float(row['i']))
        d.append(float(row['d']))
        uff.append(float(row['uff']))
        target_pos.append(float(row['target_pos']))
        current_pos.append(float(row['current_pos']))
        error_pos.append(float(row['error_pos']))
        pwm.append(float(row['pwm']))
        vbat.append(float(row['vbat']))

# Shift time to motor start
t0 = None
for idx, val in enumerate(target_rpm):
    if val > 0:
        t0 = t[idx]
        break
if t0 is not None:
    t = [x - t0 for x in t]

print(f"Dots: {len(t)}")
print(f"Duration: {t[-1]:.3f} сек")

# ==== Graph 1: Position ====
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8), sharex=True)

ax1.plot(t, target_pos, label='target_pos', linestyle='--', color='gray')
ax1.plot(t, current_pos, label='current_pos', color='blue')
ax1.set_ylabel('Position')
ax1.set_title('Position: target / current')
ax1.grid(True, alpha=0.3)
ax1.legend()

# ==== Graph 2: Speed ====
ax2.plot(t, current_rpm, label='current_rpm', color='black')
ax2.plot(t, target_rpm, label='target_rpm', color='red')
ax2.plot(t, pwm, label='pwm', color='blue')
ax2.axhline(y=MAX_RPM, color='red', linestyle='--', alpha=0.3, label=f'Max rpm={MAX_RPM}')
ax2.set_xlabel('Time, sec')
ax2.set_ylabel('Value')
ax2.set_title(f'Speed, Rpm')
ax2.grid(True, alpha=0.3)
ax2.legend()

plt.tight_layout()
plt.savefig(OUTPUT_FILE, dpi=100)
print(f"График сохранён в {OUTPUT_FILE}")
plt.show()