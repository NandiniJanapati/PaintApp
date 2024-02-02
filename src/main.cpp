#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>


#define WINDOW_WIDTH 1200 //we can change the default window size later.
#define WINDOW_HEIGHT 800

float frameBuffer[WINDOW_HEIGHT][WINDOW_WIDTH][3];
bool mask[WINDOW_HEIGHT][WINDOW_WIDTH]; //bottom left is 0,0??
GLFWwindow *window;

// Color structure. Can be used to define the brush and background color.
struct color { float r, g, b; }; //r,g,b values range from 0 to 1

// A function clamping the input values to the lower and higher bounds
#define CLAMP(in, low, high) ((in) < (low) ? (low) : ((in) > (high) ? (high) : in))

color COLOR_SELECTED{ 1.0f, 1.0f, 1.0f }; //defalut is white
color BG_COLOR{ 0.0f, 0.0f, 0.0f }; //defalut is white
int BRUSH_SIZE = 1; //default set to 1 (covers 4 pixels)
bool SPRAYPAINT = false;
bool SQUAREBRUSH = true;

// Set a particular pixel ((x,y) coord) of the frameBuffer to the provided color
void SetFrameBufferPixel(int x, int y, struct color lc) //the pixel values range from 0 to (width - 1) and (height - 1)
{
	// Origin is upper left corner. (pos y axis is down, x axis is same)
	// Changes the origin from the lower-left corner to the upper-left corner
	y = WINDOW_HEIGHT - 1 - y;

	x = CLAMP(x, 0, WINDOW_WIDTH - 1);
	y = CLAMP(y, 0, WINDOW_HEIGHT - 1);

	frameBuffer[y][x][0] = lc.r;
	frameBuffer[y][x][1] = lc.g;
	frameBuffer[y][x][2] = lc.b;

}

void SetTouchedPixel(int x, int y)
{
	//y = WINDOW_HEIGHT - 1 - y;

	x = CLAMP(x, 0, WINDOW_WIDTH - 1);
	y = CLAMP(y, 0, WINDOW_HEIGHT - 1);

	mask[y][x] = true;
}

void ClearMask() {
	memset(mask, false, sizeof(bool) * WINDOW_WIDTH * WINDOW_HEIGHT);
}

void ClearFrameBuffer() //set frameBuffer to black
{
	memset(frameBuffer, 0.0f, sizeof(float) * WINDOW_WIDTH * WINDOW_HEIGHT * 3);
}

void ClearandSetBG() {
	for (int i = 0; i < WINDOW_HEIGHT; i++) {
		for (int j = 0; j < WINDOW_WIDTH; j++) {
			SetFrameBufferPixel(j, i, BG_COLOR);
		}
	}
	ClearMask();
}

void ChangeBGColor() {
	/*std::cout << "printing masked points:" << std::endl;*/
	for (int i = 0; i < WINDOW_WIDTH; ++i) {
		for (int j = 0; j < WINDOW_HEIGHT; j++) {
			
			if (mask[j][i] == false) {
				SetFrameBufferPixel(i, j, BG_COLOR);
			}
		}
	}
}

// Display frameBuffer on screen
void Display()
{	
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, frameBuffer);
}

void ChangeBrooshSize(bool inc) {
	BRUSH_SIZE =  (inc) ?  (BRUSH_SIZE * 2) : (BRUSH_SIZE / 2);
	std::cout << "brush size is now: " << BRUSH_SIZE << std::endl;
}


//drawing or something,,, (can i still create a new global variable to save the size of the brush??)
void Drawing(double xpos, double ypos, int brooshSize, color coolorSelctd, bool spray_paint, bool squarebroosh) {
	//xpos and ypos are mouse's coords
	for (double i = -1 * brooshSize; i <= brooshSize; i++) {
		for (int j = -1 * brooshSize; j <= brooshSize; j++) {
			if (spray_paint == false) {
				if (squarebroosh) {
					SetFrameBufferPixel(xpos + i, ypos + j, coolorSelctd); //setFBP never tries to access anything outside the FB array
					SetTouchedPixel(xpos + i, ypos + j);
				}
				else { //circle brush, fill normal. 
					//Brush size is radius, calc dist from middle pixel coord to the other and see if less than radius.
					float r = sqrt( (i)*(i) + (j)*(j) ); //radius from mouse center to whatever pixel we check
					if (r <= brooshSize) {
						SetFrameBufferPixel(xpos + i, ypos + j, coolorSelctd); //setFBP never tries to access anything outside the FB array
						SetTouchedPixel(xpos + i, ypos + j);
					}
				}
				
			}
			else {
				if (squarebroosh) {
					int temp = rand() % 3;
					if (temp == 0 || temp == 1) { //66.6% of pixels drawn
						SetFrameBufferPixel(xpos + i, ypos + j, coolorSelctd);
						SetTouchedPixel(xpos + i, ypos + j);
					}
				}
				else { //circle brush spraypaint
					int temp = rand() % 3;
					if (temp == 0 || temp == 1) { //66.6% of pixels drawn
						float r = sqrt((i) * (i)+(j) * (j)); //radius from mouse center to whatever pixel we check
						if (r <= brooshSize) {
							SetFrameBufferPixel(xpos + i, ypos + j, coolorSelctd); //setFBP never tries to access anything outside the FB array
							SetTouchedPixel(xpos + i, ypos + j);
						}
					}
					
				}
				
			}
			
		}
	}
}



// Mouse position callback function
void CursorPositionCallback(GLFWwindow* lWindow, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
		//std::cout << "Mouse position is: x - " << xpos << ", y - " << ypos << std::endl;
		Drawing(xpos, ypos, BRUSH_SIZE, COLOR_SELECTED, SPRAYPAINT, SQUAREBRUSH);
	}
}

// Mouse callback function
void MouseCallback(GLFWwindow* lWindow, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
	{
		std::cout << "Mouse left button is pressed." << std::endl;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		std::cout << "Mouse right button is pressed." << std::endl;
		ClearandSetBG();
	}

}

// You can use "switch" or "if" to compare key to a specific character.
// for example,
// if (key == '0')
//     DO SOMETHING

// Keyboard callback function
void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	std::cout << "Key " << (char)key << " is pressed." << std::endl;
	switch ((char)key) {
		case '+':
			if (BRUSH_SIZE == 128) {std::cout << "Brush size is at the max, sorry" << std::endl;}
			else {ChangeBrooshSize(true);}
			break;
		case '-':
			if (BRUSH_SIZE == 1) { std::cout << "Brush size is at the min, sorry" << std::endl;}
			else {ChangeBrooshSize(false);}
			break;
		case '0':
			COLOR_SELECTED = { 0.0f, 0.0f, 0.0f };
			break;
		case '1':
			COLOR_SELECTED = { 0.0f, 0.0f, 1.0f };
			break;
		case '2':
			COLOR_SELECTED = { 0.0f, 1.0f, 0.0f };
			break;
		case '3':
			COLOR_SELECTED = { 0.0f, 1.0f, 1.0f };
			break;
		case '4':
			COLOR_SELECTED = { 1.0f, 0.0f, 0.0f };
			break;
		case '5':
			COLOR_SELECTED = { 1.0f, 0.0f, 1.0f };
			break;
		case '6':
			COLOR_SELECTED = { 1.0f, 1.0f, 0.0f };
			break;
		case '7':
			COLOR_SELECTED = { 1.0f, 1.0f, 1.0f };
			break;
			//changing bg color buttons
		case ')':
			BG_COLOR = { 0.0f, 0.0f, 0.0f };
			ChangeBGColor();
			break;
		case '!':
			BG_COLOR = { 0.0f, 0.0f, 1.0f };
			ChangeBGColor();
			break;
		case '@':
			BG_COLOR = { 0.0f, 1.0f, 0.0f };
			ChangeBGColor();
			break;
		case '#':
			BG_COLOR = { 0.0f, 1.0f, 1.0f };
			ChangeBGColor();
			break;
		case '$':
			BG_COLOR = { 1.0f, 0.0f, 0.0f };
			ChangeBGColor();
			break;
		case '%':
			BG_COLOR = { 1.0f, 0.0f, 1.0f };
			ChangeBGColor();
			break;
		case '^':
			BG_COLOR = { 1.0f, 1.0f, 0.0f };
			ChangeBGColor();
			break;
		case '&':
			BG_COLOR = { 1.0f, 1.0f, 1.0f };
			ChangeBGColor();
			break;
		
		case 's':
			SPRAYPAINT = !SPRAYPAINT;
			(SPRAYPAINT) ? std::cout << "spray paint brush selected" << std::endl : std::cout << "normal paint brush selected" << std::endl;
			
			break;

		case 'b':
			SQUAREBRUSH = !SQUAREBRUSH;
			(SQUAREBRUSH) ? std::cout << "square brush selected" << std::endl : std::cout << "round brush selected" << std::endl;
			break;
		
		default:
			std::cout << "I'm afraid the key you pressed was nothing special" << std::endl;
	}
}

void Init()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment 1 - <Nandini Janapati>", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetMouseButtonCallback(window, MouseCallback);
	glfwSetCursorPosCallback(window, CursorPositionCallback);
	glfwSetCharCallback(window, CharacterCallback);
	glewExperimental = GL_TRUE;
	glewInit();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	ClearFrameBuffer();
	ClearMask();
}


int main()
{	
	Init();
	while (glfwWindowShouldClose(window) == 0)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}