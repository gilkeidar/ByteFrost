/*
  ByteFrost Notes
1. IDE Setup
      a. Board "Arduino NANO"
      b. CPU "ATmega328p (old bootloader)"
2. To program the Arduino - the RESET control signal (black) need to be disconnected!!.
3. Currently the Tx is used by another signal. Need to free Tx and transmit the display data on it.
4. To allow Serial printing through Rx/Tx (D1/D0) The Display is configured to use only 4 bit to the Display (D4-D7).
This is achieved by using a constuctor with only 4 data bits, and using the write4bits()



  ----------------------------------------------
  LiquidCrystal Library - Hello World

  Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
  library works with all LCD displays that are compatible with the
  Hitachi HD44780 driver. There are many of them out there, and you
  can usually tell them by the 16-pin interface.

  This sketch prints "Hello World!" to the LCD
  and shows the time.

  The circuit:
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   LCD VSS pin to ground
   LCD VCC pin to 5V
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)

  Library originally added 18 Apr 2008
  by David A. Mellis
  library modified 5 Jul 2009
  by Limor Fried (http://www.ladyada.net)
  example added 9 Jul 2009
  by Tom Igoe
  modified 22 Nov 2010
  by Tom Igoe
  modified 7 Nov 2016
  by Arturo Guadalupi

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld

*/
#define TEST_INPUT

// include the library code:
#include <LiquidCrystal.h>
#define ROWS  4
#define COLS  20
#define PRINT_CHAR  0
#define ESCAPE_CHAR 1
#define QUEUE_SIZE  256

// Library functions

#define setPin(b) ( (b)<8 ? PORTD |=(1<<(b)) : PORTB |=(1<<(b-8)) )

#define clrPin(b) ( (b)<8 ? PORTD &=~(1<<(b)) : PORTB &=~(1<<(b-8)) )

#define tstPin(b) ( (b)<8 ? (PORTD &(1<<(b)))!=0 : (PORTB &(1<<(b-8)))!=0 )


typedef struct queue_item {
  byte input_char;  // Input character
  byte type;        // How to print ('A' or 'I')
} Character;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d0 = 1, d1 = 0, d2 = 13, d3 = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7; // d0 and d1 are switched in the implementation
int display_data_out[] = {d0, d1, d2, d3, d4, d5, d6, d7};    
// LiquidCrystal lcd(rs, en, d0, d1, d2, d3, d4, d5, d6, d7);  // Initialize for 8bit
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                  // Initialize for 4bit

void pulseEnable(void)
{
  clrPin(en);
  delayMicroseconds(10);
  setPin(en);
  delayMicroseconds(15);
  clrPin(en);
  delayMicroseconds(12);
}

void write4bits(uint8_t value) 
{
  PORTD = ((value & 0x0f) << 4) |(PORTD & 0x0f);  // PD4, PD5, PD6, PD7  <- value  | dont change D3:D0
  pulseEnable();
}

void write8bits(uint8_t value) {
  //PORTD = value;

  //Serial.print("Printing ");
  //Serial.println((char) value);

  int i;
  for (i = 0; i < 8; i++)
  {
    digitalWrite(display_data_out[i], (value >> i) & 0x01);
  }

  pulseEnable();
}

// write either command or data, with automatic 4/8-bit selection
void send(uint8_t value, uint8_t mode)
{
  digitalWrite(rs, mode);

  // write8bits(value);

  write4bits(value>>4);
  write4bits(value);
}

inline size_t write(uint8_t value) 
{
  send(value, HIGH);
  return 1; // assume sucess
}

// Library functions

void scroll_up();
void new_line();
int convert_input(byte * input);
void disp_write(char input);



byte input_char;        // Input from data_bus_in
bool valid_input;       // Whether or not reading value from data_bus_in
int cur_row;            // Current row
int cur_col;            // Current column
byte shadow[ROWS][COLS];  // Shadow of display
byte disp_buffer[ROWS * COLS];  //  Display buffer 
int buffer_index;               //  Buffer index

// Buffer (queue)
Character queue[QUEUE_SIZE]; // Input queue
byte queue_base = 0;     // Base index of queue
byte queue_pos = 0;      // Position in queue

// Computer Bus vars
byte disp_en = 2;
byte disp_en_val;
byte ascii_or_int = 10;  // D10 If 0: Print as ASCII; if 1: Print as integer (hex)
//byte ascii_or_int = 20;  // A6 If 0: Print as ASCII; if 1: Print as integer (hex) - Didn't work for high speed as A6 needs analogRead that takes 100uS
byte data_bus_in[] = {14, 15, 16, 17, 18, 19, 8, 9}; // A0, A1, A2, A3, A4, A5, D8, D9

// Edge detector vars
bool was_low = true;    // For positive edge
bool was_high = true;   // For negative edge

void scroll_up()
{
  // Scroll upwards
  int i, j;
  for (i = 1; i < ROWS; i++)
  {
    lcd.setCursor(0, i - 1);
    for (j = 0; j < COLS; j++)
    {
      /*if(shadow[i - 1][j] != shadow[i][j])
      {
        write(shadow[i][j]);
      }*/
      shadow[i - 1][j] = shadow[i][j];
      //lcd.setCursor(j, i - 1);
      //lcd.write(shadow[i-1][j]);
      write(shadow[i-1][j]);
    }
  }

  lcd.setCursor(0, ROWS - 1);
  for (j = 0; j < COLS; j++)
  {
    shadow[ROWS - 1][j] = ' ';
    write(' ');
  }
}

void new_line()
{
  // Set cursor to new_line
  cur_row++;
  cur_col = 0;
  if (cur_row >= ROWS)
  {
    cur_row = ROWS - 1;
    scroll_up();
  }

  lcd.setCursor(cur_col, cur_row);
  Serial.println();
}

int convert_input(byte * input)
{
  if (*input < 10)
  {
    *input = *input + 48;
    return PRINT_CHAR;
  }
  if (*input < 16)
  {
    *input = *input - 10 + 'A';
    return PRINT_CHAR;
  }

  // ESCAPE SEQUENCES
  if (*input == 0x10)
  {
    // NEWLINE
    new_line();
    return ESCAPE_CHAR;
  }

  return PRINT_CHAR;
}

void disp_write(unsigned char input)
{
    if (cur_col >= COLS)
    {
      // Overflowed cols, change cursor position to first line on next row
      cur_col = 0;
      cur_row++;

      if (cur_row >= ROWS) // Overflowed, go back to first position
      {
        //cur_row = cur_col = 0;
        scroll_up();
        cur_row = ROWS - 1;
        cur_col = 0;
      }

      lcd.setCursor(cur_col, cur_row); 
    }
     //lcd.setCursor(cur_col, cur_row);
     //lcd.write(input);

    write(input);
    Serial.write(input);  
    // Write character to shadow
    shadow[cur_row][cur_col] = input;
    cur_col++;
}


byte input_type;
Character curr_char;
int num_interrupts = 0;
int num_low_interrupts = 0;

void disp_en_handler()
{
  // Read input and add to queue
  input_type = (PINB & 0x04) >> 2; // PB2
 // input_type = (analogRead(A6) > 100); RK: This test failed as analog read takes 100uS, so it work nice while clock is slow but cannot fit ByteFrost at 400KHz!
  input_char = (PINC & 0x3f) | ((PINB & 0x03) << 6);  // PB1, PB0, PC5..PC0

  queue[queue_pos++] = {input_char, input_type};
  num_low_interrupts++;
}

void setup() 
{
  // set up the LCD's number of columns and rows:
  lcd.begin(COLS, ROWS);
  lcd.cursor();
  lcd.blink();

  // Set data_bus_in pins
  pinMode(disp_en, INPUT);      //  Set D13 as "display_enable" input
  pinMode(ascii_or_int, INPUT); //  Set D8 as "ascii or integer" input
  int i;
  for (i = 0; i < sizeof(data_bus_in); i++)
  {
    pinMode(data_bus_in[i], INPUT); 
  }

  for (i = 0; i < 7; i++)
    pinMode(display_data_out[i], OUTPUT);

  valid_input = false;
  cur_row = 0;
  cur_col = 0;

  // Initialize display shadow to store all spaces (32)
  memset(shadow, 32, sizeof(shadow));

  buffer_index = 0;

  // Setup interrupt handling
  attachInterrupt(digitalPinToInterrupt(disp_en), disp_en_handler, RISING); // inverted source signal so both display and the display register should react to rising edge of disp_en signal

  // Debug
#ifdef TEST_INPUT
  Serial.begin(57600); // open the serial port at 57600 bps:
#endif
}

void loop() {
  if (queue_pos != queue_base)
  {
    
    // Queue not empty - print first character
    curr_char = queue[queue_base];
    if (!curr_char.type)  // Print as ASCII
    {
      //Serial.println("Printing ascii");
      if (convert_input(&(curr_char.input_char)) == PRINT_CHAR)
      {
        //Serial.print("curr_char.input_char: ");
        //Serial.println(curr_char.input_char);
        disp_write(curr_char.input_char);
      }
    }
    else
    {
      // Print as two 4-byte characters (print in hex)
      byte high_digit = curr_char.input_char >> 4;
      byte low_digit = curr_char.input_char & 0x0f;
      convert_input(&high_digit);
      convert_input(&low_digit);
      disp_write(high_digit);
      disp_write(low_digit);
    }
    queue_base++;
  }
}
