import asyncio
import websockets
import json
import time
import math

async def handler(websocket, path):
    print("Client connected")
    try:
        while True:
            # Blast synthetic 25kHz JSON payloads (59 channels)
            # In Python, we can't truly hit 25kHz easily, but we can blast as fast as possible or send batch arrays.
            # We'll just loop and send.
            payload = {
                "timestamp": int(time.time() * 1000),
                "spikes": [
                    {
                        "channel_id": i % 59,
                        "amplitude": math.sin(time.time()) * 100.0
                    } for i in range(10)
                ]
            }
            await websocket.send(json.dumps(payload))
            await asyncio.sleep(0.001)  # Sleep 1ms to prevent complete CPU hogging
    except websockets.ConnectionClosed:
        print("Client disconnected")

start_server = websockets.serve(handler, "localhost", 8080)

if __name__ == "__main__":
    print("Starting mock CL1 WebSocket server on ws://localhost:8080")
    loop = asyncio.get_event_loop()
    loop.run_until_complete(start_server)
    loop.run_forever()
