# 📝 Async Logger with Telegram Alerts

A **fast and thread-safe asynchronous logger** for C++ that writes logs to both console and files. It runs in a **separate thread**, preventing performance loss, and **automatically sends critical errors to Telegram** via a bot.

---

## 📌 Features
✅ **Asynchronous logging** – runs in a background thread.  
✅ **Writes logs to console and daily log files (`logs/YYYY-MM-DD.log`).**  
✅ **Auto Telegram alerts** – sends `LOG_ERROR` messages instantly.  
✅ **Thread-safe logging queue** to avoid race conditions.  
✅ **Log rotation** – creates a new log file every day.  
✅ **Reads Telegram credentials from `.env` file** for security.  
✅ **Cross-platform support** – works on Windows, Linux, and macOS.  

---

## 🚀 Installation & Setup

### **1️⃣ Install Dependencies**
Ensure you have **cURL** installed, as it’s required for Telegram integration.

#### **Linux/macOS**
```sh
sudo apt install libcurl4-openssl-dev  # Ubuntu/Debian
brew install curl                      # macOS
```

#### **Windows**
1. Install **cURL** from [official site](https://curl.se/).
2. Add it to **system PATH**.

---

### **2️⃣ Create `.env` File**
Store your **Telegram Bot Token** and **Chat ID** in a `.env` file:
```
TELEGRAM_BOT_TOKEN=your_bot_token
TELEGRAM_CHAT_ID=your_chat_id
```

---

### **3️⃣ Usage**
Include `Logger.h` and start logging:

```cpp
#include "Logger.h"

int main() {
    Logger& logger = Logger::getInstance();

    logger.log(LogLevel::INFO, "Application started");
    logger.log(LogLevel::WARNING, "This is a warning");
    logger.log(LogLevel::LOG_ERROR, "🔥 CRITICAL ERROR! System failure!");

    return 0;
}
```

---

## 🎯 How It Works
1️⃣ **Logs are written asynchronously** to a queue.  
2️⃣ A **separate thread** writes logs to files & console.  
3️⃣ **Errors (`LOG_ERROR`) are instantly sent to Telegram.**  
4️⃣ **New log file is created daily (`logs/YYYY-MM-DD.log`).**  

---

## 📩 Contributing
Feel free to **open issues, suggest features, or submit PRs**!  

---

## ⚖️ License
This project is licensed under the **MIT License**.

---

🔗 **Created with ❤️ by Alek Morles**
