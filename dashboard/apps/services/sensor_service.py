from apps import db
from apps.authentication.models import Sensor, SensorReading
from datetime import datetime, timezone


class SensorService:

    @staticmethod
    def create_sensor(user_id, name, sensor_type, unit):
        sensor = Sensor(
            user_id=user_id,
            name=name,
            type=sensor_type,
            unit=unit
        )
        db.session.add(sensor)
        db.session.commit()
        return sensor

    @staticmethod
    def add_reading(sensor_id, value, timestamp=None):
        reading = SensorReading(
            sensor_id=sensor_id,
            value=value,
            timestamp=timestamp or datetime.now(timezone.utc)
        )
        db.session.add(reading)
        db.session.commit()
        return reading

    @staticmethod
    def get_latest_reading(sensor_id):
        return (
            SensorReading.query
            .filter_by(sensor_id=sensor_id)
            .order_by(SensorReading.timestamp.desc())
            .first()
        )

    @staticmethod
    def get_history(sensor_id, limit=100):
        return (
            SensorReading.query
            .filter_by(sensor_id=sensor_id)
            .order_by(SensorReading.timestamp.desc())
            .limit(limit)
            .all()
        )
    @staticmethod
    def sensor_exists(sensor_id):
        return db.session.get(Sensor, sensor_id) is not None

    @staticmethod
    def get_sensor(sensor_id):
        return db.session.get(Sensor, sensor_id)
