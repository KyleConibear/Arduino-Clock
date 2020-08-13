#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DS3231M.h> // https://github.com/SV-Zanshin/DS3231M/wiki

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

// Clock RTC - DS3231M
DS3231M_Class DS3231M; // Create an instance of the DS3231M
bool hasClock = false;

DateTime clock;

// Menu Navigation
int currentMenu = 0;
int menus[6] =
{
  0, // DisplayTime
  1, // SetClockYear,
  2, // SetClockMonth,
  3, // SetClockDay,
  4, // SetClockHour,
  5, // SetClockMinute
};

// Buttons
#define backButton 3
#define downButton 4
#define upButton 5
#define selectButton 6

bool backButtonState = false;
bool downButtonState = false;
bool upButtonState = false;
bool selectButtonState = false;


void ClockSetup()
{
  while (!DS3231M.begin()) { // Initialize RTC communications
    Serial.println("Unable to find Real Time Clock(RTC) DS3231M. Checking again in 3 seconds.");
    delay(3000);
  } // loop until device is located

  hasClock = true;
  Serial.println("Real Time Clock(RTC) Module DS3231M found.");
}

void DisplayValue(int value)
{
  if (value < 10)
  {
    lcd.print("0");
  }
  lcd.print(value);
}

void DisplayHour(int value)
{
  lcd.setCursor(6, 0);
  if (value < 13)
  {
    lcd.print("AM");
  }
  else
  {
    lcd.print("PM");
  }

  lcd.setCursor(0, 0);

  if (value > 12)
  {
    value = value - 12;
  }

  if (value < 10)
  {
    lcd.setCursor(1, 0);
  }

  lcd.print(value);
}

void ClockPrint()
{
  clock = DS3231M.now();

  DisplayHour(clock.hour()); Serial.print("Hour:   "); Serial.println(clock.hour());

  lcd.setCursor(2, 0);
  lcd.print(":");
  DisplayValue(clock.minute()); Serial.print("Minute: "); Serial.println(clock.minute());

  lcd.setCursor(0, 1);
  DisplayValue(clock.day()); Serial.print("Day:    "); Serial.println(clock.day());

  lcd.setCursor(2, 1);
  lcd.print("/");

  DisplayValue(clock.month()); Serial.print("Month:  "); Serial.println(clock.month());

  lcd.setCursor(5, 1);
  lcd.print("/");

  DisplayValue(clock.year()); Serial.print("Year:   "); Serial.println(clock.year());

  delay(200);
}

void SetClock(int amount)
{
  Serial.print("Menu "); Serial.println(currentMenu);
  switch (currentMenu)
  {
    // Hour
    case 1:
      //lcd.setCursor(3, 0);
      DS3231M.adjust(DateTime(clock.year(),
                              clock.month(),
                              clock.day(),
                              clock.hour() + amount,
                              clock.minute(),
                              clock.second()));
      break;
    // Minute
    case 2:
      //lcd.setCursor(1, 0);
      DS3231M.adjust(DateTime(clock.year(),
                              clock.month(),
                              clock.day(),
                              clock.hour(),
                              clock.minute() + amount,
                              clock.second()));
      break;
    // Day
    case 3:
      //lcd.setCursor(1, 1);
      DS3231M.adjust(DateTime(clock.year(),
                              clock.month(),
                              clock.day() + amount,
                              clock.hour(),
                              clock.minute(),
                              clock.second()));
      break;

      break;
    // Month
    case 4:
      //lcd.setCursor(3, 1);
      DS3231M.adjust(DateTime(clock.year(),
                              clock.month() + amount,
                              clock.day(),
                              clock.hour(),
                              clock.minute(),
                              clock.second()));
      break;
    // Year
    case 5:
      //lcd.setCursor(6, 1);
      DS3231M.adjust(DateTime(clock.year() + amount,
                              clock.month(),
                              clock.day(),
                              clock.hour(),
                              clock.minute(),
                              clock.second()));
      break;

      // lcd.blink();
  }
}

void ButtonsSetup()
{
  pinMode(backButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(upButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
}

void DetectButtonPress()
{
  if (digitalRead(backButton) == LOW)
  {
    backButtonState = true;
  }
  else if (backButtonState == true && digitalRead(backButton) == HIGH)
  {
    backButtonState = false;
    Serial.println("Back Button released.");
    GoToPreviousMenu();
  }

  else if (digitalRead(downButton) == LOW)
  {
    downButtonState = true;
  }
  else if (downButtonState == true && digitalRead(downButton) == HIGH)
  {
    downButtonState = false;
    Serial.println("Down Button released.");
    SetClock(-1);
  }

  else if (digitalRead(upButton) == LOW)
  {
    upButtonState = true;
  }
  else if (upButtonState == true && digitalRead(upButton) == HIGH)
  {
    upButtonState = false;
    Serial.println("Up Button released.");
    SetClock(1);
  }

  else if (digitalRead(selectButton) == LOW)
  {
    selectButtonState = true;
  }
  else if (selectButtonState == true && digitalRead(selectButton) == HIGH)
  {
    selectButtonState = false;
    Serial.println("Select Button released.");
    GoToNextMenu();
  }
}

bool hasDisplay = false;

// Display
void DisplaySetup()
{
  lcd.init();                      // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
}

// Menu Navigation
void GoToPreviousMenu()
{
  currentMenu--;
  if (currentMenu <= 0)
  {
    currentMenu = 0;
    lcd.noCursor();
  }
  else
  {
    lcd.cursor();
  }
  Serial.print("currentMenu changed to "); Serial.println(currentMenu);
}

void GoToNextMenu()
{
  currentMenu++;
  if (currentMenu >= sizeof menus / sizeof menus[0])
  {
    currentMenu = 0;
    lcd.noCursor();
  }
  else
  {
    lcd.cursor();
  }

  Serial.print("currentMenu changed to "); Serial.println(currentMenu);
}

// Arduino Methods
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  ClockSetup();
  DisplaySetup();
  ButtonsSetup();
}

void loop()
{
  DetectButtonPress();

  delay(200);

  ClockPrint();
}
