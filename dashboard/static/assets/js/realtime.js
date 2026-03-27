import { KPIProcessors } from "./kpis.js";
import { SensorProcessors } from "./sensors.js";
import { initDashboardPageCharts } from "./dashboard.js";

function isDashboardRealtimePage() {
  return Boolean(
    document.getElementById("co2-total") &&
    document.getElementById("efficiency") &&
    document.getElementById("co2-monthly") &&
    document.getElementById("active-time") &&
    document.getElementById("gaugeco2") &&
    document.getElementById("gaugeph") &&
    document.getElementById("gaugeTemperature") &&
    document.getElementById("gaugeLuminosity")
  );
}

// cuida da função de atualizar os gauges e KPIs na dashboard em tempo real
function updateGauge(sensorKey, value) {
  if (SensorProcessors[sensorKey]) // objeto dicionario que guarda as funcoes de processamento dos sensores, busca o valor inserindo a chave sensorKey
  { 
    SensorProcessors[sensorKey](value);
    return;
  }


  console.warn("Sensor/KPI desconhecido:", sensorKey);
}

// function updateKPI(kpiKey, value) {
//   if (KPIProcessors[kpiKey]) {
//     KPIProcessors[kpiKey](value);
//     return;
//   } else {
//     console.warn("KPI desconhecido:", kpiKey);
//   }
// }

function updateKPI(kpis) {
  Object.entries(kpis).forEach(([kpiKey, value]) => {
    if (KPIProcessors[kpiKey]) {
      KPIProcessors[kpiKey](value);
    } else {
      console.warn("KPI desconhecido:", kpiKey);
    }
  });
}

function initRealtimeDashboard() {
  const socket = io("http://127.0.0.1:5000", {
    transports: ["websocket"]
  });

  socket.on("connect", () => {
    console.log("WebSocket conectado");
  });

  socket.on("sensor_update", (data) => {
      updateGauge(data.sensor_key, data.value);
  });

  socket.on("kpi_update", (data) => {
      updateKPI(data);
  });

  window.addEventListener("beforeunload", () => {
    socket.disconnect();
  });
}


document.addEventListener("DOMContentLoaded", () => {
  if (!isDashboardRealtimePage()) {
    return;
  }

  initDashboardPageCharts();
  initRealtimeDashboard();
});
