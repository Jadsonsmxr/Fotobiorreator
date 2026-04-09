import { ensureDashboardGauge } from "./dashboard.js";

function refreshGauge(gaugeName, gauge, value) {
    const readyGauge = gauge && typeof gauge.refresh === "function"
        ? gauge
        : ensureDashboardGauge(gaugeName, value);

    if (!readyGauge || typeof readyGauge.refresh !== "function") {
        return;
    }

    readyGauge.refresh(value);
}

export const SensorProcessors = {
    "CO2 Externo": value => {}, //window.gaugeCo2External?.refresh(value),
    "CO2 Interno": value => refreshGauge("gaugeCo2", window.gaugeCo2, value),
    "pH": value => refreshGauge("gaugePh", window.gaugePh, value),
    "Temperatura Interno": value => refreshGauge("gaugeTemperature", window.gaugeTemperature, value),
    "Luminosidade": value => refreshGauge("gaugeLuminosity", window.gaugeLuminosity, value),
};
