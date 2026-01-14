const socket = io("http://127.0.0.1:5000", {
  transports: ["websocket"]
});

function updateGauge(sensorKey, value) {
  switch(sensorKey) {
    case "CO2 Interno":
      window.gaugeCo2?.refresh(value);  //continuar nos outros gauges
      break;
    case "CO2 Externo":
      // Se houver um gauge para CO2 Externo, atualize aqui
      break;
    case "pH":
      window.gaugePh?.refresh(value);
      break;
    case "Temperatura Interno":
      window.gaugeTemperature?.refresh(value);
      break;
    case "Luminosidade":
      window.gaugeLuminosity?.refresh(value);
      break;
    default:
      console.warn("Sensor desconhecido:", sensorKey);
  }
}



socket.on("connect", () => {
  console.log("WebSocket conectado");
});


socket.on("sensor_update", (data) => {
    //console.log("Atualização recebida:", data);
    //console.log("Atualização recebida:", data.sensor_key, data.value);

    // Exemplo: atualizar KPI ou gauge
    updateGauge(data.sensor_key, data.value);
});



// Apenas o Tooltip Main está sendo usado no momento, os outros são para futuros gráficos
var gradientChartOptionsConfigurationWithTooltipmain = {
      maintainAspectRatio: false,

      scales: { 
        yAxes: [{
            id: 'y-axis-0',
            display: true,
            ticks: {
                min: 200,
                max: 500,
                stepSize: 25,
            },
        }]
    
        },


      legend: { display: false },

      tooltips: {
        backgroundColor: '#f5f5f5',
        titleFontColor: '#333',
        bodyFontColor: '#666',
        bodySpacing: 4,
        xPadding: 12,
        mode: "nearest",
        intersect: 0,
        position: "nearest"
      },
      responsive: true,

      annotation: {
        annotations: [{
            type: 'line',
            mode: 'horizontal',
            scaleID: 'y-axis-0',
            value: 350,
            borderColor: '#4dff50ff',
            borderWidth: 1,
            label: {
                enabled: true,
                content: 'Limite seguro',
                position: "top",  
                yAdjust: -10, 
                backgroundColor: "rgba(0, 0, 0, 0.2)"
                
            }
        }]
    },

    };


    var gradientChartOptionsConfigurationWithTooltipco2 = {
      maintainAspectRatio: false,

      scales: { 
        yAxes: [{
            id: 'y-axis-0',
            display: true,
            ticks: {
                min: 300,
                max: 500,
                stepSize: 25,
            },
        }]
    
        },


      legend: { display: false },

      tooltips: {
        backgroundColor: '#f5f5f5',
        titleFontColor: '#333',
        bodyFontColor: '#666',
        bodySpacing: 4,
        xPadding: 12,
        mode: "nearest",
        intersect: 0,
        position: "nearest"
      },
      responsive: true,


    };

     var gradientChartOptionsConfigurationWithTooltippH = {
      maintainAspectRatio: false,

      scales: { 
        yAxes: [{
            id: 'y-axis-0',
            display: true,
            ticks: {
                min: 0,
                max: 14,
                stepSize: 1,
            },
        }]
    
        },


      legend: { display: false },

      tooltips: {
        backgroundColor: '#f5f5f5',
        titleFontColor: '#333',
        bodyFontColor: '#666',
        bodySpacing: 4,
        xPadding: 12,
        mode: "nearest",
        intersect: 0,
        position: "nearest"
      },
      responsive: true,


    };

     var gradientChartOptionsConfigurationWithTooltipTemperature = {
      maintainAspectRatio: false,

      scales: { 
        yAxes: [{
            id: 'y-axis-0',
            display: true,
            ticks: {
                min: 200,
                max: 500,
                stepSize: 25,
            },
        }]
    
        },


      legend: { display: false },

      tooltips: {
        backgroundColor: '#f5f5f5',
        titleFontColor: '#333',
        bodyFontColor: '#666',
        bodySpacing: 4,
        xPadding: 12,
        mode: "nearest",
        intersect: 0,
        position: "nearest"
      },
      responsive: true,


    };

     var gradientChartOptionsConfigurationWithTooltipLuminosity = {
      maintainAspectRatio: false,

      scales: { 
        yAxes: [{
            id: 'y-axis-0',
            display: true,
            ticks: {
                min: 200,
                max: 500,
                stepSize: 25,
            },
        }]
    
        },


      legend: { display: false },

      tooltips: {
        backgroundColor: '#f5f5f5',
        titleFontColor: '#333',
        bodyFontColor: '#666',
        bodySpacing: 4,
        xPadding: 12,
        mode: "nearest",
        intersect: 0,
        position: "nearest"
      },
      responsive: true,


    };



   




main = {



    createLineChart: function(canvasId, labels, dataset_label, data, color, options, pR = 3, fill = false) {
    var ctx = document.getElementById(canvasId).getContext('2d');

    return new Chart(ctx, {
      type: 'line',
      data: {
        labels: labels,
        datasets: [{
          label: dataset_label,
          data: data,
          borderColor: color,
          pointBackgroundColor: color,
          backgroundColor: color.replace('1)', '0.13)'),
          borderWidth: 2,
          pointRadius: pR,
          fill: fill
        }]
      },
      options: options
    })
    },
  


 initDashboardPageCharts: function () {
    // dados fictícios para exemplo - eixo x

    var chart_labels_Quinzenal_main = ['1','2','3','4','5','6','7','8','9','10','11','12','13','14','15'];
    var chart_labels_5min_co2 = ['12:00','12:05','12:10','12:15','12:20','12:25','12:30','12:35','12:40','12:45','12:50','12:55'];
    
    // dados fictícios para exemplo - eixo y

    var chart_data_Quinzenal_main = [ 425,420,416,411,407,402,398,393,389,384,379,373,368,360,350];
    var chart_data_5min_co2 = [422,421,419,420,418,417,415,414,412,411,409,408]
    var chart_data_5min_pH = [7,7,7,7,7,7,7,7,7,7,7,7]


    this.createLineChart("chartBig1", chart_labels_Quinzenal_main, 'CO2', chart_data_Quinzenal_main, 'rgba(0, 214, 180, 1)', gradientChartOptionsConfigurationWithTooltipmain, 3 ,true)
    // this.createLineChart("chartCo2", chart_labels_5min_co2, 'CO2', chart_data_5min_co2, 'rgba(0, 214, 68, 1)', gradientChartOptionsConfigurationWithTooltipco2, 0)
    // this.createLineChart("chartpH", chart_labels_5min_co2, 'pH', chart_data_5min_pH, 'rgba(214, 0, 180, 1)', gradientChartOptionsConfigurationWithTooltippH)
    // this.createLineChart("chartTemperature", chart_labels_5min_co2, 'Temperatura', chart_data_5min_co2, 'rgba(214, 180, 0, 1)', gradientChartOptionsConfigurationWithTooltipTemperature)
    // this.createLineChart("chartLuminosity", chart_labels_5min_co2, 'Luminosidade', chart_data_5min_co2, 'rgba(255, 255, 255, 1)', gradientChartOptionsConfigurationWithTooltipLuminosity)

const configco2 = {
  "id": "gaugeco2",
  "value": 445,
  "min": 200,
  "max": 500,
  "decimals": 1,
  "symbol": "",
  "reverse": false,
  
  "relativeGaugeSize": true,
  "gaugeWidthScale": 0.8,
  "gaugeColor": "#1e1e2f",
  "valueFontColor": "#bdbdbdff",
  "labelFontColor": "#787878ff",
  "levelColors": [
    "rgba(0, 214, 180, 1)"
  ],
  "noGradient": false,
  "showSectorColors": false,
  "title": "",
  "titleFontColor": "#cb1313ff",
  "titleFontFamily": "Arial",
  "titleFontWeight": "bold",
  "titlePosition": "above",
  "label": "ppm",
  "minTxt": "",
  "maxTxt": "",
  "hideValue": false,
  "hideMinMax": false,
  "humanFriendly": false,
  "formatNumber": false,
  "donut": false,
  "donutStartAngle": 90,
  "differential": false,
  "displayRemaining": false,
  "pointer": false,
  "targetLineColor": "#000000",
  "targetLineWidth": 1.5,
  "startAnimationTime": 700,
  "startAnimationType": ">",
  "refreshAnimationTime": 700,
  "counter": false,
  "showInnerShadow": true,
  "shadowOpacity": 0.2,
  "shadowSize": 5,
  "shadowVerticalOffset": 3
}

const configph = {
  "id": "gaugeph",
  "value": 7.4383517856623955,
  "min": 0,
  "max": 14,
  "decimals": 1,
  "symbol": "",
  "reverse": false,
  
  "relativeGaugeSize": true,
  "gaugeWidthScale": 0.8,
  "gaugeColor": "#1e1e2f",
  "valueFontColor": "#bdbdbdff",
  "labelFontColor": "#787878ff",
  "levelColors": [
    "rgba(0, 214, 180, 1)"
  ],
  "noGradient": false,
  "showSectorColors": false,
  "title": "",
  "titleFontColor": "#cb1313ff",
  "titleFontFamily": "Arial",
  "titleFontWeight": "bold",
  "titlePosition": "above",
  
  "minTxt": "",
  "maxTxt": "",
  "hideValue": false,
  "hideMinMax": false,
  "humanFriendly": false,
  "formatNumber": false,
  "donut": false,
  "donutStartAngle": 90,
  "differential": false,
  "displayRemaining": false,
  "pointer": false,
  "targetLineColor": "#000000",
  "targetLineWidth": 1.5,
  "startAnimationTime": 700,
  "startAnimationType": ">",
  "refreshAnimationTime": 700,
  "counter": false,
  "showInnerShadow": true,
  "shadowOpacity": 0.2,
  "shadowSize": 5,
  "shadowVerticalOffset": 3
}

const configTemperature = {
  "id": "gaugeTemperature",
  "value": 32,
  "min": 20,
  "max": 40,
  "decimals": 1,
  "symbol": "",
  "reverse": false,
  
  "relativeGaugeSize": true,
  "gaugeWidthScale": 0.8,
  "gaugeColor": "#1e1e2f",
  "valueFontColor": "#bdbdbdff",
  "labelFontColor": "#787878ff",
  "levelColors": [
    "rgba(0, 214, 180, 1)"
  ],
  "noGradient": false,
  "showSectorColors": false,
  "title": "",
  "titleFontColor": "#cb1313ff",
  "titleFontFamily": "Arial",
  "titleFontWeight": "bold",
  "titlePosition": "above",
  "label": "°C",
  
  "minTxt": "",
  "maxTxt": "",
  "hideValue": false,
  "hideMinMax": false,
  "humanFriendly": false,
  "formatNumber": false,
  "donut": false,
  "donutStartAngle": 90,
  "differential": false,
  "displayRemaining": false,
  "pointer": false,
  "targetLineColor": "#000000",
  "targetLineWidth": 1.5,
  "startAnimationTime": 700,
  "startAnimationType": ">",
  "refreshAnimationTime": 700,
  "counter": false,
  "showInnerShadow": true,
  "shadowOpacity": 0.2,
  "shadowSize": 5,
  "shadowVerticalOffset": 3
}

const configLuminosity = {
  "id": "gaugeLuminosity",
  "value": 712,
  "min": 0,
  "max": 1000,
  "decimals": 1,
  "symbol": "",
  "reverse": false,
  
  "relativeGaugeSize": true,
  "gaugeWidthScale": 0.8,
  "gaugeColor": "#1e1e2f",
  "valueFontColor": "#bdbdbdff",
  "labelFontColor": "#787878ff",
  "levelColors": [
    "rgba(0, 214, 180, 1)"
  ],
  "noGradient": false,
  "showSectorColors": false,
  "title": "",
  "titleFontColor": "#cb1313ff",
  "titleFontFamily": "Arial",
  "titleFontWeight": "bold",
  "titlePosition": "above",
  "label": "lux",
  "minTxt": "",
  "maxTxt": "",
  "hideValue": false,
  "hideMinMax": false,
  "humanFriendly": false,
  "formatNumber": false,
  "donut": false,
  "donutStartAngle": 90,
  "differential": false,
  "displayRemaining": false,
  "pointer": false,
  "targetLineColor": "#000000",
  "targetLineWidth": 1.5,
  "startAnimationTime": 700,
  "startAnimationType": ">",
  "refreshAnimationTime": 700,
  "counter": false,
  "showInnerShadow": true,
  "shadowOpacity": 0.2,
  "shadowSize": 5,
  "shadowVerticalOffset": 3
}
    // Cria os gauges e armazena em variáveis globais
    window.gaugeCo2 = new JustGage(configco2);
    window.gaugePh = new JustGage(configph);
    window.gaugeTemperature = new JustGage(configTemperature);
    window.gaugeLuminosity = new JustGage(configLuminosity);
    // Elementos de KPIs
    window.elementoCo2Total = document.getElementById("co2-total");
    window.elementoEfficiency = document.getElementById("efficiency");
    window.elementoCo2Monthly = document.getElementById("co2-monthly");
    window.elementoActiveTime = document.getElementById("active-time");
    //document.getElementById("co2-total").innerText = "80.5 kg"; // Exemplo de atualização do valor total de CO2 removido
    

 }
};

window.onload = function() { 
  main.initDashboardPageCharts(); // inicializa gauges logo ao carregar a página 
 };
