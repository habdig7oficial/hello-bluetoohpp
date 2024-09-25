/*
    Copyright © 2024 Marcos Alexandre Vieira, Mateus Felipe da Silveira Vieira

    This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. 
*/


// Exemplo 9 - Display LCD 16x2
#include <LiquidCrystal.h> // biblioteca Liquid Crystal

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <RTClib.h>

#define MPU_I2CADDR_AD0_HIGH 0x69

#define ardRX   1
#define ardTX   2
#define ardSNS  3

#define led_1   22
#define led_2   23
#define led_3   24
#define led_4   25

#define CR      0x0d        // Cariage Return
#define LF      0x0a        // Line Feed

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // pinos do LCD - RS E D4 D5 D6 D7

Adafruit_MPU6050 mpu;

RTC_DS1307 rtc;

char hora[6]    = { 53, 52, 51, 50, 49, 48 };
char minuto[6]  = { 47, 46, 45, 44, 43, 42 };
char segundo[6] = { 41, 40, 39, 38, 37, 36 };

int h = 0;
int m = 0;
int s = 0;
int ms = 0;

int pos = 32;
int direction = ardRX;
char str[49] = "                                                     ";
char c;

int l1 = LOW;
int l2 = LOW;
int l3 = LOW;
int l4 = LOW;

char buf[2][40] = { "# MackTempo #\xd\xaSeja bem vindo!!\xd\xa",
                    "Quem é você?\xd\xa"
                  };
int count = 0;
int linha = 0;

char alerta[8] = "Caiu!\xd\xa";
int  l = 0;
int caiu = 0;

sensors_event_t a, g, temp;
sensors_event_t a_old, g_old, temp_old;


void setup()
{
  int i;

  lcd.begin(16, 2); // inicializa LCD 16x2
  lcd.clear(); // limpa tela do LCD

  pinMode (led_1, OUTPUT);
  pinMode (led_2, OUTPUT);
  pinMode (led_3, OUTPUT);
  pinMode (led_4, OUTPUT);

  for (i=0; i < 6; i++)
  {
    if (i != 5)
      pinMode (hora[i], OUTPUT);
    pinMode (minuto[i], OUTPUT);
    pinMode (segundo[i], OUTPUT);
  }

  //Serial.begin(115200); // Inicia o monitor serial via USB
  Serial.begin(115200);
  while (!Serial)
    delay (10);

  Serial1.begin(9600); // inicia a porta serial 1, do Bluetooth
  while (!Serial1)
    delay (10);

  if (!rtc.begin())
  {
    Serial.println ("RTC nao encontrado");
    while (1);
  }

  if (!rtc.isrunning())
  {
    Serial.println ("ajuste do RTC pelo PC");
    rtc.adjust (DateTime(F(__DATE__), F(__TIME__)));
  }

  // Tentando inicializar
  Serial.println("MPU6050 teste!");
  if (!mpu.begin(MPU_I2CADDR_AD0_HIGH)) 
  { 
    // Se caso o sensor não iniciar
    Serial.println ("A inicialização sensor MPU-6050 falhou");
    while (1) {} // Se a inicialização falhar, o programa irá travar aqui
  }
  Serial.println("MPU6050 encontrado!");
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); // Definindo o range de ação do acelerometro.
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);      // Definindo o range de ação do giroscopio.
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);   // Configurando a largura da banda do filtro

  mpu.getEvent(&a_old, &g_old, &temp_old);

  Serial.println("Iniciado OK"); // Fim das configuracoes iniciais
  delay(100);
}



void acende_led (char *vet, int mask, int valor)
{
  int i;

  for (i=0; mask; i++)
  {
    if (valor & mask)
      digitalWrite (vet[i], HIGH);
    else
      digitalWrite (vet[i], LOW);  
    mask >>= 1;
  }
}



void loop()
{
  int i;

  DateTime now = rtc.now();

  acende_led (hora,    0x10, now.hour());
  acende_led (minuto,  0x20, now.minute());
  acende_led (segundo, 0x20, now.second());

  /*
  acende_led (hora,    0x10, h);
  acende_led (minuto,  0x20, m);
  acende_led (segundo, 0x20, s);

  ms++;
  if (ms == 5 )
  {
    ms = 0;
    s++;
    if (s == 60)
    {
      s = 0;
      m++;
      if (m == 60)
      {
        m = 0;
        h++;
        if (h == 24)
          h = 0;
      }
    }
  }
  */
  delay (10);


  l1 = (l1 == HIGH)? LOW : HIGH;
 // digitalWrite (led_1, l1);

  mpu.getEvent(&a, &g, &temp);

  //if (abs(a.acceleration.x - a_old.acceleration.x) > 0.2 || abs(a.acceleration.y-a_old.acceleration.y) > 0.2 || abs(a.acceleration.z-a_old.acceleration.z) > 0.2)
  if (abs(g.gyro.z-g_old.gyro.z) > 0.5)
  {
    Serial1.println(++caiu);

    // Imprimindo no monitor Serial os valores capturados
    Serial.print("Aceleracao em X: ");
    Serial.print(a.acceleration.x);
    Serial.print(", Y: ");
    Serial.print(a.acceleration.y);
    Serial.print(", Z: ");
    Serial.print(a.acceleration.z);
    Serial.println(" m/s^2");

    Serial.print("Rotacao em X: ");
    Serial.print(g.gyro.x);
    Serial.print(", Y: ");
    Serial.print(g.gyro.y);
    Serial.print(", Z: ");
    Serial.print(g.gyro.z);
    Serial.println(" rad/s");
      
    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.println(" oC");
    Serial.println("");

    Serial.print ("Caiu ");
    Serial.println(caiu);

    //direction = ardSNS;
  }

  a_old = a;
  g_old = g;
  temp_old = temp;



  //digitalWrite (led_2, l1);

  //digitalWrite (led_2, l2);
  //l2 = (l2 == HIGH)? LOW : HIGH;

  if (Serial1.available())
  { 
    // Lendo os caracteres enviados
    c = Serial1.read ();
    //Serial.print (c);

    if (c != CR && c != LF)
    {
      str[pos++] = c;
      if (pos >= 48)
      {
        for (i=0; i<32; i++)
          str[i] = str[i + 16];
        pos = 32;
      }
    }

    if (c == LF && pos != 32)
    {
      while (pos < 48)
        str[pos++] = 32;

      for (i=0; i<32; i++)
        str[i] = str[i + 16];

      pos = 32;
      direction = ardTX;
    }

    lcd.setCursor(0, 0); // selecionando coluna 0 e linha 0
    lcd.print(str);
    lcd.setCursor(0, 1);
    lcd.print(str+16);
  }



 // digitalWrite (led_3, l1);

  //digitalWrite (led_3, l3);
  //l3 = (l3 == HIGH)? LOW : HIGH;

  while (direction == ardTX)
    if (Serial1.availableForWrite())
    {
      Serial1.write(buf[linha][count++]);
      if (buf[linha][count] == 0)
      {
        count = 0;
        linha++;
        if (linha >= 2)
          linha = 0;
        direction = ardRX;
      }
    }



  //digitalWrite (led_4, l1);

  /*
  //digitalWrite (led_4, l4);
  //l4 = (l4 == HIGH)? LOW : HIGH;

  while (direction == ardSNS)
    if (Serial1.availableForWrite())
    {
      Serial1.write(alerta[l++]);
      if (l >= 7)
      {
        l = 0;
        direction = ardRX;
        count = 0;
        linha = 0;
        pos = 32;
        for (i = 0; i < sizeof(str) - 1; i++)
          str[i] = ' ';
      }
    }
  */
}
