import asyncio
import websockets
import json
import time
import random

async def blast_spikes(websocket, path):
    print("Client connected, blasting spikes at ~25kHz")
    # 25kHz means 25000 spikes per second.
    # We can batch them, e.g., send 250 spikes every 0.01s
    batch_size = 250
    interval = 0.01

    try:
        while True:
            spikes = []
            now = int(time.time() * 1000)
            for i in range(batch_size):
                spikes.append({
                    "ts": now + i,
                    "ch": random.randint(0, 58),
                    "amp": random.uniform(-5.0, 5.0)
                })
            
            payload = json.dumps({"spikes": spikes})
            await websocket.send(payload)
            await asyncio.sleep(interval)
    except websockets.exceptions.ConnectionClosed:
        print("Client disconnected")

async def main():
    async with websockets.serve(blast_spikes, "localhost", 8080):
        print("Mock CL1 Server running on ws://localhost:8080")
        await asyncio.Future()  # run forever

if __name__ == "__main__":
    asyncio.run(main())