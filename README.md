# STM32 Motor Controller

Motor control system on STM32G474RE (NUCLEO-G474RE).
Портирование [motor-control-ff-pid](https://github.com/RaduKostashchuk/motor-control-ff-pid) 
с Arduino на STM32.

## Status
- ✅ UART + DMA (ReceiveToIdle)
- ✅ Reader (FSM для парсинга команд)
- ✅ Parser (MOTOR / STOP / MODE / STATUS)
- ⏳ Encoder (TIM encoder mode)
- ⏳ Motor (PWM)
- ⏳ Controller (FF + PID)
- ⏳ Telemetry через DMA

## Hardware
- STM32G474RE (NUCLEO-G474RE)
- JGA25-370B motor + encoder
- TB6612 driver

## Stack
- C, STM32CubeIDE, HAL

## Related
- [motor-control-ff-pid](https://github.com/RaduKostashchuk/motor-control-ff-pid) — Arduino-версия с экспериментальной настройкой PID

## Testing

Host-side unit tests (Unity): **~109 tests**, all passing.

| Module | Tests | Coverage |
|---|---|---|
| format | 44 | INT32_MIN, float rounding, NULL guards, buffer overflow |
| parser | 29 | Valid/invalid commands, long tokens, edge whitespace |
| reader | 15 | FSM, ring buffer, multi-command, discard, CR/LF |
| estimator | 13 | Bidirectional, glitch rejection, boundary, recovery |
| controller | 13 | P/I/D, anti-windup (both directions), deadband, saturation |

Run: `cd test && make test`