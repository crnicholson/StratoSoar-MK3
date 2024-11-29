import asyncio
import websockets
import json
from sondehub.amateur import Uploader
import pandas as pd
import os
import datetime
import threading

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
                received_abort = received["abort"]
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

                df = pd.DataFrame()

                if os.path.exists(f"{cwd}/data/{call_sign}.csv"):
                    df = pd.read_csv(f"{cwd}/data/{call_sign}.csv")

                if not df.empty and df.loc[-1, "Time"] == utc_time_str:
                    print("Duplicate data received. Ignoring.")
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
                            "Abort": received_abort,
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
                        "Received abort": received_abort,
                        "TX count": tx_count,
                        "RX count": rx_count,
                        "RSSI": rssi,
                        "SNR": snr,
                        "Uploader lat": u_lat,
                        "Uploader lon": u_lon,
                        "Uploader alt": u_alt,
                        "ID": id,
                    }
                    
                    print(f"Lat: {lat}, lon: {lon}, alt: {alt}, target lat: {t_lat}, target lon: {t_lon}, yaw: {yaw}, pitch: {pitch}, roll: {roll}, time: {utc_time_str}, temp: {temp}, pressure: {pressure}, humidity: {humidity}, volts: {volts}, received abort: {received_abort}, tx count: {tx_count}, rx count: {rx_count}, rssi: {rssi}, snr: {snr}, uploader lat: {u_lat}, uploader lon: {u_lon}, uploader alt: {u_alt}, id: {id}")

                    df = pd.DataFrame([data])

                    file = f"{cwd}/data/{call_sign}.csv"
                    df.to_csv(
                        file,
                        mode="a",
                        header=not pd.io.common.file_exists(file),
                        index=False,
                    )

                    df = pd.read_csv(f"{cwd}/data/{call_sign}_to_glider.csv")

                    response = {
                        "callSign": call_sign,
                        "tLat": df.loc[-1, "tLat"],
                        "tLon": df.loc[-1, "tLon"],
                        "abort": df.loc[-1, "abort"],
                    }

                    # Send response back to the client.
                    await websocket.send(json.dumps(response))
                    print("Response sent.")

                # This creates a list of all stations that have received data. It also prints active stations.
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
                    print("Printing stations receiving in the last 60 seconds.")
                    df = pd.read_csv(f"{cwd}/stations.csv")
                    last_print = datetime.datetime.second()
                    for i in range(len(df)):
                        if (
                            datetime.datetime.utcnow()
                            - datetime.datetime.strptime(
                                df.loc[i, "Time"], "%Y-%m-%dT%H:%M:%SZ"
                            )
                        ).seconds < 60:
                            print(
                                df.loc[i, "ID"],
                                "received data from",
                                df.loc[i, "Call sign"],
                                "at",
                                df.loc[i, "Time"],
                                "with SNR",
                                df.loc[i, "SNR"],
                                "and RSSI",
                                df.loc[i, "RSSI"],
                            )
                        else:
                            print("No stations received data in the last 60 seconds.")

            except json.JSONDecodeError:
                print("Error: Received data is not valid JSON")
                # await websocket.send(json.dumps({"error": "Invalid JSON"}))

    except websockets.ConnectionClosed:
        print("Connection closed")
    except Exception as e:
        print(f"Error: {e}")


def input_thread():
    while True:
        try:
            print("Avialable call signs:")
            os.listdir(f"{cwd}/data").replace(".csv", "")
            s_call_sign = input("Enter the call sign you would like to change:\n")
            if s_call_sign in os.listdir(f"{cwd}/data").replace(".csv", ""):
                df = pd.read_csv(f"{cwd}/data/{s_call_sign}.csv")

                print(f"Lat: {df.loc[-1, 'Lat']}, lon: {df.loc[-1, 'Lon']}, alt: {df.loc[-1, 'Alt']}, last received at {df.loc[-1, 'Time']}, voltage: {df.loc[-1, 'Voltage']}")
                print(f"Abort status: {df.loc[-1, "Abort"]}")
                print(f"Target latitude: {df.loc[-1, "Target lat"]}")
                print(f"Target longitude: {df.loc[-1, "Target lon"]}")

                new_t_lat = float(
                    input("Enter the new target latitude. Enter 0 to do nothing:\n")
                )
                new_t_lon = float(
                    input("Enter the new target longitude. Enter 0 to do nothing:\n")
                )
                abort = bool(
                    input(
                        "Enter 1 to abort the mission or 0 to do nothing or revert your change:\n"
                    )
                )

                if new_t_lat == 0:
                    new_t_lat = df.loc[-1, "Target lat"]
                if new_t_lon == 0:
                    new_t_lon = df.loc[-1, "Target lon"]

                if abort != 0 or 1:
                    print(f"Abort changed to {abort}, which is not 0 or 1.")
                    abort = input("Please enter 0 (do nothing) or 1 (abort mission):\n")
                    if abort != 0 or 1:
                        print("Abort status still invalid. Resorting to orginal value.")
                        abort = df.loc[-1, "Abort"]

                if os.path.exists(f"{cwd}/data/{s_call_sign}_to_glider.csv"):
                    df = pd.read_csv(f"{cwd}/data/{s_call_sign}_to_glider.csv")
                    df.loc[-1, "tLat"] = new_t_lat
                    df.loc[-1, "tLon"] = new_t_lon
                    df.loc[-1, "abort"] = abort
                else:
                    data = {
                        "tLat": new_t_lat,
                        "tLon": new_t_lon,
                        "abort": abort,
                    }

                    df = pd.DataFrame([data])

                    file = f"{cwd}/data/{s_call_sign}_to_glider.csv"
                    df.to_csv(
                        file,
                        mode="a",
                        header=not pd.io.common.file_exists(file),
                        index=False,
                    )

                print(
                    f"Data for {s_call_sign} has been changed to coordinates {new_t_lat, new_t_lon} with abort status of {abort}. Sending to ground station shortly."
                )
        except ValueError:
            print(
                "Invalid input. Please enter numerical values for latitude and longitude."
            )


# Main WebSocket server coroutine. 
async def main():
    print(f"Starting server on ws://0.0.0.0:{PORT}")
    async with websockets.serve(handle_connection, "0.0.0.0", PORT):
        await asyncio.Future()  # Run forever. 


if __name__ == "__main__":
    threading.Thread(target=input_thread).start()
    asyncio.run(main())