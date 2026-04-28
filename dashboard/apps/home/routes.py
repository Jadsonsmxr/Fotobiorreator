# -*- encoding: utf-8 -*-
"""
Copyright (c) 2019 - present AppSeed.us
"""

from apps.authentication.models import Sensor, SensorReading
from datetime import datetime, time, timezone
from zoneinfo import ZoneInfo
from apps.home import blueprint
from apps.mqtt.client import get_mqtt_runtime_status
from apps.services.cycle_service import CycleService
from flask import current_app, redirect, render_template, request, url_for
from flask_login import current_user, login_required
from jinja2 import TemplateNotFound


MANAUS_TZ = ZoneInfo("America/Manaus")
UTC = timezone.utc


def to_manaus_time(value):
    if value is None:
        return None
    if value.tzinfo is None:
        value = value.replace(tzinfo=UTC)
    return value.astimezone(MANAUS_TZ)


def format_manaus_time(value, fmt='%d/%m/%Y %H:%M'):
    localized = to_manaus_time(value)
    return localized.strftime(fmt) if localized else None


@blueprint.route('/index')
@login_required
def index():
    return render_template('home/index.html', segment='index')


@blueprint.route('/cycles/start', methods=['POST'])
@login_required
def start_cycle():
    try:
        cycle = CycleService.start_cycle(
            name=request.form.get('name', ''),
            notes=request.form.get('notes', ''),
            created_by_id=current_user.id,
        )
        return redirect(
            url_for(
                'home_blueprint.route_template',
                template='cycles.html',
                feedback='success',
                message='Ciclo iniciado com sucesso.',
                cycle_id=cycle.id,
            )
        )
    except ValueError as error:
        return redirect(
            url_for(
                'home_blueprint.route_template',
                template='cycles.html',
                feedback='error',
                message=str(error),
            )
        )


@blueprint.route('/cycles/<int:cycle_id>/finish', methods=['POST'])
@login_required
def finish_cycle(cycle_id):
    try:
        CycleService.finish_cycle(cycle_id)
        return redirect(
            url_for(
                'home_blueprint.route_template',
                template='cycles.html',
                feedback='success',
                message='Ciclo finalizado com sucesso.',
                cycle_id=cycle_id,
            )
        )
    except ValueError as error:
        return redirect(
            url_for(
                'home_blueprint.route_template',
                template='cycles.html',
                feedback='error',
                message=str(error),
                cycle_id=cycle_id,
            )
        )


@blueprint.route('/cycles/<int:cycle_id>/delete', methods=['POST'])
@login_required
def delete_cycle(cycle_id):
    redirect_template = request.form.get('redirect_template', 'cycles.html')
    redirect_page = request.form.get('redirect_page', type=int)
    redirect_status = request.form.get('redirect_status')
    redirect_date_from = request.form.get('redirect_date_from')
    redirect_date_to = request.form.get('redirect_date_to')

    try:
        CycleService.delete_cycle(cycle_id)
        redirect_kwargs = {
            'template': redirect_template,
            'feedback': 'success',
            'message': 'Ciclo excluído com sucesso.',
        }
        if redirect_page:
            redirect_kwargs['page'] = redirect_page
        if redirect_status:
            redirect_kwargs['status'] = redirect_status
        if redirect_date_from:
            redirect_kwargs['date_from'] = redirect_date_from
        if redirect_date_to:
            redirect_kwargs['date_to'] = redirect_date_to

        return redirect(url_for('home_blueprint.route_template', **redirect_kwargs))
    except ValueError as error:
        redirect_kwargs = {
            'template': redirect_template,
            'feedback': 'error',
            'message': str(error),
        }
        if redirect_page:
            redirect_kwargs['page'] = redirect_page
        if redirect_status:
            redirect_kwargs['status'] = redirect_status
        if redirect_date_from:
            redirect_kwargs['date_from'] = redirect_date_from
        if redirect_date_to:
            redirect_kwargs['date_to'] = redirect_date_to

        return redirect(url_for('home_blueprint.route_template', **redirect_kwargs))


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
        elif template == 'cycles.html':
            context.update(build_cycles_context(request))
        elif template == 'cycles_history.html':
            context.update(build_cycles_history_context(request))

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
            "latest_reading_at": format_manaus_time(latest_reading.timestamp) if latest_reading else None,
            "latest_reading_value": latest_reading.value if latest_reading else None,
            "latest_reading_sensor": latest_reading.sensor.name if latest_reading and latest_reading.sensor else None,
        }
    }


def build_cycles_context(request):
    active_cycle = CycleService.get_active_cycle()
    history = CycleService.list_cycles(limit=12)

    selected_cycle_id = request.args.get('cycle_id', type=int)
    selected_cycle = CycleService.get_cycle(selected_cycle_id) if selected_cycle_id else None
    if selected_cycle is None:
        selected_cycle = active_cycle or (history[0] if history else None)

    active_summary = CycleService.summarize_cycle(active_cycle) if active_cycle else None
    selected_summary = CycleService.summarize_cycle(selected_cycle) if selected_cycle else None

    return {
        "cycle_feedback": {
            "type": request.args.get('feedback'),
            "message": request.args.get('message'),
        },
        "active_cycle": serialize_cycle(active_cycle, active_summary) if active_cycle and active_summary else None,
        "cycle_history": [
            serialize_cycle(cycle, CycleService.summarize_cycle(cycle))
            for cycle in history
        ],
        "selected_cycle": serialize_cycle(selected_cycle, selected_summary) if selected_cycle and selected_summary else None,
    }


def build_cycles_history_context(request):
    page = request.args.get('page', type=int, default=1)
    per_page = 12
    status_filter = request.args.get('status', default='all')
    date_from = request.args.get('date_from', default='')
    date_to = request.args.get('date_to', default='')

    started_from = None
    started_to = None

    if date_from:
        try:
            started_from = datetime.combine(
                datetime.strptime(date_from, '%Y-%m-%d').date(),
                time.min,
                tzinfo=MANAUS_TZ,
            ).astimezone(UTC)
        except ValueError:
            date_from = ''

    if date_to:
        try:
            started_to = datetime.combine(
                datetime.strptime(date_to, '%Y-%m-%d').date(),
                time.max,
                tzinfo=MANAUS_TZ,
            ).astimezone(UTC)
        except ValueError:
            date_to = ''

    normalized_status = status_filter if status_filter in {'all', 'active', 'finished'} else 'all'
    service_status = None if normalized_status == 'all' else normalized_status

    cycles, total_cycles = CycleService.list_cycles_paginated(
        page=page,
        per_page=per_page,
        status=service_status,
        started_from=started_from,
        started_to=started_to,
    )
    total_pages = max((total_cycles + per_page - 1) // per_page, 1)
    current_page = min(max(page, 1), total_pages)

    if current_page != page:
        cycles, total_cycles = CycleService.list_cycles_paginated(
            page=current_page,
            per_page=per_page,
            status=service_status,
            started_from=started_from,
            started_to=started_to,
        )

    return {
        "cycle_feedback": {
            "type": request.args.get('feedback'),
            "message": request.args.get('message'),
        },
        "cycles_filters": {
            "status": normalized_status,
            "date_from": date_from,
            "date_to": date_to,
        },
        "cycles_page": {
            "records": [
                serialize_cycle(cycle, CycleService.summarize_cycle(cycle))
                for cycle in cycles
            ],
            "current_page": current_page,
            "total_pages": total_pages,
            "total_cycles": total_cycles,
            "has_prev": current_page > 1,
            "has_next": current_page < total_pages,
            "prev_page": current_page - 1,
            "next_page": current_page + 1,
        },
    }


def serialize_cycle(cycle, summary):
    started_at, ended_at = CycleService.resolve_cycle_window(cycle)
    return {
        "id": cycle.id,
        "name": cycle.name,
        "notes": cycle.notes,
        "status": cycle.status,
        "status_label": "Em andamento" if cycle.status == 'active' else "Finalizado",
        "status_class": "is-success" if cycle.status == 'active' else "is-muted",
        "started_at": cycle.started_at,
        "ended_at": cycle.ended_at,
        "window_started_at": started_at,
        "window_ended_at": ended_at,
        "started_at_display": format_manaus_time(cycle.started_at),
        "started_date_display": format_manaus_time(cycle.started_at, '%d/%m/%Y'),
        "window_started_at_display": format_manaus_time(started_at),
        "window_ended_at_display": format_manaus_time(ended_at),
        "duration_label": format_duration(summary.duration_seconds),
        "reading_count": summary.reading_count,
        "sensor_count": summary.sensor_count,
        "sensor_summaries": summary.sensor_summaries,
        "insights": summary.insights,
    }


def format_duration(total_seconds):
    total_seconds = max(int(total_seconds or 0), 0)
    days, remainder = divmod(total_seconds, 86400)
    hours, remainder = divmod(remainder, 3600)
    minutes, _ = divmod(remainder, 60)

    parts = []
    if days:
        parts.append(f"{days}d")
    if hours:
        parts.append(f"{hours}h")
    if minutes or not parts:
        parts.append(f"{minutes}min")
    return ' '.join(parts)


# Helper - Extract current page name from request
def get_segment(request):
    try:
        segment = request.path.split('/')[-1]

        if segment == '':
            segment = 'index'

        return segment

    except Exception:
        return None
