#!/bin/bash

# kill any existing instances
pkill -f bridge.py
pkill -f chromium

# start bridge in background
python3 /home/anesthesia/mas/bridge.py &

# wait for bridge to be ready
sleep 2

# launch chromium kiosk
DISPLAY=:0 chromium --kiosk \
  --touch-events=enabled \
  --disable-infobars \
  --noerrdialogs \
  --disable-session-crashed-bubble \
  file:///home/anesthesia/mas/ui.html
