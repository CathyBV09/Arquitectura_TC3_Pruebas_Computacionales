#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI   11
#define OLED_CLK    13
#define OLED_DC     9
#define OLED_CS     10
#define OLED_RESET  8

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         &SPI, OLED_DC, OLED_RESET, OLED_CS);

#define BUTTON_MOVE 2
#define BUTTON_OK   3

int cursorX = 0;
int cursorY = 0;

int board[3][3] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

int currentPlayer = 1;  // 1 = humano, 2 = CPU
bool gameOver = false;

// ---------------------
// FUNCIONES DEL JUEGO
// ---------------------

void drawBoard() {
  display.clearDisplay();

  // líneas verticales
  display.drawLine(42, 0, 42, 64, WHITE);
  display.drawLine(85, 0, 85, 64, WHITE);

  // líneas horizontales
  display.drawLine(0, 21, 128, 21, WHITE);
  display.drawLine(0, 43, 128, 43, WHITE);

  // dibuja X, O y el cursor
  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 3; x++) {
      int px = x * 42 + 14;
      int py = y * 21 + 8;

      if (board[y][x] == 1) {
        display.drawCircle(px, py, 6, WHITE);  // O humano
      } 
      else if (board[y][x] == 2) {
        display.drawLine(px - 5, py - 5, px + 5, py + 5, WHITE);
        display.drawLine(px - 5, py + 5, px + 5, py - 5, WHITE);
      }

      // cursor solo si está vacío
      if (!gameOver && cursorX == x && cursorY == y && board[y][x] == 0) {
        display.drawRect(x * 42 + 2, y * 21 + 2, 40, 19, WHITE);
      }
    }
  }

  display.display();
}

int checkWinner() {
  for (int i = 0; i < 3; i++) {
    if (board[i][0] != 0 &&
        board[i][0] == board[i][1] &&
        board[i][1] == board[i][2])
      return board[i][0];

    if (board[0][i] != 0 &&
        board[0][i] == board[1][i] &&
        board[1][i] == board[2][i])
      return board[0][i];
  }

  if (board[0][0] != 0 &&
      board[0][0] == board[1][1] &&
      board[1][1] == board[2][2])
    return board[0][0];

  if (board[0][2] != 0 &&
      board[0][2] == board[1][1] &&
      board[1][1] == board[2][0])
    return board[0][2];

  bool full = true;
  for (int y = 0; y < 3; y++)
    for (int x = 0; x < 3; x++)
      if (board[y][x] == 0) full = false;

  if (full) return 3; // empate

  return 0; // sigue jugando
}

void cpuMove() {
  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 3; x++) {
      if (board[y][x] == 0) {
        board[y][x] = 2;
        return;
      }
    }
  }
}

// ----------------------

void setup() {
  pinMode(BUTTON_MOVE, INPUT_PULLUP);
  pinMode(BUTTON_OK, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.display();

  drawBoard();
}

void loop() {
  if (gameOver) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 25);

    int winner = checkWinner();

    if (winner == 1) display.print("GANASTE");
    else if (winner == 2) display.print("CPU GANA");
    else display.print("EMPATE");

    display.display();
    delay(2000);

    // reiniciar
    for (int y = 0; y < 3; y++)
      for (int x = 0; x < 3; x++)
        board[y][x] = 0;

    cursorX = cursorY = 0;
    currentPlayer = 1;
    gameOver = false;
    drawBoard();
    return;
  }

  // botón mover
  if (!digitalRead(BUTTON_MOVE)) {
    cursorX++;
    if (cursorX > 2) {
      cursorX = 0;
      cursorY++;
      if (cursorY > 2) cursorY = 0;
    }
    drawBoard();
    delay(200);
  }

  // botón OK
  if (!digitalRead(BUTTON_OK)) {
    if (board[cursorY][cursorX] == 0) {
      board[cursorY][cursorX] = 1;

      int w = checkWinner();
      if (w != 0) { gameOver = true; return; }

      cpuMove();

      w = checkWinner();
      if (w != 0) { gameOver = true; return; }
    }
    drawBoard();
    delay(200);
  }
}
