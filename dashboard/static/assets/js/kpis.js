function setKPIValue(element, value) {
  if (!element) {
    return;
  }

  element.innerText = value;
  element.classList.remove("is-pending-data");
}

export const KPIProcessors = {
  co2_total(value) {
    if (!window.elementoCo2Total) {
      return;
    }
    setKPIValue(window.elementoCo2Total, `${value.toFixed(1)} kg`);
  },

  efficiency(value) {
    if (!window.elementoEfficiency) {
      return;
    }
    setKPIValue(window.elementoEfficiency, `${value.toFixed(1)} %`);
  },

  co2_monthly(value) {
    if (!window.elementoCo2Monthly) {
      return;
    }
    setKPIValue(window.elementoCo2Monthly, `${value.toFixed(1)} kg`);
  },

  active_time(value) {
    if (!window.elementoActiveTime) {
      return;
    }
    setKPIValue(window.elementoActiveTime, `${value} h`);
  }
};
