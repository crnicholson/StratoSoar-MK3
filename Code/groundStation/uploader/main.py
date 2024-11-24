import asyncio
import websockets
import json
from sondehub.amateur import Uploader
import pandas as pd
import os
import datetime

PORT = 8080  # Match this with the port in your ESP32 code.
COMMENT = "StratoSoar MK3 high altitude glider."
ANTENNA = "Wire monopole"
RADIO = "SX1278 LoRa module"
FREQ = "433.000"  # Frequency in MHz of the received telemetry.

cwd = os.getcwd()

last_print = 0


# Handler for incoming WebSocket connections.
async def handle_connection(websocket, path):
    print("New connection established")
    try:
        # Keep listening for messages from the client.
        async for message in websocket:
            print(f"Received: {message}")
            try:
                received = json.loads(message)
                print(f"Parsed JSON: {received}")

                lat = received["lat"]
                lon = received["lon"]
                alt = received["altitude"]
                t_lat = received["tLat"]
                t_lon = received["tLon"]
                temp = received["temperature"]
                pressure = received["pressure"]
                humidity = received["humidity"]
                volts = received["volts"]
                yaw = received["yaw"]
                pitch = received["pitch"]
                roll = received["roll"]
                year = received["year"]
                month = received["month"]
                day = received["day"]
                hour = received["hour"]
                minute = received["minute"]
                second = received["second"]
                receivedAbort = received["abort"]
                tx_count = received["txCount"]
                rx_count = received["rxCount"]
                u_lat = received["u_lat"]
                u_lon = received["u_lon"]
                u_alt = received["u_alt"]
                rssi = received["rssi"]
                snr = received["snr"]
                call_sign = received["callsign"]
                id = received["id"]

                utc_time = datetime.datetime.utcnow().replace(
                    year=year,
                    month=month,
                    day=day,
                    hour=hour,
                    minute=minute,
                    second=second,
                    microsecond=0,
                )
                utc_time_str = utc_time.strftime("%Y-%m-%dT%H:%M:%SZ")

                if os.path.exists(f"{cwd}/data/{call_sign}.csv"):
                    df = pd.read_csv(f"{cwd}/data/{call_sign}.csv")
                    if df.loc[-1, "Time"] == utc_time_str:
                        print("Duplicate data received. Ignoring.")

                        data = {
                            "ID": id,
                            "Time": utc_time_str,
                            "Call sign": call_sign,
                            "SNR": snr,
                            "RSSI": rssi,
                        }

                        df = pd.DataFrame([data])

                        file = f"{cwd}/stations.csv"
                        df.to_csv(
                            file,
                            mode="a",
                            header=not pd.io.common.file_exists(file),
                            index=False,
                        )
                        
                        if datetime.datetime.second() - last_print > 60:
                            print("Stations receiving the in the last 10 minutes.")
                            df = pd.read_csv(f"{cwd}/stations.csv")
                            for 
                    else:
                        uploader = Uploader(
                            call_sign,
                            uploader_position=[u_lat, u_lon, u_alt],
                            uploader_radio=RADIO,
                            uploader_antenna=ANTENNA,
                        )

                        uploader.add_telemetry(
                            call_sign,
                            utc_time_str,
                            lat,
                            lon,
                            alt,
                            frame=tx_count,
                            batt=volts,
                            temp=temp,
                            pressure=pressure,
                            snr=snr,
                            frequency=FREQ,
                            rssi=rssi,
                            modulation="LoRa",
                            extra_fields={
                                "ID": id,
                                "Comment": COMMENT,
                                "Target lat": t_lat,
                                "Target lon": t_lon,
                                "Yaw": yaw,
                                "Pitch": pitch,
                                "Roll": roll,
                                "Humidity": humidity,
                                "RX count": rx_count,
                                "Abort": receivedAbort,
                            },
                        )

                        data = {
                            "Lat": lat,
                            "Lon": lon,
                            "Alt": alt,
                            "Target lat": t_lat,
                            "Target lon": t_lon,
                            "Yaw": yaw,
                            "Pitch": pitch,
                            "Roll": roll,
                            "Time": utc_time_str,
                            "Temperature": temp,
                            "Pressure": pressure,
                            "Humidity": humidity,
                            "Voltage": volts,
                            "Abort": receivedAbort,
                            "TX count": tx_count,
                            "RX count": rx_count,
                            "RSSI": rssi,
                            "SNR": snr,
                            "Uploader lat": u_lat,
                            "Uploader lon": u_lon,
                            "Uploader alt": u_alt,
                            "ID": id,
                        }

                        df = pd.DataFrame([data])

                        file = f"{cwd}/data/{call_sign}.csv"
                        df.to_csv(
                            file,
                            mode="a",
                            header=not pd.io.common.file_exists(file),
                            index=False,
                        )

                # # Example: Create a response JSON
                # response = {
                #     "message": "Data received successfully",
                #     "chipId": chip_id,
                #     "status": status,
                # }

                # # Send response back to the client
                # await websocket.send(json.dumps(response))
                # print("Response sent.")
            except json.JSONDecodeError:
                print("Error: Received data is not valid JSON")
                # await websocket.send(json.dumps({"error": "Invalid JSON"}))

    except websockets.ConnectionClosed:
        print("Connection closed")
    except Exception as e:
        print(f"Error: {e}")


# Main WebSocket server coroutine
async def main():
    print(f"Starting server on ws://0.0.0.0:{PORT}")
    async with websockets.serve(handle_connection, "0.0.0.0", PORT):
        await asyncio.Future()  # Run forever


# Run the server
if __name__ == "__main__":
    asyncio.run(main())
