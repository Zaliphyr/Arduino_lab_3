// Define all IO
const int p1_but = 2,
          p2_but = 3,
          p1_led = 4,
          p2_led = 5,

          green_led = 6,
          red_led = 7,
          blue_led = 8,
          buzzer = 9;

// Everything needed for timer
unsigned long game_time = 0;
unsigned long previous_millis = 0;
unsigned long current_millis = 0;

// Everything menu and score system
int p1_score = 0;
int p2_score = 0;
int incomingByte = 0;
int selector = 0;
unsigned long light_changed = 0;
unsigned long reaction = 0;

void setup() {
  randomSeed(analogRead(A0));
  Serial.begin(9600);

  pinMode(p1_but, INPUT);
  pinMode(p2_but, INPUT);
  pinMode(p1_led, OUTPUT);
  pinMode(p2_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  menu_message();
}
// s = 115
// q = 113
// r = 114
void loop() {
  if (Serial.available() > 0){
    incomingByte = Serial.read();

    switch (incomingByte) {
      case 115:
        Serial.println("Game starting!");
        game();
        Serial.flush();
        break;
      case 113:
        Serial.flush();
        Serial.println("Good bye!");
        exit(0);
        break;
      case 114:
        p1_score = 0;
        p2_score = 0;
        Serial.println("Scores reset");
        Serial.flush();
        break;
    }
  }
}

void game(){
  // Set random time length between 3 and 6 sec
  game_time = random(3000, 6000);
  selector = random(1, 11);
  digitalWrite(red_led, HIGH);
  
  // Reset timer
  current_millis = millis();
  previous_millis = current_millis;
  
  // Game loop
  while (digitalRead(p1_but) == LOW && digitalRead(p2_but) == LOW){
    current_millis = millis();

    // Check if game time has passed
    if (current_millis - previous_millis >= game_time){
      if (light_changed == 0) light_changed = millis();
      if (selector < 8){
        digitalWrite(red_led, LOW);
        digitalWrite(green_led, HIGH);
      } else {
        digitalWrite(red_led, LOW);
        digitalWrite(blue_led, HIGH);
        if (current_millis > (previous_millis + game_time + 2000)){
           digitalWrite(blue_led, LOW);
           break;
        }
      }
    }
  }
  reaction = millis() - light_changed;

  // Check if button was pressed while red led still on
  if (digitalRead(red_led) == HIGH){
    digitalWrite(red_led, LOW);
    
    // Check who pressed the button
    if (digitalRead(p1_but) == HIGH){
      change_score(1, -1000);
      print_score();
      beep(500);
    } else if (digitalRead(p2_but) == HIGH){
      change_score(2, -1000);
      print_score();
      beep(500);
    }
    
  // Check if button was pressed while green led on
  } else if (digitalRead(green_led) == HIGH){
    digitalWrite(green_led, LOW);

    // Check who pressed the button
    if (digitalRead(p1_but) == HIGH){
      change_score(1, (1000-reaction));
      print_score();
      digitalWrite(p1_led, HIGH);
      beep(1000);
    } else if (digitalRead(p2_but) == HIGH){
      change_score(2, (1000-reaction));
      print_score();
      digitalWrite(p2_led, HIGH);
      beep(1000);
    }

  // Check if button was pressed while blue led on
  } else if (digitalRead(blue_led) == HIGH){
    digitalWrite(blue_led, LOW);

    if (digitalRead(p1_but) == HIGH){
      change_score(1, -2000);
      print_score();
      beep(300);
    } else if (digitalRead(p2_but) == HIGH){
      change_score(2, -2000);
      print_score();
      beep(300);
    }
  }

  light_changed = 0;
  digitalWrite(p1_led, LOW);
  digitalWrite(p2_led, LOW);
  if (p1_score < 3000 && p2_score < 3000){
    game();
  } else {
    if (p1_score >= 3000){
      Serial.println("P1 WON!!");
      win(1);
      menu_message();
    } else {
      Serial.println("P2 WON!!");
      win(2);
      menu_message();
    }
  }
  Serial.flush();
}
// Send menu message to serial
void menu_message(){
  Serial.println("Welcome to this amazing game where you can challange other people to see who has quickest reactions!");
  Serial.println();
  Serial.println("First to press the button after the light turns green gets 1000 points minus reaction time");
  Serial.println("If you press too soon you loose 1000 points");
  Serial.println("Make sure you DONT press if light turns blue, then you loose 2000 points");
  Serial.println("First to 3000 wins!");
  Serial.println();
  Serial.println("Send 's' to start the game");
  Serial.println("Send 'r' to reset the score");
  Serial.println("Send 'q' to quit");
}

// Change the points for a player (witch player, amount)
void change_score(int player, int amount){
  if (player == 1){
    p1_score += amount;
    if (p1_score < 0) p1_score = 0;
  } else if (player == 2){
    p2_score += amount;
    if (p2_score < 0) p2_score = 0;
  }
}

// Sends score to serial
void print_score(){
  Serial.println("Player 1: " + String(p1_score) + " Player 2: " + String(p2_score) + " reaction time: " + String(reaction));
}

// Function for lights and sound if p1 won
void win(int plr){
  if (plr == 1){
    digitalWrite(p1_led, HIGH);
  } else digitalWrite(p2_led, HIGH);
  for (int j = 0; j < 5; j++){
    for (int i = 800; i < 1200; i++){
      tone(buzzer, i);
      if (i%200==0){
        digitalWrite(green_led, !digitalRead(green_led));
        if (plr == 1){
          digitalWrite(p1_led, !digitalRead(p1_led));
        } else digitalWrite(p2_led, !digitalRead(p2_led));
      }
      delay(1);
    }
  }
  noTone(buzzer);
  digitalWrite(p1_led, LOW);
  digitalWrite(p2_led, LOW);
}

// Dunction to run the win tone and lights
void beep(int sound){
  for (int i = 0; i <100; i++){
    if (i%25==0){
      tone(buzzer, sound, 60);
    }
    delay(5);
  }
}
