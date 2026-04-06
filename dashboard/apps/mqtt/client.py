import json
import os
import paho.mqtt.client as mqtt
from apps.services.sensor_service import SensorService


MQTT_ENABLED = os.getenv("MQTT_ENABLED", "true").lower() == "true"
MQTT_REQUIRED = os.getenv("MQTT_REQUIRED", "false").lower() == "true"
MQTT_BROKER = os.getenv("MQTT_BROKER", "127.0.0.1")
MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"))
MQTT_TOPIC = os.getenv("MQTT_TOPIC", "cba_fotobiorreator/sensors/+/data")



def on_connect(client, userdata, flags, rc):
    client.subscribe(MQTT_TOPIC)
    app = userdata["app"]
    app.logger.info(f"MQTT conectado. Subscrito em: {MQTT_TOPIC}")


def on_message(client, userdata, msg):
    app = userdata["app"]
    
    try:
        payload = json.loads(msg.payload.decode())

        sensor_id = payload.get("sensor_id")
        value = payload.get("value")
        
        if sensor_id is None or value is None:
            app.logger.warning(f"MQTT: Payload inválido em {msg.topic}: {payload}")
            return

        with app.app_context():
            if not SensorService.sensor_exists(sensor_id):
                app.logger.warning(f"MQTT: Sensor ID {sensor_id} não existe no banco. Leitura ignorada.")
                return

            SensorService.add_reading(
                sensor_id=sensor_id,
                value=value
            )
            app.logger.info(f"MQTT: ✓ Sensor {sensor_id} = {value}")
            
    except json.JSONDecodeError as e:
        app.logger.error(f"MQTT: Erro ao decodificar JSON: {e}")
    except Exception as e:
        app.logger.error(f"MQTT: Erro ao processar mensagem: {e}")
        

       




def start_mqtt(app):
    if not MQTT_ENABLED:
        app.logger.warning("MQTT desabilitado por configuracao de ambiente.")
        return None

    client = mqtt.Client(userdata={"app": app})
    client.on_connect = on_connect
    client.on_message = on_message

    try:
        client.connect(MQTT_BROKER, MQTT_PORT, 60)
        client.loop_start()
        app.logger.info(f"MQTT client iniciado em {MQTT_BROKER}:{MQTT_PORT}.")
        return client
    except Exception as error:
        message = (
            f"Nao foi possivel conectar ao broker MQTT em {MQTT_BROKER}:{MQTT_PORT}. "
            f"Detalhe: {error}"
        )
        if MQTT_REQUIRED:
            raise RuntimeError(message) from error

        app.logger.warning(message)
        app.logger.warning("A aplicacao continuara em execucao sem ingestao MQTT.")
        return None
