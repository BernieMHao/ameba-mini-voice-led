/*
 * ============================================================
 * Ameba Mini 中英文語音控制 LED
 * ============================================================
 *
 * PC -> Ameba 指令
 *
 * LEFT_ON       左邊藍燈開
 * LEFT_OFF      左邊藍燈關
 * RIGHT_ON      右邊綠燈開
 * RIGHT_OFF     右邊綠燈關
 * BLINK_3       兩顆燈一起閃爍三次
 * GET_STATE     查詢目前狀態
 *
 *
 * Ameba -> PC 回傳範例
 *
 * READY
 *
 * STATE BLUE=0 GREEN=0
 *
 * ACK LEFT_ON BLUE=1 GREEN=0
 * ACK LEFT_OFF BLUE=0 GREEN=1
 * ACK RIGHT_ON BLUE=1 GREEN=1
 * ACK RIGHT_OFF BLUE=1 GREEN=0
 * ACK BLINK_3 BLUE=1 GREEN=1
 *
 * EVENT BLINK_3_START
 *
 * ERR UNKNOWN_COMMAND
 *
 * ============================================================
 */


// ============================================================
// LED 邏輯
// ============================================================

#define LED_ON  HIGH
#define LED_OFF LOW


// ============================================================
// LED 狀態
//
// blueState  = 左邊藍燈
// greenState = 右邊綠燈
//
// 兩顆燈彼此獨立。
// ============================================================

bool blueState = false;

bool greenState = false;


// Serial 接收 Buffer

String serialBuffer = "";


// ============================================================
// 直接設定實體 LED
// ============================================================

void setPhysicalLED(
    bool blue,
    bool green
) {

    digitalWrite(
        LED_B,
        blue ? LED_ON : LED_OFF
    );


    digitalWrite(
        LED_G,
        green ? LED_ON : LED_OFF
    );
}


// ============================================================
// 根據 blueState / greenState 更新 LED
// ============================================================

void updateLEDs() {

    setPhysicalLED(
        blueState,
        greenState
    );
}


// ============================================================
// 回傳指定 LED 狀態
// ============================================================

void sendStateValues(
    bool blue,
    bool green
) {

    Serial.print(
        "STATE BLUE="
    );


    Serial.print(
        blue ? 1 : 0
    );


    Serial.print(
        " GREEN="
    );


    Serial.println(
        green ? 1 : 0
    );
}


// ============================================================
// 回傳目前 LED 真實紀錄狀態
// ============================================================

void sendState() {

    sendStateValues(
        blueState,
        greenState
    );
}


// ============================================================
// 回傳 ACK
// ============================================================

void sendAck(
    String command
) {

    Serial.print(
        "ACK "
    );


    Serial.print(
        command
    );


    Serial.print(
        " BLUE="
    );


    Serial.print(
        blueState ? 1 : 0
    );


    Serial.print(
        " GREEN="
    );


    Serial.println(
        greenState ? 1 : 0
    );
}


// ============================================================
// 只有「目前亮著的燈」閃爍三次
//
// 例如：
// BLUE=1 GREEN=0 → 只閃藍燈
// BLUE=0 GREEN=1 → 只閃綠燈
// BLUE=1 GREEN=1 → 兩顆一起閃
// BLUE=0 GREEN=0 → 沒有燈閃
//
// 完成後恢復原本狀態。
// ============================================================

void blinkThreeTimes() {

    // 保存目前 LED 狀態
    bool oldBlueState =
        blueState;

    bool oldGreenState =
        greenState;


    // 通知網頁開始閃爍
    Serial.println(
        "EVENT BLINK_3_START"
    );


    // ========================================================
    // 如果兩顆燈都沒有亮
    // 就沒有需要閃爍的 LED
    // ========================================================

    if (
        !oldBlueState &&
        !oldGreenState
    ) {

        Serial.println(
            "EVENT BLINK_3_NO_LED_ON"
        );


        // LED 狀態維持全部關閉

        updateLEDs();


        sendAck(
            "BLINK_3"
        );


        return;
    }


    // ========================================================
    // 閃爍三次
    // ========================================================

    for (
        int i = 0;
        i < 3;
        i++
    ) {

        // ----------------------------------------------------
        // 原本亮著的 LED 再亮起
        //
        // 原本關閉的 LED 繼續保持關閉
        // ----------------------------------------------------

        setPhysicalLED(
            oldBlueState,
            oldGreenState
        );


        sendStateValues(
            oldBlueState,
            oldGreenState
        );


        delay(
            300
        );


        // ----------------------------------------------------
        // 把「原本亮著的燈」暫時熄滅
        //
        // 因為原本關閉的燈本來就是關閉，
        // 所以這裡兩顆全部 OFF 即可。
        // ----------------------------------------------------

        setPhysicalLED(
            false,
            false
        );


        sendStateValues(
            false,
            false
        );


        delay(
            300
        );
    }


    // ========================================================
    // 恢復閃爍之前的 LED 狀態
    // ========================================================

    blueState =
        oldBlueState;


    greenState =
        oldGreenState;


    updateLEDs();


    // ========================================================
    // 回傳完成 ACK
    // ========================================================

    sendAck(
        "BLINK_3"
    );
}


// ============================================================
// 處理控制指令
// ============================================================

void processCommand(
    String command
) {

    command.trim();


    // ========================================================
    // 左邊開燈
    //
    // 只改 blueState
    //
    // greenState 完全不動
    // ========================================================

    if (
        command == "LEFT_ON"
    ) {

        blueState =
            true;


        // 不修改 greenState


        updateLEDs();


        sendAck(
            "LEFT_ON"
        );
    }


    // ========================================================
    // 左邊關燈
    //
    // 只關藍燈
    //
    // 綠燈維持原狀
    // ========================================================

    else if (
        command == "LEFT_OFF"
    ) {

        blueState =
            false;


        // 不修改 greenState


        updateLEDs();


        sendAck(
            "LEFT_OFF"
        );
    }


    // ========================================================
    // 右邊開燈
    //
    // 只改 greenState
    //
    // blueState 完全不動
    // ========================================================

    else if (
        command == "RIGHT_ON"
    ) {

        greenState =
            true;


        // 不修改 blueState


        updateLEDs();


        sendAck(
            "RIGHT_ON"
        );
    }


    // ========================================================
    // 右邊關燈
    //
    // 只關綠燈
    //
    // 藍燈維持原狀
    // ========================================================

    else if (
        command == "RIGHT_OFF"
    ) {

        greenState =
            false;


        // 不修改 blueState


        updateLEDs();


        sendAck(
            "RIGHT_OFF"
        );
    }


    // ========================================================
    // 閃爍三次
    // ========================================================

    else if (
        command == "BLINK_3"
    ) {

        blinkThreeTimes();
    }


    // ========================================================
    // 查詢 LED 狀態
    // ========================================================

    else if (
        command == "GET_STATE"
    ) {

        sendState();
    }


    // ========================================================
    // 空白資料
    // ========================================================

    else if (
        command.length() == 0
    ) {

        return;
    }


    // ========================================================
    // 未知指令
    //
    // 不可以改變 LED 狀態
    // ========================================================

    else {

        Serial.println(
            "ERR UNKNOWN_COMMAND"
        );
    }
}


// ============================================================
// setup
// ============================================================

void setup() {

    Serial.begin(
        115200
    );


    pinMode(
        LED_B,
        OUTPUT
    );


    pinMode(
        LED_G,
        OUTPUT
    );


    // ========================================================
    // 開機時
    //
    // 藍燈關
    // 綠燈關
    // ========================================================

    blueState =
        false;


    greenState =
        false;


    updateLEDs();


    delay(
        500
    );


    Serial.println(
        "READY"
    );


    sendState();
}


// ============================================================
// loop
// ============================================================

void loop() {

    while (
        Serial.available() > 0
    ) {

        char c =
            Serial.read();


        // ====================================================
        // 收到換行
        //
        // 代表一個完整指令
        // ====================================================

        if (
            c == '\n'
        ) {

            processCommand(
                serialBuffer
            );


            serialBuffer =
                "";
        }


        // ====================================================
        // 忽略 \r
        // ====================================================

        else if (
            c != '\r'
        ) {

            serialBuffer +=
                c;


            // =================================================
            // 防止異常超長資料
            // =================================================

            if (
                serialBuffer.length() > 100
            ) {

                serialBuffer =
                    "";


                Serial.println(
                    "ERR COMMAND_TOO_LONG"
                );
            }
        }
    }
}