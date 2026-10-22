void updateDisplayAndBlynk() {

  if (!fingerDetected) {
    showNoFinger();
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V2, "No finger");
    digitalWrite(BUZZER_PIN, LOW);
    return;
  }

  // Temperature
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  if (tempC == DEVICE_DISCONNECTED_C || tempC < -10.0) {
    Serial.println("Temp sensor error!");

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(8, 20);
    display.print("TEMP ERR");
    display.setTextSize(1);
    display.setCursor(22, 44);
    display.print("Check DS18B20");
    display.display();

    return;
  }

  // Alert logic
  bool alert = false;

  if (beatAvg > 120 ||
      (beatAvg > 0 && beatAvg < 50)) {
    alert = true;
  }

  if (tempC > 35.5) {
    alert = true;
  }

  // Buzzer
  digitalWrite(BUZZER_PIN, alert ? HIGH : LOW);

  // Display
  showReadings(beatAvg, tempC, alert);

  // Blynk
  Blynk.virtualWrite(V0, beatAvg);
  Blynk.virtualWrite(V1, tempC);
  Blynk.virtualWrite(V2, alert ? "ALERT!" : "Normal");

  if (alert) {
    unsigned long now = millis();

    if (now - lastAlertTime > ALERT_COOLDOWN) {
      Blynk.logEvent(
        "health_alert",
        "Abnormal condition detected!"
      );

      lastAlertTime = now;
    }
  }

  Serial.printf(
    "BPM: %.0f | Avg: %d | Temp: %.1f C | Alert: %s\n",
    beatsPerMinute,
    beatAvg,
    tempC,
    alert ? "YES" : "no"
  );
}
