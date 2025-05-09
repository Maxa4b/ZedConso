@echo off
net session >nul 2>&1
if %errorLevel% NEQ 0 (
    echo 🔐 Restart in admin mode...
    powershell -Command "Start-Process '%~f0' -Verb runAs"
    exit /b
)

echo ⚠️ check if process use port 1883...

FOR /F "tokens=5" %%P IN ('netstat -ano ^| findstr ":1883"') DO (
    echo 🔪 Killed PID %%P using port 1883...
    taskkill /F /PID %%P >nul 2>&1
)

echo ✅ Port 1883 free

cd /d "%~dp0"

start "" ".\mosquitto\mosquitto.exe" -v