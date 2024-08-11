from flask import Flask, request, jsonify
from flask_cors import CORS
import pandas as pd
import os

cwd = os.getcwd()

app = Flask(__name__)
CORS(app)

callsign = "KC1SFR"

tLat = 0 # 0 by default means glider uses programmed target location.
tLon = 0 

abort = 0 # 0 is don't abort, 1 is abort.

@app.route("/upload-station-data", methods=["POST"])
def upload_station_data():
    received = request.get_json()
    if received["callsign"] == callsign:
        received["lat"] = received.lat;
        received["lon"] = received.lon;
        received["altitude"] = received.altitude;
        received["tLat"] = received.tLat;
        received["tLon"] = received.tLon;
        received["temperature"] = received.temperature;
        received["pressure"] = received.pressure;
        received["humidity"] = received.humidity;
        received["volts"] = received.volts;
        received["yaw"] = received.yaw;
        received["pitch"] = received.pitch;
        received["roll"] = received.roll;
        received["hour"] = received.hour;
        received["minute"] = received.minute;
        received["second"] = received.second;
        received["abort"] = received.abort;
        received["txCount"] = received.txCount;
        received["rxCount"] = rxCount;
        received["U_LAT"] = U_LAT; // Uploader location!
        received["U_LON"] = U_LON;
        received["U_ALT"] = U_ALT;
        received["rssi"] = rssi;
        received["snr"] = snr;
        received["callsign"] = received.callSign;
        
        lat = received["lat"]
        lon = received["lon"]
        altitude = received["altitude"]
        tLat = received["tLat"]
        tLon = received["tLon"]
        temperature = received["temperature"]
        pressure = received["pressure"]
        humidity = received["humidity"]
        volts = received["volts"]
        yaw = received["yaw"]
        pitch = received["pitch"]
        roll = received["roll"]
        hour = received["hour"]
        minute = received["minute"]
        second = received["second"]
        txCount = received["txCount"]
        rxCount = received["rxCount"]
        U_LAT = received["U_LAT"]
        U_LON = received["U_LON"]
        U_ALT = received["U_ALT"]
        rssi = received["rssi"]
        snr = received["snr"]
            
    return "Wrong callsign.", 400


@app.route("/change-glider-data", methods=["POST"])
def change_target():
    received = request.get_json()
    if received["callsign"] == callsign:
        return jsonify({"tLat": tLat, "tLon": tLon}), 200
    return "Wrong callsign.", 400
