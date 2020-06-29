#include "mylib.h"
#include "led-matrix.h"
#include "graphics.h"
#include <vector>
#include <list>
#include "libevdev.h"


using rgb_matrix::Font;
using rgb_matrix::Canvas;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Color;

using namespace std;

int get_selected_button(vector<Button> buttons){
	for(int i = 0; i < buttons.size(); i++){
		if(buttons[i].is_selected){
				return i;
		}
	}
	
	return -1;
	
}

int random(int min, int max) //range : [min, max)
{
   static bool first = true;
   if (first) 
   {  
      srand( time(NULL) ); //seeding for the first time only!
      first = false;
   }

   return min + rand() % (( max + 1 ) - min);
}

void draw_line(Canvas *c, int row0, int col0, int row1, int col1, const Color &color) {
  rgb_matrix::DrawLine(c,col0,row0, col1,  row1, color);
}

void ResetCanvas(Canvas *canvas, int n_rows, int n_cols, Color bg_color) { // swpas x and y so indexing is matrix 
  for(int i = 0; i<n_rows; i++){
    for(int j = 0; j<n_cols; j++){
      SetPixel(canvas, i, j, bg_color);
  }
}
}

void SetPixel(Canvas *canvas, int row, int col, uint8_t red, uint8_t green, uint8_t blue) { // swpas x and y so indexing is matrix 
  
  canvas->SetPixel(col, row, red, green, blue);
}

void SetPixel(Canvas *canvas, int row, int col, Color color) { // swpas x and y so indexing is matrix 
  
  SetPixel(canvas, row, col, color.r, color.g, color.b);
}

void SetPixel(Canvas *canvas, Point point, Color color) { // swpas x and y so indexing is matrix 
  
  SetPixel(canvas, point.row, point.col, color.r, color.g, color.b);
}

void draw_text(Canvas *canvas, Font &font, int row, int col, Color color, Color bg_color, char *text, int letter_spacing){

  rgb_matrix::DrawText(canvas, font, col, row,
                           color, &bg_color, text,
                           letter_spacing);
  
}

void DrawRect(Canvas *canvas, int row, int col, int height, int width, Color color) {
  
  for (int i = row; i<= row + height; i++){
    for(int j = col; j<=col +width; j++){
      if(i==row||j==col||i==row+height||j ==col+width){
        SetPixel(canvas, i, j, color);
      }
    }
  }
}

void SetPixels(Canvas *canvas, int LED_matrix[32][64], Color color, int n_rows = 32, int n_cols = 64) {
  
    for (int i = 0; i < n_rows; i++) { // for each row
      for (int j = 0; j < n_cols; j++) { // for each column
        
        if (LED_matrix[i][j] == 1){
          
          SetPixel(canvas, i, j, color);
          
        } else {
          SetPixel(canvas, i, j, 0,0,0);
        }
        
    }
  }
  
}


ControllerInput parse_ps4_input(struct input_event ev){
  
  string input_string = string(libevdev_event_code_get_name(ev.type, ev.code));
 
  
  int type;
  int value; 
  
  if (input_string.compare("ABS_HAT0X") == 0) { // left right dpad
    
    type = 'x';
    value = ev.value;
    
  
  } else if (input_string.compare("ABS_HAT0Y") == 0) { // up down dpad
    
    type = 'y';
    value = ev.value;
    
    //face buttons
  } else if (input_string.compare("BTN_SOUTH") == 0){ 
    type = 'S';
    value = ev.value;
    
    
  } else if (input_string.compare("BTN_EAST") == 0){
    type = 'E';
    value = ev.value;
    
  } else if (input_string.compare("BTN_WEST") == 0){
    type = 'W';
    value = ev.value;
    
  } else if (input_string.compare("BTN_NORTH") == 0){
    type = 'N';
    value = ev.value;
    
    // bumpers
  } else if (input_string.compare("BTN_TL") == 0){
    type = 'l';
    value = ev.value;
  } else if (input_string.compare("BTN_TR") == 0){
    type = 'r';
    value = ev.value;
  
  // triggers
  } else if (input_string.compare("BTN_TL2") == 0){
    type = 'L';
    value = ev.value;
  } else if (input_string.compare("BTN_TR2") == 0){
    type = 'R';
    value = ev.value;
    
  // start button
  } else if (input_string.compare("BTN_START") == 0){
    type = 'p';
    value = ev.value;
    
    // input we arent interested in 
  } else {
      type = -1;
      value = 0;
  }
  
  
  
  ControllerInput input{type, value};
  return input;
  
}


list<ControllerInput> get_inputs_from_ps4(struct libevdev *dev){
      struct input_event ev;
      int rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
      list<ControllerInput> inputs;
      
      while(rc == 0) { // get all events since last iteration of game loop 
        ControllerInput input = parse_ps4_input(ev);
        
        if(input.type != -1){
          inputs.push_back(input);
         }
         
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);  
      }
      
      
      return inputs;
}


void draw_buttons(Canvas *canvas, vector <Button> buttons, Font &font, Color selected_color, Color unselected_color) {
      Color bg_color(0, 0, 0);
      int letter_spacing = 0;
      for(int i = 0; i < buttons.size(); i++) { // put this is a function

      //draw_text(canvas, font, buttons[i].position.row + font.baseline(), buttons[i].position.col,
                             //color, bg_color, buttons[i].text,
                             //letter_spacing);
                            
      if(buttons[i].is_selected){
        // border
        //DrawRect(canvas, buttons[i].position.row -1, buttons[i].position.col -1, 6, 20, highlight_color);
        draw_text(canvas, font, buttons[i].position.row + font.baseline(),  buttons[i].position.col, selected_color, bg_color, buttons[i].text, letter_spacing);
      } else {
        //DrawRect(canvas, buttons[i].position.row -1, buttons[i].position.col -1, 6, 20, bg_color);
        draw_text(canvas, font,  buttons[i].position.row + font.baseline(), buttons[i].position.col, unselected_color, bg_color, buttons[i].text, letter_spacing);
      }
    }
}


void change_selected_button(vector<Button> &buttons, int dir, int &current_selected) {
  
  if(current_selected < buttons.size()-1 && dir == 1) { //down
    buttons[current_selected].is_selected = false;
    current_selected +=1;
    buttons[current_selected].is_selected = true;
  }
  else if(current_selected > 0 && dir == -1) { //up
    buttons[current_selected].is_selected = false;
    current_selected -=1;
    buttons[current_selected].is_selected = true;
  }

  
  
}

int get_direction_from_inputs(list <ControllerInput> inputs){
  
  int dir = -1;
  
  for(const auto &input: inputs){
    
      switch(input.type) {  // go from first input as unlikely to have multiple inputs perframes with no sleep

          case 'y':
            if(input.value == -1) {
              dir = 'u';
            }else if(input.value == 1){
              dir = 'd';
            }
            break;
            
          case 'x':
            if(input.value == -1) {
              dir = 'l';
            }else if(input.value == 1){
              dir = 'r';
            }
            break;
          default:
            break;
            
        }
      }
      
      
      
      return dir;
}

