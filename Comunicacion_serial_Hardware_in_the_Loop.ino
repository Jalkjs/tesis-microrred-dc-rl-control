// ======================= LIBRERÍAS =======================
#include <Wire.h>
#include <Adafruit_INA219.h>

// ======================= CONFIGURACIÓN =======================
const int PIN_VOLT  = 32;  const int PIN_PWM  = 25;
const int PIN_VOLT2 = 34;  const int PIN_PWM2 = 26;
const int PIN_VOLT3 = 35;  const int PIN_PWM3 = 27;

const float VREF       = 3.3;
const int   ADC_RES    = 4095;
const float SCALE_VOLT = 3.357;

// ======================= INA219 =======================
Adafruit_INA219 ina219(0x40);    // Conv1 → default
Adafruit_INA219 ina219_2(0x41);  // Conv2 → A0=VCC
Adafruit_INA219 ina219_3(0x44);  // Conv3 → A1=VCC

// Flags para saber si cada INA está activo
bool ina1_ok = false;
bool ina2_ok = false;
bool ina3_ok = false;

// ======================= PWM =======================
const int PWM_CHANNEL  = 0;
const int PWM_CHANNEL2 = 1;
const int PWM_CHANNEL3 = 2;
const int PWM_FREQ = 15000;
const int PWM_RES  = 10;

// ======================= VARIABLES =======================
int u_control  = 0;
int u_control2 = 0;
int u_control3 = 0;

// ======================= FUNCIONES =======================
float readADC(int pin) {
  uint32_t sum = 0;
  for (int i = 0; i < 10; i++) sum += analogRead(pin);
  return ((sum / 10.0) / ADC_RES) * VREF;
}

float readVoltage(int pin) {
  return readADC(pin) * SCALE_VOLT;
}

//Lectura segura: si el INA no respondió en setup, devuelve 0
float readCurrent(Adafruit_INA219 &sensor, bool ok) {
  if (!ok) return 0.0;
  return sensor.getCurrent_mA() / 1000.0;
}

void setPWM(int u, int channel) {
  u = constrain(u, 0, 255);
  ledcWrite(channel, (int)(u * ((1 << PWM_RES) - 1) / 255.0));
}

// ======================= SETUP =======================
void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  ledcSetup(PWM_CHANNEL,  PWM_FREQ, PWM_RES); ledcAttachPin(PIN_PWM,  PWM_CHANNEL);
  ledcSetup(PWM_CHANNEL2, PWM_FREQ, PWM_RES); ledcAttachPin(PIN_PWM2, PWM_CHANNEL2);
  ledcSetup(PWM_CHANNEL3, PWM_FREQ, PWM_RES); ledcAttachPin(PIN_PWM3, PWM_CHANNEL3);

  Wire.begin();
  Wire.setClock(400000); // I2C a 400kHz (modo Fast) — reduce tiempo de lectura a la mitad

  ina1_ok = ina219.begin();
  ina2_ok = ina219_2.begin();
  ina3_ok = ina219_3.begin();

  Serial.println(ina1_ok ? "OK: INA Conv1 (0x40)" : "ERROR: INA Conv1 (0x40)");
  Serial.println(ina2_ok ? "OK: INA Conv2 (0x41)" : "ERROR: INA Conv2 (0x41)");
  Serial.println(ina3_ok ? "OK: INA Conv3 (0x44)" : "ERROR: INA Conv3 (0x44)");

  if (ina1_ok) ina219.setCalibration_32V_2A();
  if (ina2_ok) ina219_2.setCalibration_32V_2A();
  if (ina3_ok) ina219_3.setCalibration_32V_2A();
}

// ======================= LOOP =======================
void loop() {
  // -------- RECEPCIÓN PWM --------
  if (Serial.available() >= 3) {
    u_control  = Serial.read();
    u_control2 = Serial.read();
    u_control3 = Serial.read();
    setPWM(u_control,  PWM_CHANNEL);
    setPWM(u_control2, PWM_CHANNEL2);
    setPWM(u_control3, PWM_CHANNEL3);
  }

  // -------- ENVÍO CONTROLADO --------
  static unsigned long lastSend = 0;
  if (millis() - lastSend >= 50) {
    lastSend = millis();

    float V1 = readVoltage(PIN_VOLT);
    float I1 = readCurrent(ina219,   ina1_ok);
    float V2 = readVoltage(PIN_VOLT2);
    float I2 = readCurrent(ina219_2, ina2_ok);
    float V3 = readVoltage(PIN_VOLT3);
    float I3 = readCurrent(ina219_3, ina3_ok);

    // Un solo Serial.print con el paquete completo
    char buf[64];
    snprintf(buf, sizeof(buf), "%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;\n",
             V1, I1, V2, I2, V3, I3);
    Serial.print(buf);
  }
}