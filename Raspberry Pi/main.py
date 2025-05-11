# main.py

from Sensor_Listener import SensorListener
from GUI_App import app

if __name__ == '__main__':
    # Start CAN sensor listener
    listener = SensorListener()
    listener.start()

    # Run Flask GUI application
    app.run(host='0.0.0.0', port=5000, threaded=True)
