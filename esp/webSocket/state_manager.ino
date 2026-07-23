
void led_manage(bool is_blink) {
  unsigned long interval = is_blink ? 250 : 1000;

  if (millis() - led_timer >= interval) {
    led_timer = millis();
    led_state = !led_state;
    digitalWrite(led, led_state);
  }
}

void buzz_manage(){
  unsigned long interval = 250;

  if (millis() - buzz_timer >= interval) {
    buzz_timer = millis();
    buzz_state = !buzz_state;
    if (buzz_state) {
      tone(buzz, 1000);
    } else {
      noTone(buzz);
      digitalWrite(buzz, HIGH);  // noTone() leaves the pin LOW, which is "on" for this active-low module
    }
  }
}


void handle_armed() {
  led_manage(false);
}

void handle_sos() {
  led_manage(true);
  buzz_manage();
}

void handle_rst() {
  buzz_state = false;
  noTone(buzz);
  digitalWrite(buzz, HIGH);
}


void state_loop() {
  // this switch for sending state to the socket and run once
  if (state != LastState) {
    LastState = state;
    switch (state) {
      case armed:
        {
          String json = setObj(armed, "esp is in armed mode");
          client.send(json);
        }
        break;
      case sos:
        {
          String json = setObj(sos, "esp sends an sos");
          client.send(json);
        }
        break;
      case rst:
        {
          String json = setObj(rst, "esp sends an reset");
          client.send(json);
          state = armed;
        }
        break;
    }
  }

  // this switch rund like a loop
  switch (state) {

    case armed:
      handle_armed();
      break;

    case sos:
      handle_sos();
      break;

    case rst:
      handle_rst();
      break;
  }
}