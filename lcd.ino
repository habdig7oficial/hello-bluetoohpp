/*

    Copyright © 2024 Marcos Alexandre Vieira, Mateus Felipe da Silveira Vieira

    This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. 
*/


// Exemplo 9 - Display LCD 16x2
#include <LiquidCrystal.h> // biblioteca Liquid Crystal

#define ardRX 1
#define ardTX 2

LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // pinos do LCD - RS E D4 D5 D6 D7

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

void setup()
{
    lcd.begin(16, 2); // inicializa LCD 16x2
    lcd.clear(); // limpa tela do LCD

    Serial.begin(9600); // Inicia o monitor serial
    Serial1.begin(9600); // inicia a porta serial 1, do Bluetooth
    delay(500); // atraso de 0,5 segundos
}

void loop()
{
  if (direction == ardRX)
    if (Serial1.available())
    { // Lendo os caracteres enviados
      c = Serial1.read();
      Serial.print (c);
      if (c != 0x0d && c != 0x0a)
      {
         str[pos++] = c;
        if (pos >= 48)
          pos = 32;
      }

      if (c == 0x0a)
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
}
