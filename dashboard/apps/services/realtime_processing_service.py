import math


SENSOR_RULES = {
    "co2": {"min": 0.0, "max": 5000.0},
    "ph": {"min": 0.0, "max": 14.0},
    "temperatura": {"min": 0.0, "max": 80.0},
    "temperature": {"min": 0.0, "max": 80.0},
    "luminosidade": {"min": 0.0, "max": 200000.0},
    "luminosity": {"min": 0.0, "max": 200000.0},
}


class RealtimeProcessingService:
    @staticmethod
    def _sensor_lookup_keys(sensor) -> list[str]:
        keys = []
        for candidate in (getattr(sensor, "name", None), getattr(sensor, "type", None)):
            if candidate:
                keys.append(str(candidate).strip().lower())
        return keys

    @staticmethod
    def _resolve_rules(sensor) -> dict[str, float] | None:
        for key in RealtimeProcessingService._sensor_lookup_keys(sensor):
            for rule_key, rules in SENSOR_RULES.items():
                if rule_key in key:
                    return rules
        return None

    @staticmethod
    def process_sensor_reading(sensor, raw_value):
        try:
            numeric_value = float(raw_value)
        except (TypeError, ValueError):
            return {
                "accepted": False,
                "quality": "discarded",
                "reason": "valor_nao_numerico",
                "raw_value": raw_value,
                "value": None,
            }

        if not math.isfinite(numeric_value):
            return {
                "accepted": False,
                "quality": "discarded",
                "reason": "valor_nao_finito",
                "raw_value": raw_value,
                "value": None,
            }

        rules = RealtimeProcessingService._resolve_rules(sensor)
        if rules and (numeric_value < rules["min"] or numeric_value > rules["max"]):
            return {
                "accepted": False,
                "quality": "discarded",
                "reason": "fora_da_faixa",
                "raw_value": numeric_value,
                "value": None,
            }

        return {
            "accepted": True,
            "quality": "ok",
            "reason": None,
            "raw_value": numeric_value,
            "value": numeric_value,
        }
