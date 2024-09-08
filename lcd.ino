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

#define ardRX  1
#define ardTX  2
#define ardSNS 3

#define CR     0x0d        // Cariage Return
#define LF     0x0a        // Line Feed

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // pinos do LCD - RS E D4 D5 D6 D7

Adafruit_MPU6050 mpu;

int i = 0;
int pos = 32;
int direction = ardRX;
char str[49] = "                                                     ";
char c;

char buf[2][40] = { "# MackTempo #\xd\xaSeja bem vindo!!\xd\xa",
                    "Quem é você?\xd\xa"
                  };
int count = 0;
int linha = 0;

char alerta[8] = "Caiu!\xd\xa";
int  l = 0;

sensors_event_t a, g, temp;
sensors_event_t a_old, g_old, temp_old;


void setup()
{
  lcd.begin(16, 2); // inicializa LCD 16x2
  lcd.clear(); // limpa tela do LCD

  Serial.begin(9600); // Inicia o monitor serial via USB
  while (!Serial)
    delay (10);

  Serial1.begin(9600); // inicia a porta serial 1, do Bluetooth
  while (!Serial1)
    delay (10);

  // Tentando inicializar
  Serial.println("MPU6050 teste!");
  if (!mpu.begin()) 
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

void loop()
{
  if (direction == ardRX)
    if (Serial1.available())
    { 
      // Lendo os caracteres enviados
      c = Serial1.read ();
      //Serial.print (c);

      if (c != CR && c != LF)
      {
         str[pos++] = c;
        if (pos >= 48)
          pos = 32;
      }

      if (c == LF)
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

  if (direction == ardTX)
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

  if (direction == ardSNS)
  {
    if (Serial1.availableForWrite())
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
  else
  {
    mpu.getEvent(&a, &g, &temp);

    //if (g.gyro.x!=g_old.gyro.x || g.gyro.y!=g_old.gyro.y || g.gyro.z!=g_old.gyro.z)
    //if (abs(a.acceleration.x - a_old.acceleration.x) > 0.1 || abs(a.acceleration.y-a_old.acceleration.y) > 0.1 || abs(a.acceleration.z-a_old.acceleration.z) > 0.1)
    if (abs(g.gyro.z-g_old.gyro.z) > 0.5)
    {
      /*
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
      */

      //Serial.println ("Caiu!");
      direction = ardSNS;
    }

    a_old = a;
    g_old = g;
    temp_old = temp; 
  }
}
