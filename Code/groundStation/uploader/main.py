import asyncio
import websockets
import json
from sondehub.amateur import Uploader
import pandas as pd
import os
import datetime
import threading
from flask import Flask, request, jsonify
from flask_cors import CORS
import numpy as np

WEBSOCKET_PORT = 41773  # Match this with the port in your ESP32 code.
HTTP_PORT = 5000
COMMENT = "StratoSoar MK3 high altitude glider."
ANTENNA = "Wire monopole"
RADIO = "SX1278 LoRa module"
FREQ = "433.000"  # Frequency in MHz of the received telemetry.

cwd = os.getcwd()

last_print = 0

app = Flask(__name__)
CORS(app)


# Handler for incoming WebSocket connections.
async def handle_connection(websocket):
    global last_print
    print("New connection established")
    # try:
    # Keep listening for messages from the client.
    async for message in websocket:
        # print(f"Received: {message}")
        try:
            received = json.loads(message)
            # print(f"Parsed JSON: {received}")

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
            u_lat = received["uLat"]
            u_lon = received["uLon"]
            u_alt = received["uAlt"]
            rssi = received["rssi"]
            snr = received["snr"]
            call_sign = received["callsign"]
            id = received["id"]

            utc_time = datetime.datetime.now(datetime.UTC).replace(
                year=year,
                month=month,
                day=day,
                hour=hour,
                minute=minute,
                second=second,
                microsecond=0,
            )
            utc_time_str = utc_time.strftime("%Y-%m-%dT%H:%M:%SZ")

            print(f"\nReceived data from {call_sign} at {utc_time_str}")

            df = pd.DataFrame()

            if os.path.exists(f"{cwd}/data/{call_sign}.csv"):
                df = pd.read_csv(f"{cwd}/data/{call_sign}.csv")

            if not df.empty and df.iloc[-1]["Time"] == utc_time_str:
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

                print(
                    f"Lat: {lat}, lon: {lon}, alt: {alt}, target lat: {t_lat}, target lon: {t_lon}, yaw: {yaw}, pitch: {pitch}, roll: {roll}, time: {utc_time_str}, temp: {temp}, pressure: {pressure}, humidity: {humidity}, volts: {volts}, received abort: {received_abort}, tx count: {tx_count}, rx count: {rx_count}, rssi: {rssi}, snr: {snr}, uploader lat: {u_lat}, uploader lon: {u_lon}, uploader alt: {u_alt}, id: {id}"
                )

                df = pd.DataFrame([data])

                file = f"{cwd}/data/{call_sign}.csv"
                df.to_csv(
                    file,
                    mode="a",
                    header=not pd.io.common.file_exists(file),
                    index=False,
                )

                if os.path.exists(f"{cwd}/data/{call_sign}_to_glider.csv"):
                    df = pd.read_csv(f"{cwd}/data/{call_sign}_to_glider.csv")

                    response = {
                        "callSign": call_sign,
                        "tLat": df.iloc[-1]["tLat"],
                        "tLon": df.iloc[-1]["tLon"],
                        "abort": df.iloc[-1]["abort"],
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

            if datetime.datetime.now().second - last_print > 60:
                print("Printing stations receiving in the last 60 seconds.")
                df = pd.read_csv(f"{cwd}/stations.csv")
                last_print = datetime.datetime.now().second
                for i in range(len(df)):
                    if (
                        datetime.datetime.now(datetime.UTC)
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
        except json.JSONDecodeError:
            await websocket.send(json.dumps({"error": "Invalid JSON format"}))
            print("Error: Invalid JSON format")
            continue
    # except Exception as e:
    #     print(f"Error: {e}")
    # except websockets.ConnectionClosed:
    #     print("Connection closed")


def input_thread():
    while True:
        try:
            print("\nAvialable call signs:")
            print(
                [
                    file.replace(".csv", "")
                    for file in os.listdir(f"{os.getcwd()}/data")
                    if "glider" not in file
                ]
            )
            s_call_sign = input("Enter the call sign you would like to change:\n")
            if s_call_sign + ".csv" in os.listdir(f"{cwd}/data"):
                df = pd.read_csv(f"{cwd}/data/{s_call_sign}.csv")

                print(f"\nData for {s_call_sign}:")
                print(
                    f"Lat: {df.iloc[-1]['Lat']}, lon: {df.iloc[-1]['Lon']}, alt: {df.iloc[-1]['Alt']}, last received at {df.iloc[-1]['Time']}, voltage: {df.iloc[-1]['Voltage']}"
                )
                print(f"Abort status: {df.iloc[-1]['Received abort']}")
                print(f"Target latitude: {df.iloc[-1]['Target lat']}")
                print(f"Target longitude: {df.iloc[-1]['Target lon']}")

                new_t_lat = float(
                    input("\nEnter the new target latitude. Enter 0 to do nothing:\n")
                )
                new_t_lon = float(
                    input("Enter the new target longitude. Enter 0 to do nothing:\n")
                )
                abort = int(
                    input(
                        "Enter 1 to abort the mission or 0 to do nothing or revert your change:\n"
                    )
                )

                if new_t_lat == 0:
                    new_t_lat = df.iloc[-1]["Target lat"]
                if new_t_lon == 0:
                    new_t_lon = df.iloc[-1]["Target lon"]

                if abort != 0 and abort != 1:
                    print(f"Abort changed to {abort}, which is not 0 or 1.")
                    abort = input("Please enter 0 (do nothing) or 1 (abort mission):\n")
                    if abort != 0 and abort != 1:
                        print("Abort status still invalid. Resorting to orginal value.")
                        abort = df.iloc[-1]["Received abort"]

                if os.path.exists(f"{cwd}/data/{s_call_sign}_to_glider.csv"):
                    df = pd.read_csv(f"{cwd}/data/{s_call_sign}_to_glider.csv")

                    data = {
                        "tLat": new_t_lat,
                        "tLon": new_t_lon,
                        "abort": abort,
                    }

                    df = df._append(data, ignore_index=True)

                    df.to_csv(f"{cwd}/data/{s_call_sign}_to_glider.csv", index=False)
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
                    f"\nData for {s_call_sign} has been changed to coordinates {new_t_lat, new_t_lon} with abort status of {abort}. Sending to ground station shortly."
                )
            else:
                print(f"{s_call_sign} not found in data directory.")
        except ValueError:
            print(
                "Invalid input. Please enter numerical values for latitude and longitude."
            )


@app.route("/api/get-stations", methods=["POST"])
def get_stations():
    df = pd.read_csv(f"{cwd}/stations.csv")
    recent_stations = []

    for i in range(len(df)):
        station_time = datetime.datetime.strptime(
            df.loc[i, "Time"], "%Y-%m-%dT%H:%M:%SZ"
        )
        time_diff = datetime.datetime.now(datetime.timezone.utc) - station_time

        if time_diff.total_seconds() < 60:
            station_data = {
                "ID": df.loc[i, "ID"],
                "Call sign": df.loc[i, "Call sign"],
                "Time": df.loc[i, "Time"],
                "SNR": df.loc[i, "SNR"],
                "RSSI": df.loc[i, "RSSI"],
            }
            recent_stations.append(station_data)  #
        else:
            if not recent_stations:
                print("No stations received data in the last 60 seconds.")

    return jsonify(recent_stations)


@app.route("/api/get-gliders", methods=["POST"])
def get_gliders():
    gliders = [
        file.replace(".csv", "")
        for file in os.listdir(f"{os.getcwd()}/data")
        if "glider" not in file
    ]
    return jsonify({"gliders": gliders}), 200


@app.route("/api/get-glider-data", methods=["POST"])
def get_data():
    received = request.get_json()
    call_sign = received["call_sign"]
    if not call_sign:
        return jsonify({"error": "Missing call_sign parameter"}), 400

    if not os.path.isfile(f"{cwd}/data/{call_sign}.csv"):
        return jsonify({"error": f"Data file for {call_sign} not found"}), 404

    try:
        df = pd.read_csv(f"{cwd}/data/{call_sign}.csv")
    except Exception as e:
        return jsonify({"error": f"Error reading data file: {str(e)}"}), 500

    if df.empty:
        return jsonify({"error": "Data file is empty"}), 404

    latest_data = df.iloc[-1]

    def safe_get(column):
        value = latest_data.get(column, None)
        if pd.isna(value):
            return None
        # Convert numpy types to native Python types
        if isinstance(value, (np.int64, np.int32)):
            return int(value)
        elif isinstance(value, (np.float64, np.float32)):
            return float(value)
        return value

    response_data = {
        "Lat": safe_get("Lat"),
        "Lon": safe_get("Lon"),
        "Alt": safe_get("Alt"),
        "Target_Lat": safe_get("Target_Lat"),
        "Target_Lon": safe_get("Target_Lon"),
        "Yaw": safe_get("Yaw"),
        "Pitch": safe_get("Pitch"),
        "Roll": safe_get("Roll"),
        "Time": safe_get("Time"),
        "Temp": safe_get("Temp"),
        "Pressure": safe_get("Pressure"),
        "Humidity": safe_get("Humidity"),
        "Volts": safe_get("Voltage"),
        "Received_Abort": safe_get("Received_Abort"),
        "Tx_Count": safe_get("Tx_Count"),
        "Rx_Count": safe_get("Rx_Count"),
        "RSSI": safe_get("RSSI"),
        "SNR": safe_get("SNR"),
        "Uploader_Lat": safe_get("Uploader_Lat"),
        "Uploader_Lon": safe_get("Uploader_Lon"),
        "Uploader_Alt": safe_get("Uploader_Alt"),
        "ID": safe_get("ID"),
    }

    return jsonify(response_data)


@app.route("/api/change-glider-data", methods=["POST"])
def change_data():
    received = request.get_json()
    call_sign = received["call_sign"]
    abort = received["abort"]
    new_t_lat = received["t_lat"]
    new_t_lon = received["t_lon"]

    if not os.path.isfile(f"{cwd}/data/{call_sign}.csv"):
        return jsonify({"error": f"Data file for {call_sign} not found"}), 404

    try:
        df = pd.read_csv(f"{cwd}/data/{call_sign}.csv")
    except Exception as e:
        return jsonify({"error": f"Error reading data file: {str(e)}"}), 500

    if new_t_lat == 0 or new_t_lon == None:
        new_t_lat = df.iloc[-1]["Target lat"]
    if new_t_lon == 0 or new_t_lon == None:
        new_t_lon = df.iloc[-1]["Target lon"]
    if abort == None:
        abort = df.iloc[-1]["Received abort"]

    if os.path.exists(f"{cwd}/data/{call_sign}_to_glider.csv"):
        df = pd.read_csv(f"{cwd}/data/{call_sign}_to_glider.csv")

        data = {
            "tLat": new_t_lat,
            "tLon": new_t_lon,
            "abort": abort,
        }

        df = df._append(data, ignore_index=True)

        df.to_csv(f"{cwd}/data/{call_sign}_to_glider.csv", index=False)
    else:
        data = {
            "tLat": new_t_lat,
            "tLon": new_t_lon,
            "abort": abort,
        }

        df = pd.DataFrame([data])

        file = f"{cwd}/data/{call_sign}_to_glider.csv"
        df.to_csv(
            file,
            mode="a",
            header=not pd.io.common.file_exists(file),
            index=False,
        )

    return jsonify({"message": "Success"}), 200


# Main WebSocket server coroutine.
async def main():
    print(f"Starting server on ws://0.0.0.0:{WEBSOCKET_PORT}")
    async with websockets.serve(handle_connection, "0.0.0.0", WEBSOCKET_PORT):
        await asyncio.Future()  # Run forever.


if __name__ == "__main__":
    app.run(debug=True, port=HTTP_PORT)
    threading.Thread(target=input_thread).start()
    asyncio.run(main())
