# -*- encoding: utf-8 -*-
"""
Copyright (c) 2019 - present AppSeed.us
"""

from apps.authentication.models import Sensor, SensorReading
from apps.home import blueprint
from apps.mqtt.client import get_mqtt_runtime_status
from flask import current_app, render_template, request
from flask_login import current_user, login_required
from jinja2 import TemplateNotFound


@blueprint.route('/index')
@login_required
def index():

    return render_template('home/index.html', segment='index')


@blueprint.route('/<template>')
@login_required
def route_template(template):

    try:

        if not template.endswith('.html'):
            template += '.html'

        segment = get_segment(request)
        context = {"segment": segment}

        if template == 'user.html':
            context.update(build_settings_context())

        return render_template("home/" + template, **context)

    except TemplateNotFound:
        return render_template('home/page-404.html'), 404

    except Exception:
        current_app.logger.exception("Falha ao renderizar template '%s'.", template)
        return render_template('home/page-500.html'), 500


def build_settings_context():
    try:
        mqtt_status = get_mqtt_runtime_status() or {}
    except Exception:
        current_app.logger.exception("Falha ao obter status MQTT para a página de configurações.")
        mqtt_status = {}

    try:
        latest_reading = (
            SensorReading.query
            .order_by(SensorReading.timestamp.desc())
            .first()
        )
    except Exception:
        current_app.logger.exception("Falha ao consultar a última leitura para a página de configurações.")
        latest_reading = None
    try:
        sensor_count = Sensor.query.count()
    except Exception:
        current_app.logger.exception("Falha ao contar sensores cadastrados.")
        sensor_count = 0

    try:
        reading_count = SensorReading.query.count()
    except Exception:
        current_app.logger.exception("Falha ao contar leituras registradas.")
        reading_count = 0

    return {
        "mqtt_status": {
            "enabled": mqtt_status.get("enabled", False),
            "required": mqtt_status.get("required", False),
            "broker": mqtt_status.get("broker", "Não configurado"),
            "port": mqtt_status.get("port", "-"),
            "topic": mqtt_status.get("topic", "Não configurado"),
            "status": mqtt_status.get("status", "disabled"),
            "label": mqtt_status.get("label", "Indisponível"),
            "detail": mqtt_status.get("detail", "Sem detalhes adicionais."),
        },
        "system_status": {
            "mode": "Debug" if current_app.debug else "Production",
            "sensor_count": sensor_count,
            "reading_count": reading_count,
            "latest_reading_at": latest_reading.timestamp if latest_reading else None,
            "latest_reading_value": latest_reading.value if latest_reading else None,
            "latest_reading_sensor": latest_reading.sensor.name if latest_reading and latest_reading.sensor else None,
        }
    }


# Helper - Extract current page name from request
def get_segment(request):

    try:

        segment = request.path.split('/')[-1]

        if segment == '':
            segment = 'index'

        return segment

    except:
        return None
