from flask import current_app, request

from apps.authentication.models import Sensor
from apps.extensions import socketio
from apps.services.kpi_service import KPIService
from apps.services.sensor_service import SensorService


def emit_sensor_update(sensor, reading, processed=None, room=None):
    payload = {
        "sensor_key": sensor.name,
        "value": reading.value,
        "timestamp": reading.timestamp.isoformat(),
    }

    if processed:
        payload.update({
            "raw_value": processed.get("raw_value"),
            "quality": processed.get("quality", "ok"),
            "processing_reason": processed.get("reason"),
        })

    socketio.emit("sensor_update", payload, namespace="/", room=room)


def emit_kpi_update(room=None):
    kpis = KPIService.get_kpis()
    if kpis:
        socketio.emit("kpi_update", kpis, namespace="/", room=room)


def emit_dashboard_snapshot(app, room=None):
    with app.app_context():
        sensores = Sensor.query.all()
        for sensor in sensores:
            leitura = SensorService.get_latest_reading(sensor.id)
            if leitura:
                emit_sensor_update(sensor, leitura, room=room)
        emit_kpi_update(room=room)


@socketio.on("connect")
def handle_dashboard_connect():
    emit_dashboard_snapshot(current_app._get_current_object(), room=request.sid)


def emitir_periodicamente(app):
    with app.app_context():
        while True:
            emit_dashboard_snapshot(app)
            socketio.sleep(15)
