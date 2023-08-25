//General Parameters
#define NS 6
#define NP 5
// params for CPU
#define CPU_I_IDLE 0.002
#define CPU_T_ACT 31
#define CPU_ON_I_ON 13
#define CPU_ON_T_ON 6
// params for sensor air_quality_sensor
#define AIR_BASE_ADDRESS 100
#define AIR_REG_DIMENSION 50
#define AIR_QUALITY_SENSOR_I_ON_READ 48.2
#define AIR_QUALITY_SENSOR_T_ON_READ 30
#define AIR_QUALITY_SENSOR_I_ON_WRITE 49.2
#define AIR_QUALITY_SENSOR_T_ON_WRITE 30
#define AIR_QUALITY_SENSOR_I_IDLE 0.002
#define AIR_QUALITY_SENSOR_T_ACT 0
// params for sensor temperature_sensor
#define TEMPERATURE_BASE_ADDRESS 200
#define TEMPERATURE_REG_DIMENSION 75
#define TEMPERATURE_SENSOR_I_ON 0.3
#define TEMPERATURE_SENSOR_T_ON 6
#define TEMPERATURE_SENSOR_I_IDLE 0.002
#define TEMPERATURE_SENSOR_T_ACT 0
// params for sensor methane_sensor
#define METHANE_BASE_ADDRESS 300
#define METHANE_REG_DIMENSION 25
#define METHANE_SENSOR_I_ON 18
#define METHANE_SENSOR_T_ON 30
#define METHANE_SENSOR_I_IDLE 0.002
#define METHANE_SENSOR_T_ACT 0
// params for sensor mic_click_sensor
#define MIC_CLICK_BASE_ADDRESS 400
#define MIC_CLICK_REG_DIMENSION 15
#define MIC_CLICK_SENSOR_I_ON 0.15
#define MIC_CLICK_SENSOR_T_ON 12
#define MIC_CLICK_SENSOR_I_IDLE 0.002
#define MIC_CLICK_SENSOR_T_ACT 0

// params for RF
#define RF_BASE_ADDRESS 500
#define RF_REG_DIMENSION 10
#define RF_ON_I_ON 0.1
#define RF_ON_T_ON 24
#define RF_I_IDLE 0.001

#define VREF_BUS 3.3
#define SIM_LEN 7736400