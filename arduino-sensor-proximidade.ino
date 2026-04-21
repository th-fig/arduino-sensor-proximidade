//
//
// Caro individuo que está lendo isso, não tente entender como esse código funciona, nem eu mesmo sei o que eu fiz nele, não se submeta a isso
//
//
#include <LiquidCrystal_I2C.h>
#include <math.h>

#define pin_return 8 // pino de saída
#define start_stage 1 // estado inicial do pino de saída (0 ou 1 // LOW ou HIGH)
#define pin_trig 7 // pino trig do sensor ultrassônico
#define pin_echo 6 // pino echo do sensor ultrassônico
#define count_max 5 // contagem de tempo para o sensor atuar (1 == 100ms)
#define distance_min 10 // distância mínima para o sensor atuar em cm


LiquidCrystal_I2C lcd(0x27, 16, 2);

int distance = 0;
int distance_old = 0;
int count = 0; 

// variaveis para evitar oscilações no display

int d1 = 0;
int d2 = 0; 

int ultrasonic_read(int TRIG, int ECHO) { // por não existir biblioteca de sensor ultrassonico no tinkercad tive que criar a função do zero
  float duracao;
  int distancia;
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duracao = pulseIn(ECHO, HIGH);

  distancia = ceil(duracao * 0.034 / 2);

  return distancia;
}

void lcd_count() { // mostra contagem de tempo na tela LCD_I2C
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Contagem: ");
  lcd.print(count);
}

void lcd_distance() { // mostra distância do sensor na tela LCD_I2C
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Distancia: ");
    lcd.setCursor(11,1);
    lcd.print(distance);
    lcd.print("cm");
}

void pin_control() { // controla pino de saída do arduino

  while (distance < distance_min) { // contagem de tempo
    distance = ultrasonic_read(pin_trig, pin_echo);

    if (count >= count_max) {
      break;
    }
    count = count + 1;
    lcd_count();
    delay(1000);
  } 

  if ((count >= count_max) and (digitalRead(pin_return) == HIGH)) {
    digitalWrite(pin_return, LOW);
    count = 0;
  }

  if ((count >= count_max) and (digitalRead(pin_return) == LOW)) {
    digitalWrite(pin_return, HIGH);
    count = 0;
  }
}

bool compara() { // função pra tentar fazer o lcd não oscilar eternamente
  if ((distance != distance_old) and (distance != d1) and (distance != d2)) {
    return true;
  }
  else {
    return false;
  }
}
void setup() {

  Wire.begin();
  lcd.init();
  lcd.display();
  lcd.backlight();

  pinMode(pin_return, OUTPUT);
  digitalWrite(pin_return, start_stage);
}

void loop() {

  distance = ultrasonic_read(pin_trig, pin_echo);
  if (distance >= distance_min) {
      count = 0;
      if (compara()) {
        lcd_count();
      }

    }
  if ((distance < distance_min) && (compara())) {
    pin_control();
  }
  distance_old = ultrasonic_read(pin_trig, pin_echo);
  d1 = distance_old + 1;
  d2 = distance_old - 1;
}
