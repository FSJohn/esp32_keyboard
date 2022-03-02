#include <main.h>
#include <WiFi.h>
#include <BleKeyboard.h>
#include <keyOpt.h>


// 无线网的用户名和密码
const char* ssid = "0x0722BC";
const char* password = "123456zqh";

// 创建server对象，80是端口号
WiFiServer server(80);

// 用于判断网络是否超时，设为2000ms
unsigned long previousTime = 0;
const long timeoutTime = 500;
 
// 蓝牙键盘相关
BleKeyboard bleKeyboard("FSJohn Keyboard","Espressif",100);

// 定义热键
KEYMAP keymap;
uint8_t arrKeyCode[9][3] = { // 255表示此位置无按键，为了演示追求代码简洁（用list来表示快捷键逻辑更清晰且易扩展）
                        {255,   255,          255},
                        {255,   255,          255},
                        {KEY_LEFT_CTRL,   'v',          255},
                        {KEY_LEFT_CTRL,   'c',           255},
                        {KEY_LEFT_GUI,    'e',     255},
                        {KEY_LEFT_CTRL,   KEY_LEFT_ALT, KEY_DELETE},
                        {KEY_LEFT_CTRL,   KEY_LEFT_GUI, KEY_LEFT_ARROW},
                        {KEY_LEFT_GUI,    'L',          255},
                        {KEY_LEFT_CTRL,   KEY_LEFT_GUI, KEY_RIGHT_ARROW},
                      };

//上面数组含有的快捷键个数
const int count = sizeof(arrKeyCode) / sizeof(arrKeyCode[0]);

// 按键初始化
void init_keyboard(){
    for(int i = 0;i<count;i++){
        KeySetting key;
        key.key_type = HotKey;
        memcpy(&key.key_val,&arrKeyCode[i],sizeof(arrKeyCode[i]));
        keymap.setKeyMap(i+1,key);
        keymap.showKeyMap(i+1);
    }
    keymap.delKeyMap(1);
    KeySetting key;
    key.key_type = MediaKey;
    memcpy(&key.key_val,&KEY_MEDIA_PLAY_PAUSE,sizeof(KEY_MEDIA_PLAY_PAUSE));
    keymap.setKeyMap(1,key);
    keymap.showKeyMap(1);
}


void setup() {
    // 初始化串口
    Serial.begin(115200);

    // 初始化蓝牙键盘
    Serial.println("Starting BLE work!");
    bleKeyboard.begin();


    // 连接WiFi
    Serial.print("正在连接：");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("WiFi已连接. IP地址: ");
    Serial.println(WiFi.localIP());

    // 按键初始化
    init_keyboard();
    Serial.println("按键初始化完毕！");
    keymap.showKeyMap();

    // 启动网络服务
    server.begin();
}

void loop() {
    WiFiClient client = server.available();
    if (client) {                             // If a new client connects,
        unsigned long currentTime = millis();
        previousTime = currentTime;
        Serial.println("==>+<== 连接建立");

        String currentLine = "";

        while (client.connected() && currentTime - previousTime <= timeoutTime) {  
            // loop while the client's connected
            currentTime = millis();
            if (client.available()) {             // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                // Serial.write(c);                    // print it out the serial monitor
                if (c == '\n') {                    // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        Serial.println("下载配置文件");
                        keymap.getKeyMapConfig(client);
                        break;
                    } else { // if you got a newline, then clear currentLine
                        Serial.println(currentLine);
                        bool ret = keymap.loadKeyMapConfig(currentLine);
                        if(ret){
                            Serial.println("上传配置文件成功！");
                        }else{
                            Serial.println("上传配置文件失败！");
                        }
                        keymap.showKeyMap();
                        currentLine = "";
                    }
                } else if (c != '\r') {  // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }
            }
        }
        // Close the connection
        client.stop();
        Serial.println("==>x<== 连接断开\n");
    }

    // 蓝牙连接
    if (bleKeyboard.isConnected()) {
        int keypressed = customKeypad.getKey();
        if (keypressed != NO_KEY)
        {
            Serial.println(keypressed);
            keymap.tapKeyMap(keypressed,bleKeyboard);
        }
    }
}