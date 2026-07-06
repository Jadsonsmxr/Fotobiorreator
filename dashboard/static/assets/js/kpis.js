function setKPIValue(element, value) {
  if (!element) {
    return;
  }

  element.innerText = value ?? "--";
  element.classList.remove("is-pending-data");
}

function formatHours(value) {
  if (value === null || value === undefined || Number.isNaN(Number(value))) {
    return "--";
  }

  const totalMinutes = Math.round(Number(value) * 60);
  const hours = Math.floor(totalMinutes / 60);
  const minutes = totalMinutes % 60;

  if (hours <= 0) {
    return `${minutes} min`;
  }

  return `${hours}h ${String(minutes).padStart(2, "0")}min`;
}

export const KPIProcessors = {
  medium_condition(value) {
    if (!window.elementoMediumCondition) {
      return;
    }
    setKPIValue(window.elementoMediumCondition, value === null ? "--" : `${Math.round(value)}%`);
  },

  culture_stability(value) {
    if (!window.elementoCultureStability) {
      return;
    }
    setKPIValue(window.elementoCultureStability, value);
  },

  adequate_condition_time(value) {
    if (!window.elementoAdequateTime) {
      return;
    }
    setKPIValue(window.elementoAdequateTime, formatHours(value));
  },

  sensor_integrity(value) {
    if (!window.elementoSensorIntegrity) {
      return;
    }
    if (!value || value.reliable === undefined || value.total === undefined) {
      setKPIValue(window.elementoSensorIntegrity, "--");
      return;
    }
    setKPIValue(window.elementoSensorIntegrity, `${value.reliable}/${value.total}`);
  }
};

// KPIs legados mantidos como referência:
// co2_total(value) => `${value.toFixed(1)} kg`
// efficiency(value) => `${value.toFixed(1)} %`
// co2_monthly(value) => `${value.toFixed(1)} kg`
// active_time(value) => `${value} h`

// KPIs futuros quando CO2 e luminosidade voltarem a ser confiáveis:
// co2_trend_ppm_per_hour, photosynthetic_potential, estimated_co2_removal, capture_efficiency
