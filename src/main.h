#pragma once
#include <Keypad.h>
//定义键盘管脚
#define X3 23
#define X2 22
#define X1 21
#define Y1 19
#define Y2 18 
#define Y3 5

// 键盘定义
typedef unsigned char byte;
const byte Rows= 3; //行数
const byte Cols= 3; //列数
uint8_t keys[Rows][Cols] = {
  {1,2,3,},
  {4,5,6,},
  {7,8,9,},
};

// 输入引脚
byte rPins[Rows]= {Y1,Y2,Y3}; //输入行引脚
byte cPins[Cols]= {X1,X2,X3}; //输入列引脚

//初始化键盘
Keypad customKeypad = Keypad(makeKeymap(keys), rPins, cPins, Rows, Cols);