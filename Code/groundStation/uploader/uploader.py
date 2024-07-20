from sondehub.amateur import Uploader
import serial
import struct
import datetime
import time
from inputimeout import inputimeout

baud_rate = 115200
serial_port = "/dev/cu.usbserial-A50285BI"  # Replace this with your serial port (e.g., 'COM3' for Windows).
lora_update_rate = 5000  # Update rate in milliseconds of the sender.

# Uploader position, commented out because the data is sent from the ground station sketch.
# u_lon = 42.30
# u_lat = -71.30
# u_alt = 0

call_sign = "KC1SFR"  # Change this to your ham radio call sign.
comment = "StratoSoar MK3 high altitude glider."  # Put your comment here.
antenna = "Wire monopole"  # Your antenna, probably don't change.
radio = "SX1278 LoRa module"  # The type of LoRa module.
freq = "433.000"  # Frequency in MHz of the received telemetry.

ser = serial.Serial(serial_port, baud_rate)

while True:
    if ser.in_waiting > 0:
        lat = struct.unpack("f", ser.read(4))[0]
        lon = struct.unpack("f", ser.read(4))[0]
        alt = struct.unpack("f", ser.read(4))[0]
        t_lat = struct.unpack("f", ser.read(4))[0]
        t_lon = struct.unpack("f", ser.read(4))[0]
        temp = struct.unpack("i", ser.read(4))[0]
        pressure = struct.unpack("f", ser.read(4))[0]
        humidity = struct.unpack("i", ser.read(4))[0]
        volts = struct.unpack("f", ser.read(4))[0]
        yaw = struct.unpack("i", ser.read(4))[0]
        pitch = struct.unpack("i", ser.read(4))[0]
        roll = struct.unpack("i", ser.read(4))[0]
        hour = struct.unpack("i", ser.read(4))[0]
        minute = struct.unpack("i", ser.read(4))[0]
        second = struct.unpack("i", ser.read(4))[0]
        tx_count = struct.unpack("i", ser.read(4))[0]
        rx_count = struct.unpack("i", ser.read(4))[0]
        u_lat = struct.unpack("f", ser.read(4))[0]
        u_lon = struct.unpack("f", ser.read(4))[0]
        u_alt = struct.unpack("f", ser.read(4))[0]
        rssi = struct.unpack("i", ser.read(4))[0]
        snr = struct.unpack("i", ser.read(4))[0]
        ser.reset_input_buffer()

        utc_time = datetime.datetime.now(datetime.UTC)
        utc_time.replace(hour=hour, minute=minute, second=second, microsecond=0)
        utc_time = utc_time.strftime("%Y-%m-%dT%H:%M:%SZ")

        uploader = Uploader(call_sign)

        uploader = Uploader(
            call_sign,
            uploader_position=[u_lat, u_lon, u_alt],  # [latitude, longitude, altitude]
            uploader_radio=radio,  # Radio information - Optional
            uploader_antenna=antenna,  # Antenna information - Optional
        )

        uploader.add_telemetry(
            call_sign,  # Callsign
            utc_time,  # Time
            lat,
            lon,
            alt,
            frame=tx_count,  # Frame counter (e.g. packet number)
            batt=volts,  # Battery voltage (V)
            temp=temp,  # Temperature (C)
            pressure=pressure,  # Pressure (hPa)
            snr=snr,  # Signal-to-Noise Ratio of the received telemetry, in dB.
            frequency=freq,  # Frequency of the received telemetry, in MHz.
            rssi=rssi,  # Received Signal Strength, in dBm
            modulation="LoRa",
            extra_fields={
                "Comment": comment,
                "Target lat:": t_lat,
                "Target lon:": t_lon,
                "Yaw:": yaw,
                "Pitch:": pitch,
                "Roll:": roll,
                "RX count:": rx_count,
            },
        )

        print(
            f"""
        Latitude: {lat}
        Longitude: {lon}
        Altitude: {alt}
        Target Latitude: {t_lat}
        Target Longitude: {t_lon}
        Temperature: {temp}
        Pressure: {pressure}
        Humidity: {humidity}
        Volts: {volts}
        Yaw: {yaw}
        Pitch: {pitch}
        Roll: {roll}
        Hour: {hour}
        Minute: {minute}
        Second: {second}
        TC Count: {tc_count}
        RX Count: {rx_count}
        Updated Latitude: {u_lat}
        Updated Longitude: {u_lon}
        Updated Altitude: {u_alt}
        RSSI: {rssi}
        SNR: {snr}
        """
        )
    else:
        start = int(round(time.time() * 1000))
        prev_t_lat = t_lat
        prev_t_lon = t_lon
        print(
            f"Put in new target lat and lon, you have {lora_update_rate} milliseconds."
        )
        print(f"Current target lat: {t_lat}, lon: {t_lon}")
        t_lat = float(
            inputimeout(
                prompt="Enter new target lat, at least 4 decimal places:",
                timeout=(lora_update_rate - 250) / 1000,
            )
        )
        millis = int(round(time.time() * 1000))
        t_lon = float(
            inputimeout(
                prompt="Enter new target lon, at least 4 decimal places:",
                timeout=((lora_update_rate - 250) - (millis - start)) / 1000,
            )
        )
        str_t_lat = str(t_lat)
        str_t_lon = str(t_lon)
        if len(str_t_lat.split(".")[1]) < 4:
            print("Latitude must have at least 4 decimal places.")
        if len(str_t_lon.split(".")[1]) < 4:
            print("Latitude must have at least 4 decimal places.")
        elif prev_t_lat != t_lat or prev_t_lon != t_lon:
            print(f"New target lat: {t_lat}, lon: {t_lon}")
            ser.write(struct.pack("f", t_lat))
            ser.write(struct.pack("f", t_lon))
            ser.flush()
        else:
            print("No valid input, keeping previous target lat and lon.")
            t_lat = prev_t_lat
            t_lon = prev_t_lon
