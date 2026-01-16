export const SensorProcessors = {
    "CO2 Externo": value => {}, //window.gaugeCo2External?.refresh(value),
    "CO2 Interno": value => window.gaugeCo2?.refresh(value),
    "pH": value => window.gaugePh?.refresh(value),
    "Temperatura Interno": value => window.gaugeTemperature?.refresh(value),
    "Luminosidade": value => window.gaugeLuminosity?.refresh(value),
};
