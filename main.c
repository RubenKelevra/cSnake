//---------------------------------------------------------------------------
#include "main.h"
//Author: Ruben Kelevra
//Mail: cyrond at gmail dot com
//Licence: AGPL 3.0
//---------------------------------------------------------------------------


//static chars
  //levelsigns
    char cWall = 219;
    char cLevelSpace = ' ';
    char cApple = 149;

/* DO NOT CHANGE THE FOLLOWING VALUES */

    char cBlankWormPosition = 255;
    //signs used for drawing the worm
      char cWormVertical = 186;
      char cWormHorizontal = 205;
      char cWormLeftUp = 188;
      char cWormLeftDown = 187;
      char cWormRightUp = 200;
      char cWormRightDown = 201;
    //signs for the menu
      char cSelectL_Indicator = 26;
      char cSelectR_Indicator = 27;
    //worm start position
      char cStartX = 40;
      char cStartY = 23;
      char cFinishX = 40;
      char cFinishY = 0;

/*         END          */

void gotoXY(short int x, short int y) {
  printf("\033[%d;%dH", y+1, x+1);
}

void SetFG_Color(int ForgC) //This will set the forground color for printing in a console window.
{   //thanks to www.dreamincode.net
  WORD wColor;
  //We will need this handle to get the current background attribute
  HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  //We use csbi for the wAttributes word.
  if(GetConsoleScreenBufferInfo(hStdOut, &csbi))
  {
    //Mask out all but the background attribute, and add in the forgournd color
    wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
    SetConsoleTextAttribute(hStdOut, wColor);
  }
  return;
}

void SetColor(int ForgC, int BackC) //This will set the forground and background color for printing in a console window.
{   //thanks to www.dreamincode.net
  WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);;
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wColor);
  return;
}

int drawApples(char cField[24][80], unsigned short int usiAmount) //create apples
{
  char cX;
  char cY;
  char cRetrys = 10;  //retrys to set an apple, else asume a filled level
  int i;
  char c2;
  char cTrys;
  unsigned short int usiPlacedApples = 0;
  srand ( time(NULL) );
  for(i = 0;i < usiAmount;i++)
  {
    for(c2 = 0;c2 < cRetrys;c2++)
    {
      cY = rand() % 22 + 1;
      cX = rand() % 78 + 1;
      if( !( cField[cY][cX] == cWall || cField[cY][cX] == cApple || cField[cY][cX] == cWormVertical || cField[cY][cX] == cWormHorizontal || cField[cY][cX] == cWormLeftUp || cField[cY][cX] == cWormLeftDown || cField[cY][cX] == cWormRightUp || cField[cY][cX] == cWormRightDown) )
      {
        cField[cY][cX] = cApple;
        usiPlacedApples++;
        cTrys = 0;
        break;
      }
      else
      {
        cTrys++;
        continue;
      }
    }
    if(cTrys > 0)
      break;
  }
  return usiPlacedApples;
}

int main()
{
  //console width 80 chars
  //console high 25 lines

  //playground high 24 lines

  //fields content
    char cFieldsContent[24][80]; //bottom line used for some other information

  //snake
    char cSnakePosition[2500][2]; // maximal 2500 moves | Y, X
    int iActualSnakePosition;
    int iSnakeLength;
    char bAppleEaten;
    char cSnakeDirection; //0=to top; 1=to right; 2=to bottom;3=to left
    char cNewSnakeDirection;
    char cSnakeLives;

  //app vars
    //preselected vars
      unsigned int uiAppleRedrawLimit_ms = 90000; //playable milliseconds after this time new apples are drawn to the level
    //changeable game vars
      unsigned short int usiDrawnApples;
      unsigned int uiGameLength_ms = uiAppleRedrawLimit_ms;  //ms counter for played time count to 0
      unsigned short int usiGameSpeed = 200; //default gamespeed in milliseconds
      unsigned short int usiEatenApples; //counter of the eaten apples for each level
      char cNextField; //the head-field of the worm which has to be drawn
      char cLastField; //the second field from the top of the worm which has to be drawn
      char cPressedKey; //hold the last pressed key
      char bGameDataChanged = 0; //the defaultmap has been altered?
      char bError = 0; //an error has occoured?
      int iModus = 6; //modus operandi - intro = 6 | menu start = 4  | 7=testing
      char bAppRunning = 1; //should mainprogram run?
      char sLogo[8][78] = {"\n          .-.                .\n", "         (   )               |\n", "          `-.   .--.   .-.   |.-.   .-.\n", "         (   )  |  |  (   )  |-.'  (.-'    Ver. 0.5b\n", "          `-'   '  `-  `-'`- '  `-  `--'.                       .\n", "                      .- .-. .-. .- .-. |  .-,   . . .-, .-. .- . .-. .-.\n", "                      `- `-' ' ' -' `-' '- `'-    `  `'- '   -' ' `-' ' '\n\n", "                          written by Ruben Wisniewski\n\n\n\n\n"};
    //changeable vars for settings
      unsigned short int usiApples = 35; //defaultnumber of apples in the level

    //keys
      char cArrowLeft = 75;
      char cArrowRight = 77;
      char cArrowUp = 72;
      char cArrowDown = 80;
      char cEnter = 13;
      char cF2 = '<';
    //menu
      char sMenu[3][15] = {"Start New Game", "Settings", "Quit"}; //   all menu entries
      short int siMenu[3] = {29, 32, 34}; // 80 - len(menu-text) / 2 - 3   >> the space from the left in front of the left indicator
      char bMainMenu = 0;
      char cMenuSelected = 0;
    //generetic counter
      int i;
      int i2;
      int i3;
      char c1;

  while(bAppRunning) //mainloop
  {
    SetColor(15, 0);
    clear();
    switch (iModus)
    {
      case 0: //load level and run game
        bGameDataChanged = 1; //now changing default data
        usiDrawnApples = drawApples(cFieldsContent, usiApples); //create randomized apples
        cPressedKey = 0;

        //draw the map the first time with startup screen
        SetColor(15, 8);
        for(i = 0;i <= 23;i++) //lines
        {
          for(i2 = 0;i2 < 80;i2++) //chars
          {
            if( (i >= 7 && i <= 15) && (i2 >= 18 && i2 <= 61) ) //catch the standardoutput in this area
            {
              if(i == 7 && i2 == 18)
              {
                SetColor(15, 0);
                gotoXY(26, 8);
                printf("Use the arrow keys for movation");
                gotoXY(20, 10);
                printf("Press F2 at any time to start a new game");
                gotoXY(33, 12);
                printf("Hit enter to exit");
                gotoXY(30, 14);
                printf("Press any key to start");
                SetColor(15, 8);
              }
              continue;
            }
            if(i >= 7 && i <= 15 && i2 == 62)
              gotoXY(i2, i);
            if(cFieldsContent[i][i2] == cApple)
              SetFG_Color(12);
            if(cFieldsContent[i][i2] == cWormVertical || cFieldsContent[i][i2] == cWormHorizontal || cFieldsContent[i][i2] == cWormLeftUp  || cFieldsContent[i][i2] == cWormLeftDown || cFieldsContent[i][i2] == cWormRightUp || cFieldsContent[i][i2] == cWormRightDown)
              SetFG_Color(10);
            printf("%c", cFieldsContent[i][i2]);
            SetFG_Color(15);
          }
        }
        SetColor(15,0);
        printf("Remaining Playtime:%4d sec | Apples:%4d | Level:   1 | Lives:%4d", uiGameLength_ms/1000, usiEatenApples, cSnakeLives);
        getch();

        //draw over the black box
        SetColor(15, 8);
        for(i = 7;i <= 15;i++) //lines
        {
          for(i2 = 18;i2 < 62;i2++) //chars
          {
            gotoXY(i2, i);
            if(cFieldsContent[i][i2] == cApple)
              SetFG_Color(12);
            if(cFieldsContent[i][i2] == cWormVertical || cFieldsContent[i][i2] == cWormHorizontal || cFieldsContent[i][i2] == cWormLeftUp  || cFieldsContent[i][i2] == cWormLeftDown || cFieldsContent[i][i2] == cWormRightUp || cFieldsContent[i][i2] == cWormRightDown)
              SetFG_Color(10);
            printf("%c", cFieldsContent[i][i2]);
            SetFG_Color(15);
          }
        }
        fflush(stdin);
        while(cSnakeLives)
        {
          Sleep(usiGameSpeed);
          //userinput
          if(kbhit() != 0)
          {
            cPressedKey = (char)getch(); //dummy get
            cPressedKey = (char)getch();
          }

          if(cPressedKey == cArrowUp && cSnakeDirection != 2)
            cNewSnakeDirection = 0;
          else {
            if(cPressedKey == cArrowRight && cSnakeDirection != 3)
              cNewSnakeDirection = 1;
            else {
              if(cPressedKey == cArrowDown && cSnakeDirection != 0)
                cNewSnakeDirection = 2;
              else {
                if(cPressedKey == cArrowLeft && cSnakeDirection != 1)
                  cNewSnakeDirection = 3;
                else {
                  if(cPressedKey == cEnter)
                  {
                    cSnakeLives = 0; //FIXME: "Would you like to?"
                    continue;
                  } else {
                    if(cPressedKey == cF2) //FIXME: nicer way to restart
                    {
                      bGameDataChanged = 0;
                      iModus = 5;
                      cSnakeLives = 0;
                      continue;
                    }
                    else
                      cNewSnakeDirection = cSnakeDirection;
                  }
                }
              }
            }
          }
          switch (cNewSnakeDirection)
          {
            case 0: //to the top
              cNextField = cFieldsContent[cSnakePosition[iActualSnakePosition][0]-1][cSnakePosition[iActualSnakePosition][1]]; //get char of the next field
              if( !(cNextField == cLevelSpace || cNextField == cApple))
              {
                cSnakeLives = 0; //FIXME =-1 is correct
                //FIXME: regenerate level, worm, apples
              }
              else
              {
                if(cNextField == cApple) //set bool to true if an apple has been eaten
                  bAppleEaten = 1;
                switch(cSnakeDirection)
                {
                  case 0: //from the bottom
                    cLastField = cNextField = cWormVertical;
                    break;
                  case 1: //from the left
                    cLastField = cWormLeftUp;
                    cNextField = cWormVertical;
                    break;
                  case 3: //from the right
                    cLastField = cWormRightUp;
                    cNextField = cWormVertical;
                    break;
                  default:
                    SetColor(15, 0);
                    clear();
                    printf("FATAL ERROR: unexpected direction value for the worm in decition for movation from the bottom, exiting now\n\n");
                    bAppRunning = 0;
                    bError = 1;
                    goto EndOfApp;
                }
                cFieldsContent[cSnakePosition[iActualSnakePosition][0]-1][cSnakePosition[iActualSnakePosition][1]] = cNextField;
                cFieldsContent[cSnakePosition[iActualSnakePosition][0]][cSnakePosition[iActualSnakePosition][1]] = cLastField;
                cSnakePosition[iActualSnakePosition+1][0] = cSnakePosition[iActualSnakePosition][0]-1;
                cSnakePosition[iActualSnakePosition+1][1] = cSnakePosition[iActualSnakePosition][1];
                if(bAppleEaten == 1)
                {
                  iSnakeLength++;
                  usiEatenApples++;
                }
                else
                  cFieldsContent[cSnakePosition[iActualSnakePosition-iSnakeLength-1][0]][cSnakePosition[iActualSnakePosition-iSnakeLength-1][1]] = cLevelSpace;
              }
              break;
            case 1: //to the right
              cNextField = cFieldsContent[cSnakePosition[iActualSnakePosition][0]][cSnakePosition[iActualSnakePosition][1]+1]; //get char of the next field
              if( !(cNextField == cLevelSpace || cNextField == cApple))
              {
                cSnakeLives = 0; //FIXME =-1 is correct
                //FIXME: regenerate level, worm, apples
              }
              else
              {
                if(cNextField == cApple) //set bool to true if an apple has been eaten
                  bAppleEaten = 1;
                switch(cSnakeDirection)
                {
                  case 0:  //from the bottom
                    cLastField = cWormRightDown;
                    cNextField = cWormHorizontal;
                    break;
                  case 1: //from the left
                    cLastField = cNextField = cWormHorizontal;
                    break;
                  case 2: //from the top
                    cLastField = cWormRightUp;
                    cNextField = cWormHorizontal;
                    break;
                  default:
                    SetColor(15, 0);
                    clear();
                    printf("FATAL ERROR: unexpected direction value for the worm in decition for movation from the left, exiting now\n\n");
                    bAppRunning = 0;
                    bError = 1;
                    goto EndOfApp;
                }
                cFieldsContent[cSnakePosition[iActualSnakePosition][0]][cSnakePosition[iActualSnakePosition][1]+1] = cNextField;
                cFieldsContent[cSnakePosition[iActualSnakePosition][0]][cSnakePosition[iActualSnakePosition][1]] = cLastField;
                cSnakePosition[iActualSnakePosition+1][0] = cSnakePosition[iActualSnakePosition][0];
                cSnakePosition[iActualSnakePosition+1][1] = cSnakePosition[iActualSnakePosition][1]+1;
                if(bAppleEaten == 1)
                {
                  iSnakeLength++;
                  usiEatenApples++;
                }
                else
                  cFieldsContent[cSnakePosition[iActualSnakePosition-iSnakeLength-1][0]][cSnakePosition[iActualSnakePosition-iSnakeLength-1][1]] = cLevelSpace;
              }
              break;
            case 2: //to the bottom
              cNextField = cFieldsContent[cSnakePosition[iActualSnakePosition][0]+1][cSnakePosition[iActualSnakePosition][1]]; //get char of the next field
              if( !(cNextField == cLevelSpace || cNextField == cApple))
              {
                cSnakeLives = 0; //FIXME =-1 is correct
                //FIXME: regenerate level, worm, apples
              }
              else
              {
                if(cNextField == cApple) //set bool to true if an apple has been eaten
                  bAppleEaten = 1;
                switch(cSnakeDirection)
                {
                  case 1: //from the left
                    cLastField = cWormLeftDown;
                    cNextField = cWormVertical;
                    break;
                  case 2: //from the top
                    cLastField = cNextField = cWormVertical;
                    break;
                  case 3:  //from the right
                    cLastField = cWormRightDown;
                    cNextField = cWormVertical;
                    break;
                  default:
                    SetColor(15, 0);
                    clear();
                    printf("FATAL ERROR: unexpected direction value for the worm in decition for movation from the top, exiting now\n\n");
                    bAppRunning = 0;
                    bError = 1;
                    goto EndOfApp;
                }
                cFieldsContent[cSnakePosition[iActualSnakePosition][0]+1][cSnakePosition[iActualSnakePosition][1]] = cNextField;
                cFieldsContent[cSnakePosition[iActualSnakePosition][0]][cSnakePosition[iActualSnakePosition][1]] = cLastField;
                cSnakePosition[iActualSnakePosition+1][0] = cSnakePosition[iActualSnakePosition][0]+1;
                cSnakePosition[iActualSnakePosition+1][1] = cSnakePosition[iActualSnakePosition][1];
                if(bAppleEaten == 1)
                {
                  iSnakeLength++;
                  usiEatenApples++;
                }
                else
                  cFieldsContent[cSnakePosition[iActualSnakePosition-iSnakeLength-1][0]][cSnakePosition[iActualSnakePosition-iSnakeLength-1][1]] = cLevelSpace;
              }
              break;
            case 3: //to the left
              cNextField = cFieldsContent[cSnakePosition[iActualSnakePosition][0]][cSnakePosition[iActualSnakePosition][1]-1]; //get char of the next field
              if( !(cNextField == cLevelSpace || cNextField == cApple))
              {
                cSnakeLives = 0; //FIXME =-1 is correct
                //FIXME: regenerate level, worm, apples
              }
              else
              {
                if(cNextField == cApple) //set bool to true if an apple has been eaten
                  bAppleEaten = 1;
                switch(cSnakeDirection)
                {
                  case 0: //from the bottom
                    cLastField = cWormLeftDown;
                    cNextField = cWormHorizontal;
                    break;
                  case 2: //from the top
                    cLastField = cWormLeftUp;
                    cNextField = cWormHorizontal;
                    break;
                  case 3: //from the right
                    cNextField = cLastField = cWormHorizontal;
                    break;
                  default:
                    SetColor(15, 0);
                    clear();
                    printf("FATAL ERROR: unexpected direction value for the worm in decition for movation from the right, exiting now\n\n");
                    bAppRunning = 0;
                    bError = 1;
                    goto EndOfApp;
                }
                cFieldsContent[cSnakePosition[iActualSnakePosition][0]][cSnakePosition[iActualSnakePosition][1]-1] = cNextField;
                cFieldsContent[cSnakePosition[iActualSnakePosition][0]][cSnakePosition[iActualSnakePosition][1]] = cLastField;
                cSnakePosition[iActualSnakePosition+1][0] = cSnakePosition[iActualSnakePosition][0];
                cSnakePosition[iActualSnakePosition+1][1] = cSnakePosition[iActualSnakePosition][1]-1;
                if(bAppleEaten == 1)
                {
                  iSnakeLength++;
                  usiEatenApples++;
                } else
                  cFieldsContent[cSnakePosition[iActualSnakePosition-iSnakeLength-1][0]][cSnakePosition[iActualSnakePosition-iSnakeLength-1][1]] = cLevelSpace;
              }
              break;
            default:
              SetColor(15, 0);
              clear();
              printf("FATAL ERROR: unexpected direction value for the worm, exiting now\n\n");
              bAppRunning = 0;
              bError = 1;
              goto EndOfApp;
          }
          cSnakeDirection = cNewSnakeDirection;

          /*Printing*/

          //if the start would be a space a wall has to be set
          if(cFieldsContent[cStartY][cStartX] == cLevelSpace) //if the start would be a levelspace replacing with a wall
            cFieldsContent[cStartY][cStartX] = cWall;

          //printing the space behind the worm
          if( (cFieldsContent[cSnakePosition[iActualSnakePosition-iSnakeLength-1][0]][cSnakePosition[iActualSnakePosition-iSnakeLength-1][1]] == cLevelSpace || cFieldsContent[cSnakePosition[iActualSnakePosition-iSnakeLength-1][0]][cSnakePosition[iActualSnakePosition-iSnakeLength-1][1]] == cWall) && iActualSnakePosition > 0)
          {
            SetColor(15, 8);
            gotoXY(cSnakePosition[iActualSnakePosition-iSnakeLength-1][1], cSnakePosition[iActualSnakePosition-iSnakeLength-1][0]);
            printf("%c", cFieldsContent[cSnakePosition[iActualSnakePosition-iSnakeLength-1][0]][cSnakePosition[iActualSnakePosition-iSnakeLength-1][1]]);
          }

          //saving new position and time
          iActualSnakePosition++;
          if(uiGameLength_ms >= usiGameSpeed+1)
            uiGameLength_ms -= (usiGameSpeed+1); // +1 for the drawing time of the changed fields
          else
          {
            usiDrawnApples += drawApples(cFieldsContent, usiApples); //create randomized apples
            SetFG_Color(12); //set to red
            for(i = 0;i <= 23;i++) //lines
            {
              for(i2 = 0;i2 < 80;i2++) //chars
              {
                if(cFieldsContent[i][i2] == cApple)  //draw every apple in the array to the map
                {
                  gotoXY(i2, i);
                  printf("%c", cFieldsContent[i][i2]);
                }
              }
            }
            if(cFieldsContent[cFinishY][cFinishX] == cLevelSpace) //if finish is already opend, close it again
            {
              cFieldsContent[cFinishY][cFinishX] = cWall;
              SetColor(15, 8);
              gotoXY(cFinishX, cFinishY);
              printf("%c", cWall);
            }
            SetFG_Color(15); //set back to white
            uiGameLength_ms = uiAppleRedrawLimit_ms;
          }

          if(usiEatenApples == usiDrawnApples) //all apples has been eaten
          {
            if(cSnakePosition[iActualSnakePosition][1] == cFinishX && cSnakePosition[iActualSnakePosition][0] == cFinishY)
            {
              SetColor(15, 0);
              clear();
              printf("You have won the game!");
              getch();
              iModus = 4;
              break;
            }
            if(cFieldsContent[cFinishY][cFinishX] != cLevelSpace)
            {
              cFieldsContent[cFinishY][cFinishX] = cLevelSpace;
              SetColor(15, 8);
              gotoXY(cFinishX, cFinishY);
              printf("%c", cLevelSpace);
            }
          }

          //printing the worm
          SetColor(10, 8);
          //new field
          gotoXY(cSnakePosition[iActualSnakePosition][1], cSnakePosition[iActualSnakePosition][0]);
          printf("%c", cFieldsContent[cSnakePosition[iActualSnakePosition][0]][cSnakePosition[iActualSnakePosition][1]]);
          //second field
          gotoXY(cSnakePosition[iActualSnakePosition-1][1], cSnakePosition[iActualSnakePosition-1][0]);
          printf("%c", cFieldsContent[cSnakePosition[iActualSnakePosition-1][0]][cSnakePosition[iActualSnakePosition-1][1]]);

          SetColor(15,0);
          gotoXY(19, 24);
          printf("%4d", uiGameLength_ms/1000);
          gotoXY(37, 24);
          printf("%4d", usiEatenApples);
          gotoXY(63, 24);
          printf("%4d", cSnakeLives);

          /*This move has ended, setting for next move*/
          bAppleEaten = 0;
        }
        if(iModus != 5 && iModus != 4) //game is over, if not F2 (iModus 5) was pressed or the player has won the game (iModus 4) for a new game, go to the game lost screen
          iModus = 1;
        break;
      case 1: //game over //FIXME: A nicer ending
        printf("you've lost the game, your worm has died, press any key to return to the main screen");
        getch();
        iModus = 5; //cleaning and returning to main screen
        break;
      case 2: //settings //FIXME: A Setting Screen
        printf("not yet implemented");
        getch();
        iModus = 4;
        break;
      case 3: //select level
        printf("No extra level(s) could be found, starting with default level\n\nPress any key to start");
        getch();
        iModus = 0;
        break;
      case 4: //main menu
        bMainMenu = 1;

        for(i=0;i < 8;i++)
        {
          printf("%s", sLogo[i]);
        }
        for(i = 0;i < 3;i++) //Menu size
        {
          i3 = siMenu[i];
          for(i2 = 0;i2 < i3+3;i2++) { printf(" "); }

          printf("%s\n\n", sMenu[i]);
        }

        while(bMainMenu)
        {
          i2 = 14; //startline (first menu entry)
          for(i = 0;i < 3;i++)
          {
            if(cMenuSelected == i) //Show or hide indicators on menu points
            {
              gotoXY(siMenu[i], i2);
              printf("%c", cSelectL_Indicator);
              gotoXY(siMenu[i]+5+strlen(sMenu[i]), i2);
              printf("%c", cSelectR_Indicator);
            }
            else
            {
              gotoXY(siMenu[i], i2);
              printf(" ");
              gotoXY(siMenu[i]+5+strlen(sMenu[i]), i2);
              printf(" ");
            }
            i2 += 2;  //menu entries have one space between them, so we need to add 2
          }

          i2 = getch();
          if (i2 == cArrowUp && cMenuSelected > 0) //if this key was pressed and the selection is not on the top
            cMenuSelected--; //move it one up
          if (i2 == cArrowDown && cMenuSelected < 2) //if this key was pressed and the selection is not on the button
            cMenuSelected++; //move it one down
          if (i2 == cEnter)
          {
            switch(cMenuSelected)
            {
              case 0: //game
                iModus = 5;
                bMainMenu = 0;
                break;
              case 1: //settings
                iModus = 2;
                bMainMenu = 0;
                break;
              case 2: //quit
                bAppRunning = 0;
                bMainMenu = 0;
                break;
              default:
                SetColor(15, 0);
                clear();
                printf("FATAL ERROR: unexpected mainmenu-selection, exiting now\n\n");
                bAppRunning = 0;
                bError = 1;
                goto EndOfApp;
            }
          }
        }
        break;
      case 5: // initialise / cleaning
        if(bGameDataChanged == 0)
          printf("initialising...");
        else
          printf("unloading ressources...");

        /* create default worm */
        cSnakePosition[0][0] = cStartY; //worm start position y
        cSnakePosition[0][1] = cStartX; //worm start position x
        cSnakeDirection = 0;
        iActualSnakePosition = 0;
        iSnakeLength = 0; //0 equals one field
        uiGameLength_ms = uiAppleRedrawLimit_ms;
        usiEatenApples = 0;

        cSnakeLives = 1; //default: Three lives | FIXME: one is set for testing

        for(i = 1;i < 2500;i++) //writing out of range values to empty fields
        {
          cSnakePosition[i][0] = cSnakePosition[i][1] = cBlankWormPosition;
        }

        /* create default level */
        //create empty level
        for(i = 0;i <= 23;i++) //moving thru all level lines
        {
          if(i == 0 || i == 23) //on top and buttomline of the level
          {
            for(i2 = 0;i2 <= 79;i2++) //should be drawn a wall
            {
              if( !(i2 == cStartX && i == cStartY) ) // drawing a wall
                cFieldsContent[i][i2] = cWall;
              else //drawing the entrance
                cFieldsContent[i][i2] = cWormVertical;
            }
          } else
          {
            cFieldsContent[i][0] = cWall; //draw left wall

            for(i2 = 1;i2 < 79; i2++) //draw center levelspace
            {
              cFieldsContent[i][i2] = cLevelSpace;
            }
            cFieldsContent[i][79] = cWall; //draw right wall
          }
        }

        //default level creation done

        if(bGameDataChanged == 0) //true returns to menu | false starts game
          iModus = 3;
        else
          iModus = 4;

        bGameDataChanged = 0;
        break;
      case 6: //intro
        for(i=0;i < 8;i++)
        {
          printf("%s", sLogo[i]);
          Sleep(330);
        }
        iModus = 4;
        break;
      case 7: //testing
         bAppRunning = 0;
         break;
      default:
        SetColor(15, 0);
        clear();
        printf("FATAL ERROR: unexpected app-mode value, exiting now\n\n");
        bAppRunning = 0;
        bError = 1;
        goto EndOfApp;
    }
  }
  EndOfApp:
  if(bError == 1)
    printf("\nwe're sorry for any inconvenience\n\n\npress any key to exit");
  else
  {
    SetColor(15, 0);
    clear();
    printf("press any key to exit"); //FIXME: A better ending
  }
  getch();
  return 0;
}
