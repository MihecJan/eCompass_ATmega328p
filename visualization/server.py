import asyncio
import serial
import websockets
import threading

PORT = "COM7"
BAUD = 250000

WIDTH = 128
HEIGHT = 64
PAGES = 8
PAGE_BYTES = 128

START_BYTE = b'\x02'

clients = set()
latest_frame = None


def uart_reader():
    global latest_frame

    ser = serial.Serial(PORT, BAUD, timeout=1)
    pages = [None] * PAGES
    current_page = 0

    while True:

        while True:
            b = ser.read(1)

            if not b:
                continue

            if b != START_BYTE:
                continue

            b = ser.read(1)

            if not b:
                continue

            if b != START_BYTE:
                continue

            break
        
        while True:
            b = ser.read(1)
            if b != b'\xAA':
                print("Expected U8X8_MSG_BYTE_START_TRANSFER")
                break
        
            # skip 2 bytes
            ser.read(2)

            page_select = ser.read(1)[0]
            # Page select: 0xB0..0xB7
            if 0xB0 <= page_select <= 0xB7:

                current_page = page_select - 0xB0

                # read page data
                pages[current_page] = ser.read(PAGE_BYTES)

                b = ser.read(1)
                if b != b'\x55':
                    print("Expected U8X8_MSG_BYTE_END_TRANSFER")

                # if we have all pages → full frame
                if all(p is not None for p in pages):
                    latest_frame = pages
                    pages = [None] * PAGES
                    break
    

async def ws_handler(websocket):
    print(f"Client connected: {websocket.remote_address}")
    clients.add(websocket)

    try:
        while True:
            if latest_frame:
                await websocket.send(latest_frame)
            await asyncio.sleep(0.05)  # ~20 FPS max
    finally:
        print(f"Client disconnected: {websocket.remote_address}")
        clients.remove(websocket)


async def main():
    async with websockets.serve(ws_handler, "localhost", 8765):
        print("Server started on ws://localhost:8765")
        await asyncio.Future()  # run forever


if __name__ == "__main__":
    threading.Thread(target=uart_reader, daemon=True).start()
    asyncio.run(main())