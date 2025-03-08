#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
 
#define CLK_PIN 13
#define DATA_PIN 11
#define CS_PIN 10
#define MAX_DEVICES 4

#define VERT_PIN A1
#define HORZ_PIN A0
#define SEL_PIN 7

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

int vert_set=-1;
int horz_set=0;
unsigned long long int time=0;
int* x;
int* y;
int* poz_apple;
int pozx=0;
int pozy=0;
int check=1;
int score=0;
int highscore=0;
int snake_length=3;
int auxx, auxy;
int menu=0;

LiquidCrystal_I2C lcd(0x27, 20, 4);

MD_MAX72XX snake = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_Parola display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

int* Poz_apple(int* x, int* y, int snake_length)
{
  int* poz_apple;
  poz_apple=(int*)calloc(2, sizeof(int));
  poz_apple[0]=random(0, 31);
  poz_apple[1]=random(0, 7);
  for (int i=0; i<snake_length; i++)
  {
    if (x[i]==poz_apple[0] && y[i]==poz_apple[1])
    {
      free(poz_apple);
      poz_apple=Poz_apple(x, y, snake_length);
      break;
    }
  }
  return poz_apple;
}

void GameBegin()
{
  lcd.print("3");
  delay(100);
  lcd.setCursor(1, 0);
  lcd.print(".");
  delay(300);
  lcd.setCursor(2, 0);
  lcd.print(".");
  delay(300);
  lcd.setCursor(3, 0);
  lcd.print(".");
  delay(300);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("2");
  delay(100);
  lcd.setCursor(1, 0);
  lcd.print(".");
  delay(300);
  lcd.setCursor(2, 0);
  lcd.print(".");
  delay(300);
  lcd.setCursor(3, 0);
  lcd.print(".");
  delay(300);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1");
  delay(100);
  lcd.setCursor(1, 0);
  lcd.print(".");
  delay(300);
  lcd.setCursor(2, 0);
  lcd.print(".");
  delay(300);
  lcd.setCursor(3, 0);
  lcd.print(".");
  delay(300);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("START!");
  delay(500);
}

void Pause()
{
  int sel=0;
  lcd.clear();
  lcd.setCursor(6, 1);
  lcd.print("Pause");
  while(digitalRead(SEL_PIN)==LOW)
  {
    delay(100);
  }
  while (sel==0)
  {
    if (digitalRead(SEL_PIN)==LOW)
    {
      sel=1;
      delay(200);
    }
  }
  lcd.setCursor(0,0);
  lcd.print("Score: ");
  lcd.print(score);
}

void setup() { 
  Serial.begin(115200);

  pinMode(SEL_PIN, INPUT_PULLUP); 

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  
  GameBegin();

  time=millis();

  display.begin();
  display.setIntensity(0);
  display.displayClear();

  x=(int*)calloc(256, sizeof(int));
  y=(int*)calloc(256, sizeof(int));

  x[0]=15;
  y[0]=4;
  x[1]=15;
  y[1]=5;
  x[2]=15;
  y[2]=6;

  snake.begin();
  snake.clear();

  poz_apple=Poz_apple(x, y, snake_length);
  snake.setPoint(poz_apple[1], poz_apple[0], true);
  snake.update();

  pinMode(VERT_PIN, INPUT);
  pinMode(HORZ_PIN, INPUT);
}

void loop() {
  int vert=analogRead(VERT_PIN);
  int horz=analogRead(HORZ_PIN);

  if (digitalRead(SEL_PIN)==LOW)
    Pause();
  
  lcd.setCursor(0,0);
  lcd.print("Score: ");
  lcd.print(score);
  if (score>highscore)
  {
    lcd.setCursor(0,1);
    lcd.print("New Highscore: ");
    lcd.print(score);
  }
  else
  {
    lcd.setCursor(0,1);
    lcd.print("Highscore: ");
    lcd.print(highscore);
  }

  if (horz>700 && horz_set!=-1 && check==1)
  {
    vert_set=0;
    horz_set=1;
    check=0;
  }
  else if (horz<312 && horz_set!=1 && check==1)
  {
    vert_set=0;
    horz_set=-1;
    check=0;
  }
  else if (vert>700 && vert_set!=1 && check==1)
  {
    vert_set=-1;
    horz_set=0;
    check=0;
  }
  else if (vert<312 && vert_set!=-1 && check==1)
  {
    vert_set=1;
    horz_set=0;
    check=0;
  }
  if ((millis()-time)>200)
  {
    time=millis();
    pozx=x[snake_length-1];
    pozy=y[snake_length-1];

    auxx=x[0];
    auxy=y[0];

    if (vert_set==-1)
      y[0]=y[0]-1;
    else if (vert_set==1)
      y[0]=y[0]+1;
    if (horz_set==1)
      x[0]=x[0]+1;
    else if (horz_set==-1)
      x[0]=x[0]-1;
    if (x[0]>31)
      x[0]=0;
    else if (x[0]<0)
      x[0]=31;
    if (y[0]>7)
      y[0]=0;
    else if (y[0]<0)
      y[0]=7;
    
    for (int i=snake_length-1; i>1; i--)
    {
        x[i]=x[i-1];
        y[i]=y[i-1];
    }

    x[1]=auxx;
    y[1]=auxy;

    check=1;
  }

  if (snake_length==3)
  {
    snake.setPoint(y[0], x[0], true);
    snake.setPoint(y[1], x[1], true);
    snake.setPoint(y[2], x[2], true);
  }
  else
    snake.setPoint(y[0], x[0], true);
  if (x[0]==poz_apple[0] && y[0]==poz_apple[1])
  {
    free(poz_apple);
    poz_apple=Poz_apple(x, y, snake_length);
    snake_length++;
    score++;
  }
  snake.setPoint(poz_apple[1], poz_apple[0], true);
  if (pozx!=x[0] || pozy!=y[0])
    snake.setPoint(pozy, pozx, false);
  snake.update();

  for (int i=4; i<snake_length; i++)
  {
    if (x[0]==x[i] && y[0]==y[i])
    {
      snake.clear();
      free(poz_apple);
      free(x);
      free(y);
      x=(int*)calloc(256, sizeof(int));
      y=(int*)calloc(256, sizeof(int));
      x[0]=15;
      y[0]=4;
      x[1]=15;
      y[1]=5;
      x[2]=15;
      y[2]=6;
      vert_set=-1;
      horz_set=0;
      check=1;
      snake_length=3;
      poz_apple=Poz_apple(x, y, snake_length);  
      display.setTextAlignment(PA_CENTER);
      display.print("DEAD");
      lcd.clear();
      lcd.setCursor(5, 1);
      lcd.print("GAME OVER");
      delay(4000);
      if (score>highscore)
      {
        lcd.clear();
        lcd.setCursor(1, 1);
        lcd.print("New Highscore: ");
        lcd.print(score);
        highscore=score;
      }
      else
      {
        lcd.clear();
        lcd.setCursor(4, 1);
        lcd.print("Score: ");
        lcd.print(score);
      }
      score=0;
      delay(4000);
      time=millis();
      lcd.clear();
      display.displayClear();
      GameBegin();
      break;
    }
  }
}
