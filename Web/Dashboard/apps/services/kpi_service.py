
# class KPIService:

#     SENSOR_KPI_MAP = {
#         "CO2 Interno": [
#             "co2_total",
#             "efficiency",
#             "co2_monthly",
#             "active_time",
#         ],
#         "Luminosidade": [
#             "luminosity"
#         ]
#     }

#     # KPI_PROCESSORS = {
#     #     "co2_total": lambda v: KPIService.process_co2_total(v),
#     #     "efficiency": lambda v: KPIService.process_efficiency(v),
#     #     "co2_monthly": lambda v: KPIService.process_co2_monthly(v),
#     #     "active_time": lambda v: KPIService.process_active_time(v),
#     #     "luminosity": lambda v: KPIService.process_luminosity(v),
#     # }

#     MOCK_VALUES = {
#         "co2_total": 80.5,       # kg
#         "efficiency": 92.3,      # %
#         "co2_monthly": 12.4,     # kg
#         "active_time": 36        # horas
#     }

#     @staticmethod
#      # def process_kpi(sensor_key, value):
#     #     kpis = KPIService.SENSOR_KPI_MAP.get(sensor_key, [])
#     #     results = {}

#     #     for kpi in kpis:
#     #         processor = KPIService.KPI_PROCESSORS.get(kpi)
#     #         if processor:
#     #             results[kpi] = processor(value)

#     #     return results
   
#     def process(sensor_key: str) -> dict:
#             """
#             Retorna KPIs mockados para o sensor informado
#             """
#             kpis = KPIService.SENSOR_KPI_MAP.get(sensor_key, [])

#             results = {}

#             for kpi in kpis:
#                 results[kpi] = KPIService.MOCK_VALUES.get(kpi)

#             return results

#     # @staticmethod
#     # def process_co2_total(value):
#     #     # Implement processing logic for total CO2
#     #     pass

#     # @staticmethod
#     # def process_efficiency(value):
#     #     # Implement processing logic for efficiency
#     #     pass

#     # @staticmethod
#     # def process_co2_monthly(value):
#     #     # Implement processing logic for monthly CO2
#     #     pass

#     # @staticmethod
#     # def process_active_time(value):
#     #     # Implement processing logic for active time
#     #     pass

#     # @staticmethod
#     # def process_luminosity(value):
#     #     # Implement processing logic for luminosity
#     #     pass

class KPIService:
# serviço responsável por calcular e fornecer KPIs (Key Performance Indicators)
# ainda será implementado a aquisição dos dados e o calculo real dos KPIs

    @staticmethod
    def get_kpis():
        return {
            "co2_total": 80.5,       # kg
            "efficiency": 92.3,      # %
            "co2_monthly": 12.4,     # kg
            "active_time": 36,       # horas
            
        }