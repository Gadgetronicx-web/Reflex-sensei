/*
Reflex sensei is a Relex trainer game built using Arduino Uno. This trainer game has 4 modes to cater Athletes, Adults, Kids and Aged.
Also there is 4 different levels in each modes to make the experience challenging and fun. 

Authors: Nihar Gupta and Labishetty Mrudula from www.gadgetronicx.com 

Features

Mode selection: 
We are have four modes to select which are among athletes, adults, kids and aged groups. 
The mode is selected using keypad. According to the selected mode the level of the game will be decided.

Levels:
  There are 4 levels corresponding to each mode. 
  If anyone mode is selected then it contains it has four levels and every level has a threshold value which need to be crossed to play second round. By keeping different levels we can create interest to players.
Score Displaying
  After completion of playing the scores will be compared with the previous data in the EEPROM and top four players names will be displayed on the LCD with their respective scores
*/

// declaring header files 
#include<LiquidCrystal.h>
#include<EEPROM.h>

// declaring the lcd variables 
#define lcd_register_select A0
#define lcd_enable A1
#define lcd_data_pin_4 A2
#define lcd_data_pin_5 A3
#define lcd_data_pin_6 A4
#define lcd_data_pin_7 A5

// declaring the interrupt pin
#define interrupt_pin 2

//defining buttons (i.e. led pins)
#define button1 0
#define button2 1
#define button3 3
#define button4 4

// defining delay factor for multiplexed pins of keypad
#define delay_button 400 //400ms

// initilizing score of a player with 0
int score=0;

// initilizing random_value variable for storing the random values
int random_value=0;

//defining rows and coloums for keypad
#define row1 5
#define row2 6
#define row3 7
#define row4 8
#define row5 9
#define col1 10
#define col2 11
#define col3 12
#define col4 13


// initilizing modeselected as 0
int modeselected=0;

// Defining playersname variable to store the current player name
String playersname; //to store name of the player


// making the lcd variables
LiquidCrystal lcd(lcd_register_select, lcd_enable, lcd_data_pin_4, lcd_data_pin_5, lcd_data_pin_6, lcd_data_pin_7);

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2); // initializing lcd
  attachInterrupt(digitalPinToInterrupt(interrupt_pin), point_count, RISING);      //Setting the external interrupt on pin 2 raising edge

  // declaring button pins as output pins to make led's glow 
  pinMode(button1,OUTPUT) ; //Button 1
  pinMode(button2,OUTPUT) ; //Button 2
  pinMode(button3,OUTPUT) ; //Button 3
  pinMode(button4,OUTPUT) ; //Button 4

  // defining keypad pins as output and input
  pinMode(row1,OUTPUT); // defining rows are output and coloumn as input
  pinMode(row2,OUTPUT); // rows are used to write the data (i.e. 0 or 1) and coloumn are used for output data (i.e. 0 or 1)
  pinMode(row3,OUTPUT); 
  pinMode(row4,OUTPUT);
  pinMode(row5,OUTPUT);
  pinMode(col1,INPUT);
  pinMode(col2,INPUT);
  pinMode(col3,INPUT);
  pinMode(col4,INPUT);

  // initilizing all buttons (i.e. led's) low
  digitalWrite(button1,LOW);
  digitalWrite(button2,LOW);
  digitalWrite(button3,LOW);
  digitalWrite(button4,LOW);

  // initilizing EEPROM memory by a default value
  EEPROM_init(); // initializing eeprom will 0 scores
}

// loop part runs infinite times
void loop() {
  
     // initializing score to 0 so that everytime when a new player play the game his score will be 0 initially 
     score=0;

     // making lcd to print "Select the mode"
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Select the Mode");

    /* selectthemode function returns the mode selected by the user
       this funtion waits until the users selects the mode */
     modeselected=selectthemode();

     // This all indicates the game is going to start
     lcd.clear();    
     lcd.setCursor(5,0);
     lcd.print("Lets Play!!");

     // making leds high and low for 500ms to indicate the start of game
     digitalWrite(button1,HIGH);
     delay(500);
     digitalWrite(button1,LOW);
     digitalWrite(button2,HIGH);
     delay(500);
     digitalWrite(button2,LOW);
     digitalWrite(button3,HIGH);
     delay(500);
     digitalWrite(button3,LOW);
     digitalWrite(button4,HIGH);
     delay(500);
     digitalWrite(button4,LOW);
     lcd.clear();
     delay(1000);

     /* As per the player input "modeselected " 
      respective function will be called to start the game */
      if(modeselected==1)  athletes(); //calling athletes funtion
      else if(modeselected==2)  kids(); //calling kids funtion
      else if(modeselected==3)  adults(); //calling adults funtion
      else if(modeselected==4)  aged(); //calling aged funtion
     

     /* check_learderboard will check the leaderboard
        it checks wheather the current player is allowded to enter into the leaderboard
        This checking depends on  modeselected and also previous player scores
        This funtion returns a boolean value 
            true --> if the current player is allowded into the leaderboard
            false --> if the current player is not allowded into the leaderboard
      */
     bool check = check_leaderboard();
     
     if(check==true)
     {  
        /* 
         *  This true block will be exeguted when the player is allowded into leaderboard 
         *  This entry requires the player name 
         *  It takes the player name from player_name funtion 
         *  and it will update the leaderboard
         */
         
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Enter your name");
        playersname=player_name();  // It returns the player name with 5 characters
        update_leaderboard();     // It updates the leaderboard and also prints the present leaderboard in lcd
     }
     else
     {
        /*
         * This block will be executed when the player is not allowded into leaderboard
         * So this will direcly jumps to print the leaderboard
         */
        lcd_leaderboard();      // It prints the leaderborad i.e. previous leaderboard only
     }

}


/*
 * player_name funtion will take the player name as input
 * It takes exactly 5 characters of the players name
 * This function internally calls get_char function 
 * Which returns a character according to the user input from the keyboard
 * After taking 5 characters it is converted into string and that string i.e. player name
 * is returned to the loop block
 */
String player_name()
{
  int character_number=1;
  String player_name="";
  while(character_number<=5)
  {  // this loop runs 5 times and each time it takes a character from the get_char funtion
    char character;
    character=get_char();
    player_name+=character;
    character_number+=1;
    delay(300);
  }
  
  return player_name;
}



/*
 * get_char function will be used to get a character from the user
 * This function activates the keypad
 * It activates i.e. makes each row high for some time and at each time
 * it takes the colomns values and checks which coloums are high 
 * And returns that equavalent character to the  player_name() funtion
 * Some buttons are multiplexed with two characters in such a case it waits for delay_button time
 * and checks if the same button is pressed once more
 * And according to the condition it returns the equivalent character
 * 
 * KEYPAD EQUIVALENT CHARACTERS
 *                COL1    COL2    COL3    COL4
 *      ROW 1     'A'     'B'     'C'     'D'
 *      ROW 2     'E'     'F'     'G'     'H'
 *      ROW 3     'I'     'J'     'K'     'L'
 *      ROW 4     'M/N'   'O/P'   'Q/R'   'S/T'
 *      ROW 5     'U/V'   'W/X'   'Y/Z'   ' ' (i.e. white space)
 */

char get_char()
{ 
  while(1) // runs until the player presses a button
  {
    digitalWrite(row1,HIGH);//making row1 as high and all other as low
    digitalWrite(row2,LOW);
    digitalWrite(row3,LOW);
    digitalWrite(row4,LOW);
    digitalWrite(row5,LOW);
    if(digitalRead(col1)==1) 
    {
       while(digitalRead(col1)==1); //wating until the player release the pressed button
       return 'A'; // returning resp character if any button is pressed
    }
    else if(digitalRead(col2)==1)
    { 
      while(digitalRead(col2)==1); //wating until the player release the pressed button
      return 'B'; // returning resp character if any button is pressed
    }
    else if(digitalRead(col3)==1) 
    { while(digitalRead(col3)==1);//wating until the player release the pressed button
      return 'C';  // returning resp character if any button is pressed
    }
    else if(digitalRead(col4)==1) 
    { while(digitalRead(col4)==1); //wating until the player release the pressed button
      return 'D';  // returning resp character if any button is pressed
    }
    digitalWrite(row1,LOW);   // making row1 low (which was high before)
    digitalWrite(row2,HIGH);  // now making row2 as high and here all other rows will be low
    if(digitalRead(col1)==1) 
    {
       while(digitalRead(col1)==1); //wating until the player release the pressed button
       return 'E'; // returning resp character if any button is pressed
    }
    else if(digitalRead(col2)==1)
    { while(digitalRead(col2)==1); //wating until the player release the pressed button
      return 'F'; // returning resp character if any button is pressed
    }
    else if(digitalRead(col3)==1) 
    { while(digitalRead(col3)==1); //wating until the player release the pressed button
      return 'G'; // returning resp character if any button is pressed
    }
    else if(digitalRead(col4)==1) 
    { while(digitalRead(col4)==1); //wating until the player release the pressed button
      return 'H'; // returning resp character if any button is pressed
    }
    digitalWrite(row2,LOW);// making row1 low (which was high before)
    digitalWrite(row3,HIGH);// now making row2 as high and here all other rows will be low
    if(digitalRead(col1)==1) 
    {
       while(digitalRead(col1)==1); //wating until the player release the pressed button
       return 'I';// returning resp character if any button is pressed
    }
    else if(digitalRead(col2)==1)
    { while(digitalRead(col2)==0); //wating until the player release the pressed button
      return 'J'; // returning resp character if any button is pressed
    }
    else if(digitalRead(col3)==1) 
    { while(digitalRead(col3)==1); //wating until the player release the pressed button
      return 'K';// returning resp character if any button is pressed
    }
    else if(digitalRead(col4)==1) 
    { while(digitalRead(col4)==0); //wating until the player release the pressed button
      return 'L'; // returning resp character if any button is pressed
    }
    digitalWrite(row3,LOW);// making row1 low (which was high before)
    digitalWrite(row4,HIGH);// now making row2 as high and here all other rows will be low
    if(digitalRead(col1)==1)   
    {  // these buttons have multiplexed characters each time it cheaks wheater it is pressed one more time
      
        while(digitalRead(col1)==1); //wating until the player release the pressed button
        for(int i=0;i<delay_button;i+=10) // checking the same button is pressed for one more time for a delay_button time
        { delay(10);
          if(digitalRead(col1)==1) 
          {   // if the same button is pressed 
              while(digitalRead(col1)==1); //wating until the player release the pressed button
              return 'M'; // returns the second character associated with the button
          }
        }
        return 'N';  // returns the first character associated with the button
    }
    else if(digitalRead(col2)==1)
    { 
       // these buttons have multiplexed characters each time it cheaks wheater it is pressed one more time
        while(digitalRead(col2)==1);//wating until the player release the pressed button
        for(int i=0;i<delay_button;i+=10)  // checking the same button is pressed for one more time for a delay_button time
        { delay(10);
          if(digitalRead(col2)==1)
          {  // if the same button is pressed 
              while(digitalRead(col2)==1);//wating until the player release the pressed button
              return 'O';  // returns the second character associated with the button
          }
        }
        return 'P';   // returns the first character associated with the button
    }
    else if(digitalRead(col3)==1) 
    {   
        // these buttons have multiplexed characters each time it cheaks wheater it is pressed one more time
        while(digitalRead(col3)==1);  //wating until the player release the pressed button
        for(int i=0;i<delay_button;i+=10)  // checking the same button is pressed for one more time for a delay_button time
        { delay(10);
          if(digitalRead(col3)==1)
          {  // if the same button is pressed 
              while(digitalRead(col3)==1);  //wating until the player release the pressed button
              return 'Q';  // returns the second character associated with the button
          }
        }
        return 'R';  // returns the first character associated with the button
    }
    else if(digitalRead(col4)==1) 
    {   
        // these buttons have multiplexed characters each time it cheaks wheater it is pressed one more time
        while(digitalRead(col4)==1);  //wating until the player release the pressed button
        for(int i=0;i<delay_button;i+=10)  // checking the same button is pressed for one more time for a delay_button time
        { delay(10);
          if(digitalRead(col4)==1)
          {  // if the same button is pressed 
              while(digitalRead(col4)==1);  //wating until the player release the pressed button
              return 'S';  // returns the second character associated with the button
          }
        }
        return 'T';  // returns the first character associated with the button
    }
    digitalWrite(row4,LOW);// making row1 low (which was high before)
    digitalWrite(row5,HIGH); // now making row2 as high and here all other rows will be low
    if(digitalRead(col1)==1) 
    {
        // these buttons have multiplexed characters each time it cheaks wheater it is pressed one more time
        while(digitalRead(col1)==1);  //wating until the player release the pressed button
        for(int i=0;i<delay_button;i+=10)  // checking the same button is pressed for one more time for a delay_button time
        { delay(10);
          if(digitalRead(col1)==1)
          {  // if the same button is pressed 
              while(digitalRead(col1)==1);  //wating until the player release the pressed button
              return 'U';  // returns the second character associated with the button
          }
        }
        return 'V';  // returns the first character associated with the button
    }
    else if(digitalRead(col2)==1)
    {   
        // these buttons have multiplexed characters each time it cheaks wheater it is pressed one more time
        while(digitalRead(col2)==1);  //wating until the player release the pressed button
        for(int i=0;i<delay_button;i+=10)  // checking the same button is pressed for one more time for a delay_button time
        { delay(10);
          if(digitalRead(col2)==1)
          {  // if the same button is pressed 
              while(digitalRead(col2)==1);  //wating until the player release the pressed button
              return 'W';  // returns the second character associated with the button
          }
        }
        return 'X';  // returns the first character associated with the button
    }
    else if(digitalRead(col3)==1) 
    {   
        // these buttons have multiplexed characters each time it cheaks wheater it is pressed one more time
        while(digitalRead(col3)==1);  //wating until the player release the pressed button
        for(int i=0;i<delay_button;i+=10)   // checking the same button is pressed for one more time for a delay_button time
        { delay(10);
          if(digitalRead(col3)==1)
          {  // if the same button is pressed 
              while(digitalRead(col3)==1);  //wating until the player release the pressed button
              return 'Y';  // returns the second character associated with the button
          }
        }
        return 'Z';  // returns the first character associated with the button
    }
    else if(digitalRead(col4)==1) 
    {  // this button returns a space 
      while(digitalRead(col4)==1);  //wating until the player release the pressed button
      return ' ';
    }
  }
}


/* function for mode selection 
This function call the get_mode function to get the mode
This funtion returns an integer based on the mode selected 

 i.e    Return value      Mode Associated 
            1                 Athletes
            2                 Kids
            3                 Adults
            4                 Aged
*/
int selectthemode() //function for mode selction 
{ int mode;
  mode=get_mode();
  return mode;
}


/* This function will be used to select the mode 
 It activates the row1 and keeps reading the colomns 
 It waits until a key is pressed 
 It also waits until a user release the pressed key 
*/
int get_mode()
{ 
 while(1)// It also waits until a user release the pressed key 
  {
    digitalWrite(row1,HIGH);//making row1 as high and all other as low
    digitalWrite(row2,LOW);
    digitalWrite(row3,LOW);
    digitalWrite(row4,LOW);
    digitalWrite(row5,LOW);
    if(digitalRead(col1)==1) 
    {
       while(digitalRead(col1)==1);
       return 1; // returning mode 1 if the button is pressed
    }
    else if(digitalRead(col2)==1)
    { 
      while(digitalRead(col2)==1);
      return 2; // returning mode 2 if the button is pressed
    }
    else if(digitalRead(col3)==1) 
    { 
      while(digitalRead(col3)==1);
      return 3;  // returning mode 3 if the button is pressed
    }
    else if(digitalRead(col4)==1) 
    { 
      while(digitalRead(col4)==1);
      return 4;  // returning mode 4 if the button is pressed
    } 
  }
}



/*check_leaderboard function will check the 
Given score will change the eeprom or not
The main logic here is checking the least player score with the present player score
If the least player score is less than present player score then return TRUE else FALSE
*/

bool check_leaderboard()
{
        if(modeselected == 1) return score>EEPROM[3]; //return true if score greater than threshold value for Athletes mode    
        else if(modeselected == 2) return score>EEPROM[7]; //return true if score greater than threshold value for Kids mode
        else if(modeselected == 3) return score>EEPROM[11]; //return true if score greater than threshold value for Adults mode
        else if(modeselected == 4) return score>EEPROM[15]; //return true if score greater than threshold value for Aged mode
        return false;
}



/*update_leaderboard will update the leaderboard accoring to the requirements
This updation depends on the mode_selected
It also depends on the present person score compaired with all other persons in that respective mode 
And updates the eeprom data respectively 
*/

/*
 * EEPROM Memory allocation
 * ----------------------------------------------------------------
 * EERPOM_Memory_Index        Used_For
 * -----------------------------------------------------------------
 *      [0-3]                 Scores of Athletes mode four players
 *      [4-7]                 Scores of Kids mode four players
 *      [8-11]                Scores of Adults mode four players
 *      [12-15]               Scores of Aged mode four players
 * -----------------------------------------------------------------
 *      Mode        Player_Number     EEPROM_Memeory_Index
 *                                    for storing Player_Name (i.e. 5 characters)
 * -----------------------------------------------------------------
 *      Athletes          1             [16-20]
 *                        2             [21-25]
 *                        3             [25-30]
 *                        4             [31-35]
 *      Kids              1             [36-40]
 *                        2             [41-45]
 *                        3             [45-50]
 *                        4             [51-55]
 *      Adults            1             [56-60]
 *                        2             [61-65]
 *                        3             [65-70]
 *                        4             [71-75]
 *      Aged              1             [76-80]
 *                        2             [81-85]
 *                        3             [85-90]
 *                        4             [91-95]
 * ------------------------------------------------------------------
 */

void update_leaderboard()
{     // here in this funtion i represents index of EEPROM
      if(modeselected == 1)  // checking the present mode is Athletes
      {
              if(score>EEPROM[0])   // checking with 1st person
              {   
                  for(int i=31;i<=35;i++)   EEPROM[i]=char(EEPROM[i-5]); // updating 4th person with 3rd person    
                  for(int i=26;i<=30;i++)   EEPROM[i]=char(EEPROM[i-5]); // updating 3rd person with 2nd person
                  for(int i=21;i<=25;i++)   EEPROM[i]=char(EEPROM[i-5]); // updating 2nd person with 1st person 
                  for(int i=16;i<=20;i++)   EEPROM[i] = playersname[i-16]; // updating the present person into 1st position 
                  EEPROM[3]=EEPROM[2];  // changing the saved score according to the above changes  
                  EEPROM[2]=EEPROM[1];
                  EEPROM[1]=EEPROM[0];
                  EEPROM[0]=score;
              }
              else if(score>EEPROM[1])  // checking with 2nd person
              {    
                   for(int i=31;i<=35;i++)   EEPROM[i]=char(EEPROM[i-5]); // updating 4th person with 3rd person   
                   for(int i=26;i<=30;i++)   EEPROM[i]=char(EEPROM[i-5]); // updating 3rd person with 2nd person
                   for(int i=21;i<=25;i++)   EEPROM[i]= playersname[i-21]; // updating the present person into 2nd position 
                   EEPROM[3]=EEPROM[2]; // changing the saved score according to the above changes  
                   EEPROM[2]=EEPROM[1];
                   EEPROM[1]=score;
              }
              else if(score>EEPROM[2])  // checking with 3rd person
              {   
                   for(int i=31;i<=35;i++)  EEPROM[i]=char(EEPROM[i-5]);  // updating 4th person with 3rd person 
                   for(int i=22;i<=26;i++)  EEPROM[i]=char(playersname[i-22]);  // updating the present person into 3rd position 
                   EEPROM[3]=EEPROM[2]; // changing the saved score according to the above changes  
                   EEPROM[2]=score;
              }
              else if(score>EEPROM[3])  // checking with 4th person
              {
                  for(int i=31;i<=35;i++)   EEPROM[i]=playersname[i-31];// updating the present person into 4th position 
                  EEPROM[3]=score; // changing the saved score according to the above changes  
              }
      }
      else if(modeselected==2)// checking the present mode is Kids
      {
              if(score>EEPROM[4]) // checking with 1st person
              {   
                  for(int i=51;i<=60;i++)   EEPROM[i]=char(EEPROM[i-5]);  // updating 4th person with 3rd person    
                  for(int i=46;i<=50;i++)   EEPROM[i]=char(EEPROM[i-5]);// updating 3rd person with 2nd person
                  for(int i=41;i<=45;i++)   EEPROM[i]=char(EEPROM[i-5]);// updating 2nd person with 1st person 
                  for(int i=36;i<=40;i++)   EEPROM[i] = playersname[i-36]; // updating the present person into 1st position
                  EEPROM[7]=EEPROM[6];// changing the saved score according to the above changes  
                  EEPROM[6]=EEPROM[5];
                  EEPROM[5]=EEPROM[4];
                  EEPROM[4]=score;
              }
              else if(score>EEPROM[5])// checking with 2nd person
              {    
                   for(int i=51;i<=55;i++)  EEPROM[i]=char(EEPROM[i-5]);// updating 4th person with 3rd person   
                   for(int i=46;i<=50;i++)  EEPROM[i]=char(EEPROM[i-5]);// updating 3rd person with 2nd person
                   for(int i=41;i<=45;i++)  EEPROM[i]= playersname[i-41];// updating the present person into 1st position
                   EEPROM[7]=EEPROM[6];// changing the saved score according to the above changes  
                   EEPROM[6]=EEPROM[5];
                   EEPROM[5]=score;
              }
              else if(score>EEPROM[6]) //checking with 3nd person
              {
                   for(int i=51;i<=55;i++)  EEPROM[i]=char(EEPROM[i-5]); // updating 4th person with 3rd person
                   for(int i=46;i<=51;i++)  EEPROM[i]=playersname[i-46]; // updating the present person into 1st position
                   EEPROM[7]=EEPROM[6];// changing the saved score according to the above changes  
                   EEPROM[6]=score;
              }
              else if(score>EEPROM[7]) //checking with 4th person
              {
                  for(int i=51;i<=55;i++)   EEPROM[i]=playersname[i-51];
                  EEPROM[7]=score;// changing the saved score according to the above changes  
              }
      }
      else if(modeselected == 3) //executes if Adults
      {
              if(score>EEPROM[8]) //checking with 1st person
              {   
                  for(int i=71;i<=75;i++)   EEPROM[i]=char(EEPROM[i-5]);// updating 4th person with 3rd person    
                  for(int i=66;i<=70;i++)   EEPROM[i]=char(EEPROM[i-5]);// updating 3rd person with 2nd person
                  for(int i=61;i<=65;i++)   EEPROM[i]=char(EEPROM[i-5]);// updating 2nd person with 1st person 
                  for(int i=56;i<=60;i++)   EEPROM[i] = playersname[i-56]; // updating the present person into 1st position
                  EEPROM[11]=EEPROM[10];// changing the saved score according to the above changes
                  EEPROM[10]=EEPROM[9];
                  EEPROM[9]=EEPROM[8];
                  EEPROM[8]=score;
              }
              else if(score>EEPROM[9])// checking with 2nd person
              {    
                   for(int i=71;i<=75;i++)  EEPROM[i]=char(EEPROM[i-5]);// updating 4th person with 3rd person   
                   for(int i=66;i<=70;i++)  EEPROM[i]=char(EEPROM[i-5]);// updating 3rd person with 2nd person
                   for(int i=61;i<=65;i++)  EEPROM[i]= playersname[i-61];// updating the present person into 2nd position
                   EEPROM[11]=EEPROM[10];// changing the saved score according to the above changes
                   EEPROM[10]=EEPROM[9];
                   EEPROM[9]=score;
              }
              else if(score>EEPROM[10]) //checking with 3rd person
              {   
                   for(int i=71;i<=75;i++)  EEPROM[i]=char(EEPROM[i-5]); // updating 4th person with 3rd person
                   for(int i=66;i<=70;i++)  EEPROM[i]=playersname[i-66];// updating the present person into 3rd position
                   EEPROM[11]=EEPROM[10];// changing the saved score according to the above changes
                   EEPROM[10]=score;
              }
              else if(score>EEPROM[11]) //checking with 4th person
              {
                  for(int i=71;i<=75;i++)   EEPROM[i]=playersname[i-71];// updating the present person into 4th position
                  EEPROM[11]=score; // changing the saved score according to the above changes
              }
      }
      else if(modeselected == 4) //executes if Adults
      {
              if(score>EEPROM[12]) //checking with 1st person
              {   
                  for(int i=91;i<=95;i++)   EEPROM[i]=char(EEPROM[i-5]);  // updating 4th person with 3rd person    
                  for(int i=86;i<=90;i++)   EEPROM[i]=char(EEPROM[i-5]);// updating 3rd person with 2nd person
                  for(int i=81;i<=85;i++)   EEPROM[i]=char(EEPROM[i-5]);// updating 2nd person with 1st person 
                  for(int i=76;i<=80;i++)   EEPROM[i] = playersname[i-76]; // updating the present person into 1st position
                  EEPROM[15]=EEPROM[14];// changing the saved score according to the above changes
                  EEPROM[14]=EEPROM[13];
                  EEPROM[13]=EEPROM[12];
                  EEPROM[12]=score;
              }
              else if(score>EEPROM[13])// checking with 2nd person
              {    
                   for(int i=91;i<=95;i++)  EEPROM[i]=char(EEPROM[i-5]);// updating 4th person with 3rd person   
                   for(int i=86;i<=90;i++)  EEPROM[i]=char(EEPROM[i-5]);// updating 3rd person with 2nd person
                   for(int i=81;i<=85;i++)  EEPROM[i]= playersname[i-81];// updating the present person into 2nd position
                   EEPROM[15]=EEPROM[14];// changing the saved score according to the above changes
                   EEPROM[14]=EEPROM[13];
                   EEPROM[13]=score;
              }
              else if(score>EEPROM[14]) //checking with 3rd person 
              {   
                   for(int i=91;i<=95;i++)  EEPROM[i]=char(EEPROM[i-5]);// updating 4th person with 3rd person 
                   for(int i=86;i<=90;i++)  EEPROM[i]=playersname[i-86];// updating the present person into 3rd position
                   EEPROM[15]=EEPROM[14];// changing the saved score according to the above changes
                   EEPROM[14]=score;
              }
              else if(score>EEPROM[15]) //checking with 4th person
              {
                  for(int i=91;i<=95;i++)   EEPROM[i]=playersname[i-91]; //updating the present person into 4th position
                  EEPROM[15]=score;// changing the saved score according to the above changes
              }
      }
      lcd_leaderboard(); //caling the leaderboard for making the content to display using EEPROM data
}

/* 
 *  lcd_leaderboard funtion is used to display the `learderboard of the current mode
 *  It generates four strings 
 *  string 1 === related to first person name and score in leaderboard
 *  string 2 === related to first person name and score in leaderboard
 *  string 3 === related to first person name and score in leaderboard
 *  string 4 === related to first person name and score in leaderboard
 */
void lcd_leaderboard()
{
        String s1=" 1. ",s2=" 2. ",s3=" 3. ",s4=" 4. ";
        
        // Each string here is related to the each leader 
        // i.e. the s1 is related to first person
        // in the same way s2 to second s3 to third and s4 to fourth
        // according to the mode the score is considered
        // updating this strings according to the  mode of selection 
        // each string starts with the person rank in that mode
        // and then with his name
        // and then with his score
        // ex   1. Frank :- 25

        // Here in this funtion i varable is used to access EEPROM via index method 
        if( modeselected == 1 ) //strings will be modified according to the leaderboard(here Athletes mode)
        { 
          for(int i=16;i<=20;i++)   s1=s1+char(EEPROM[i]); //Athletes mode 1st playername
          for(int i=21;i<=25;i++)   s2=s2+char(EEPROM[i]); //Athletes mode 2nd player name
          for(int i=26;i<=30;i++)   s3=s3+char(EEPROM[i]); //Athletes mode 3rd player name
          for(int i=31;i<=35;i++)   s4=s4+char(EEPROM[i]); //Athletes mode 4th player name
          s1+=" :- "+String(EEPROM[0]); //Athletes mode 1st player score
          s2+=" :- "+String(EEPROM[1]); //Athletes mode 2nd player score
          s3+=" :- "+String(EEPROM[2]); //Athletes mode 3rd player score
          s4+=" :- "+String(EEPROM[3]); //Athletes mode 4th player score
        }
        else if( modeselected == 2 ) //strings will be modified according to the leaderboard(here Kids)
        {
          for(int i=36;i<=40;i++)   s1=s1+char(EEPROM[i]); //Kids mode 1st playername
          for(int i=41;i<=45;i++)   s2=s2+char(EEPROM[i]); //Kids mode 2nd playername
          for(int i=46;i<=50;i++)   s3=s3+char(EEPROM[i]); //Kids mode 3rd playername
          for(int i=51;i<=55;i++)   s4=s4+char(EEPROM[i]); //Kids mode 4th playername
          s1+=" :-"+String(EEPROM[4]);//Kids mode 1st player score
          s2+=" :-"+String(EEPROM[5]);//Kids mode 2nd player score
          s3+=" :-"+String(EEPROM[6]);//Kids mode 3rd player score
          s4+=" :-"+String(EEPROM[7]);//Kids mode 4th player score
        }
        else if( modeselected == 3) //strings will be modified according to the leaderboard(here Adults mode)
        {
          for(int i=56;i<=60;i++)   s1=s1+char(EEPROM[i]); //Adults mode 1st playername
          for(int i=61;i<=65;i++)   s2=s2+char(EEPROM[i]); //Adults mode 2nd playername
          for(int i=66;i<=70;i++)   s3=s3+char(EEPROM[i]);//Adults mode 3rd playername
          for(int i=71;i<=75;i++)   s4=s4+char(EEPROM[i]); //Adults mode 4th playername
          s1+=" :-"+String(EEPROM[8]);//Adults mode 1st playerscore
          s2+=" :-"+String(EEPROM[9]);//Adults mode 2nd playerscore
          s3+=" :-"+String(EEPROM[10]);//Adults mode 3rd playerscore
          s4+=" :-"+String(EEPROM[11]); //Adults mode 4th playerscore
        }
        else if( modeselected == 4) //strings will be modified according to the leaderboard(here aged mode)
        {
          for(int i=76;i<=80;i++)   s1=s1+char(EEPROM[i]); //Aged mode 1st playername
          for(int i=81;i<=85;i++)   s2=s2+char(EEPROM[i]); //Aged mode 2nd playername
          for(int i=86;i<=90;i++)   s3=s3+char(EEPROM[i]); //Aged mode 3rd playername
          for(int i=91;i<=95;i++)   s4=s4+char(EEPROM[i]); //Aged mode 4th playername
          s1+=" :-"+String(EEPROM[12]);//Aged mode 1st playerscore
          s2+=" :-"+String(EEPROM[13]);//Aged mode 2nd playerscore
          s3+=" :-"+String(EEPROM[14]);//Aged mode 3rd playerscore
          s4+=" :-"+String(EEPROM[15]);//Aged mode 4th playerscore
        }

        // after merging the scores and player names into the strings
        // we are going to print those strings in a lcd
        // Printing will scroll in a vertical manner 
        // after 5 times the scrolling stops and the function ends


        // But in the mean while if any other player is ready to play then printing stops and 
        // program flow goes to starting point
        
        String s5 = "Mode : "; // this string is used to display the selected mode 
        if(modeselected==1) s5+="Athletes";
        else if(modeselected==2) s5+="Kids";
        else if(modeselected==3) s5+="Adults";
        else if(modeselected==4) s5+="Aged";
        
        lcd.clear();
        modeselected=0; // overriding the modeselected value to 0 to know the player is ready for next game
        for(int i=0;i<3;i++) //loop to print the names on lcd for three cycles
        { //It scrolls the lcd in vertical manner for five times
          // in the mean time if any player is ready to write for next game then this loop breaks and goes back to loop function
          
          lcd.setCursor(0,1);
          lcd.print(s5);
          delay(500);
          lcd.setCursor(0,0);
          lcd.print(s5);
          lcd.setCursor(0,1);
          lcd.print(s1);
          delay(500);
          lcd.setCursor(0,0);
          lcd.print(s1);
          lcd.setCursor(0,1);
          lcd.print(s2);
          delay(500);
          lcd.setCursor(0,0);
          lcd.print(s2);
          lcd.setCursor(0,1);
          lcd.print(s3);
          delay(500);
          lcd.setCursor(0,0);
          lcd.print(s3);
          lcd.setCursor(0,1);
          lcd.print(s4);
          delay(500);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print(s4);
          lcd.clear();
          delay(1000);
        }
}

void athletes() //game module for athletes mode
{
  int delay_val[5]={0,700,650,600,580};  // delay values for different levels in this mode
                                          // neglect the zero index in this array
  int thresold_score[4]={0,8,17,26}; // thresold scores for entering into next level
                                      // Neglect the 0th index
  for(int level=1;level<=4;level++) //entire 4 levels will run
  {
      for(int leds_glown=0;leds_glown<10;leds_glown++)
      {
        random_value = random(1,5); //generating random value to make led glow or to run the pattern
        if(random_value==1)
        {
          digitalWrite(button1,HIGH);//led glows
          delay(delay_val[level]); //delay whose value depends on level
          digitalWrite(button1,LOW); //led off
          delay(200);
        }
        else if(random_value==2)
        {
          digitalWrite(button2,HIGH); //led glow
          delay(delay_val[level]); //delay whose value depends on level
          digitalWrite(button2,LOW); //led off
          delay(200);
        }
        else if(random_value==3)
        {
          digitalWrite(button3,HIGH); //led glow
          delay(delay_val[level]); //delay whose value depends on level
          digitalWrite(button3,LOW); //led off
          delay(200);
        }
        else if(random_value==4)
        {
          digitalWrite(button4,HIGH); //led glow
          delay(delay_val[level]); //delay whose value depends on level
          digitalWrite(button4,LOW); //led off
          delay(200);
        }
      }
      if(level==4) break;
      if(score<thresold_score[level])
      {
        return;   // game ends (player not allowded into next level)
      }
      level_change(); // this funtion is used to indicate the change of the level
                      // It makes all leds HIGH 
  }
}

void kids() //game module for kids mode
{
  int delay_val[5]={0,750,650,600,580}; // delay values for different levels in this mode
                                        // neglect the zero index in this array
  int thresold_score[4]={0,7,16,25}; // thresold scores for entering into next level
                                      // Neglect the 0th index
  for(int level=1;level<=4;level++) //running four levels
  {
      for(int leds_glown=0;leds_glown<10;leds_glown++) //generation of patten
      {
        random_value = random(1,5); //random value to make led glow
        if(random_value==1)
        {
          digitalWrite(button1,HIGH); //makes led on
          delay(delay_val[level]); //acording to level delay is given
          digitalWrite(button1,LOW); //led off
          delay(200);
        }
        else if(random_value==2)
        {
          digitalWrite(button2,HIGH);//makes led on
          delay(delay_val[level]); //acording to level delay is given
          digitalWrite(button2,LOW); //led off
          delay(200);
        }
        else if(random_value==3)
        {
          digitalWrite(button3,HIGH);//makes led on
          delay(delay_val[level]); //acording to level delay is given
          digitalWrite(button3,LOW); //led off
          delay(200);
        }
        else if(random_value==4)
        {
          digitalWrite(button4,HIGH);//makes led on
          delay(delay_val[level]); //acording to level delay is given
          digitalWrite(button4,LOW); //led off
          delay(200);
        }
      }
      if(level==4) break;
      if(score<thresold_score[level])
      {
        return;// game ends (player not allowded into next level)
      }
      level_change(); // this funtion is used to indicate the change of the level
                      // It makes all leds HIGH 
  }
}

void adults() //game module for adults mode
{
  int delay_val[5]={0,850,800,750,720};// delay values for different levels in this mode
                                        // neglect the zero index in this array
  int thresold_score[4]={0,7,15,24};// thresold scores for entering into next level
                                        // Neglect the 0th index
  for(int level=1;level<=4;level++)
  {
      for(int leds_glown=0;leds_glown<10;leds_glown++)
      {
        random_value = random(1,5);
        if(random_value==1)
        {
          digitalWrite(button1,HIGH);//makes led on
          delay(delay_val[level]); //acording to level delay is given
          digitalWrite(button1,LOW);//led off
          delay(200);
        }
        else if(random_value==2)
        {
          digitalWrite(button2,HIGH);//makes led on
          delay(delay_val[level]); //acording to level delay is given
          digitalWrite(button2,LOW);//led off
          delay(200);
        }
        else if(random_value==3)
        {
          digitalWrite(button3,HIGH);//makes led on
          delay(delay_val[level]);//acording to level delay is given
          digitalWrite(button3,LOW);//led off
          delay(200);
        }
        else if(random_value==4)
        {
          digitalWrite(button4,HIGH);//makes led on
          delay(delay_val[level]);//acording to level delay is given
          digitalWrite(button4,LOW);//led off
          delay(200);
        }
      }
      if(level == 4) break;
      if(score<thresold_score[level])
      {
        return;// game ends (player not allowded into next level)
      }
      level_change(); // this funtion is used to indicate the change of the level
                      // It makes all leds HIGH 
  }
}
void aged()  //game module for aged mode
{
  int delay_val[5]={0,900,850,800,770};// delay values for different levels in this mode
                                        // neglect the zero index in this array
  int thresold_score[4]={0,6,15,24}; // thresold scores for entering into next level
                                      //Neglect the 0th index
  for(int level=1;level<=4;level++)
  {
      for(int leds_glown=0;leds_glown<10;leds_glown++)
      {
        random_value = random(1,5);
        if(random_value==1)
        {
          digitalWrite(button1,HIGH);//makes led on
          delay(delay_val[level]);//acording to level delay is given
          digitalWrite(button1,LOW);//led off
          delay(200);
        }
        else if(random_value==2)
        {
          digitalWrite(button2,HIGH);//makes led on
          delay(delay_val[level]);//acording to level delay is given
          digitalWrite(button2,LOW);//led off
          delay(200);
        }
        else if(random_value==3)
        {
          digitalWrite(button3,HIGH);//makes led on
          delay(delay_val[level]);//acording to level delay is given
          digitalWrite(button3,LOW);//led off
          delay(200);
        }
        else if(random_value==4)
        {
          digitalWrite(button4,HIGH);//makes led on
          delay(delay_val[level]);//acording to level delay is given
          digitalWrite(button4,LOW);//led off
          delay(200);
        }
      }
      if(level==4)break;
      if(score<thresold_score[level])
      {
        return; // game ends (player not allowded into next level)
      }
      level_change(); // this funtion is used to indicate the change of the level
                      // It makes all leds HIGH
  }
}


/*
 * Level_change funtion is used to indicate the level change to the player
 * by making all led's HIGH for a second and then making low
 */
void level_change()
{
  noInterrupts(); // deactivating interrupts
  int i=0;
  while(i<3){
  digitalWrite(button1,HIGH);
  digitalWrite(button2,HIGH);
  digitalWrite(button3,HIGH);
  digitalWrite(button4,HIGH);
  delay(800);
  digitalWrite(button1,LOW);
  digitalWrite(button2,LOW);
  digitalWrite(button3,LOW);
  digitalWrite(button4,LOW);
  i+=1;
  }
  interrupts(); //activating interrupts
}


/*
 * Point_count funtion is an ISR
 * It is used to increment the score when the interrupt occur
 * i.e. when player presses correct button related to led
 * First it makes respective led LOW then increments the count
 */
void point_count()              //Couting score when interrupt encounters
{
  if(random_value==1)
  {
    digitalWrite(button1,LOW);
  }
  else if(random_value==2)
  {
    digitalWrite(button2,LOW);
  }
  else if(random_value==3)
  {
    digitalWrite(button3,LOW);
  }
  else if(random_value==4)
  {
    digitalWrite(button4,LOW);
  }
  score=score+1;
}

/*
 * EEPROM_init funtion is used to initilize the EEPROM with default value
 * Scores with 0 
 * Names with -
 */
void EEPROM_init()
{   // Here i varable used to access the EEPROM memory using indexing method
  for(int i=0;i<=15;i++) EEPROM[i]=0;
  for(int i=16;i<=95;i++) EEPROM[i]='-';
}
