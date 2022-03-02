#include <keyOpt.h>
/*
 * @brief Construct a new KEYMAP::KEYMAP object 初始化
 * 
 */
KEYMAP::KEYMAP(){
    memset(KEYMAP::KeyMap,255,sizeof(KEYMAP::KeyMap));    // 初始化全255
}


void KEYMAP::setKeyMap(int keynum, KeySetting input){
    memcpy(&KEYMAP::KeyMap[keynum],&input,sizeof(input));   // 写入
}

/**
 * @brief 读取按键
 * 
 * @param keynum 按键号码
 * @return KeySetting 按键参数
 */
KeySetting KEYMAP::getKeyMap(int keynum){
    KeySetting key = KEYMAP::KeyMap[keynum];                
    return key;
}

/**
 * @brief 删除按键配置
 * 
 * @param keynum 按键序号
 */
void KEYMAP::delKeyMap(int keynum){
    KeySetting empty;
    memset(&empty,255,sizeof(empty));    //初始化全255
    memcpy(&KEYMAP::KeyMap[keynum],&empty,sizeof(empty));   // 拷贝到配置数组中
    Serial.printf("Del_Key:%d\n",keynum);
    KEYMAP::showKeyMap(keynum);
}


/**
 * @brief 输入按键
 * 
 * @param keynum 按键序号
 * @param bleKeyboard 键盘对象
 */
void KEYMAP::tapKeyMap(int keynum,BleKeyboard bleKeyboard){
    KeySetting key = KEYMAP::getKeyMap(keynum);
    KEYMAP::showKeyMap(keynum);
    if (key.key_type == HotKey){
        // 一般快捷按键
        for (int m = 0; m < HotKey; m++) {
            if (255 != key.key_val[m])
            bleKeyboard.press(key.key_val[m]);
        }
        for (int m = 0; m < HotKey; m++) {
            if (255 != key.key_val[m])
            bleKeyboard.release(key.key_val[m]);
        }
    }else if(key.key_type == MediaKey){
        // 媒体按键处理
        MediaKeyReport mediakey;
        memcpy(mediakey,&key.key_val,sizeof(mediakey));
        bleKeyboard.write(mediakey);
    }
}

/**
 * @brief 输出热键配置文件
 */
void KEYMAP::getKeyMapConfig(WiFiClient &client){
    StaticJsonDocument<1024> doc;
    JsonArray root = doc.to<JsonArray>();
    for (u_char i = 1; i <= 9; i++)
    {
        JsonObject node = root.createNestedObject();
        node["k"] = i;
        node["t"] = KEYMAP::KeyMap[i].key_type;
        JsonArray levels = node.createNestedArray("c");
        levels.add(KEYMAP::KeyMap[i].key_val[0]);
        levels.add(KEYMAP::KeyMap[i].key_val[1]);
        levels.add(KEYMAP::KeyMap[i].key_val[2]);
    }
    serializeJson(root, client);
}

/**
 * @brief 载入热键配置文件
 * 
 * @param config json配置文件
 * @return true 载入成功
 * @return false 载入失败
 */
bool KEYMAP::loadKeyMapConfig(String config){
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, config);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return false;
    }
    for (u_char i = 1; i <= 9; i++)
    {
        JsonObject node = doc[i-1];
        u_char key_type = (unsigned char)node["t"].as<unsigned char>();
        u_char key_val_0 = (unsigned char)node["c"][0].as<unsigned char>();
        u_char key_val_1 = (unsigned char)node["c"][1].as<unsigned char>();
        u_char key_val_2 = (unsigned char)node["c"][2].as<unsigned char>();
        memcpy(&KEYMAP::KeyMap[i].key_type,&key_type,sizeof(key_type)); 
        memcpy(&KEYMAP::KeyMap[i].key_val[0],&key_val_0,sizeof(key_val_0)); 
        memcpy(&KEYMAP::KeyMap[i].key_val[1],&key_val_1,sizeof(key_val_1)); 
        memcpy(&KEYMAP::KeyMap[i].key_val[2],&key_val_2,sizeof(key_val_2)); 
    }
    return true;
}


/**
 * @brief 显示当前热键
 * 
 * @param keynum 
 */
void KEYMAP::showKeyMap(int keynum){
    KeySetting key = KEYMAP::KeyMap[keynum];
    Serial.printf("Key:%d,Type:%d\n",keynum,key.key_type);
    Serial.printf("{%d,%d,%d}\n",key.key_val[0],key.key_val[1],key.key_val[2]);
}

/**
 * @brief 显示传入的按键配置
 * 
 * @param key 按键配置
 */
void KEYMAP::showKeyMap(KeySetting key){
    Serial.printf("Type:%d\n",key.key_type);
    Serial.printf("{%d,%d,%d}\n",key.key_val[0],key.key_val[1],key.key_val[2]);
}
/**
 * @brief 显示全部热键配置
 * 
 */
void KEYMAP::showKeyMap(){
    Serial.println("===show all key map ===");
    for(unsigned char keynum = 1;keynum <= 9;keynum++){
        KeySetting key = KEYMAP::KeyMap[keynum];
        Serial.printf("Key:%d,Type:%d\n",keynum,key.key_type);
        Serial.printf("{%d,%d,%d}\n",key.key_val[0],key.key_val[1],key.key_val[2]);
    }
    Serial.println("======================");
}
