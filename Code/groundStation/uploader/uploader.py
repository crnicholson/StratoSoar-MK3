import threading
import struct
import datetime
import time
from sondehub.amateur import Uploader
import serial

baud_rate = 115200
serial_port = "/dev/cu.usbserial-A50285BI"  # Replace with your serial port.
lora_update_rate = 5000  # Update rate in milliseconds of the sender.

# Uploader configuration
call_sign = "KC1SFR"  # Change to your ham radio call sign.
comment = "StratoSoar MK3 high altitude glider."
antenna = "Wire monopole"
radio = "SX1278 LoRa module"
freq = "433.000"  # Frequency in MHz of the received telemetry.

ser = serial.Serial(serial_port, baud_rate)

# Initialize target coordinates
t_lat = 0.0
t_lon = 0.0


def read_serial_data():
    global t_lat, t_lon
    while True:
        if ser.in_waiting > 0:
            # Read and unpack the data
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
                    "RX count": rx_count,
                },
            )

            print(
                f"Data received:\n Latitude: {lat}\n Longitude: {lon}\n Altitude: {alt}\n Target Latitude: {t_lat}\n Target Longitude: {t_lon}\n"
            )
        time.sleep(0.1)  # Add a small delay to prevent high CPU usage.


def user_input_loop():
    global t_lat, t_lon
    while True:
        try:
            prev_t_lat = t_lat
            prev_t_lon = t_lon
            print(f"Current target lat: {t_lat}, lon: {t_lon}")
            t_lat = float(input("Enter new target lat, at least 4 decimal places: "))
            t_lon = float(input("Enter new target lon, at least 4 decimal places: "))
            if len(str(t_lat).split(".")[1]) < 4 or len(str(t_lon).split(".")[1]) < 4:
                print(
                    "Both latitude and longitude must have at least 4 decimal places."
                )
                t_lat, t_lon = prev_t_lat, prev_t_lon
            else:
                ser.write(struct.pack("f", t_lat))
                ser.write(struct.pack("f", t_lon))
                ser.flush()
                print(f"New target lat: {t_lat}, lon: {t_lon} sent to Arduino.")
        except Exception as e:
            print(f"Error: {e}. Retaining previous target coordinates.")
            t_lat, t_lon = prev_t_lat, prev_t_lon


if __name__ == "__main__":
    threading.Thread(target=read_serial_data, daemon=True).start()

    user_input_loop()
