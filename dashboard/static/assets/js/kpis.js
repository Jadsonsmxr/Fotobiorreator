export const KPIProcessors = {
  co2_total(value) {
    if (!window.elementoCo2Total) {
      return;
    }
    window.elementoCo2Total.innerText = `${value.toFixed(1)} kg`;
  },

  efficiency(value) {
    if (!window.elementoEfficiency) {
      return;
    }
    window.elementoEfficiency.innerText = `${value.toFixed(1)} %`;
  },

  co2_monthly(value) {
    if (!window.elementoCo2Monthly) {
      return;
    }
    window.elementoCo2Monthly.innerText = `${value.toFixed(1)} kg`;
  },

  active_time(value) {
    if (!window.elementoActiveTime) {
      return;
    }
    window.elementoActiveTime.innerText = `${value} h`;
  }
};
