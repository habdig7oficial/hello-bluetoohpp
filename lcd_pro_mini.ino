/*
    Copyright © 2024 Marcos Alexandre Vieira, Mateus Felipe da Silveira Vieira

    This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. 
*/


// Exemplo 9 - Display LCD 16x2
//#include <LiquidCrystal.h> // biblioteca Liquid Crystal

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <RTClib.h>

#define MPU_I2CADDR_AD0_HIGH 0x69

#define ardRX       1
#define ardTX       2
#define ardSNS      3

#define led_PILOTO  2

#define CR          0x0d        // Cariage Return
#define LF          0x0a        // Line Feed

//LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // pinos do LCD - RS E D4 D5 D6 D7

Adafruit_MPU6050 mpu;

RTC_DS1307 rtc;

int ld = HIGH;

//char hora[6]    = { 53, 52, 51, 50, 49, 48 };
//char minuto[6]  = { 2, 3, 4, 5, 6, 7 };
//char segundo[6] = { 8, 9, 10, 11, 12, 13 };

//int h = 0;
//int m = 0;
//int s = 0;
//int ms = 0;

int pos = 32;
int direction = ardRX;
char str[49] = "                                                ";
char c;

char buf[2][40] = { "# MackTempo #\xd\xaSeja bem vindo!!\xd\xa",
                    "Quem é você?\xd\xa"
                  };
int count = 0;
int linha = 0;

char alerta[8] = "Caiu!\xd\xa";
int  l = 0;
int caiu = 0;

int dia;
int mes;
int ano;

sensors_event_t a, g, temp;
sensors_event_t a_old, g_old, temp_old;


void setup()
{
  int i;

  pinMode (led_PILOTO, OUTPUT);
  digitalWrite (led_PILOTO, ld);

//  for (i=0; i < 6; i++)
//  {
//    if (i != 5)
//      pinMode (hora[i], OUTPUT);
//    pinMode (minuto[i], OUTPUT);
//    pinMode (segundo[i], OUTPUT);
//  }

  Serial.begin(9600);
  while (!Serial)
    {
      ld = (ld = HIGH)? LOW : HIGH;
      digitalWrite (led_PILOTO, ld);
      delay (200);
    }

  ld = HIGH;
  digitalWrite (led_PILOTO, ld);

  if (!rtc.begin())
  {
    while (1)    // Se a inicialização falhar, o programa irá travar aqui
    {
      ld = (ld = HIGH)? LOW : HIGH;
      digitalWrite (led_PILOTO, ld);
      delay (100);
    }
  }

  if (!rtc.isrunning())
  {
    rtc.adjust (DateTime(F(__DATE__), F(__TIME__)));
  }


  // Tentando inicializar
  if (!mpu.begin(MPU_I2CADDR_AD0_HIGH)) 
  { 
    while (1)    // Se a inicialização falhar, o programa irá travar aqui
    {
      ld = (ld = HIGH)? LOW : HIGH;
      digitalWrite (led_PILOTO, ld);
      delay (200);
    }
  }
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G); // Definindo o range de ação do acelerometro.
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);      // Definindo o range de ação do giroscopio.
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);   // Configurando a largura da banda do filtro

  mpu.getEvent(&a_old, &g_old, &temp_old);

  delay(100);
}


/*
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
*/


void loop()
{
  int i;

  DateTime now = rtc.now();

//  acende_led (hora,    0x10, now.hour());
//  acende_led (minuto,  0x20, now.minute());
//  acende_led (segundo, 0x20, now.second());

  ld = (ld == HIGH)? LOW : HIGH;
  digitalWrite (led_PILOTO, ld);

  mpu.getEvent(&a, &g, &temp);

  //if (abs(a.acceleration.x - a_old.acceleration.x) > 0.2 || abs(a.acceleration.y-a_old.acceleration.y) > 0.2 || abs(a.acceleration.z-a_old.acceleration.z) > 0.2)
  if (abs(g.gyro.z-g_old.gyro.z) > 0.5)
  {
    //dia = now.day();
    //mes = now.month();
    //ano = now.year();

    //Serial.print (dia);
    //Serial.print ("\\");
    //Serial.print (mes);
    //Serial.print ("\\");
    //Serial.print (ano);
    //Serial.print (" - ");
    Serial.println (++caiu);

    // Imprimindo no monitor Serial os valores capturados
    /*
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
    */
  }

  a_old = a;
  g_old = g;
  temp_old = temp;



  if (Serial.available())
  { 
    // Lendo os caracteres enviados
    c = Serial.read ();

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
  }



  while (direction == ardTX)
    if (Serial.availableForWrite())
    {
      Serial.write(buf[linha][count++]);

      if (buf[linha][count] == 0)
      {
        count = 0;
        linha++;
        if (linha >= 2)
          linha = 0;
        direction = ardRX;
      }
    }
}
