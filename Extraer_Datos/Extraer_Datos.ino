// === Lectura de 30 datos tras reset y promedio (ESP32) ===
const int PIN_ADC = 36;      // ADC1 recomendado si usas Wi‑Fi
const int NUM_DATOS = 30;    // cantidad pedida
const int SAMPLE_DELAY_MS = 50; // pausa entre muestras para estabilizar

// --------- CALIBRACIÓN (REEMPLAZA SEGÚN TU CASO) ---------
const float V_MIN_mV  = 2061.47;    // mV al nivel mínimo  <-- REEMPLAZA
const float V_MAX_mV  = 2975.37;   // mV al nivel máximo  <-- REEMPLAZA
const float NIVEL_MIN = 20;      // p. ej., 0 %
const float NIVEL_MAX = 70;    // p. ej., 100 %
// ---------------------------------------------------------

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  if (in_max == in_min) return out_min;
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static float clampToRange(float v, float a, float b) {
  float lo = a < b ? a : b;
  float hi = a < b ? b : a;
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

void setup() {
  Serial.begin(115200);
  delay(200);

  analogReadResolution(12);                 // 0–4095
  analogSetPinAttenuation(PIN_ADC, ADC_11db); // amplia rango útil (~3.1 V aprox.)

  // Acumuladores para promedios
  double sum_raw = 0.0;
  double sum_mv  = 0.0;
  double sum_lvl = 0.0;

  Serial.println("Inicio de muestreo (30 lecturas):");

  for (int i = 1; i <= NUM_DATOS; i++) {
    int raw = analogRead(PIN_ADC);             // RAW (sin unidades)
    int mv  = analogReadMilliVolts(PIN_ADC);   // mV (calibrado)

    float nivel = mapFloat((float)mv, V_MIN_mV, V_MAX_mV, NIVEL_MIN, NIVEL_MAX);
    nivel = clampToRange(nivel, NIVEL_MIN, NIVEL_MAX);

    sum_raw += raw;
    sum_mv  += mv;
    sum_lvl += nivel;

    Serial.print("#"); Serial.print(i);
    Serial.print(" RAW: "); Serial.print(raw);
    Serial.print(" | mV: "); Serial.print(mv);
    Serial.print(" | Nivel: "); Serial.print(nivel, 2);
    Serial.println();

    delay(SAMPLE_DELAY_MS);
  }

  double prom_raw = sum_raw / NUM_DATOS;
  double prom_mv  = sum_mv  / NUM_DATOS;
  double prom_lvl = sum_lvl / NUM_DATOS;

  Serial.println("---- PROMEDIOS ----");
  Serial.print("RAW_prom: "); Serial.println(prom_raw, 2);
  Serial.print("mV_prom:  "); Serial.println(prom_mv, 2);
  Serial.print("Nivel_prom: "); Serial.println(prom_lvl, 2);

  Serial.println("Esperando nuevo RESET para repetir...");
}

void loop() {
  // Inactivo hasta próximo reset físico
  delay(1000);
}

