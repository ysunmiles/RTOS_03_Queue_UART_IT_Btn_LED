# RTOS_03_Queue_UART_IT_Btn_LED

## Introduction

This project is built for the **STM32F103** series MCU (Cortex-M3) based on the STM32 HAL library and **FreeRTOS Kernel V10.3.1** using the **CMSIS-RTOS v2** API wrapper. It demonstrates inter-task communication (ITC) using multiple **FreeRTOS Message Queues** (`osMessageQueue`) and handles UART input via interrupts.

The system implements a command-driven architecture where both hardware buttons and UART serial commands can control a set of LEDs.

---

## Key Features

- **Multi-Queue Communication**:
  - `LEDCmdQueue`: Transfers structured LED commands (`LEDCmdType`) to the dedicated LED control task.
  - `UARTStrQueue`: Transfers raw strings from the UART Interrupt Service Routine (ISR) to the command processing task.
- **Button Task (`BtnTask`)**:
  - Monitors the user button with software debouncing (20 ms).
  - Toggles the state of an LED and sends a command to the `LEDCmdQueue`.
  - Provides visual feedback on the OLED display when a press is detected.
- **UART Command Task (`CommandTask`)**:
  - Uses `HAL_UARTEx_ReceiveToIdle_IT` for efficient, variable-length string reception.
  - Parses commands received via UART (e.g., `"LED1ON"`, `"LED2OFF"`, `"LED3ON"`) and translates them into `LEDCmdType` messages.
- **LED Control Task (`LEDTask`)**:
  - Blocks on `LEDCmdQueue` awaiting new commands.
  - Controls three physical LEDs (LED1, LED2, LED3) based on received messages.
  - Updates the OLED display with the active LED number and its current state (0/1).
- **OLED Display Interface**: 0.96" SSD1306 driven via software I2C for real-time status monitoring.
- **Dedicated HAL Timebase**: Uses a hardware timer (e.g., `TIM4`) as the HAL timebase to avoid conflict with the FreeRTOS SysTick.

---

## Task & Queue Specifications

### Tasks

| Task Name | Priority | Stack Size | Description |
| :--- | :--- | :--- | :--- |
| **`BtnTask`** | `osPriorityHigh` | 512 B | Hardware button monitoring and command generation. |
| **`CommandTask`** | `osPriorityHigh1` | 512 B | UART string reception and command parsing. |
| **`LEDTask`** | `osPriorityNormal` | 512 B | LED hardware control and status display. |

### Message Queues

| Queue Identifier | Capacity | Item Size | Purpose |
| :--- | :--- | :--- | :--- |
| **`LEDCmdQueue`** | 10 items | `sizeof(LEDCmdType)` | Passes LED number and state to `LEDTask`. |
| **`UARTStrQueue`** | 5 items | 16 Bytes | Passes received UART strings to `CommandTask`. |

---

## Command Interface (UART)

The system listens for specific string commands via USART1 (`115200 8N1`):
- `LED[1-3]ON`: Turns on the specified LED (e.g., `LED1ON`).
- `LED[1-3]OFF`: Turns off the specified LED (e.g., `LED2OFF`).

---

## File Structure

```text
├── Core/
│   ├── App/
│   │   ├── Tasks/
│   │   │   ├── BtnTask.c       # Button monitoring logic
│   │   │   ├── CommandTask.c   # UART parsing and command routing
│   │   │   └── LEDTask.c       # LED hardware control
│   │   └── Types/
│   │       └── LEDType.h       # Shared data structures (LEDCmdType)
│   ├── Inc/
│   │   ├── FreeRTOSConfig.h    # Kernel configuration
│   │   ├── OLED.h              # OLED driver header
│   │   └── ...
│   └── Src/
│       ├── freertos.c          # RTOS object creation and task entry points
│       ├── main.c              # Hardware initialization
│       ├── OLED.c              # SSD1306 driver implementation
│       └── ...
├── Middlewares/
│   └── Third_Party/FreeRTOS/   # Kernel source code
└── ...
```

---

## Hardware Specification & Pinout

| Peripheral / Signal | Pin | Mode / Configuration | Description |
| :--- | :--- | :--- | :--- |
| **User Button (`Btn`)** | `PB11` | GPIO Input (Pull-Up) | Active Low, debounced button (Toggles LED1) |
| **LED 1** | `PA4` | GPIO Output Push-Pull | Controlled by Button or UART |
| **LED 2** | `PA5` | GPIO Output Push-Pull | Controlled by UART |
| **LED 3** | `PA6` | GPIO Output Push-Pull | Controlled by UART |
| **OLED SCL** | `PB8` | GPIO Output Open-Drain | Software I2C Clock |
| **OLED SDA** | `PB9` | GPIO Output Open-Drain | Software I2C Data |
| **USART1 TX** | `PA9` | Alternate Function PP | Serial Transmit (115200 8N1) |
| **USART1 RX** | `PA10` | Input Floating | Serial Receive (Interrupt-driven) |

> **Note**: In `OLED.c` the software-I2C macros map `OLED_W_SCL` to `PB8` and `OLED_W_SDA` to `PB9`. The pin names in `main.h` (`OLED_SDA_Pin`/`OLED_SCL_Pin`) are defined the other way around; the macros in `OLED.c` are what actually drive the display.

---

## Build Environment & Toolchain

- **Build System**: CMake 3.22+ with Ninja
- **Compiler**: Arm GNU Toolchain (`arm-none-eabi-gcc`) — see `cmake/gcc-arm-none-eabi.cmake`
- **Libraries**: STM32F1xx HAL Driver & FreeRTOS Kernel V10.3.1 (CMSIS-RTOS v2)
- **Heap**: FreeRTOS heap_4 (`USE_FreeRTOS_HEAP_4`, `configTOTAL_HEAP_SIZE` = 3072 B)

---

## Build & Flash Instructions

### 1. Build using Terminal or VS Code

```bash
# Configure the build with Debug preset
cmake --preset Debug

# Compile and link the target executable
cmake --build --preset Debug
```

### 2. Flashing & Running

1. Flash the compiled ELF/HEX file (`build/Debug/config.elf`) to the target board using **ST-Link**, **DAP-Link**, or **J-Link**.
2. **Behavior on Run**:
   - **Button Interaction**: Pressing the user button on `PB11` sends a toggle command to `LEDCmdQueue`. `LEDTask` receives it, toggles **LED1**, and shows `"Btn Pressed"` on the OLED.
   - **UART Interaction**: Send commands like `LED1ON`, `LED2OFF`, or `LED3ON` via a serial terminal (115200 8N1).
     - The UART ISR receives the string and puts it into `UARTStrQueue`.
     - `CommandTask` parses the string and sends a corresponding command to `LEDCmdQueue`.
     - `LEDTask` updates the target LED state and displays the LED number and state on the OLED.

---

## License

This project is licensed under the **MIT License**.
