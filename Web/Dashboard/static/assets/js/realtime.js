import { KPIProcessors } from "./kpis.js";
import { SensorProcessors } from "./sensors.js";
import { initDashboardPageCharts } from "./dashboard.js";
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


// atualiza os gauges e KPIs no frontend em tempo real usando WebSocket e recebe a emissao do websocket do Flask-SocketIO
const socket = io("http://127.0.0.1:5000", {
  transports: ["websocket"]
});


socket.on("connect", () => {
  console.log("WebSocket conectado");
});


socket.on("sensor_update", (data) => {
    //console.log("Atualização recebida:", data);
    //console.log("Atualização recebida:", data.sensor_key, data.value);

    // Exemplo: atualizar KPI ou gauge
    updateGauge(data.sensor_key, data.value);
});

socket.on("kpi_update", (data) => {
    //console.log("Atualização de KPI recebida:", data);
    //console.log("Atualização de KPI recebida:", data.kpi_key, data.value);

    // Exemplo: atualizar KPI
    updateKPI(data);
});


document.addEventListener("DOMContentLoaded", () => {
  initDashboardPageCharts();
});