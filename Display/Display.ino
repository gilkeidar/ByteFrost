/*
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
int _data_pins[] = {d0, d1, d2, d3, d4, d5, d6, d7};    // For modified write4bits we're using
LiquidCrystal lcd(rs, en, d0, d1, d2, d3, d4, d5, d6, d7);

void pulseEnable(void) {
  clrPin(en);
  delayMicroseconds(10);
  setPin(en);
  delayMicroseconds(15);
  clrPin(en);
  delayMicroseconds(12);
  
  //digitalWrite(en, LOW);
  //delayMicroseconds(1);    
  //digitalWrite(en, HIGH);
  //delayMicroseconds(1);    // enable pulse must be >450ns
  //digitalWrite(en, LOW);
  //delayMicroseconds(1);   // commands need > 37us to settle
}

void write4bits(uint8_t value) {
  //for (int i = 0; i < 4; i++) {
  //  digitalWrite(_data_pins[i], (value >> i) & 0x01);
    /*if ((value >> i) & 0x01)
    {
      setPin(_data_pins[i]);
    }
    else
    {
      clrPin(_data_pins[i]);
    }*/
  //}

  PORTD = ((value & 0x0f) << 2) |(PORTD & 0xc3);
  
  //delayMicroseconds(1);

  pulseEnable();
}

void write8bits(uint8_t value) {
  //PORTD = value;

  //Serial.print("Printing ");
  //Serial.println((char) value);

  int i;
  for (i = 0; i < 8; i++)
  {
    digitalWrite(_data_pins[i], (value >> i) & 0x01);
  }

  pulseEnable();
}

// write either command or data, with automatic 4/8-bit selection
void send(uint8_t value, uint8_t mode) {
  digitalWrite(rs, mode);
  /*if (mode)
    setPin(rs);
  else
    clrPin(rs);

  delayMicroseconds(1);*/

  // if there is a RW pin indicated, set it low to Write
  /*if (_rw_pin != 255) { 
    digitalWrite(_rw_pin, LOW);
  }*/
  
  /*if (_displayfunction & LCD_8BITMODE) {
    write8bits(value); 
  } else {
    write4bits(value>>4);
    write4bits(value);
  }*/

  write8bits(value);

  //write4bits(value>>4);
  //write4bits(value);
}

inline size_t write(uint8_t value) {
  send(value, HIGH);
  return 1; // assume sucess
}

// Library functions

void scroll_up();
void new_line();
int convert_input(byte * input);
void disp_write(char input);



byte input_char;        // Input from bus
bool valid_input;       // Whether or not reading value from bus
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
byte ascii_or_int = 10;  // If 0: Print as ASCII; if 1: Print as integer (hex)
byte bus[] = {14, 15, 16, 17, 18, 19, 8, 9}; // A0, A1, A2, A3, A4, A5, D8, D9

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
    /*if (shadow[ROWS - 1][j] != ' ')
      write(' ');*/
    shadow[ROWS - 1][j] = ' ';
    //lcd.setCursor(j, ROWS - 1);
    //lcd.write(' ');
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

void disp_write(char input)
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

    // Write character to shadow
    shadow[cur_row][cur_col] = input;
    cur_col++;
}

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(COLS, ROWS);
  lcd.cursor();
  lcd.blink();

  // Set Bus pins
  pinMode(disp_en, INPUT);      //  Set D13 as "display_enable" input
  pinMode(ascii_or_int, INPUT); //  Set D8 as "ascii or integer" input
  int i;
  for (i = 0; i < sizeof(bus); i++)
  {
    pinMode(bus[i], INPUT); 
  }

  for (i = 0; i < 7; i++)
    pinMode(_data_pins[i], OUTPUT);

  valid_input = false;
  cur_row = 0;
  cur_col = 0;

  // Initialize display shadow to store all spaces (32)
  memset(shadow, 32, sizeof(shadow));

  buffer_index = 0;

  // Setup interrupt handling
  attachInterrupt(digitalPinToInterrupt(disp_en), disp_en_handler, RISING); // inverted source signal so both display and the display register should react to rising edge of disp_en signal

  // Setup interupt handling
  // Source: https://www.electrosoftcloud.com/en/pcint-interrupts-on-arduino/
  //PCICR |= B00000001; // We activate the interrupts of the PB port
  //PCMSK0 |= B00100000;  // Activate interrupt on display enable (D13) pin (PCINT3)

  // Debug
  //Serial.begin(57600); // open the serial port at 57600 bps:

  /*queue[0] = {'A', 0};
  queue[1] = {'B', 0};
  queue[2] = {'C', 0};
  queue[3] = {'D', 0};
  queue[4] = {'E', 0};
  queue[5] = {'F', 0};
  queue[6] = {'G', 0};
  queue[7] = {'H', 0};
  queue[8] = {'I', 0};
  queue[9] = {'J', 0};
  queue[10] = {'K', 0};
  queue[11] = {'L', 0};
  queue_pos = 12;*/

}


byte input_type;
Character curr_char;
int num_interrupts = 0;
int num_low_interrupts = 0;

void disp_en_handler()
{
  // Read input and add to queue
  input_type = (PINB & 0x04) >> 2;
  input_char = (PINC & 0x3f) | ((PINB & 0x03) << 6);
  queue[queue_pos++] = {input_char, input_type};
  num_low_interrupts++;
}

/*ISR (PCINT0_vect)
{
  //Serial.println("Interrupt!");
  //if (!digitalRead(disp_en))
  if (!(PINB & 0x20))
  {
    //Serial.println("Interrupt handler (disp_en is low)");
    // Read input and add to queue
    input_char = (PINC & 0x3f) | ((PINB & 0x03) << 6);
    //input_type = digitalRead(ascii_or_int);
    input_type = (PINB & 0x04) >> 2; 
    //if (input_type)
    //  input_type = 'I';
    //else
    //  input_type = 'A';

    queue[queue_pos++] = {input_char, input_type};
    num_low_interrupts++;
  }
  num_interrupts++;
}*/

void loop() {
  if (queue_pos != queue_base)
  {
    
    // Queue not empty - print first character
    curr_char = queue[queue_base];
    //Serial.print("Printing character ");
    //Serial.print(curr_char.input_char);
    //Serial.print(" type:");
    //Serial.println(curr_char.type, HEX);

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

    // Print shadow
    //Serial.println("Shadow:");
    //int i, j;
    //for (i = 0; i < ROWS; i++)
    //{
    //  for (j = 0; j < COLS; j++)
    //    Serial.print((char)shadow[i][j]);
    //  Serial.println();
    //}

    //if (curr_char.input_char == 0x2E)
    //{
    //  lcd.print("\nlow_ints :");
    //  lcd.print(num_low_interrupts);
    //}

    queue_base++;
  }
}

/*
void loop() {
  // From bus
  disp_en_val = digitalRead(disp_en);
  if (was_high && disp_en_val == LOW)
  {
    valid_input = true;
    was_high = false;
  }
  else if (disp_en_val == HIGH)
  {
    was_high = true;
  }
  //valid_input = true;
  //int i;
  //input_char = 0;
  //for (i = 0; i < sizeof(bus); i++)
  //{
  //  input_char = input_char | (digitalRead(bus[i]) << i); 
  //}
  input_char = (PINC & 0x3f) | ((PINB & 0x03) << 6);

  //input_char = PINB & 0x03;
  //input_char = input_char << 6;
  //input_char = input_char | (PINC & 0x3f);

  // Display
  if (valid_input)
  {
    
    //Serial.print("Input: ");
    //Serial.println(input_char, HEX);
    //lcd.write(input_char);
    if (!digitalRead(ascii_or_int)) // Print as ASCII
    {
      if (convert_input(&input_char) == PRINT_CHAR)
      {
        //Serial.print("printing ");
        //Serial.print((char)input_char);
        //Serial.println();
        disp_write(input_char);
      }
      else
      {
        //Serial.println("printing escape character");
      }
    }
    else  // Print as two 4-byte characters (print in hex)
    {
      byte high_digit = input_char >> 4;
      byte low_digit = input_char & 0x0f;
      convert_input(&high_digit);
      convert_input(&low_digit);
      disp_write(high_digit);
      disp_write(low_digit);
    }
    valid_input = false;
  }
}*/
