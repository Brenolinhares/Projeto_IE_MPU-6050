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

#include <SPI.h>                                //Biblioteca para comunicação SPI do display
#include <Adafruit_GFX.h>                       // Biblioteca gráfica para exibir graficos como linhas círculos, retângulos etc
#include <Adafruit_SSD1306.h>                   // Biblioteca para display OLED
#include<Wire.h>                                //Biblioteca para comunicação I2C


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI   23
#define OLED_CLK    18
#define OLED_DC     16
#define OLED_CS     5
#define OLED_RESET  17

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
  
const int MPU = 0x68;         // Endereco I2C do sensor MPU-6050

float AccX, AccY, AccZ;      // Variaveis Globais para armazenar valores de aceleração nos eixos X, Y e Z

float AXc,AYc,AZc; // saída após calibração

float angulo_x; // angulo medido utilizando vetores de aceleracao em relação ao eixo x
float angulo_y; // angulo em relação ao eixo Y
float angulo_z; // angulo em relação ao eixo Z

float modulo;


// PROTOTIPOS DE FUNCAO

void mostra_display_OLED(); // função para visualização de dados no display

void setup() {
  
  // MPU
  
  Serial.begin(9600);       // inicializa comunicação serial com baud rate de 9600 bits/s

  Wire.begin();             // Inicializa comunicação I2C com a MPU-6050
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x08);        // seta registrador para habilitar acelerometro e desbilitar giroscópio e sensor de temperatura
  //0x08
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

  // DISPLAY OLED

  //Serial.begin(115200);
    if(!display.begin(SSD1306_SWITCHCAPVCC))
    {
     Serial.println(F("SSD1306 allocation failed")); // mensagem de erro caso tenha falha na comunicação com o dsiplay
      for(;;);
    }
  
   display.clearDisplay();  // limpa tela do display
   display.display();
   delay(1000);
   
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

  // EQUACOES de calibracao com GANHOS e OFFSETS calculados 

  AXc = 0.99745*(int16_t(AccX)/16384.0)+0.04255; // calibracao eixo X do acelerômetro
  AYc = 0.99*(int16_t(AccY)/16384.0)-0.0193;     // calibracao eixo Y do acelerômetro
  AZc = 1.00815*(int16_t(AccZ)/16384.0)-0.04815; // calibracao eixo Z do acelerômetro

 // CONVERCAO DE DADOS PARA ANGULO (GRAUS)

  modulo = sqrt((AXc*AXc) + (AYc*AYc)+ (AZc*AZc)); // modulo do vetor aceleração
  angulo_x = acos(AXc/modulo) * 180/PI; // angulo em relacao ao eixo X ROLL 180/PI conversão de radianos para graus
  angulo_y = acos(AYc/modulo) * 180/PI; // angulo em relacao ao eixo Y PITCH
  angulo_z = acos(AZc/modulo) * 180/PI; // angulo em relacao ao eixo Y YAW
  
  
  // Imprime na Serial os valores obtidos em frações de gravidade 1g = 9,82 m/s^2 já calibrados
  
  // PRINT DOS RESULTADOS APÓS CALIBRAÇÃO EM FRAÇÕES DE GRAVIDADE [g]
 
   //Serial.print("AXc:");
   //Serial.print(AXc);
   //Serial.print(" ");
   //Serial.print("AYc:");
   //Serial.print(AYc);
   //Serial.print(" ");
   //Serial.print("AZc:");
   //Serial.println(AZc);
  
  // PRINT RESULTADOS ANTES DA CALIBRAÇÃO EM FRAÇÕES DE GRAVIDADE [g]
  
  //Serial.print("AX:");
  //Serial.println(int16_t(AccX) / 16384.0); // divisão por 16384 para fundo de escala de +/-2g
  //Serial.print(" ");
  //Serial.print("AY:");
  //Serial.println(int16_t(AccY) / 16384.0);
  //Serial.print(" ");
  //Serial.print("AZ:");
  //Serial.println(int16_t(AccZ) / 16384.0);

  // PRINT DOS ANGULOS

   //Serial.print("angulo_x_1:");
   //Serial.print(angulo_x_1);
   //Serial.print(" ");
   Serial.print("angulo_y:");
   Serial.println(angulo_y);
   //Serial.print(" ");
   //Serial.print("angulo_z_1:");
   //Serial.println(angulo_z_1);
  

  mostra_display_OLED();

  delay(200);  // taxa de atualização de valores
}

void mostra_display_OLED(){

  display.display();
  display.clearDisplay();   // clears the screen and buffer 
  // text display tests
  display.setTextSize(1);
  //display.setTextColor(WHITE);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);

 /*
  // Imprime no display VALORES NAO CALIBRADOS
  
  display.print("AX: ");
  display.println(int16_t(AccX) / 16384.0); // valor sem calibração em frações de g
  //display.print("\n");
  
  display.print("AY: ");
  display.println(int16_t(AccY) / 16384.0);
  //display.print("\n");
  
  display.print("AZ: ");
  display.println(int16_t(AccZ) / 16384.0);
  display.print("\n");

  */
  // Imprime no display VALORES APOS CALIBRACAO
  
   display.print("AXc: ");
  display.println(AXc); 
  
  display.print("AYc: ");
  display.println(AYc);
  
  display.print("AZc: ");
  display.println(AZc);
  
  // Imprime no display o VALOR DO ÂNGULO em relação ao eixo Y
  display.print("\n");
  display.print("Angulo: ");
  display.println(angulo_y);
 
   
  delay(10);
}
