#pragma once

struct QuizQuestion {
  String question;
  String answer;
};

// Main Menu Titles
const char* mainMenuTitles[] = {
  "1. Binary to Decimal",
  "2. Decimal to Binary",
  "3. Binary Bomb",
  "4. Learning Mode"
};
const int totalMainMenuItems = sizeof(mainMenuTitles) / sizeof(mainMenuTitles[0]);

// Learning Mode Titles
const char* learningMenuTitles[] = {
  "1. What is Binary?",
  "2. Real-Life Uses",
  "3. Try Binary Bits"
};
const int totalLearningMenuItems = sizeof(learningMenuTitles) / sizeof(learningMenuTitles[0]);


// ---Mode 1 ---
const QuizQuestion binaryToDecimal[] = {
  {"Convert 0000 to Decimal:", "0"},
  {"Convert 0001 to Decimal:", "1"},
  {"Convert 0010 to Decimal:", "2"},
  {"Convert 0011 to Decimal:", "3"},
  {"Convert 0100 to Decimal:", "4"},
  {"Convert 0101 to Decimal:", "5"},
  {"Convert 0110 to Decimal:", "6"},
  {"Convert 0111 to Decimal:", "7"},
  {"Convert 1000 to Decimal:", "8"},
  {"Convert 1001 to Decimal:", "9"},
  {"Convert 1010 to Decimal:", "10"},
  {"Convert 1011 to Decimal:", "11"},
  {"Convert 1100 to Decimal:", "12"},
  {"Convert 1101 to Decimal:", "13"},
  {"Convert 1110 to Decimal:", "14"},
  {"Convert 1111 to Decimal:", "15"}
};
const int numBinaryToDecimal = sizeof(binaryToDecimal) / sizeof(QuizQuestion);

// --- Mode 2 ---
const QuizQuestion decimalToBinary[] = {
  {"Convert 0 to Binary:", "0000"},
  {"Convert 1 to Binary:", "0001"},
  {"Convert 2 to Binary:", "0010"},
  {"Convert 3 to Binary:", "0011"},
  {"Convert 4 to Binary:", "0100"},
  {"Convert 5 to Binary:", "0101"},
  {"Convert 6 to Binary:", "0110"},
  {"Convert 7 to Binary:", "0111"},
  {"Convert 8 to Binary:", "1000"},
  {"Convert 9 to Binary:", "1001"},
  {"Convert 10 to Binary:", "1010"},
  {"Convert 11 to Binary:", "1011"},
  {"Convert 12 to Binary:", "1100"},
  {"Convert 13 to Binary:", "1101"},
  {"Convert 14 to Binary:", "1110"},
  {"Convert 15 to Binary:", "1111"}
};
const int numDecimalToBinary = sizeof(decimalToBinary) / sizeof(QuizQuestion);

// What Is Binary Slides
const char* whatIsBinarySlides[][10] = {
  {//Slide 1
    "What is Binary?",
    "---------------",
    "-Binary = only 0 and 1.",
    "-Used by all computers.",
    "-It is a number system.",
    "-Bin=Base-2, Dec=Base-10"
  },
  {//Slide 2
    "Why Binary?",
    "-----------",
    "-ON = 1, OFF = 0",
    "-Easy to detect.",
    "-Fast and reliable.",
    " "
  },
  {//Slide 3
    "Computers & Binary",
    "-------------------",
    "-Computers can ONLY ",
    "understand binary.",
    "-All data becomes 0s and",
    "1s to them."
  },
  {//Slide 4
    "Binary = Foundation",
    "--------------------",
    "-Binary is the base of",
    "digital electronics.",
    "-All devices work with",
    "1s and 0s inside."
  },
  {//Slide 5
    "What Can It Store?",
    "-------------------",
    "Binary can store:",
    "- Text",
    "- Images",
    "- Sounds"
  },
  {//Slide 6
    "What is a Bit?",
    "---------------",
    "-Bit = one 0 or 1",
    "-Stands for Binary Digit",
    "-8 bits = 1 byte",
    "-Bits form all data"
  },
  {//Slide 7
    "Bit Positions",
    "-------------",
    "B3  B2  B1  B0",
    "|   |   |   |",
    "1   0   1   1",
    "8 + 0 + 2 + 1 = 11"
  },
  {//Slide 8
    "Binary to Decimal",
    "------------------",
    "Example: >> 1011 <<",
    "1x8 + 0x4 + 1x2 + 1x1",
    "= 8 +  0  +  2  +  1 = 11",
    "So 1011 = 11"
  }
};
const int numWhatIsBinarySlides = sizeof(whatIsBinarySlides) / sizeof(whatIsBinarySlides[0]);

//Real Life Uses Slides

#pragma once

const char* realLifeUsesSlides[][7] = {
  { // Slide 1 
    "Text and Letters",
    "-----------------",
    "Each letter = number",
    "A = 65 = 01000001",
    "H = 72 = 01001000",
    "I = 73 = 01001001"
  },
  { // Slide 2 
    "Typing is Binary",
    "-----------------",
    "-When you type 'HI'",
    "-You send this:",
    "-01001000 01001001",
    "Computer understands!"
  },
  { // Slide 3 
    "Colors on Screens",
    "------------------",
    "-Every color has 3 parts:",
    "-Red, Green, and Blue",
    "-Each part is a number",
    "-Stored as binary (0s/1s)"
  },
  { // Slide 4 
    "Binary in Storage",
    "------------------",
    "-Files stored as binary.",
    "-Documents,Photos,Videos",
    "-All saved as rows of",
    "1s and 0s on memory chips."
  },
  { // Slide 5 
    "Binary in Keyboards",
    "---------------------",
    "-Each key you press",
    "sends a number to the PC.",
    "-That number becomes",
    "binary: 01001000"
  }
};

const int numRealLifeUsesSlides = sizeof(realLifeUsesSlides) / sizeof(realLifeUsesSlides[0]);



