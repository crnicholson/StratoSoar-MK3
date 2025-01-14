from flask import Flask, request, jsonify
from flask_cors import CORS
from flask_socketio import SocketIO, emit
from sondehub.amateur import Uploader
import pandas as pd
import os
import datetime
import numpy as np

PORT = 36563
COMMENT = "StratoSoar MK3 high altitude glider."
ANTENNA = "Wire monopole"
RADIO = "SX1278 LoRa module"
FREQ = "433.000"  # Frequency in MHz of the received telemetry.

cwd = os.getcwd()
app = Flask(__name__)
CORS(app)
socketio = SocketIO(app, cors_allowed_origins="*")  # Enable SocketIO.

# /data: get sample shitty data. GET. Returns a JSON object.
# /greet: greet the user. GET. Returns a string.
# /hello: say hello to the world. GET. Returns a string.
# /api/get-stations: get the latest stations. POST. Returns a JSON object.
# /api/get-gliders: get the list of gliders. POST. Returns a JSON object.
# /api/get-glider-data: get the data of a glider. POST. Returns a JSON object. Need password and call sign and user name.
# /api/change-glider-data: change the data of a glider. POST. Returns a JSON object. Need password, call sign, and user name.

@app.route("/data", methods=["GET"])
def data():
    sample_data = {"id": 1, "name": "Sample Data", "value": 123}
    return jsonify(sample_data)


@app.route("/greet", methods=["GET"])
def greet():
    name = request.args.get("name", "Guest")
    return f"Hello, {name}!"


@app.route("/hello", methods=["GET"])
def hello():
    return "Hello, World!"


@app.route("/api/get-stations", methods=["POST"])
def get_stations():
    df = pd.read_csv(f"{cwd}/stations.csv")
    recent_stations = []

    for i in range(len(df)):
        station_time = datetime.datetime.strptime(
            df.loc[i, "Time"], "%Y-%m-%dT%H:%M:%SZ"
        )

        station_time = station_time.replace(tzinfo=datetime.timezone.utc)

        time_diff = datetime.datetime.now(datetime.timezone.utc) - station_time

        if time_diff.total_seconds() > 0:
            station_data = {
                "ID": int(df.loc[i, "ID"]),
                "Call sign": df.loc[i, "Call sign"],
                "Time": df.loc[i, "Time"],
                "SNR": df.loc[i, "SNR"],
                "RSSI": df.loc[i, "RSSI"],
            }
            recent_stations.append(station_data)
        else:
            if not recent_stations:
                print("No stations received data in the last 60 seconds.")

    return jsonify({"stations": recent_stations}), 200


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
    password = received["password"]

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
        if isinstance(value, (np.int64, np.int32)):
            return int(value)
        elif isinstance(value, (np.float64, np.float32)):
            return float(value)
        elif isinstance(value, np.bool_):  # Handle numpy boolean type
            return bool(value)
        return value

    if int(password) != int(safe_get("Password")):
        print("\nWRONG PASSWORD\n")
        print(f"Password: {password}")
        print(f"Correct password: {safe_get('Password')}\n")
        return jsonify({"error": "Wrong password!"}), 403

    response_data = {
        "lat": safe_get("Lat"),
        "lon": safe_get("Lon"),
        "alt": safe_get("Alt"),
        "t_lat": safe_get("Target lat"),
        "t_lon": safe_get("Target lon"),
        "yaw": safe_get("Yaw"),
        "pitch": safe_get("Pitch"),
        "roll": safe_get("Roll"),
        "time": safe_get("Time"),
        "temp": safe_get("Temp"),
        "pressure": safe_get("Pressure"),
        "humidity": safe_get("Humidity"),
        "volts": safe_get("Voltage"),
        "received_abort": safe_get("Received abort"),
        "tx_count": safe_get("TX count"),
        "rx_count": safe_get("RX count"),
        "rssi": safe_get("RSSI"),
        "snr": safe_get("SNR"),
        "u_lat": safe_get("Uploader lat"),
        "u_lon": safe_get("Uploader lon"),
        "u_alt": safe_get("Uploader alt"),
        # "user_1": safe_get("User 1"),
        # "user_2": safe_get("User 2"),
        # "user_3": safe_get("User 3"),
        # "user_4": safe_get("User 4"),
        # "user_5": safe_get("User 5"),
        "id": safe_get("ID"),
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


@socketio.on("connect")
def handle_connect():
    print("New WebSocket connection established.")


@socketio.on("data")
def handle_data(message):
    try:
        received = message

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
        user_1 = received["user1"]
        user_2 = received["user2"]
        user_3 = received["user3"]
        user_4 = received["user4"]
        user_5 = received["user5"]
        password = received["password"]
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
                "User 1": user_1,
                "User 2": user_2,
                "User 3": user_3,
                "User 4": user_4,
                "User 5": user_5,
                "Password": password,
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

                emit("response", response)
                print("Response sent.")

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
    except Exception as e:
        emit("error", {"error": str(e)})
        print(f"Error processing WebSocket data: {e}")


@socketio.on("disconnect")
def handle_disconnect():
    print("WebSocket client disconnected.")


if __name__ == "__main__":
    socketio.run(app, port=PORT, debug=True)
