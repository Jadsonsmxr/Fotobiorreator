function refreshGauge(gauge, value) {
    if (!gauge || typeof gauge.refresh !== "function") {
        return;
    }

    gauge.refresh(value);
}

export const SensorProcessors = {
    "CO2 Externo": value => {}, //window.gaugeCo2External?.refresh(value),
    "CO2 Interno": value => refreshGauge(window.gaugeCo2, value),
    "pH": value => refreshGauge(window.gaugePh, value),
    "Temperatura Interno": value => refreshGauge(window.gaugeTemperature, value),
    "Luminosidade": value => refreshGauge(window.gaugeLuminosity, value),
};
