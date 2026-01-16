// import { initDashboardPageCharts } from "./dashboard.js";
// import { updateGauge } from "./realtime.js";
// // atualiza os gauges e KPIs no frontend em tempo real usando WebSocket e recebe a emissao do websocket do Flask-SocketIO
// const socket = io("http://127.0.0.1:5000", {
//   transports: ["websocket"]
// });


// socket.on("connect", () => {
//   console.log("WebSocket conectado");
// });


// socket.on("sensor_update", (data) => {
//     //console.log("Atualização recebida:", data);
//     //console.log("Atualização recebida:", data.sensor_key, data.value);

//     // Exemplo: atualizar KPI ou gauge
//     updateGauge(data.sensor_key, data.value);
// });


// document.addEventListener("DOMContentLoaded", () => {
//   initDashboardPageCharts();
// });