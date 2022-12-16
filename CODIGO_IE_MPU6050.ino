/*
 * ALUNOS:
 * Breno Linhares de Sousa - 170007057
 * Haniel Rodrigues Guimaraes de Lima - 150036558
 * Vitor Rangel de Aquino Silva - 1700064107
 * 
   Programa para Leitura do sensor de Aceleração nos eixos Ax, Ay e Az da MPU-6050
   Leitura feita via baramento I2C, impressão na Serial para ser vizualizada com o Serial Plotter do Arduino IDE

   Componentes:
   - ESP-32;
   - Sensor MPU-6050;
   - Protoboard;
   - Fios;
   - Cabo USB para carregar codigo

   Ligação
   MPU6050    ESP-32
   Vcc         3.3v
   GND         GND
   SCL         SCL   -  SERIAL CLOCK
   SDA         SDA   -  SERIAL DATA
*/

#include<Wire.h>                                //Biblioteca para comunicação I2C

const int MPU = 0x68;                           // Endereco I2C do sensor MPU-6050

float AccX, AccY, AccZ;      // Variaveis Globais para armazenar valores de aceleração nos eixos X, Y e Z

void setup() {
  
  Serial.begin(9600);       // inicializa comunicação serial com baud rate de 9600 bits/s

  Wire.begin();             // Inicializa comunicação I2C com a MPU-6050
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x08);
  Wire.endTransmission(true);


  // Configuração do fundo de escala do Acelerômetro
  /*
    0b00000000 -> fundo de escala em +/-2g
    0b00001000 -> fundo de escala em +/-4g
    0b00010000 -> fundo de escala em +/-8g
    0b00011000 -> fundo de escala em +/-16g
  */
  
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);                // Acesso ao registrador para configuração do fundo de escala
  Wire.write(0b00000000);          // definindo fundo de escala desejado para +/-2g do acelerômetro
  Wire.endTransmission();
}

void loop() {
  
  Wire.beginTransmission(MPU);     // Inicializando transmissão de dados
  Wire.write(0x3B);                // endereço da MPU-6050
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Solicita os dados ao sensor

  // Armazena o valor de 16 bits dos sensores de aceleracao nas variaveis de correspondente a cada eixo
  
  AccX = Wire.read() << 8 | Wire.read(); //0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AccY = Wire.read() << 8 | Wire.read(); //0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AccZ = Wire.read() << 8 | Wire.read(); //0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
 

  /* Alterar divisão conforme fundo de escala escolhido para conversão do valor em frações de gravidade:
      Acelerômetro
      +/-2g = 16384
      +/-4g = 8192
      +/-8g = 4096
      +/-16g = 2048
  */

  // Imprime na Serial os valores obtidos em frações de gravidade 1g = 9,82 m/s^2
  
  Serial.print("AX:");
  Serial.print(AccX / 16384); // divisão por 16384 para fundo de escala de +/-2g
  Serial.print(" ");
  Serial.print("AY:");
  Serial.print(AccY / 16384);
  Serial.print(" ");
  Serial.print("AZ:");
  Serial.println(AccZ / 16384);

  delay(200);  // taxa de atualização de valores
}
