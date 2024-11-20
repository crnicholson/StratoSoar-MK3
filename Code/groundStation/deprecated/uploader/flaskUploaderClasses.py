from flask import Flask, request, jsonify
from flask_cors import CORS
import pandas as pd
import os
import datetime
from sondehub.amateur import Uploader
import threading
from threading import Lock
import decimal

cwd = os.getcwd()

app = Flask(__name__)
CORS(app)

# Uploader configuration
call_sign = "KC1SFR"  # Change to your ham radio call sign.
comment = "StratoSoar MK3 high altitude glider."
antenna = "Wire monopole"
radio = "SX1278 LoRa module"
freq = "433.000"  # Frequency in MHz of the received telemetry.


# State Manager Class
class StateManager:
    def __init__(self):
        self._lock = Lock()
        self._state = {
            "abort": 0,
            "new_t_lat": 0,
            "new_t_lon": 0,
            "t_lat": 0,
            "t_lon": 0,
            "received_abort": 0,
        }

    def get_state(self, key):
        with self._lock:
            return self._state.get(key, None)

    def set_state(self, key, value):
        with self._lock:
            self._state[key] = value


# Instantiate the state manager
state_manager = StateManager()


def input_handler():
    while True:
        try:
            t_lat = state_manager.get_state("t_lat")
            t_lon = state_manager.get_state("t_lon")
            received_abort = state_manager.get_state("received_abort")
            print(f"Current received target lat and lon: {t_lat}, {t_lon}.")

            abort_status = state_manager.get_state("abort")
            if abort_status == 0:
                print("There is NO LOCAL ABORT set.")
            else:
                print("There is a local abort set.")

            if received_abort == 0:
                print("There is NO RECEIVED ABORT.")
            else:
                print("The glider's mission has been successfully aborted.")

            command = input("\nEnter command (set_lat_lon/abort): \n").strip()
            if command == "set_lat_lon":
                print(
                    "Please enter a latitude and longitude with at least 4 decimal places."
                )
                new_t_lat = float(input("Enter new target latitude: "))
                new_t_lon = float(input("Enter new target longitude: "))

                str_lat = decimal.Decimal(str(new_t_lat))
                str_lon = decimal.Decimal(str(new_t_lon))
                if (
                    abs(str_lat.as_tuple().exponent) < 4
                    or abs(str_lon.as_tuple().exponent) < 4
                ):
                    print("Latitude and Longitude must have at least 4 decimal places.")
                else:
                    state_manager.set_state("new_t_lat", new_t_lat)
                    state_manager.set_state("new_t_lon", new_t_lon)
                    print(f"Target location updated to {new_t_lat}, {new_t_lon}.")

            elif command == "abort":
                if abort_status == 1:
                    print("The glider has already been set to abort.")
                else:
                    confirmation = input(
                        'Type "abort" again to confirm, anything else to cancel:\n'
                    )
                    if confirmation == "abort":
                        state_manager.set_state("abort", 1)
                        print("Abort set.")
            else:
                print('Invalid command. Use "set_lat_lon" or "abort".')
        except ValueError:
            print(
                "Invalid input. Please enter numerical values for latitude and longitude."
            )


@app.route("/upload-station-data", methods=["POST"])
def upload_station_data():
    received = request.get_json()
    if received["callsign"] == call_sign:
        lat = received["lat"]
        lon = received["lon"]
        alt = received["altitude"]
        state_manager.set_state("t_lat", received["tLat"])
        state_manager.set_state("t_lon", received["tLon"])
        state_manager.set_state("received_abort", received["abort"])

        utc_time = datetime.datetime.utcnow().replace(
            hour=received["hour"],
            minute=received["minute"],
            second=received["second"],
            microsecond=0,
        )
        utc_time_str = utc_time.strftime("%Y-%m-%dT%H:%M:%SZ")

        uploader = Uploader(
            call_sign,
            uploader_position=[received["u_lat"], received["u_lon"], received["u_alt"]],
            uploader_radio=radio,
            uploader_antenna=antenna,
        )

        uploader.add_telemetry(
            call_sign,
            utc_time_str,
            lat,
            lon,
            alt,
            frame=received["txCount"],
            batt=received["volts"],
            temp=received["temperature"],
            pressure=received["pressure"],
            snr=received["snr"],
            frequency=freq,
            rssi=received["rssi"],
            modulation="LoRa",
            extra_fields={
                "Comment": comment,
                "Target lat": received["tLat"],
                "Target lon": received["tLon"],
                "Yaw": received["yaw"],
                "Pitch": received["pitch"],
                "Roll": received["roll"],
                "Humidity": received["humidity"],
                "RX count": received["rxCount"],
                "Abort": received["abort"],
            },
        )

        data = {
            "Lat": lat,
            "Lon": lon,
            "Alt": alt,
            "Target lat": received["tLat"],
            "Target lon": received["tLon"],
            "Time": utc_time_str,
            "Temperature": received["temperature"],
            "Pressure": received["pressure"],
            "Humidity": received["humidity"],
            "Voltage": received["volts"],
            "Abort": received["abort"],
            "TX count": received["txCount"],
            "RX count": received["rxCount"],
            "RSSI": received["rssi"],
            "SNR": received["snr"],
            "Uploader lat": received["u_lat"],
            "Uploader lon": received["u_lon"],
            "Uploader alt": received["u_alt"],
        }

        df = pd.DataFrame([data])
        file = f"{cwd}/data.csv"
        df.to_csv(
            file, mode="a", header=not pd.io.common.file_exists(file), index=False
        )

        return "Data uploaded successfully.", 200
    return "Wrong callsign.", 400


@app.route("/change-glider-data", methods=["POST"])
def change_target():
    received = request.get_json()
    abort_status = state_manager.get_state("abort")
    new_t_lat = state_manager.get_state("new_t_lat")
    new_t_lon = state_manager.get_state("new_t_lon")

    if received["callsign"] == call_sign:
        if new_t_lat == 0 or new_t_lon == 0:
            print("Latitude or longitude are 0. Using current target location.")
            new_t_lat = state_manager.get_state("t_lat")
            new_t_lon = state_manager.get_state("t_lon")
        return (
            jsonify({"tLat": new_t_lat, "tLon": new_t_lon, "abort": abort_status}),
            200,
        )

    return "Wrong callsign.", 400


if __name__ == "__main__":
    input_thread = threading.Thread(target=input_handler)
    input_thread.daemon = True
    input_thread.start()

    app.run(debug=True, port=41773)
