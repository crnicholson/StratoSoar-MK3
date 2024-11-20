from flask import Flask, request, jsonify
from flask_cors import CORS
import pandas as pd
import os
import datetime
from sondehub.amateur import Uploader
import threading
import decimal

cwd = os.getcwd()

app = Flask(__name__)
CORS(app)

# Configuration
CALL_SIGN = "KC1SFR"  # Change to your ham radio call sign. At the moment, you'll need to run your own instance of this server with your own call sign. 
COMMENT = "StratoSoar MK3 high altitude glider."
ANTENNA = "Wire monopole"
RADIO = "SX1278 LoRa module"
FREQ = "433.000"  # Frequency in MHz of the received telemetry.


def validate_coordinates(lat, lon, current_lat, current_lon):
    try:
        if lat == 0 or lon == 0:
            print(
                "Latitude or longitude equal 0. Glider will use current target location."
            )
            return current_lat, current_lon

        str_lat = decimal.Decimal(str(lat))
        str_lon = decimal.Decimal(str(lon))
        if abs(str_lat.as_tuple().exponent) < 4 or abs(str_lon.as_tuple().exponent) < 4:
            print("Latitude or longitude must have at least 4 decimal places.")
            return current_lat, current_lon

        return lat, lon
    except decimal.InvalidOperation:
        print(
            "Invalid latitude or longitude input. Retaining current target coordinates."
        )
        return current_lat, current_lon


def input_handler(state):
    while True:
        print(
            f"Current received target lat and lon: {state['t_lat']}, {state['t_lon']}."
        )
        print(f"Abort status: {'Aborted' if state['receivedAbort'] else 'Not Aborted'}")

        command = input("Enter command (set_lat_lon/abort): ").strip()
        if command == "set_lat_lon":
            try:
                new_lat = float(input("Enter new target latitude: "))
                new_lon = float(input("Enter new target longitude: "))
                state["t_lat"], state["t_lon"] = validate_coordinates(
                    new_lat, new_lon, state["t_lat"], state["t_lon"]
                )
                print(
                    f"Target coordinates updated to {state['t_lat']}, {state['t_lon']}."
                )
            except ValueError:
                print(
                    "Invalid input. Please enter numerical values for latitude and longitude."
                )
        elif command == "abort":
            if state["abort"] == 1:
                print("Abort has already been set. No further action required.")
            else:
                confirmation = input(
                    "Type 'abort' again to confirm, or anything else to cancel: "
                ).strip()
                if confirmation == "abort":
                    state["abort"] = 1
                    print(
                        "Abort signal will be sent on the next ground station beacon."
                    )
                else:
                    print("Abort canceled.")
        else:
            print("Invalid command. Please type 'set_lat_lon' or 'abort'.")


@app.route("/upload-station-data", methods=["POST"])
def upload_station_data():
    received = request.get_json()
    if received["callsign"] == CALL_SIGN:
        state = upload_station_data.state
        state["t_lat"] = received["tLat"]
        state["t_lon"] = received["tLon"]
        state["receivedAbort"] = received["abort"]

        utc_time = (
            datetime.datetime.utcnow()
            .replace(
                hour=received["hour"],
                minute=received["minute"],
                second=received["second"],
                microsecond=0,
            )
            .strftime("%Y-%m-%dT%H:%M:%SZ")
        )

        uploader = Uploader(
            CALL_SIGN,
            uploader_position=[received["u_lat"], received["u_lon"], received["u_alt"]],
            uploader_radio=RADIO,
            uploader_antenna=ANTENNA,
        )

        uploader.add_telemetry(
            CALL_SIGN,
            utc_time,
            received["lat"],
            received["lon"],
            received["altitude"],
            frame=received["txCount"],
            batt=received["volts"],
            temp=received["temperature"],
            pressure=received["pressure"],
            snr=received["snr"],
            frequency=FREQ,
            rssi=received["rssi"],
            modulation="LoRa",
            extra_fields={
                "Comment": COMMENT,
                "Target lat": state["t_lat"],
                "Target lon": state["t_lon"],
                "Yaw": received["yaw"],
                "Pitch": received["pitch"],
                "Roll": received["roll"],
                "Humidity": received["humidity"],
                "RX count": received["rxCount"],
                "Abort": "aborted." if state["receivedAbort"] else "not aborted.",
            },
        )

        file = f"{cwd}/data.csv"
        pd.DataFrame([received]).to_csv(
            file, mode="a", header=not os.path.exists(file), index=False
        )

        return "Data uploaded successfully.", 200
    return "Wrong callsign.", 400


@app.route("/change-glider-data", methods=["POST"])
def change_target():
    received = request.get_json()
    if received["callsign"] == CALL_SIGN:
        state = upload_station_data.state
        return (
            jsonify(
                {
                    "tLat": state["t_lat"],
                    "tLon": state["t_lon"],
                    "abort": state["abort"],
                }
            ),
            200,
        )
    return "Wrong callsign.", 400


if __name__ == "__main__":
    state = {"t_lat": 0, "t_lon": 0, "abort": 0, "receivedAbort": 0}

    upload_station_data.state = state

    input_thread = threading.Thread(target=input_handler, args=(state,))
    input_thread.daemon = True
    input_thread.start()

    app.run(debug=True, port=41773)
