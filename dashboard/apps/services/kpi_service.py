from __future__ import annotations

from datetime import datetime, timedelta, timezone

from sqlalchemy import func

from apps import db
from apps.authentication.models import Sensor, SensorReading


class KPIService:
    RECENT_READING_WINDOW_MINUTES = 30
    STABILITY_WINDOW_HOURS = 2
    ADEQUATE_WINDOW_HOURS = 24

    RELIABLE_SENSOR_NAMES = {
        "Temperatura Interno",
        "pH",
    }

    SENSOR_STATUS_NOTES = {
        "CO2 Interno": "temporariamente fora dos KPIs por instabilidade",
        "CO2 Externo": "temporariamente fora dos KPIs por baixa amostragem",
        "Luminosidade": "temporariamente fora dos KPIs por falha do sensor",
    }

    TEMPERATURE_RANGE = {
        "ideal_min": 24.0,
        "ideal_max": 32.0,
        "acceptable_min": 18.0,
        "acceptable_max": 36.0,
    }

    PH_RANGE = {
        "ideal_min": 7.0,
        "ideal_max": 10.0,
        "acceptable_min": 6.5,
        "acceptable_max": 10.5,
    }

    @staticmethod
    def _now() -> datetime:
        return datetime.now(timezone.utc)

    @staticmethod
    def _normalize_timestamp(value: datetime | None) -> datetime | None:
        if value is None:
            return None
        if value.tzinfo is None:
            return value.replace(tzinfo=timezone.utc)
        return value.astimezone(timezone.utc)

    @staticmethod
    def _sensor_by_name(name: str) -> Sensor | None:
        return Sensor.query.filter(Sensor.name == name).first()

    @staticmethod
    def _latest_reading(sensor_name: str) -> SensorReading | None:
        sensor = KPIService._sensor_by_name(sensor_name)
        if sensor is None:
            return None

        return (
            SensorReading.query
            .filter(SensorReading.sensor_id == sensor.id)
            .order_by(SensorReading.timestamp.desc())
            .first()
        )

    @staticmethod
    def _recent_reading_value(sensor_name: str) -> float | None:
        reading = KPIService._latest_reading(sensor_name)
        if reading is None:
            return None

        timestamp = KPIService._normalize_timestamp(reading.timestamp)
        if timestamp is None:
            return None

        if KPIService._now() - timestamp > timedelta(minutes=KPIService.RECENT_READING_WINDOW_MINUTES):
            return None

        return float(reading.value)

    @staticmethod
    def _score_value(value: float | None, ranges: dict[str, float]) -> float | None:
        if value is None:
            return None

        if ranges["ideal_min"] <= value <= ranges["ideal_max"]:
            return 100.0

        if ranges["acceptable_min"] <= value < ranges["ideal_min"]:
            span = ranges["ideal_min"] - ranges["acceptable_min"]
            return max(0.0, min(100.0, ((value - ranges["acceptable_min"]) / span) * 100.0))

        if ranges["ideal_max"] < value <= ranges["acceptable_max"]:
            span = ranges["acceptable_max"] - ranges["ideal_max"]
            return max(0.0, min(100.0, ((ranges["acceptable_max"] - value) / span) * 100.0))

        return 0.0

    @staticmethod
    def _condition_score() -> int | None:
        temperature = KPIService._recent_reading_value("Temperatura Interno")
        ph = KPIService._recent_reading_value("pH")

        scores = [
            KPIService._score_value(temperature, KPIService.TEMPERATURE_RANGE),
            KPIService._score_value(ph, KPIService.PH_RANGE),
        ]
        valid_scores = [score for score in scores if score is not None]

        if not valid_scores:
            return None

        return round(sum(valid_scores) / len(valid_scores))

    @staticmethod
    def _sensor_amplitude(sensor_name: str) -> float | None:
        sensor = KPIService._sensor_by_name(sensor_name)
        if sensor is None:
            return None

        start = KPIService._now() - timedelta(hours=KPIService.STABILITY_WINDOW_HOURS)
        row = (
            db.session.query(
                func.min(SensorReading.value).label("min_value"),
                func.max(SensorReading.value).label("max_value"),
            )
            .filter(SensorReading.sensor_id == sensor.id)
            .filter(SensorReading.timestamp >= start)
            .first()
        )

        if row is None or row.min_value is None or row.max_value is None:
            return None

        return float(row.max_value) - float(row.min_value)

    @staticmethod
    def _culture_stability() -> str:
        temperature_amplitude = KPIService._sensor_amplitude("Temperatura Interno")
        ph_amplitude = KPIService._sensor_amplitude("pH")

        if temperature_amplitude is None and ph_amplitude is None:
            return "Sem dados"

        scores = []
        if temperature_amplitude is not None:
            if temperature_amplitude <= 1.0:
                scores.append(0)
            elif temperature_amplitude <= 3.0:
                scores.append(1)
            else:
                scores.append(2)

        if ph_amplitude is not None:
            if ph_amplitude <= 0.20:
                scores.append(0)
            elif ph_amplitude <= 0.50:
                scores.append(1)
            else:
                scores.append(2)

        worst_score = max(scores)
        if worst_score == 0:
            return "Estável"
        if worst_score == 1:
            return "Moderado"
        return "Oscilante"

    @staticmethod
    def _adequate_ratio(sensor_name: str, ranges: dict[str, float], start: datetime) -> float | None:
        sensor = KPIService._sensor_by_name(sensor_name)
        if sensor is None:
            return None

        total = (
            SensorReading.query
            .filter(SensorReading.sensor_id == sensor.id)
            .filter(SensorReading.timestamp >= start)
            .count()
        )
        if total == 0:
            return None

        adequate = (
            SensorReading.query
            .filter(SensorReading.sensor_id == sensor.id)
            .filter(SensorReading.timestamp >= start)
            .filter(SensorReading.value >= ranges["acceptable_min"])
            .filter(SensorReading.value <= ranges["acceptable_max"])
            .count()
        )

        return adequate / total

    @staticmethod
    def _adequate_condition_hours() -> float | None:
        now = KPIService._now()
        start = now - timedelta(hours=KPIService.ADEQUATE_WINDOW_HOURS)

        ratios = [
            KPIService._adequate_ratio("Temperatura Interno", KPIService.TEMPERATURE_RANGE, start),
            KPIService._adequate_ratio("pH", KPIService.PH_RANGE, start),
        ]
        valid_ratios = [ratio for ratio in ratios if ratio is not None]

        if not valid_ratios:
            return None

        conservative_ratio = min(valid_ratios)
        return round(KPIService.ADEQUATE_WINDOW_HOURS * conservative_ratio, 2)

    @staticmethod
    def _sensor_integrity() -> dict[str, int]:
        all_physical_sensors = (
            Sensor.query
            .filter(Sensor.name.in_({
                "CO2 Interno",
                "Temperatura Interno",
                "pH",
                "Luminosidade",
            }))
            .all()
        )

        total = len(all_physical_sensors)
        reliable = 0
        for sensor in all_physical_sensors:
            if sensor.name not in KPIService.RELIABLE_SENSOR_NAMES:
                continue
            if KPIService._recent_reading_value(sensor.name) is not None:
                reliable += 1

        return {
            "reliable": reliable,
            "total": total,
        }

    @staticmethod
    def get_kpis():
        return {
            "medium_condition": KPIService._condition_score(),
            "culture_stability": KPIService._culture_stability(),
            "adequate_condition_time": KPIService._adequate_condition_hours(),
            "sensor_integrity": KPIService._sensor_integrity(),
        }

    # KPIs legados preservados para referência, mas não calculados neste momento:
    # - co2_total
    # - efficiency
    # - co2_monthly
    # - active_time

    # KPIs futuros quando CO2 e luminosidade estiverem confiáveis:
    # - co2_trend_ppm_per_hour
    # - photosynthetic_potential
    # - estimated_co2_removal
    # - capture_efficiency
