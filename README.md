# Ameba Mini Voice-Controlled LED

使用瀏覽器語音辨識與 Web Serial API 控制 **Ameba Mini** 板載 LED 的物聯網實作專案。

使用者可以透過電腦麥克風輸入中文或英文語音指令，網頁辨識語音後將其轉換成控制指令，透過 USB Serial 傳送至 Ameba Mini，控制板上的藍色與綠色 LED。

## 功能

本專案支援以下功能：

| 功能 | 中文語音 | English Command | Serial 指令 |
|---|---|---|---|
| 左邊開燈 | 左邊開燈 | Turn on the left light | `LEFT_ON` |
| 左邊關燈 | 左邊關燈 | Turn off the left light | `LEFT_OFF` |
| 右邊開燈 | 右邊開燈 | Turn on the right light | `RIGHT_ON` |
| 右邊關燈 | 右邊關燈 | Turn off the right light | `RIGHT_OFF` |
| 閃爍三次 | 閃爍三次 | Blink three times | `BLINK_3` |
| 中斷通訊 | 通訊中斷 | Disconnect | 瀏覽器關閉 Serial |

### LED 獨立控制

左右 LED 採用獨立控制。

例如先執行：

`左邊開燈`

再執行：

`右邊開燈`

最後藍燈與綠燈都會保持亮起。

LED 只有在收到對應的「關燈」指令後才會關閉。

### 閃爍三次

執行「閃爍三次」時，只有目前亮著的 LED 會閃爍三次。

例如：

- 只有藍燈亮 → 藍燈閃爍三次
- 只有綠燈亮 → 綠燈閃爍三次
- 藍燈、綠燈都亮 → 兩顆一起閃爍三次
- 藍燈、綠燈都關閉 → 不改變 LED

閃爍完成後會恢復閃爍前的 LED 狀態。

## 系統架構

```text
使用者
  │
  │ 語音
  ▼
電腦麥克風
  │
  ▼
Web Speech Recognition
  │
  │ 語音辨識結果
  ▼
網頁控制介面
  │
  │ Web Serial API
  │ 115200 baud
  ▼
Ameba Mini
  │
  ├── Blue LED
  │
  └── Green LED
```

Ameba Mini 執行完成後會透過 Serial 回傳 LED 狀態，因此網頁顯示的 LED 狀態以開發板回傳資訊為依據，而不是由網頁自行假設。

## Serial 通訊協定

### PC → Ameba

```text
LEFT_ON
LEFT_OFF
RIGHT_ON
RIGHT_OFF
BLINK_3
GET_STATE
```

### Ameba → PC

例如：

```text
READY
STATE BLUE=0 GREEN=0
ACK LEFT_ON BLUE=1 GREEN=0
ACK RIGHT_ON BLUE=1 GREEN=1
ACK LEFT_OFF BLUE=0 GREEN=1
ACK RIGHT_OFF BLUE=0 GREEN=0
```

執行閃爍時：

```text
EVENT BLINK_3_START
STATE BLUE=1 GREEN=0
STATE BLUE=0 GREEN=0
...
ACK BLINK_3 BLUE=1 GREEN=0
```

如果收到未知指令：

```text
ERR UNKNOWN_COMMAND
```

未知或無法辨識的語音不會任意改變 LED 狀態。

## 專案檔案

```text
ameba_blink_left_right/
│
├── ameba_blink_left_right.ino
├── index.html
├── README.md
└── LICENSE
```

`ameba_blink_left_right.ino` 為 Ameba Mini Arduino 程式。

`index.html` 為語音辨識、Web Serial 與 LED 狀態顯示介面。

## 使用方式

### 1. 上傳 Ameba 程式

使用 Arduino IDE 開啟：

```text
ameba_blink_left_right.ino
```

選擇正確的 Ameba Mini 開發板及 COM Port，編譯並上傳程式。

### 2. 關閉 Serial Monitor

使用網頁控制前，請先關閉 Arduino IDE 的 Serial Monitor。

同一個 Serial Port 通常無法同時被 Serial Monitor 與瀏覽器 Web Serial 使用。

### 3. 啟動網頁

由於 Web Serial 與部分瀏覽器功能具有安全來源限制，建議透過 `localhost` 或 HTTPS 執行網頁，而不是直接雙擊 HTML 檔案。

例如使用 VS Code 的 Live Server 開啟 `index.html`。

### 4. 使用瀏覽器

建議使用桌面版：

- Google Chrome
- Microsoft Edge

瀏覽器必須支援 Web Serial API。

### 5. 連接 Ameba

在網頁按下：

`連接 Ameba`

選擇 Ameba Mini 對應的 Serial Port。

成功後網頁會透過：

```text
GET_STATE
```

取得目前 LED 狀態。

### 6. 語音控制

選擇中文或英文辨識模式，按下：

`🎤 開始語音辨識`

然後說出控制指令，例如：

```text
左邊開燈
右邊開燈
左邊關燈
右邊關燈
閃爍三次
通訊中斷
```

英文模式例如：

```text
Turn on the left light
Turn on the right light
Turn off the left light
Turn off the right light
Blink three times
Disconnect
```

## 通訊錯誤處理

如果 Ameba 沒有在指定時間內回傳 ACK，網頁會顯示通訊失敗。

如果 USB Serial 裝置被拔除，網頁會將 LED 狀態改為「未知」。

這是因為通訊中斷後，網頁無法再確認 Ameba Mini 上 LED 的實際狀態。

## 語音辨識注意事項

本專案使用瀏覽器提供的 Speech Recognition 功能。

如果出現：

```text
語音辨識服務連線失敗
```

或：

```text
network
```

通常代表瀏覽器的語音辨識服務目前無法正常連線。

這不代表 Ameba Mini 或 Web Serial 發生故障。

可以使用網頁上的手動控制按鈕分別測試 LED 與 Serial 通訊。

## 開發環境

- Ameba Mini
- Arduino IDE
- HTML / CSS / JavaScript
- Web Serial API
- Web Speech Recognition
- USB Serial
- Baud Rate: `115200`

## License

This project is licensed under the MIT License.

See the `LICENSE` file for details.