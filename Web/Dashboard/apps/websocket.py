
# apps/websocket.py
# import random
# from flask_socketio import SocketIO

# def start_websocket_test(socketio: SocketIO):
#     def send_data():
#         while True:
#             valor = random.randint(350, 480)
#             # print("Emitindo CO2:", valor)
#             # socketio.emit("co2", valor, namespace="/")
#             # socketio.emit("ph", random.uniform(6.5, 8.5), namespace="/")
#             # socketio.emit("temperature", random.uniform(20.0, 30.0), namespace="/")
#             # socketio.emit("luminosity", random.uniform(200.0, 500.0), namespace="/")
#             # socketio.emit("co2_total", round(random.uniform(50.0, 100.0), 1), namespace="/")
#             # socketio.emit("efficiency", round(random.uniform(70.0, 95.0), 1), namespace="/")
#             # socketio.emit("co2_monthly", round(random.uniform(10.0, 25.0), 1), namespace="/")
#             # socketio.emit("active_time", random.randint(1, 24), namespace="/")
#             socketio.sleep(2)  # coopera com o loop async
            
#     socketio.start_background_task(send_data)


from apps.extensions import socketio
from apps.services.sensor_service import SensorService
from apps.authentication.models import Sensor
from apps.services.kpi_service import KPIService

def emitir_periodicamente(app):
    with app.app_context():
        while True:
            sensores = Sensor.query.all()
            for sensor in sensores:
                leitura = SensorService.get_latest_reading(sensor.id)
                if leitura:
                    socketio.emit(
                        "sensor_update",
                        {
                            "sensor_key": sensor.name,
                            "value": leitura.value,
                            "timestamp": leitura.timestamp.isoformat()
                        },
                        namespace="/"
                        )   
                    kpis = KPIService.get_kpis()

                    if kpis:
                        socketio.emit(
                            "kpi_update",
                            kpis,
                            namespace="/"
                            )
                    
            socketio.sleep(5)  # espera 5 segundos antes de repetir
