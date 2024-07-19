from sondehub.amateur import Uploader
import serial
import struct
import datetime

baud_rate = 115200
serial_port = "/dev/cu.usbserial-A50285BI"  # Replace this with your serial port (e.g., 'COM3' for Windows).

# Uploader position. 
u_lon = 42.30
u_lat = -71.30
u_alt = 0

call_sign = "KC1SFR" # Change this to your ham radio call sign. 
comment = "StratoSoar MK3 high altitude glider." # Put your comment here. 
antenna = "Wire monopole" # Your antenna, probably don't change. 
radio = "SX1278 LoRa module" # The type of LoRa module.
freq = "433.000" # Frequency in MHz of the received telemetry.

ser = serial.Serial(serial_port, baud_rate)

while True:
    if ser.in_waiting > 0:
        lat = struct.unpack("f", ser.read(4))[0]
        lon = struct.unpack("f", ser.read(4))[0]
        alt = struct.unpack("i", ser.read(4))[0]
        speed = struct.unpack("i", ser.read(4))[0]
        tx_count = struct.unpack("i", ser.read(4))[0]
        rssi = struct.unpack("i", ser.read(4))[0]
        snr = struct.unpack("i", ser.read(4))[0]
        ser.reset_input_buffer()

        # Example double receiver (64 bits):
        # double = struct.unpack('d', ser.read(8))[0]

        current_utc_time = datetime.datetime.now(datetime.UTC)
        current_utc_time_string = current_utc_time.strftime("%Y-%m-%dT%H:%M:%SZ")

        uploader = Uploader(call_sign)

        uploader = Uploader(
            call_sign,
            uploader_position=[u_lat, u_lon, u_alt],  # [latitude, longitude, altitude]
            uploader_radio=radio,  # Radio information - Optional
            uploader_antenna=antenna,  # Antenna information - Optional
        )

        uploader.add_telemetry(
            call_sign,  # Callsign
            current_utc_time_string,  # Time
            lat,
            lon,
            alt,
            frame=tx_count,  # Frame counter (e.g. packet number)
            vel_h=speed,  # Horizontal Velocity (m/s)
            snr=snr,  # Signal-to-Noise Ratio of the received telemetry, in dB.
            frequency=freq,  # Frequency of the received telemetry, in MHz.
            rssi=rssi,  # Received Signal Strength, in dBm
            modulation="LoRa",
            extra_fields={"Comment": comment},
        )

        # Display received data
        print(f"Lat, lon, alt: {lat}, {lon}")
        print(f"Speed, TX count: {speed}, {tx_count}")
