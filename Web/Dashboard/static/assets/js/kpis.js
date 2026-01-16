export const KPIProcessors = {
  co2_total(value) {
    window.elementoCo2Total.innerText = `${value.toFixed(1)} kg`;
  },

  efficiency(value) {
    window.elementoEfficiency.innerText = `${value.toFixed(1)} %`;
  },

  co2_monthly(value) {
    window.elementoCo2Monthly.innerText = `${value.toFixed(1)} kg`;
  },

  active_time(value) {
    window.elementoActiveTime.innerText = `${value} h`;
  }
};
