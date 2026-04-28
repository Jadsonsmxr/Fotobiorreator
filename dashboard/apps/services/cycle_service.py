from __future__ import annotations

from dataclasses import dataclass
from datetime import datetime, timezone

from sqlalchemy import func

from apps import db
from apps.authentication.models import CultivationCycle, Sensor, SensorReading


@dataclass
class CycleMetricsSummary:
    reading_count: int
    sensor_count: int
    duration_seconds: int
    sensor_summaries: list[dict]
    insights: dict


class CycleService:
    @staticmethod
    def _normalize_timestamp(value: datetime | None) -> datetime | None:
        if value is None:
            return None
        if value.tzinfo is None:
            return value.replace(tzinfo=timezone.utc)
        return value

    @staticmethod
    def _classify_stability(amplitude: float | None, average: float | None) -> tuple[str, str]:
        if amplitude is None or average is None:
            return "Sem classificação", "is-muted"

        baseline = max(abs(float(average)), 1.0)
        variation_index = float(amplitude) / baseline

        if variation_index <= 0.10:
            return "Estável", "is-success"
        if variation_index <= 0.25:
            return "Moderado", "is-warning"
        return "Oscilante", "is-error"

    @staticmethod
    def get_active_cycle() -> CultivationCycle | None:
        return (
            CultivationCycle.query
            .filter_by(status="active")
            .order_by(CultivationCycle.started_at.desc())
            .first()
        )

    @staticmethod
    def get_cycle(cycle_id: int) -> CultivationCycle | None:
        return db.session.get(CultivationCycle, cycle_id)

    @staticmethod
    def list_cycles(limit: int = 20) -> list[CultivationCycle]:
        return (
            CultivationCycle.query
            .order_by(CultivationCycle.started_at.desc())
            .limit(limit)
            .all()
        )

    @staticmethod
    def list_cycles_paginated(
        page: int = 1,
        per_page: int = 12,
        status: str | None = None,
        started_from: datetime | None = None,
        started_to: datetime | None = None,
    ) -> tuple[list[CultivationCycle], int]:
        safe_page = max(int(page or 1), 1)
        safe_per_page = max(int(per_page or 12), 1)

        query = CultivationCycle.query

        if status in {"active", "finished"}:
            query = query.filter(CultivationCycle.status == status)
        if started_from is not None:
            query = query.filter(CultivationCycle.started_at >= started_from)
        if started_to is not None:
            query = query.filter(CultivationCycle.started_at <= started_to)

        query = query.order_by(CultivationCycle.started_at.desc())
        total = query.count()
        cycles = (
            query
            .offset((safe_page - 1) * safe_per_page)
            .limit(safe_per_page)
            .all()
        )
        return cycles, total

    @staticmethod
    def start_cycle(name: str, notes: str | None = None, created_by_id: int | None = None) -> CultivationCycle:
        if CycleService.get_active_cycle() is not None:
            raise ValueError("Já existe um ciclo de cultivo em andamento.")

        cycle_name = (name or "").strip()
        if not cycle_name:
            raise ValueError("Informe um nome para o ciclo.")

        cycle = CultivationCycle(
            name=cycle_name,
            notes=(notes or "").strip() or None,
            status="active",
            started_at=datetime.now(timezone.utc),
            created_by_id=created_by_id,
        )
        db.session.add(cycle)
        db.session.commit()
        return cycle

    @staticmethod
    def finish_cycle(cycle_id: int) -> CultivationCycle:
        cycle = CycleService.get_cycle(cycle_id)
        if cycle is None:
            raise ValueError("Ciclo de cultivo não encontrado.")
        if cycle.status != "active":
            raise ValueError("Esse ciclo já foi finalizado.")

        cycle.status = "finished"
        cycle.ended_at = datetime.now(timezone.utc)
        db.session.commit()
        return cycle

    @staticmethod
    def delete_cycle(cycle_id: int) -> None:
        cycle = CycleService.get_cycle(cycle_id)
        if cycle is None:
            raise ValueError("Ciclo de cultivo não encontrado.")

        db.session.delete(cycle)
        db.session.commit()

    @staticmethod
    def resolve_cycle_window(cycle: CultivationCycle) -> tuple[datetime, datetime]:
        start = CycleService._normalize_timestamp(cycle.started_at)
        end = CycleService._normalize_timestamp(cycle.ended_at) or datetime.now(timezone.utc)
        return start, end

    @staticmethod
    def summarize_cycle(cycle: CultivationCycle) -> CycleMetricsSummary:
        start, end = CycleService.resolve_cycle_window(cycle)

        reading_count = (
            SensorReading.query
            .filter(SensorReading.timestamp >= start)
            .filter(SensorReading.timestamp <= end)
            .count()
        )

        summary_rows = (
            db.session.query(
                Sensor.id.label("sensor_id"),
                Sensor.name.label("sensor_name"),
                Sensor.unit.label("sensor_unit"),
                func.count(SensorReading.id).label("reading_count"),
                func.min(SensorReading.value).label("min_value"),
                func.avg(SensorReading.value).label("avg_value"),
                func.max(SensorReading.value).label("max_value"),
            )
            .join(Sensor, Sensor.id == SensorReading.sensor_id)
            .filter(SensorReading.timestamp >= start)
            .filter(SensorReading.timestamp <= end)
            .group_by(Sensor.id, Sensor.name, Sensor.unit)
            .order_by(Sensor.name.asc())
            .all()
        )

        sensor_summaries = []
        for row in summary_rows:
            latest_reading = (
                SensorReading.query
                .filter_by(sensor_id=row.sensor_id)
                .filter(SensorReading.timestamp >= start)
                .filter(SensorReading.timestamp <= end)
                .order_by(SensorReading.timestamp.desc())
                .first()
            )

            amplitude = None
            if row.min_value is not None and row.max_value is not None:
                amplitude = float(row.max_value) - float(row.min_value)

            stability_label, stability_class = CycleService._classify_stability(amplitude, row.avg_value)
            sensor_summaries.append(
                {
                    "sensor_id": row.sensor_id,
                    "sensor_name": row.sensor_name,
                    "sensor_unit": row.sensor_unit,
                    "reading_count": int(row.reading_count or 0),
                    "min_value": row.min_value,
                    "avg_value": row.avg_value,
                    "max_value": row.max_value,
                    "amplitude": amplitude,
                    "latest_value": latest_reading.value if latest_reading else None,
                    "latest_timestamp": latest_reading.timestamp if latest_reading else None,
                    "stability_label": stability_label,
                    "stability_class": stability_class,
                }
            )

        most_variable_sensor = max(
            sensor_summaries,
            key=lambda item: item["amplitude"] if item["amplitude"] is not None else -1,
            default=None,
        )
        stable_sensor_count = sum(1 for item in sensor_summaries if item["stability_label"] == "Estável")
        oscillating_sensor_count = sum(1 for item in sensor_summaries if item["stability_label"] == "Oscilante")

        insights = {
            "most_variable_sensor": most_variable_sensor["sensor_name"] if most_variable_sensor else None,
            "most_variable_amplitude": most_variable_sensor["amplitude"] if most_variable_sensor else None,
            "most_variable_unit": most_variable_sensor["sensor_unit"] if most_variable_sensor else None,
            "stable_sensor_count": stable_sensor_count,
            "oscillating_sensor_count": oscillating_sensor_count,
        }

        duration_seconds = max(int((end - start).total_seconds()), 0)
        return CycleMetricsSummary(
            reading_count=reading_count,
            sensor_count=len(sensor_summaries),
            duration_seconds=duration_seconds,
            sensor_summaries=sensor_summaries,
            insights=insights,
        )
