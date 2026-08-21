# RTOS_02_Queue

## Introduction

This project is built for the **STM32F103** series MCU (Cortex-M3) based on the STM32 HAL library and **FreeRTOS Kernel V10.3.1** using the **CMSIS-RTOS v2** API wrapper (`cmsis_os2.c`). It demonstrates inter-task communication (ITC) and synchronization using a **FreeRTOS Message Queue** (`osMessageQueue`).

The system implements a classic **Producer-Consumer** pattern with two concurrent tasks:
1. **`BtnCntTask` (Producer)**: Scans and debounces the user button, increments a 16-bit counter, and puts the value into a message queue.
2. **`DataPrcsTask` (Consumer)**: Blocks on the message queue, retrieves the count value, simulates data processing, and outputs real-time metrics to both an OLED display and a UART serial interface.

---

## Key Features

- **CMSIS-RTOS v2 Message Queue (`osMessageQueue`)**:
  - Wraps the native FreeRTOS queue (`xQueueCreate`, `xQueueSendToBack`, `xQueueReceive`) into the standardized CMSIS-RTOS v2 API.
  - Provides safe, decoupled, and thread-safe data transfer between tasks without polling or busy-waiting.
- **Button Counting Task (`BtnCntTask` - Producer)**:
  - Scans the user button on pin `PA6` (Active-Low with internal pull-up).
  - Software debouncing (10 ms) and key-release detection to ensure reliable single-trigger counting.
  - Enqueues the 16-bit counter value (`BtnCnt`) into `BtnCntQueue` with `osMessageQueuePut` (blocking, `osWaitForever`).
  - Displays the current queue fill level (`osMessageQueueGetCount`) on OLED line 2.
- **Data Processing Task (`DataPrcsTask` - Consumer)**:
  - Blocks on `osMessageQueueGet` (infinite wait) for queue items.
  - Displays `"Data Processing."` on OLED line 3 and simulates processing with a 2 s `HAL_Delay`.
  - Updates the current counter on OLED line 1 (`OLED_ShowNum(1, 8, BtnCnt, 5)`).
  - Transmits the formatted string `"BtnCnt: <value>\r\n"` over USART1 (`115200 8N1`).
- **OLED Display Interface**: 0.96" SSD1306 (128×64) driven via software I2C (`PB8` = SCL, `PB9` = SDA, slave address `0x78`).
- **UART Serial Logging**: Status telemetry output via USART1 (`PA9` = TX, `PA10` = RX).
- **Dedicated HAL Timebase**: `TIM4` is used as the HAL timebase source (`HAL_IncTick`) to avoid conflict with the FreeRTOS SysTick tick.
- **System Clock**: 72 MHz from HSE (8 MHz) × PLL ×9; FreeRTOS tick rate 1000 Hz.
---

## Task & Queue Specifications

### Tasks

| Task Name | Priority | Stack Size | Entry Function | Description |
| :--- | :--- | :--- | :--- | :--- |
| **`BtnCntTask`** | `osPriorityNormal` | 128 words (512 B) | `StartBtnCntTask` | Key scanning, debouncing, and message enqueueing (Producer). |
| **`DataPrcsTask`** | `osPriorityLow` | 128 words (512 B) | `StartDataPrcsTask` | Message dequeueing, OLED rendering, and UART transmission (Consumer). |

### Message Queue

| Queue Identifier | Capacity | Message Item Size | Producer | Consumer |
| :--- | :--- | :--- | :--- | :--- |
| **`BtnCntQueue`** | 3 items | `sizeof(uint16_t)` (2 Bytes) | `BtnCntTask` (`osMessageQueuePut`) | `DataPrcsTask` (`osMessageQueueGet`) |

### OLED Layout

| Line | Content |
| :--- | :--- |
| 1 | `BtnCnt:` + current counter value (`OLED_ShowNum(1, 8, BtnCnt, 5)`) |
| 2 | `MsgInQ:` + current number of messages in queue (`osMessageQueueGetCount`) |
| 3 | `Data Processing.` (shown while the consumer is busy) |

---

## File Structure

```text
├── CMakeLists.txt              # Root CMake build configuration
├── CMakePresets.json           # Presets for Debug and Release builds
├── config.ioc                  # STM32CubeMX configuration file
├── STM32F103XX_FLASH.ld        # Linker script for STM32F103xB
├── startup_stm32f103xb.s       # Startup assembly file
├── Core/
│   ├── Inc/
│   │   ├── main.h              # Common definitions, peripheral pin definitions
│   │   ├── FreeRTOSConfig.h    # FreeRTOS kernel configuration (V10.3.1)
│   │   ├── stm32f1xx_hal_conf.h# HAL module configuration
│   │   ├── stm32f1xx_it.h      # Interrupt handler declarations
│   │   ├── OLED.h              # OLED driver API declarations
│   │   ├── OLED_Font.h         # ASCII font bitmaps for OLED display
│   │   ├── usart.h             # USART1 peripheral header
│   │   └── gpio.h              # GPIO peripheral header
│   └── Src/
│       ├── main.c              # System clock, HAL init, FreeRTOS scheduler launch
│       ├── freertos.c          # FreeRTOS tasks, queue creation, and task handlers
│       ├── OLED.c              # Software I2C driver and OLED rendering routines
│       ├── usart.c             # USART1 peripheral initialization
│       ├── gpio.c              # GPIO configuration (Btn, OLED)
│       ├── stm32f1xx_hal_timebase_tim.c # TIM4 HAL timebase implementation
│       ├── stm32f1xx_hal_msp.c # HAL MSP (peripheral init/deinit) routines
│       ├── stm32f1xx_it.c      # Interrupt service routines
│       ├── system_stm32f1xx.c  # System clock setup
│       ├── sysmem.c            # Heap memory management (newlib)
│       └── syscalls.c          # System call stubs (newlib)
├── cmake/
│   ├── user_sources.cmake      # User source file and include directory registration
│   ├── gcc-arm-none-eabi.cmake # Arm GNU toolchain file
│   ├── starm-clang.cmake       # Arm Clang toolchain file
│   └── stm32cubemx/            # CubeMX-generated CMake configurations
├── Drivers/                    # STM32 HAL Driver & CMSIS core
└── Middlewares/
    └── Third_Party/FreeRTOS/   # FreeRTOS kernel source code & CMSIS-RTOS v2 wrapper
```
---

## Hardware Specification & Pinout

| Peripheral / Signal | Pin | Mode / Configuration | Description |
| :--- | :--- | :--- | :--- |
| **User Button (`Btn`)** | `PA6` | GPIO Input (Pull-Up) | Active Low, debounced button |
| **OLED SCL** | `PB8` | GPIO Output Open-Drain (High-Speed) | Software I2C Clock line |
| **OLED SDA** | `PB9` | GPIO Output Open-Drain (High-Speed) | Software I2C Data line |
| **USART1 TX** | `PA9` | Alternate Function Push-Pull | Serial Transmit (115200 8N1) |
| **USART1 RX** | `PA10` | Input Floating | Serial Receive |

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
   - Pressing the user button on `PA6` triggers `BtnCntTask`.
   - The incremented count is enqueued to `BtnCntQueue`.
   - `DataPrcsTask` consumes the item, displays `"Data Processing."` on the OLED, updates the counter display (`BtnCnt`), and logs `"BtnCnt: <value>"` over USART1.

---

## License

This project is licensed under the **MIT License**.
