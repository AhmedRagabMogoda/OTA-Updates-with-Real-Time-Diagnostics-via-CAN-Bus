#!/usr/bin/env python3
"""
raspi_uploader.py

Python script to send a firmware update file from Raspberry Pi to STM32 over UART.
Implements a user prompt to start, then waits for an initial CODE_NEXT before sending chunks.

ACK/NACK payload codes (STM32 side):
    CODE_NEXT   = 0xA5  # Request next chunk
    CODE_DONE   = 0x5A  # Update complete
    CODE_ERROR  = 0xFF  # Error, retry

Configuration:
    - SERIAL_PORT: UART device on Raspberry Pi
    - BAUDRATE:    Must match STM32 UART setting
    - CHUNK_SIZE:  Size of each flash page (bytes)
    - PATH:        Path to the firmware file to send
"""

import serial
import time
import sys
import os

# === Configuration ===
SERIAL_PORT = '/dev/serial0'   # adjust as needed, e.g. '/dev/ttyS0'
BAUDRATE    = 115200
CHUNK_SIZE  = 1024             # bytes per flash page
PATH        = 'file/update'    # firmware file path

# STM32 ACK/NACK codes
CODE_NEXT  = 0xA5
CODE_DONE  = 0x5A
CODE_ERROR = 0xFF

# Retry policy
MAX_RETRIES = 3
ACK_TIMEOUT = 1.0  # seconds


def open_serial(port, baudrate, timeout=ACK_TIMEOUT):
    """Open and return a configured serial port."""
    ser = serial.Serial(port, baudrate=baudrate, timeout=timeout)
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    return ser


def wait_for_ack(ser):
    """Read one byte from serial; raise on timeout."""
    ack = ser.read(1)
    if not ack:
        raise TimeoutError("Timeout waiting for ACK/NACK")
    return ack[0]


def split_file(path, chunk_size):
    """Yield successive chunk_size-byte chunks from file, padding last."""
    with open(path, 'rb') as f:
        while True:
            data = f.read(chunk_size)
            if not data:
                break
            if len(data) < chunk_size:
                data += b'\xFF' * (chunk_size - len(data))
            yield data


def main():
    # 0) Prompt user
    choice = input("Do you want to start the update? Type 'yes' to confirm: ").strip().lower()
    if choice != 'yes':
        print("Update cancelled.")
        return

    if not os.path.isfile(PATH):
        print(f"Error: firmware file not found: {PATH}")
        return

    # Prepare chunks
    chunks = list(split_file(PATH, CHUNK_SIZE))
    total = len(chunks)
    if total == 0 or total > 255:
        print("Error: invalid number of chunks")
        return

    print(f"Firmware size: {os.path.getsize(PATH)} bytes -> {total} chunks")

    ser = open_serial(SERIAL_PORT, BAUDRATE)

    try:
        # 1) Wait for initial CODE_NEXT from STM32
        print("Waiting for initial CODE_NEXT from STM32...", end=' ', flush=True)
        try:
            initial = wait_for_ack(ser)
        except TimeoutError:
            print("No CODE_NEXT received. Ensure STM32 bootloader is running.")
            return

        if initial != CODE_NEXT:
            print(f"Received 0x{initial:02X} instead of CODE_NEXT. Aborting.")
            return
        print("Received.")

        # 2) Send total chunk count
        print("Sending total chunk count...", end=' ', flush=True)
        ser.write(bytes([total]))
        ack = wait_for_ack(ser)
        if ack != CODE_NEXT:
            print(f"Unexpected ACK 0x{ack:02X}. Aborting.")
            return
        print("OK.")

        # 3) Send each chunk
        for idx, chunk in enumerate(chunks, start=1):
            print(f"Sending chunk {idx}/{total}...", end=' ', flush=True)
            ser.write(chunk)
            ack = wait_for_ack(ser)
            if ack == CODE_NEXT:
                print("NEXT")
            elif ack == CODE_DONE and idx == total:
                print("DONE")
            elif ack == CODE_ERROR:
                print("ERROR, retrying chunk...", end=' ', flush=True)
                # retry logic
                success = False
                for attempt in range(1, MAX_RETRIES + 1):
                    ser.write(chunk)
                    try:
                        ack2 = wait_for_ack(ser)
                    except TimeoutError:
                        print(f"Timeout on retry {attempt}", end=' ')
                        continue
                    if ack2 == CODE_NEXT:
                        print(f"retry {attempt} OK")
                        success = True
                        break
                    else:
                        print(f"retry {attempt} ACK=0x{ack2:02X}", end=' ')
                if not success:
                    print("Max retries reached. Aborting.")
                    return
            else:
                print(f"Unexpected ACK 0x{ack:02X}. Aborting.")
                return

        print("Firmware update completed successfully!")

    finally:
        ser.close()


if __name__ == '__main__':
    main()
