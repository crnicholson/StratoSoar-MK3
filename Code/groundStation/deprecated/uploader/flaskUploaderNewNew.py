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

# Uploader configuration
call_sign = "KC1SFR"  # Change to your ham radio call sign.
comment = "StratoSoar MK3 high altitude glider."
antenna = "Wire monopole"
radio = "SX1278 LoRa module"
freq = "433.000"  # Frequency in MHz of the received telemetry.

# Probably don't need to change these/.
new_t_lat = 0  # 0 by default means glider uses programmed target location.
new_t_lon = 0
abort = 0
received_abort = 0
t_lat = 0
t_lon = 0


# This is all shitty code, and I don't care.
def get_abort():
    global abort
    return abort


def change_abort(status):
    global abort
    abort = status


def get_lat_lon():
    return new_t_lat, new_t_lon


def change_lat(new_lat):
    global new_t_lat
    new_t_lat = new_lat


def change_lon(new_lon):
    global new_t_lon
    new_t_lon = new_lon


def input_handler():
    while True:
        try:
            print(f"\nCurrent received target lat and lon: {t_lat}, {t_lon}.")
            abort_status = get_abort()
            if abort_status == 0:
                print("There is NO LOCAL ABORT set.")
            if abort_status == 1:
                print("There is a local abort set.")
            if received_abort == 0:
                print("There is NO RECEIVED ABORT.")
            if received_abort == 1:
                print("The glider's mission has been succesfully aborted.")

            command = input("\nEnter command (set_lat_lon/abort): \n").strip()
            if command == "set_lat_lon":
                print(
                    "Please enter a latitude and longitude with at least 4 decimal places. You can always come back later and change it."
                )
                new_t_lat = float(input("Enter new target latitude: "))
                new_t_lon = float(input("Enter new target longitude: "))
                change_lat(new_t_lat)
                change_lon(new_t_lon)
                if new_t_lat == 0 or new_t_lon == 0:
                    print(
                        "Latitude or longitude equal 0. Glider will use current target location."
                    )
                    new_t_lat = t_lat
                    new_t_lon = t_lon
                    change_lat(new_t_lat)
                    change_lon(new_t_lon)
                str_lat = decimal.Decimal(str((new_t_lat)))
                if abs(str_lat.as_tuple().exponent) < 4:
                    print(
                        "Latitude must have at least 4 decimal places. The old target latitude will be used."
                    )
                    change_lat(new_t_lat)
                str_lon = decimal.Decimal(str((new_t_lon)))
                if abs(str_lon.as_tuple().exponent) < 4:
                    print(
                        "Longitude must have at least 4 decimal places. The old target longitude will be used."
                    )
                    change_lon(new_t_lon)

                new_t_lat, new_t_lon = get_lat_lon()
                print(f"Target location updated to {new_t_lat}, {new_t_lon}.")
                print(
                    "New target lat and lon will be sent on next ground station request."
                )
            elif command == "abort":
                abort_status = get_abort()
                if abort_status == 1:
                    print(
                        "The glider has already been set to abort. No further action required."
                    )
                else:
                    command = input(
                        '\nAborting is irreversible. Are you sure you want to continue? Type "abort" again to continue. It may take two tries. Type anything else to cancel.\n'
                    ).strip()
                    if command == "abort":
                        print("\nAborting the glider on next ground station beacon.")
                        change_abort(1)
                        abort_status = get_abort()
                        print(abort_status)
                    else:
                        print("Cancelling abort.")
            else:
                print(
                    'Invalid command, please type "set_lat_lon" or "abort" next time.'
                )
        except ValueError:
            print(
                "Invalid input. Please enter numerical values for latitude and longitude."
            )


@app.route("/upload-station-data", methods=["POST"])
def upload_station_data():
    global t_lat, t_lon
    received = request.get_json()
    if received["callsign"] == call_sign:
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

        if received_abort == 0:
            received_abort = "The glider has not been aborted."
        if received_abort == 1:
            received_abort = "The glider has been aborted."

        utc_time = datetime.datetime.utcnow().replace(
            hour=hour, minute=minute, second=second, microsecond=0
        )
        utc_time_str = utc_time.strftime("%Y-%m-%dT%H:%M:%SZ")

        uploader = Uploader(
            call_sign,
            uploader_position=[u_lat, u_lon, u_alt],
            uploader_radio=radio,
            uploader_antenna=antenna,
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
            frequency=freq,
            rssi=rssi,
            modulation="LoRa",
            extra_fields={
                "Comment": comment,
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
            "Abort": received_abort,
            "TX count": tx_count,
            "RX count": rx_count,
            "RSSI": rssi,
            "SNR": snr,
            "Uploader lat": u_lat,
            "Uploader lon": u_lon,
            "Uploader alt": u_alt,
        }

        df = pd.DataFrame([data])

        file = f"{cwd}/data.csv"
        df.to_csv(
            file,
            mode="a",
            header=not pd.io.common.file_exists(file),
            index=False,
        )

        return "Data uploaded successfully.", 200
    return "Wrong callsign.", 400


@app.route("/change-glider-data", methods=["POST"])
def change_target():
    received = request.get_json()
    new_t_lat, new_t_lon = get_lat_lon()
    abort_status = get_abort()
    print(abort_status)
    if received["callsign"] == call_sign:
        if new_t_lat == 0 or new_t_lon == 0:
            print(
                "Latitude or longitude equal 0. Glider will use current target location."
            )
            new_t_lat = t_lat
            new_t_lon = t_lon
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
