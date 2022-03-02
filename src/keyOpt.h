#pragma once
#include <WiFi.h>
#include <string.h>
#include <Arduino.h>
#include <BleKeyboard.h>
#include <ArduinoJson.h>

#define NormalKey 1
#define MediaKey 2
#define HotKey 3
#define MAX_hotkey 3

// 快捷键结构体
struct KeySetting{
    unsigned char key_type; // 按键类型
    unsigned char key_val[MAX_hotkey];     // 按键值
};

//热键类
class KEYMAP{
    private:
        struct KeySetting KeyMap[9];    // 定义热键
    public:
        KEYMAP();                               
        KeySetting getKeyMap(int keynum);                   // 获取按键
        void setKeyMap(int keynum, KeySetting input);       // 设置按键
        void delKeyMap(int keynum);                         // 删除按键   
        void tapKeyMap(int keynum, BleKeyboard bleKeyboard);// 点击按键
        void getKeyMapConfig(WiFiClient &client);           // 获取配置文件
        bool loadKeyMapConfig(String config);               // 导入配置文件 
        void showKeyMap(int keynum);                        // 显示按键   
        void showKeyMap(KeySetting key);
        void showKeyMap();
};