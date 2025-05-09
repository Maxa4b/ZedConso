import paho.mqtt.client as mqtt
import csv
import os
from datetime import datetime

data = []

def on_connect(client, userdata, flags, rc):
    print("✅ Connecté au broker MQTT (code", rc, ")")
    client.subscribe("esp32/releves")

def on_message(client, userdata, msg):
    payload = msg.payload.decode()
    print("📩 Reçu :", payload)

    if payload == "END":
        if data:
            save_csv()
            data.clear()
    else:
        parts = payload.split(",")
        if len(parts) == 6:
            data.append(parts)

def save_csv():
    now = datetime.now()
    filename = f"mqtt_export_{now.strftime('%Y-%m-%d_%H-%M-%S')}.csv"
    dest_dir = os.path.join(os.path.dirname(__file__), "..", "data_samples")
    os.makedirs(dest_dir, exist_ok=True)  # Crée le dossier s'il n'existe pas
    filepath = os.path.join(dest_dir, filename)

    total_ah = 0
    total_wh = 0

    for row in data:
        try:
            current_mA = float(row[3])
            power_mW = float(row[4])
            total_ah += (current_mA / 1000.0) * (0.2 / 3600.0)
            total_wh += (power_mW / 1000.0) * (0.2 / 3600.0)
        except (ValueError, IndexError):
            continue

    if total_ah < 0.001:
        total_ah_str = f"{round(total_ah * 1000, 3)} mAh"
    else:
        total_ah_str = f"{round(total_ah, 3)} Ah"

    if total_wh < 0.001:
        total_wh_str = f"{round(total_wh * 1000, 3)} mWh"
    else:
        total_wh_str = f"{round(total_wh, 3)} Wh"

    with open(filepath, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow([
            "Formatted Time", "Shunt Voltage (mV)", "Bus Voltage (V)",
            "Current (mA)", "Power (mW)", "Load Voltage (V)",
            "Total Energy (Ah/mAh)", "Total Energy (Wh/mWh)"
        ])
        for i, row in enumerate(data):
            if i == 0:
                row += [total_ah_str, total_wh_str]
            else:
                row += ["", ""]
            writer.writerow(row)

    print(f"💾 Données enregistrées dans {filepath}\n")

# Initialisation MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("localhost", 1883, 60)

client.loop_forever()
