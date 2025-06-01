# linuxKeylogger

## (Ryan Welzel 6/1/2025)

linuxKeylogger:

Simple Linux keylogger written in C using libudev.


Features:
- Automatically detects the active keyboard using libudev
- Captures real-time keystrokes from /dev/input/eventX (wherever the keyboard is)
- Logs keystrokes to keylogger_output.txt


Requirements:
- Linux OS
- GCC compiler
- libudev headers


Install dependencies with:
- Bash: sudo apt install build-essential libudev-dev
