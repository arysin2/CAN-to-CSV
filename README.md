# CAN to CSV 

Uses C++ to convert CAN to CSV and shifts bytes accordingly

## Library: dbcppp
I used the **dbcppp** library for parsing Vector DBC files. The code uses `dbcppp` to dynamically map the DBC "Network" to the raw frames.

### 1. Byte alignment
Loggers don't include perfect C++ memory alignment. To prevent the compiler from adding "hidden" gaps between variables, I used:
* **`__attribute__((packed))`**: to match the byte-by-byte layout.
* **Manual Padding**: Added `uint8_t __pad[3]` to align the 8-byte CAN payload on a 64-bit boundary. This ensures the data isn't faulty when it is read from the stream.

### 2. Positive vs Negative
Standard decoders often treat 16-bit signals as always positive. Current can switch between positive (regen) and negative (discharge). 


### 3. Works with standard and extended bits
I implemented `(msg.Id() & 0x1FFFFFFF)` to filter frames. This bitwise masking strips away priority and control bits, ensuring the parser works with both **Standard (11-bit)** and **Extended (29-bit)** 


## Setup & Usage

### Build
```bash
g++ main.cpp -o can_parser -ldbcppp