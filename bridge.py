import asyncio
import json
import websockets
import RPi.GPIO as GPIO

CLK = 17
DT  = 27
SW  = 22

GPIO.setmode(GPIO.BCM)
GPIO.setup(CLK, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(DT,  GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(SW,  GPIO.IN, pull_up_down=GPIO.PUD_UP)

clk_last = GPIO.input(CLK)
connected_clients = set()

async def send_all(msg):
    if connected_clients:
        await asyncio.gather(*[c.send(json.dumps(msg)) for c in connected_clients])

async def handler(websocket):
    connected_clients.add(websocket)
    print("Browser connected")
    try:
        async for message in websocket:
            data = json.loads(message)
            print(f"From browser: {data}")
    finally:
        connected_clients.remove(websocket)

async def poll_encoder():
    global clk_last
    while True:
        clk = GPIO.input(CLK)
        dt  = GPIO.input(DT)
        sw  = GPIO.input(SW)

        if clk != clk_last:
            if dt != clk:
                await send_all({"type": "encoder", "dir": 1})
            else:
                await send_all({"type": "encoder", "dir": -1})
        clk_last = clk

        if sw == GPIO.LOW:
            await send_all({"type": "press"})
            await asyncio.sleep(0.3)

        await asyncio.sleep(0.001)

async def main():
    print("Bridge running on ws://localhost:8765")
    async with websockets.serve(handler, "localhost", 8765):
        await poll_encoder()

asyncio.run(main())
