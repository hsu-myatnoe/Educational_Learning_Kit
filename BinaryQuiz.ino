#include <TFT_eSPI.h>
#include <Keypad.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include "lesson_data.h"
#include "images.h"

#define ENCODER_CLK 6
#define ENCODER_DT  7
#define ENCODER_SW  10

TFT_eSPI tft = TFT_eSPI();

HardwareSerial mySerial(2); 
DFRobotDFPlayerMini myDFPlayer;


bool dfplayerReady = false;
int mode = 0;
bool selected = false;
bool inQuiz = false;
bool waitingForAnswer = false;
bool awaitingRetryResponse = false;
bool showCorrectThenContinue = false;
int lastClk = HIGH;
int lastBtn = HIGH;
bool mainMenuShown = false;
bool bombStarting = false;
unsigned long bombStartDelayTime = 0;
String input = "";
int correctAnswer;
String questionText = "";

int bombQuestionIndex = 0;
int bombScore = 0;

int birdX = 320;
unsigned long lastBirdUpdate = 0;
const unsigned long birdDelay = 50; 
const int birdY = 60; 
const int birdSpeed = 2; 

int questionPool[32];
int questionIndex = 0;
int score = 0;

unsigned long lastBombScreenUpdate = 0;
const unsigned long bombUpdateInterval = 500; 





// Binary Bomb variables 
String bombAnswer = "";
String bombInput = "";
bool bombStarted = false;
unsigned long bombStartTime = 0;
const unsigned long bombTimeLimit = 10000; 


//keypad_pins
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {2, 38, 39, 41};
byte colPins[COLS] = {42, 1, 40};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String formatBinary(int num) {
  String bin = String(num, BIN);
  while (bin.length() < 4) bin = "0" + bin;
  return bin;
}



void setup() {
  Serial0.begin(115200);
  delay(700);  

  tft.init();
  tft.setRotation(3);
  tft.invertDisplay(false);

  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);

  delay(500);  

  mySerial.begin(9600, SERIAL_8N1, 14, 13); 
  dfplayerReady = myDFPlayer.begin(mySerial);
  if (dfplayerReady) {
    myDFPlayer.volume(20); //VOLUME
    Serial0.println("DFPlayer Mini ready");
  } 
  else {
    Serial0.println("DFPlayer failed. Continuing without sound.");
  }


  delay(500);  
  showMainMenu();
}

void loop() {
  int btnState = digitalRead(ENCODER_SW);


  if (!inQuiz && !selected) {
    int currentClk = digitalRead(ENCODER_CLK);
    if (currentClk != lastClk && currentClk == LOW) {
      mode = (digitalRead(ENCODER_DT) != currentClk) ? (mode + 1) % 4 : (mode + 3) % 4;
      drawMode();
    }
    lastClk = currentClk;

    if (btnState == LOW && lastBtn == HIGH) {
      if (dfplayerReady) {
        myDFPlayer.play(7); 
        delay(300);
      }
      selected = true;
      showSelectedScreen();
      delay(500); 
    }

    lastBtn = btnState;

   
    if (millis() - lastBirdUpdate > birdDelay) {
      lastBirdUpdate = millis();
      tft.pushImage(birdX, birdY, BIRD_WIDTH, BIRD_HEIGHT, myImage + birdY * 320 + birdX);
      birdX -= birdSpeed;
      if (birdX < -BIRD_WIDTH) {
        birdX = 320;
      }
      tft.pushImage(birdX, birdY, BIRD_WIDTH, BIRD_HEIGHT, bird);

    }

    return;
  }

  
  if (btnState == LOW && lastBtn == HIGH) {
    if (dfplayerReady) {       
      myDFPlayer.play(7);      
      delay(300);
                   
    }

    resetState();
    showMainMenu();
    return;
  }
  lastBtn = btnState;

  
  if (mode < 2 && inQuiz && waitingForAnswer) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') checkAnswer();
      else if (key == '*') {
        input = "";
        drawQuestion();
      } else {
        input += key;
        drawQuestion();
      }
    }

  } else if (awaitingRetryResponse) {
    char key = keypad.getKey();
    if (key == '1') {
      input = "";
      drawQuestion();
      waitingForAnswer = true;
      awaitingRetryResponse = false;
    } else if (key == '0') {
      showAnswerScreen();
      awaitingRetryResponse = false;
      showCorrectThenContinue = true;
    }
  } else if (showCorrectThenContinue) {
    char key = keypad.getKey();
    if (key) {
      showCorrectThenContinue = false;
      questionIndex++;
      loadNextQuestion();
    }
  } 
  else if (selected && !inQuiz && mode < 2) {
    startQuiz();
  }

  
  if (selected && mode == 2 && !bombStarted && !bombStarting) {
    bombStarting = true;
    bombStartDelayTime = millis();
  }
  else if (bombStarting && millis() - bombStartDelayTime > 1000) {
    bombStarting = false;
    bombStarted = true;
    bombStartTime = millis();   
    startBinaryBomb();          
  }

  else if (mode == 2 && selected && bombStarted) {
  
    if (millis() - bombStartTime > bombTimeLimit) {
      bombStarted = false;
      showBombResult(false); 
      delay(2000); 
      return;
    }


    //TIMER REFRESH
    if (millis() - lastBombScreenUpdate > bombUpdateInterval) {
      updateBombTimer(); 
      lastBombScreenUpdate = millis();
    }

   
    char key = keypad.getKey();
    if (key) {
      if (key == '*') {
        bombInput = "";
        drawBombScreen();
      } 
      else if (key == '#') {
        bool success = (bombInput == bombAnswer);
        bombStarted = false;
        showBombResult(success);
        if (success) bombScore++;
          bombQuestionIndex++;
          delay(500);

      } 
      else if (bombInput.length() < 4 && (key == '0' || key == '1')) {
        bombInput += key;
        drawBombScreen();
      }
    }
  }


  if (mode == 3 && selected) {
  learningMode(); 
  }

}

  
  
void resetState() {
  inQuiz = false;
  selected = false;
  waitingForAnswer = false;
  awaitingRetryResponse = false;
  showCorrectThenContinue = false;
  bombStarted = false;
  bombInput = "";
  bombAnswer = "";
  bombStartTime = 0;
  mainMenuShown = false;  
}

void showMainMenu() {
  

  tft.pushImage(0, 0, 320, 240, myImage);

  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(4);
  tft.setCursor(30, 30);
  tft.print("BINARY QUIZ");

  tft.setTextSize(3);
  tft.setCursor(10, 90);
  tft.print("Choose your mode:");

  drawMode();
  while (digitalRead(ENCODER_SW) == LOW) {
    delay(10);
  }
  lastBtn = HIGH;
}


void drawMode() {
  tft.setTextSize(2);
  for (int i = 0; i < totalMainMenuItems; i++) {
    int y = 130 + i * 30; 
    tft.setCursor(30, y);
    tft.setTextColor(mode == i ? TFT_BLUE : TFT_BLACK);
    tft.print(mainMenuTitles[i]);  
  }
}

//GetReadyScreen
void showSelectedScreen() {
  tft.pushImage(0, 0, 320, 240, getreadyImage);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 100);
  tft.print(mainMenuTitles[mode]);  
  tft.setTextSize(2);
  tft.setCursor(120, 140);
  tft.print("Get ready...");
}


void startQuiz() {
  inQuiz = true;
  input = "";
  questionIndex = 0;
  score = 0;
  waitingForAnswer = true;
  awaitingRetryResponse = false;
  showCorrectThenContinue = false;

  int totalQuestions = (mode == 0) ? numBinaryToDecimal : numDecimalToBinary;

  for (int i = 0; i < totalQuestions; i++) questionPool[i] = i;
  for (int i = 0; i < totalQuestions; i++) {
    int r = random(i, totalQuestions);
    int temp = questionPool[i];
    questionPool[i] = questionPool[r];
    questionPool[r] = temp;
  }

  loadNextQuestion();
}

void loadNextQuestion() {
  if (questionIndex >= 8) {
    tft.pushImage(0, 0, 320, 240, getreadyImage);
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(3);
    tft.setCursor(20, 80);
    tft.print("Quiz Complete!");
    tft.setCursor(20, 120);
    tft.print("Score: ");
    tft.print(score);
    tft.print(" / 8");
    delay(500);
    inQuiz = false;
    waitingForAnswer = false;
    return;
  }

  input = "";

  int index = questionPool[questionIndex];

  if (mode == 0) {
    questionText = binaryToDecimal[index].question;
    correctAnswer = binaryToDecimal[index].answer.toInt();
  } else {
    questionText = decimalToBinary[index].question;
    correctAnswer = strtol(decimalToBinary[index].answer.c_str(), nullptr, 2);
  }

  drawQuestion();
  waitingForAnswer = true;
  awaitingRetryResponse = false;
  showCorrectThenContinue = false;
}

void drawQuestion() {
  tft.pushImage(0, 0, 320, 240, confirmImage);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 60);
  tft.print(questionText);
  tft.setCursor(10, 120);
  tft.print("Your answer: ");
  tft.setTextColor(TFT_BLUE);
  tft.setTextSize(5);
  tft.setCursor(160,100);
  tft.print(input);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 150);
  tft.print("Press # to submit");
  tft.setCursor(10, 170);
  tft.print("Press * to clear");
  tft.setCursor(220, 100);
  tft.print("Score: ");
  tft.print(score);
}

void checkAnswer() {
  String correctStr = (mode == 0) ? String(correctAnswer) : formatBinary(correctAnswer);
  bool isCorrect = (input == correctStr);

  if (isCorrect) {
    if (dfplayerReady) myDFPlayer.play(2);  // CORRECT
    tft.pushImage(0, 0, 320, 240, correctImage, true);
    score++;
    showCorrectThenContinue = true;
    waitingForAnswer = false;
  } else {
    if (dfplayerReady) myDFPlayer.play(5);  // INCORRECT
    tft.pushImage(0, 0, 320, 240, incorrectImage);
    waitingForAnswer = false;
    awaitingRetryResponse = true;
  }
}





void showAnswerScreen() {
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(20, 45);
  tft.print("Correct Answer:");

  String ans = (mode == 0) ? String(correctAnswer) : formatBinary(correctAnswer);

  tft.setTextSize(4);
  tft.setTextColor(TFT_RED);
  tft.setCursor(200, 35);
  tft.print(ans);

  tft.setTextSize(2);
  tft.setTextColor(TFT_BLACK);

  if (mode == 0) { // Binary to Decimal
    String bin = formatBinary(correctAnswer);

    int bits[4] = {
      bin.charAt(0) - '0',
      bin.charAt(1) - '0',
      bin.charAt(2) - '0',
      bin.charAt(3) - '0'
    };

    int total = bits[0]*8 + bits[1]*4 + bits[2]*2 + bits[3]*1;

    tft.setCursor(20, 85);
    tft.printf("Binary %s is", bin.c_str());

    tft.setCursor(20, 125);
    tft.printf("%d*8 + %d*4 + %d*2 + %d*1", bits[0], bits[1], bits[2], bits[3]);

    tft.setCursor(20, 170);
    tft.printf("So the answer is: %d", total);
  }
  if (mode == 1) { 
    tft.setCursor(20, 85);
    tft.print("Let's break it down!");

    int bits[4] = {
      ans.charAt(0) - '0',
      ans.charAt(1) - '0',
      ans.charAt(2) - '0',
      ans.charAt(3) - '0'
    };

    int total = bits[0]*8 + bits[1]*4 + bits[2]*2 + bits[3]*1;

    tft.setCursor(20, 125);
    tft.printf("%d*8 + %d*4 + %d*2 + %d*1", bits[0], bits[1], bits[2], bits[3]);
   

    tft.setCursor(20, 170);
    tft.printf("Which gives us: %d!", total);
  }

  tft.setTextSize(2);
  tft.setCursor(130, 210);
  tft.print("Press any key...");
}


void startBinaryBomb() {
  bombQuestionIndex = 0;
  bombScore = 0;
  bombInput = "";
  bombStarted = true;
  loadNextBombQuestion();
}

void drawBombScreen() {
  drawGradientBackgroundPurple();
  tft.setTextColor(TFT_PURPLE);
  tft.setTextSize(3);
  tft.setCursor(70, 30);
  tft.print("Binary Bomb!");

  tft.setTextSize(2.5);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 80);
  tft.print("Decimal: ");
  tft.setTextSize(3);
  tft.setTextColor(TFT_BLUE);
  tft.print(String(strtol(bombAnswer.c_str(), nullptr, 2)));

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(10, 120);
  tft.print("Enter binary (4-bit):");

  tft.setTextSize(4);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(10, 160);
  tft.print(bombInput);

  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(10, 210);
  tft.print("Time Left: ");
  int timeLeft = (bombTimeLimit - (millis() - bombStartTime)) / 1000;
  tft.print(timeLeft);
  tft.print("s");
}

void loadNextBombQuestion() {
  
  if (bombQuestionIndex >= 8) {
    showBombFinalScore();
    delay(3000);
    showMainMenu();
    return;
  }

  int val = random(0, 16);
  bombAnswer = formatBinary(val);
  bombInput = "";
  bombStartTime = millis();
  drawBombScreen();
}


void showBombResult(bool success) {
  uint16_t bgColor = success ? tft.color565(180, 255, 200) : tft.color565(180, 220, 255);
  uint16_t borderColor = success ? TFT_GREEN : TFT_BLUE;

  tft.fillScreen(bgColor);
  drawDoubleBorder(borderColor);

  if (dfplayerReady) {
  if (success) {
    myDFPlayer.play(2);  // CORRECT
  } else {
    myDFPlayer.play(9);  // BOMB
  }
  }


  if (success) {
    drawShieldIcon(80, 70); 
    tft.setTextSize(3);
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(140, 100);
    tft.print("Bomb");
    tft.setCursor(140, 140);
    tft.print("Defused!");
  } else {
    drawHugeBomb(80, 120);
    tft.setTextSize(4);
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(170, 110);
    tft.print("BOOM!");
  }
}

void updateBombTimer() {
  
  int timeLeft = (bombTimeLimit - (millis() - bombStartTime)) / 1000;

  for (int y = 210; y < 230; y++) {
    uint8_t r = map(y, 0, 239, 220, 60);
    uint8_t g = map(y, 0, 239, 180, 0);
    uint8_t b = map(y, 0, 239, 255, 90);
    uint16_t color = tft.color565(r, g, b);
    tft.drawFastHLine(0, y, 320, color); 
  }


  tft.setTextSize(2);
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(10, 210);
  tft.print("Time Left: ");
  tft.print(timeLeft);
  tft.print("s");
}


void showBombFinalScore() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(3);
  tft.setCursor(30, 100);
  tft.print("Bomb Quiz Done!");
  tft.setCursor(30, 140);
  tft.print("Score: ");
  tft.print(bombScore);
  tft.print(" / 8");
}

void drawGradientBackgroundPurple() {
  for (int y = 0; y < 240; y++) {
    
    uint8_t r = map(y, 0, 239, 220, 60);
    uint8_t g = map(y, 0, 239, 180, 0);
    uint8_t b = map(y, 0, 239, 255, 90);
    uint16_t color = tft.color565(r, g, b);
    tft.drawFastHLine(0, y, 320, color);
  }
}
void drawDoubleBorder(uint16_t color) {
  tft.drawRect(0, 0, 320, 240, color);   
  tft.drawRect(4, 4, 312, 232, color);  
}

void drawHugeBomb(int centerX, int centerY) {
  tft.fillCircle(centerX, centerY, 35, TFT_BLACK);  
  tft.fillCircle(centerX - 10, centerY - 10, 6, TFT_LIGHTGREY); 

  // Fuse
  tft.drawLine(centerX, centerY - 35, centerX + 5, centerY - 45, TFT_DARKGREY);
  tft.drawLine(centerX + 5, centerY - 45, centerX + 14, centerY - 40, TFT_DARKGREY);
  tft.drawLine(centerX + 14, centerY - 40, centerX + 22, centerY - 50, TFT_DARKGREY);

  
  int sparkX = centerX + 28;
  int sparkY = centerY - 56;

  tft.fillCircle(sparkX, sparkY, 5, TFT_YELLOW);       
  tft.drawLine(sparkX - 10, sparkY, sparkX + 10, sparkY, TFT_ORANGE);
  tft.drawLine(sparkX, sparkY - 10, sparkX, sparkY + 10, TFT_ORANGE);
  tft.drawLine(sparkX - 7, sparkY - 7, sparkX + 7, sparkY + 7, TFT_RED);
  tft.drawLine(sparkX - 7, sparkY + 7, sparkX + 7, sparkY - 7, TFT_RED);

}




void drawShieldIcon(int x, int y) {
  // Shield triangle
  tft.fillTriangle(x, y, x - 30, y + 60, x + 30, y + 60, TFT_GREEN);

  // Checkmark
  tft.drawLine(x - 15, y + 35, x - 5, y + 50, TFT_WHITE);
  tft.drawLine(x - 5, y + 50, x + 15, y + 25, TFT_WHITE);
}



void learningMode() {
  int selectedSubMode = 0;  
  bool subMenuSelected = false;
  lastClk = digitalRead(ENCODER_CLK);

  tft.pushImage(0, 0, 320, 240, myImage);


  tft.setTextColor(TFT_RED);
  tft.setTextSize(3);
  tft.setCursor(30, 40);
  tft.print("LEARNING MODE");

  int lastMode = -1;

  while (!subMenuSelected) {
    
    if (selectedSubMode != lastMode) {
      
      tft.setTextSize(2);

      for (int i = 0; i < 4; i++) {
        int x = (i == 3) ? 60 : 30; 
        int y = (i == 3) ? 190 : (80 + i * 35); 

        tft.setCursor(x, y);
        tft.setTextColor(i == selectedSubMode ? TFT_BLUE : TFT_BLACK);

        if (i < totalLearningMenuItems) {
          tft.print(learningMenuTitles[i]);  
        } 
        else {
        tft.print("← Back to Main Menu");
        }

      }

      lastMode = selectedSubMode;
    }

    
    int currentClk = digitalRead(ENCODER_CLK);
    if (currentClk != lastClk && currentClk == LOW) {
      if (digitalRead(ENCODER_DT) != currentClk)
        selectedSubMode = (selectedSubMode + 1) % 4;
      else
        selectedSubMode = (selectedSubMode + 3) % 4;
      delay(200);
    }
    lastClk = currentClk;

    
    if (digitalRead(ENCODER_SW) == LOW) {
      while (digitalRead(ENCODER_SW) == LOW);
      delay(100);

      if (dfplayerReady) myDFPlayer.play(4);
      delay(200);

      if (selectedSubMode == 0) whatIsBinary();
      else if (selectedSubMode == 1) realLifeUses();
      else if (selectedSubMode == 2) tryBinaryBits();
      else if (selectedSubMode == 3) {
        selected = false;
        showMainMenu();
      }

      subMenuSelected = true;
      return;
    }
  }
}


void whatIsBinary() {
  int slide = 0;

  while (true) {
    tft.fillScreen(TFT_WHITE);
    drawLearningModeDecorations();
    drawWhatIsBinarySlide(slide);

    tft.setTextColor(TFT_BLUE);
    tft.setCursor(10, 210);
    tft.printf("[%d/%d] ", slide + 1, numWhatIsBinarySlides);
    tft.print(slide < numWhatIsBinarySlides - 1 ? "# = Next | 0 = Back" : "0 = Back");

    while (true) {
      char key = keypad.getKey();
      if (key == '#') {
        if (slide < numWhatIsBinarySlides - 1) slide++;
        else { showMainMenu(); return; }
        break;
      } else if (key == '0') {
        if (slide > 0) slide--;
        else { showMainMenu(); return; }
        break;
      }

      if (digitalRead(ENCODER_SW) == LOW) {
        while (digitalRead(ENCODER_SW) == LOW);
        delay(100);
        showMainMenu();
        return;
      }
    }
  }
}


void drawWhatIsBinarySlide(int slide) {
  int y = 20;
  
  for (int i = 0; i < 6; i++) {
    if (whatIsBinarySlides[slide][i][0] == '\0') break;

    if (i == 0) {
      tft.setTextSize(2);
      tft.setTextColor(TFT_RED);  
    } else {
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK);  
    }

    tft.setCursor(10, y);
    tft.println(whatIsBinarySlides[slide][i]);
    y += (i == 0) ? 28 : 30;
  }
}


void drawRealLifeUsesSlide(int slide) {
  int y = 20;
  for (int i = 0; i < 6; i++) {
    if (realLifeUsesSlides[slide][i][0] == '\0') break;

    if (i == 0) {
      tft.setTextSize(2);
      tft.setTextColor(TFT_RED);  
    } else {
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK); 
    }

    tft.setCursor(10, y);
    tft.println(realLifeUsesSlides[slide][i]);
    y += (i == 0) ? 28 : 30;
  }
}

void realLifeUses() {
  int slide = 0;
  
  while (true) {
    tft.fillScreen(TFT_WHITE);
    drawLearningModeDecorations();
    drawRealLifeUsesSlide(slide);

    tft.setTextColor(TFT_BLUE);
    tft.setCursor(10, 210);
    tft.printf("[%d/%d] ", slide + 1, numRealLifeUsesSlides);
    tft.print(slide < numRealLifeUsesSlides - 1 ? "# = Next | 0 = Back" : "0 = Back");

    while (true) {
      char key = keypad.getKey();
      if (key == '#') {
        if (slide < numRealLifeUsesSlides - 1) slide++;
        else { showMainMenu(); return; }
        break;
      } else if (key == '0') {
        if (slide > 0) slide--;
        else { showMainMenu(); return; }
        break;
      }

      if (digitalRead(ENCODER_SW) == LOW) {
        while (digitalRead(ENCODER_SW) == LOW);
        delay(100);
        showMainMenu();
        return;
      }
    }
  }
}




void tryBinaryBits() {
  int bits[4] = {0, 0, 0, 0};  // B3 B2 B1 B0

  auto drawBits = [&]() {
    
    drawLearningModeDecorations();

    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 20);
    tft.print("Toggle Bits (B3 B2 B1 B0):");

   
    tft.setTextSize(4);
    tft.setCursor(50, 70);
    for (int i = 0; i < 4; i++) {
      tft.print(bits[i]);
      tft.print(" ");
    }

   
    tft.setTextSize(1);
    tft.setTextColor(TFT_RED);
    int bitX = 50;
    int bitY = 105;
    int spacing = 50;
    int values[4] = {8, 4, 2, 1};
    for (int i = 0; i < 4; i++) {
      tft.setCursor(bitX + i * spacing, bitY);
      tft.print(values[i]);
    }

   
    tft.setTextSize(2);
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(10, 140);
    tft.print("Decimal: ");
    tft.setTextSize(3);
    tft.setTextColor(TFT_BLUE);
    int decimal = bits[0]*8 + bits[1]*4 + bits[2]*2 + bits[3]*1;
    tft.print(decimal);

    
    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(10, 185);
    tft.print("Press 1-4 to toggle bits.");
    tft.setTextSize(2);
    tft.setCursor(10, 200);
    tft.print("1=B3  2=B2  3=B1  4=B0");
    tft.setCursor(10, 220);
    tft.print("0=Back");
  };

  drawBits();

  while (true) {
    char key = keypad.getKey();

    
    if (key == '0') {
      selected = false;
      showMainMenu();
      return;
    }

   
    if (key >= '1' && key <= '4') {
      int index = key - '1';
      bits[index] = !bits[index];
      drawBits();
    }

   
    if (digitalRead(ENCODER_SW) == LOW) {
      while (digitalRead(ENCODER_SW) == LOW);
      delay(100);
      selected = false;
      showMainMenu();
      return;
    }
  }
}

void drawLearningModeDecorations() {
  tft.pushImage(0, 0, 320, 240, myImage);
}

void drawHeart(int x, int y, int size, uint16_t color) {
  int r = size / 4;

  
  tft.fillCircle(x - r, y, r, color);
  tft.fillCircle(x + r, y, r, color);

  
  tft.fillTriangle(x - size / 2, y, x + size / 2, y, x, y + size, color);
}


