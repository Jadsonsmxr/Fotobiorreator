import json
import os

import paho.mqtt.client as mqtt

from apps.services.realtime_processing_service import RealtimeProcessingService
from apps.services.sensor_service import SensorService
from apps.websocket import emit_kpi_update, emit_sensor_update


MQTT_ENABLED = os.getenv("MQTT_ENABLED", "true").lower() == "true"
MQTT_REQUIRED = os.getenv("MQTT_REQUIRED", "false").lower() == "true"
MQTT_BROKER = os.getenv("MQTT_BROKER", "127.0.0.1")
MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"))
MQTT_TOPIC = os.getenv("MQTT_TOPIC", "cba_fotobiorreator/sensors/+/data")

MQTT_CONNECT_ERRORS = {
    1: "versao de protocolo incorreta",
    2: "identificador de cliente invalido",
    3: "servidor indisponivel",
    4: "usuario ou senha invalidos",
    5: "nao autorizado",
}


def on_connect(client, userdata, flags, rc):
    app = userdata["app"]

    if rc != 0:
        reason = MQTT_CONNECT_ERRORS.get(rc, f"codigo {rc}")
        app.logger.error(f"MQTT: conexao recusada ({reason}).")
        return

    client.subscribe(MQTT_TOPIC)
    app.logger.info(f"MQTT conectado. Subscrito em: {MQTT_TOPIC}")


def on_disconnect(client, userdata, rc):
    app = userdata["app"]

    if rc == 0:
        app.logger.info("MQTT desconectado com encerramento normal.")
        return

    app.logger.warning(f"MQTT desconectado inesperadamente (rc={rc}).")


def on_message(client, userdata, msg):
    app = userdata["app"]

    try:
        payload = json.loads(msg.payload.decode())

        sensor_id = payload.get("sensor_id")
        value = payload.get("value")

        if sensor_id is None or value is None:
            app.logger.warning(f"MQTT: payload invalido em {msg.topic}: {payload}")
            return

        with app.app_context():
            sensor = SensorService.get_sensor(sensor_id)
            if sensor is None:
                app.logger.warning(f"MQTT: sensor {sensor_id} nao encontrado. Leitura ignorada.")
                return

            processed = RealtimeProcessingService.process_sensor_reading(sensor, value)
            if not processed["accepted"]:
                app.logger.warning(
                    f"MQTT: leitura descartada para sensor {sensor_id} "
                    f"({processed['reason']}). valor={processed['raw_value']}"
                )
                return

            reading = SensorService.add_reading(sensor_id=sensor_id, value=processed["value"])
            emit_sensor_update(sensor, reading, processed)
            emit_kpi_update()

            app.logger.info(
                f"MQTT: leitura aceita para sensor {sensor_id} value={processed['value']}"
            )

    except json.JSONDecodeError as error:
        app.logger.error(f"MQTT: erro ao decodificar JSON em {msg.topic}: {error}")
    except Exception:
        app.logger.exception("MQTT: erro inesperado ao processar mensagem.")


def start_mqtt(app):
    if not MQTT_ENABLED:
        app.logger.warning("MQTT desabilitado por configuracao de ambiente.")
        return None

    client = mqtt.Client(userdata={"app": app})
    client.on_connect = on_connect
    client.on_disconnect = on_disconnect
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
