import json
import paho.mqtt.client as mqtt
from flask import current_app
from apps.services.sensor_service import SensorService
from apps.extensions import socketio

MQTT_BROKER = "localhost"
MQTT_PORT = 1883
MQTT_TOPIC = "sensors/+/data"



def on_connect(client, userdata, flags, rc):
    client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
    payload = json.loads(msg.payload.decode())

    sensor_id = payload["sensor_id"]
    value = payload["value"]

    app = userdata["app"]

    with app.app_context():
        if not SensorService.sensor_exists(sensor_id):
            app.logger.warning(f"MQTT: {sensor_id} não existe. Leitura ignorada.")
            return

        SensorService.add_reading(
            sensor_id=sensor_id,
            value=value
        )
        

       




def start_mqtt(app):
    client = mqtt.Client(userdata={"app": app})
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    client.loop_start()
    app.logger.info("MQTT client started and connected to broker.")
