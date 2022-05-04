#include <Wire.h>              // библиотека для протокола IIC
#include <LiquidCrystal_I2C.h> // подключаем библиотеку LCD IIC
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DS1307RTC.h>
#include <EEPROM.h>
#include <Time.h>
#include <avr/sleep.h>
#include <avr/power.h>

tmElements_t datetime;

#define BME_CS 10
#define SECS_PER_MIN (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY (SECS_PER_HOUR * 24L)
#define SEALEVELPRESSURE_HPA (1013.25)
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) ((_time_ % SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) (_time_ / SECS_PER_DAY)
#define showNoShow(_time_) (_time_ % 2 == 0)
#define battery_find(_val_) ((_val_ * 5. / 1023. - 3) / 1.2 * 100)
#define SLEEP_CONST  100
volatile int count_sleep = SLEEP_CONST;

int a_type_of_show = 0; // адрес eeprom
int a_light_level = 1;
int a_position = 2;
int timer = -1;
int timer_stop = 0;
int delta_timer = 0;
int show_timer = -1;
bool show = true;
bool show_menu_status = false;
bool shift_cursor = false;
bool select = false;
bool switch_screen = false;
bool light = true;
int type_of_show = EEPROM.read(a_type_of_show);
float battary_voltage = battery_find(analogRead(A0));
bool higth_freq = true;
int hight_freq_set_count = 0;
byte LT[8] =
    {
        B00111,
        B01111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111};
byte UB[8] =
    {
        B11111,
        B11111,
        B11111,
        B00000,
        B00000,
        B00000,
        B00000,
        B00000};
byte RT[8] =
    {
        B11100,
        B11110,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111};
byte LL[8] =
    {
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B01111,
        B00111};
byte LB[8] =
    {
        B00000,
        B00000,
        B00000,
        B00000,
        B00000,
        B11111,
        B11111,
        B11111};
byte LR[8] =
    {
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11110,
        B11100};
byte MB[8] =
    {
        B11111,
        B11111,
        B11111,
        B00000,
        B00000,
        B00000,
        B11111,
        B11111};
byte block[8] =
    {
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111};

byte left_dot[8] =
    {
        B00000,
        B00000,
        B10000,
        B10000,
        B10000,
        B10000,
        B00000,
        B00000};

byte right_dot[8] =
    {
        B00000,
        B00000,
        B00001,
        B00001,
        B00001,
        B00001,
        B00000,
        B00000};
LiquidCrystal_I2C lcd(0x27, 20, 2); // присваиваем имя lcd для дисплея
Adafruit_BME280 bme(BME_CS);        // hardware SPI

byte gradus[8] = {
    0b01100, 0b10010, 0b10010, 0b01100, 0b00000, 0b00000, 0b00000, 0b00000};

void clean_display()
{
  lcd.clear();
}

int seconds()
{
  if (higth_freq)
  {
    return numberOfSeconds(long(millis() / 1000.));
  }
  else
    return numberOfSeconds(long(millis() / 1000.)) * 4;
}

class Menu
{
public:
  Menu()
  {
    menu[0] = "Time setup";
    menu[1] = "Date setup";
    menu[2] = "LCD light";
    menu[3] = "Screen type";
    menu[4] = "Exit";
    menu[5] = "";
    cursor_position = 0;
  }

  bool show_menu(bool cursor_position_shift = false, bool select = false)
  {
    if (cursor_position_shift || select)
      clean_display();
    switch (level_menu)
    {
    case 0:
      main_menu(cursor_position_shift, select);
      return true;
    case 1:
      time_setup(cursor_position_shift, select);
      return true;
    case 2:
      date_setup(cursor_position_shift, select);
      return true;
    case 4:
      type_of_screen(cursor_position_shift, select);
      return true;
    case 10:
      time_setting(cursor_position_shift, select);
      return true;
    case 20:
      date_setting(cursor_position_shift, select);
      return true;
    default:
      break;
    }

    // if ((cursor_position == 4) && (select && (level_menu == 0)))
    // {
    //   cursor_position = 0;
    //   return false;
    // }
    if (select && (level_menu != 10))
    {
      cursor_position = 0;
    }
    return true;
  }

private:
  String menu[6];
  int cursor_position;
  int level_menu = 0;
  int8_t hour = 0, minute = 0, second = 0;
  int8_t day = 30, month = 4;
  int year = 2022;

  void date_setup(bool cursor_position_shift, bool select)
  {
    cursor_position += int(cursor_position_shift);
    if (cursor_position >= 2)
      cursor_position = 0;
    lcd.setCursor(0, 0);
    lcd.print("dd:mm:yy");
    if (cursor_position == 0)
      lcd.print("<-");
    lcd.setCursor(0, 1);
    lcd.print("Exit");
    if (cursor_position == 1)
      lcd.print("<-");

    if (select && (cursor_position == 1))
    {
      level_menu = 0;
      cursor_position = 0;
      main_menu(false, false);
    }
    if ((cursor_position == 0) && select)
    {

      level_menu = 20;
      lcd.cursor_on();
      lcd.blink_on();
      lcd.clear();
    }
  }
  void time_setup(bool cursor_position_shift = false, bool select = false)
  {
    cursor_position += int(cursor_position_shift);
    if (cursor_position >= 2)
      cursor_position = 0;
    lcd.setCursor(0, 0);
    lcd.print("00:00:00");
    if (cursor_position == 0)
      lcd.print("<-");
    lcd.setCursor(0, 1);
    lcd.print("Exit");
    if (cursor_position == 1)
      lcd.print("<-");

    if (select && (cursor_position == 1))
    {
      level_menu = 0;
      cursor_position = 0;
      main_menu(false, false);
    }
    if ((cursor_position == 0) && select)
    {

      level_menu = 10;
      lcd.cursor_on();
      lcd.blink_on();
      lcd.clear();
    }
  }

  void date_setting(bool cursor_position_shift, bool select)
  {
    if (cursor_position_shift)
    {
      switch (cursor_position)
      {
      case 0:
        this->day += 10;
        break;
      case 1:
        this->day++;
        break;
      case 3:
        this->month += 10;
        break;
      case 4:
        this->month++;
        break;
      case 6:
        this->year += 1000;
        break;
      case 7:
        this->year += 100;
        break;
      case 8:
        this->year += 10;
        break;
      case 9:
        this->year++;
        break;
      default:
        break;
      }
    }
    if (this->day > 31)
    {
      this->day = 1;
    }
    if (this->month > 12)
    {
      this->month = 1;
    }
    if (this->year > 9999)
    {
      this->year = 2022;
    }
    cursor_position += int(select);
    if (cursor_position >= 10)
    {

      RTC.read(datetime);

      datetime.Day = this->day;
      datetime.Month = this->month;
      datetime.Year = this->year - 1970;
      RTC.write(datetime);
      level_menu = 0;
      cursor_position = 0;
      lcd.cursor_off();
      lcd.blink_off();
    }
    if ((cursor_position == 2) || (cursor_position == 5))
    {
      cursor_position++;
    }
    lcd.setCursor(0, 0);
    if (this->day < 10)
    {
      lcd.print(0);
    }
    lcd.print(this->day);
    lcd.print('.');
    if (this->month < 10)
    {
      lcd.print(0);
    }
    lcd.print(this->month);
    lcd.print('.');
    lcd.print(this->year);
    lcd.setCursor(cursor_position, 0);
  }

  void time_setting(bool cursor_position_shift, bool select)
  {

    if (cursor_position_shift)
    {
      switch (cursor_position)
      {
      case 0:
        this->hour += 10;
        break;
      case 1:
        this->hour++;
        break;
      case 3:
        this->minute += 10;
        break;
      case 4:
        this->minute++;
        break;
      case 6:
        this->second += 10;
        break;
      case 7:
        this->second++;
        break;
      default:
        break;
      }
    }
    if (this->hour >= 23)
    {
      this->hour = 0;
    }
    if (this->minute >= 59)
    {
      this->minute = 0;
    }
    if (this->second >= 59)
    {
      this->second = 0;
    }
    cursor_position += int(select);
    if (cursor_position >= 8)
    {

      RTC.read(datetime);
      datetime.Hour = this->hour;
      datetime.Minute = this->minute;
      datetime.Second = this->second;
      RTC.write(datetime);
      level_menu = 0;
      cursor_position = 0;
      lcd.cursor_off();
      lcd.blink_off();
    }
    if ((cursor_position == 2) || (cursor_position == 5))
    {
      cursor_position++;
    }
    lcd.setCursor(0, 0);
    if (this->hour < 10)
    {
      lcd.print(0);
    }
    lcd.print(this->hour);
    lcd.print(':');
    if (this->minute < 10)
    {
      lcd.print(0);
    }
    lcd.print(this->minute);
    lcd.print(':');

    if (this->second < 10)
    {
      lcd.print(0);
    }
    lcd.print(this->second);
    lcd.setCursor(cursor_position, 0);
  }
  void main_menu(bool cursor_position_shift, bool select)
  {
    cursor_position += int(cursor_position_shift);
    if (cursor_position >= 4)
      cursor_position = 0;
    for (auto i = 0; i < 2; ++i)
    {
      lcd.setCursor(0, i);

      if (i + cursor_position != 4)
      {
        lcd.print(menu[i + cursor_position]);
      }
      else
      {
        lcd.print("Bat ");
        lcd.print(battary_voltage, 0);
        lcd.print(";V: ");
        lcd.print(5. * analogRead(A0) / 1023, 2);
      }
      if (i == 0)
        lcd.print("<-");
    }
    if (select)
    {
      level_menu = cursor_position + 1;
      clean_display();
      cursor_position = 0;
    }
  }

  void type_of_screen(bool cursor_position_shift, bool select)
  {
    cursor_position += int(cursor_position_shift);
    if (cursor_position >= 4)
    {
      cursor_position = 0;
    }
    for (int i = 0; i < 2; ++i)
    {
      lcd.setCursor(0, i);
      if (i + cursor_position + 1 < 5)
        lcd.print(i + cursor_position + 1);
      if (i == 0)
      {
        lcd.print("<-");
      }
    }
    if (select)
    {

      EEPROM.write(a_type_of_show, cursor_position);
      type_of_show = cursor_position;
      level_menu = 0;
    }
  }
};

Menu menu;

void display_time(int row = 0, int col = 0, bool seconds_show = true)
{
  RTC.read(datetime);
  int hours = datetime.Hour;
  int minutes = datetime.Minute;
  int secondss = datetime.Second;
  lcd.setCursor(col, row);

  if (hours < 10)
    lcd.print(0);
  lcd.print(hours);
  lcd.print(":");
  if (minutes < 10)
    lcd.print(0);
  lcd.print(minutes);
  if (seconds_show)
  {
    lcd.print(":");
    if (secondss < 10)
      lcd.print(0);
    lcd.print(secondss);
  }
}

void display_date(int row = 1, int col = 0)
{
  RTC.read(datetime);
  int day = datetime.Day;
  int year = datetime.Year;
  int month = datetime.Month;

  lcd.setCursor(col, row);
  if (day < 10)
    lcd.print(0);

  lcd.print(day);
  lcd.print('.');
  if (month < 10)
    lcd.print(0);
  lcd.print(month);
  lcd.print('.');
  lcd.print(1970 + year);
}

void temperature(int row)
{
  float t = bme.readTemperature();
  lcd.setCursor(0, row); // ставим курсор на 1 символ первой строки
  lcd.print("t");        // используем латинские буквы
  lcd.print(": ");
  lcd.print(t, 1); // выводим значение температуры на LCD
}

void humidity(int row)

{
  float h = bme.readHumidity();
  lcd.setCursor(0, row); // ставим курсор на 3 символ второй строки
  lcd.print("h");        // используем латинские буквы
  lcd.print(": ");       // используем латинские буквы
  lcd.print(h, 0);       // выводим значение влажности на LCD
  lcd.print("%");        // выводим знак процент
}

void show_time()
{
  display_time(0, 4, true);
  display_date(1, 3);
}

void custom0(int x)
{
  lcd.setCursor(x, 0);
  lcd.write((byte)0);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
}
void custom1(int x)
{
  lcd.setCursor(x, 0);
  lcd.write(1);
  lcd.write(2);
  lcd.print(" ");
  lcd.setCursor(x, 1);
  lcd.write(4);
  lcd.write(7);
  lcd.write(4);
}
void custom2(int x)
{
  lcd.setCursor(x, 0);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(4);
}
void custom3(int x)
{
  lcd.setCursor(x, 0);
  lcd.write(6);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(4);
  lcd.write(4);
  lcd.write(5);
}
void custom4(int x)
{
  lcd.setCursor(x, 0);
  lcd.write(3);
  lcd.write(4);
  lcd.write(7);
  lcd.setCursor(x, 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.write(7);
}
void custom5(int x)
{
  lcd.setCursor(x, 0);
  lcd.write(3);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(4);
  lcd.write(4);
  lcd.write(5);
}
void custom6(int x)
{
  lcd.setCursor(x, 0);
  lcd.write((byte)0);
  lcd.write(6);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
}
void custom7(int x)
{
  lcd.setCursor(x, 0);
  lcd.write(1);
  lcd.write(1);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.write(7);
}
void custom8(int x)
{
  lcd.setCursor(x, 0);
  lcd.write((byte)0);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(4);
  lcd.write(5);
}
void custom9(int x)
{
  lcd.setCursor(x, 0);
  lcd.write((byte)0);
  lcd.write(6);
  lcd.write(2);
  lcd.setCursor(x, 1);
  lcd.print(" ");
  lcd.print(" ");
  lcd.write(7);
}
void printDigits(int digits, int x)
{
  // utility function for digital clock display: prints preceding colon and leading 0
  switch (digits)
  {
  case 0:
    custom0(x);
    break;
  case 1:
    custom1(x);
    break;
  case 2:
    custom2(x);
    break;
  case 3:
    custom3(x);
    break;
  case 4:
    custom4(x);
    break;
  case 5:
    custom5(x);
    break;
  case 6:
    custom6(x);
    break;
  case 7:
    custom7(x);
    break;
  case 8:
    custom8(x);
    break;
  case 9:
    custom9(x);
    break;
  }
}
void printNumber(int val, int col = 9)
{
  ;
  printDigits(val / 10, col);
  printDigits(val % 10, col + 4);
}

void low_freq_set()
{
  if (higth_freq)
  {
    CLKPR = 1 << CLKPCE;
    CLKPR = 4;
  }
  higth_freq = false;
}

void hight_freq_set()
{
  if (!higth_freq)
  {
    CLKPR = 1 << CLKPCE;
    CLKPR = 0;
  }
  higth_freq = true;
}

void isr()
{
  count_sleep = SLEEP_CONST; // Инициализировать счётчик
}
void setup()
{
  // clock_prescale_set(clock_div_16);
  digitalWrite(13, LOW); // (3) Светодиод гаснет при нажатии кнопки.

  //pinMode(6, OUTPUT);
  pinMode(2, INPUT); // объявляем пин 2 как вход
  pinMode(3, INPUT_PULLUP); // объявляем пин 2 как вход
  lcd.init();        // инициализация LCD дисплея
  lcd.backlight();   // включение подсветки дисплея
  lcd.createChar(0, LT);
  lcd.createChar(1, UB);
  lcd.createChar(2, RT);
  lcd.createChar(3, LL);
  lcd.createChar(4, LB);
  lcd.createChar(5, LR);
  lcd.createChar(6, MB);
  lcd.createChar(7, block);

  // timer = seconds();
  bme.begin();
  pinMode(A0, INPUT);

  show_menu_status = false;
  attachInterrupt(1, isr, FALLING);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  low_freq_set();
  //sleep_mode();
  // EEPROM.write(address, 0);
}

// Обновление дисплея:
// 1. Очистка экрана
// 2. Новая информация
bool blacklight_status = true;
void loop()
{
  
  analogWrite(6, 150);

  if (digitalRead(2) == HIGH)
  {
    hight_freq_set_count = 10;
    hight_freq_set();
    if ((!shift_cursor))
      shift_cursor = true;

    if (timer == -1)
    {
      timer = seconds();
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("PUSH TIME:");
    }
    lcd.setCursor(6, 1);
    lcd.print(abs(timer_stop - timer));
    lcd.print("sec");
  }
  else if ((digitalRead(2) == LOW) && (timer != -1))
  {
    lcd.clear();

    delta_timer = abs(timer - timer_stop);
    if (delta_timer >= 3)
    {
      show_menu_status = !show_menu_status;
      select = false;
      shift_cursor = false;
      show = true;
      show_timer = seconds();
      timer_stop = seconds();
    }

    else if (show_menu_status && (delta_timer >= 2))
    {
      select = true;
      shift_cursor = false;
    }
    timer = -1;
    show_timer = 61;
  }
  else if (show)
  {
    show_timer = seconds();

    if (show_menu_status)
    {
      menu.show_menu(shift_cursor, select);
      if (!show_menu_status)
      {
        select = false;
        shift_cursor = false;
        show = true;
        show_timer = seconds();
        timer_stop = seconds();
        timer = -1;
        show_timer = 61;
        lcd.clear();
      }
      if (shift_cursor)
        shift_cursor = false;
      if (select)
        select = false;
    }
    else if ((type_of_show == 0 && show))
    {
      show_time();
    }
    else if ((type_of_show == 1) && show)
    {
      if (!switch_screen)
      {
        RTC.read(datetime);
        lcd.setCursor(7, 0);
        lcd.print(bme.readTemperature(), 0);
        lcd.setCursor(7, 1);
        lcd.print(bme.readHumidity(), 0);
        printNumber(datetime.Hour, 0);
        printNumber(datetime.Minute, 9);
      }
      else
      {
        lcd.setCursor(0, 0);
        lcd.print("Date");
        display_date(1, 0);
      }
      if (shift_cursor)
      {
        switch_screen = !switch_screen;
        shift_cursor = false;
        clean_display();
      }
    }
    else if ((type_of_show == 2) && show)
    {
      temperature(0);
      humidity(1);
      display_time(0, 9, false);
      lcd.setCursor(9, 1);
      if (datetime.Day < 10)
        lcd.print(0);
      lcd.print(datetime.Day);
      lcd.print('.');
      lcd.print(datetime.Month);
    }
    else if ((type_of_show == 3 && show))
    {
      if (timer_stop % 5 == 0)
      {
        switch_screen = !switch_screen;
        clean_display();
      }
      if (switch_screen)
      {
        show_time();
      }
      else
      {
        temperature(0);
        humidity(1);
      }
    }
    show_timer = seconds();
  }
  else
  {
  }
  timer_stop = seconds();
  // if (higth_freq)
  show = abs(show_timer - timer_stop) >= 0;

  // else
  // show = timer_stop % 2==0; // abs(show_timer - timer_stop) >= 0;
  if (timer_stop % 1 == 0)
  {
    battary_voltage = battery_find(analogRead(A0));
    if (battary_voltage >= 90)
      battary_voltage = 100;
    else if (battary_voltage >= 75)
      battary_voltage = 75;
    else if (battary_voltage >= 50)
      battary_voltage = 50;
    else if (battary_voltage <= 25)
      battary_voltage = 25;
  }
  if (hight_freq_set_count == 0)
    low_freq_set();
  else
    hight_freq_set_count--;
  if (count_sleep == 0)
  {
    clean_display();
    lcd.setCursor(0, 0);
    lcd.print("I will be");
    lcd.setCursor(0, 1);
    lcd.print("SLEEP");
    delay(100);
    clean_display();
    lcd.noBacklight();
    lcd.noDisplay();
    blacklight_status = false;
    sleep_mode();                                   // Переход в режим сна
  }
  else if (!show_menu_status && !higth_freq)
  {

    --count_sleep; // уменьшить счётчик на 1.
  }
  if (!blacklight_status){
    lcd.backlight();
    blacklight_status = true;
    lcd.display();
    }
  
  lcd.setCursor(0, 0);
  lcd.print(count_sleep);
}
