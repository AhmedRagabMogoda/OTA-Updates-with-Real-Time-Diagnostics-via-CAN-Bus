from flask import Flask, render_template, request, jsonify, Response, stream_with_context
import threading, queue, datetime, time, json, os, struct
from can_bus import CanBus
from server_client import ServerClient
from sensor_listener import get_weather, get_location, get_time
from diagnostics import DTC_DESCRIPTIONS, UDS_NEG_MESSAGES, DTC_LOG_FILE
from utils import *
from ota import start_firmware_upload

app = Flask(__name__)

# Global DTC log
dtc_log = []

# Load DTC log from file if exists
try:
    if os.path.exists(DTC_LOG_FILE):
        with open(DTC_LOG_FILE, 'r') as f:
            dtc_log = json.load(f)
except:
    dtc_log = []

# Initialize CAN bus
can_bus = CanBus(
    channel='can0',
    bitrate=500000,
    rx_queue_size=200,
    can_filters=[
        {'can_id': DIAG_REQUEST_ID, 'can_mask': 0x7F0},
        {'can_id': UPDATE_ACK_ID, 'can_mask': 0x7FF},
        {'can_id': SPEED_ID, 'can_mask': 0x7FF},
        {'can_id': DIAG_RESPONSE_ID, 'can_mask': 0x7FF}
    ]
)

# Queues for frontend communication
speed_q = queue.Queue()
meta_q = queue.Queue()
diag_q = queue.Queue()
dtc_q = queue.Queue()
ota_q = queue.Queue()
can_q = queue.Queue()

# OTA update state
ota_active = False
ota_ack_queue = queue.Queue()
ota_firmware_path = ""

def main_thread():
    global dtc_log, ota_active
    
    lat, lon, location = get_location()
    
    while True:
        msg = can_bus.recv(timeout=0.1)
        if msg:
            arb_id, data = msg
            
            can_q.put(json.dumps({
                'arb_id': arb_id,
                'data': list(data)
            }))
            
            if arb_id == SPEED_ID and len(data) >= 2:
                speed = struct.unpack('<H', data[:2])[0]
                speed_q.put(speed)
            
            elif arb_id == DIAG_RESPONSE_ID:
                if data[0] == 0x62 and len(data) >= 4:
                    if data[1] == 0x01:
                        value = data[2] | (data[3] << 8)
                        diag_q.put(f"TEMPERATURE_DATA:{value}")
                    elif data[1] == 0x02:
                        value = data[2] | (data[3] << 8)
                        diag_q.put(f"DISTANCE_DATA:{value}")
                
                elif data[0] == 0x59 and len(data) >= 8:
                    dtc_code = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24)
                    day = data[5]
                    month = data[6]
                    year = 2000 + data[7]
                    date_str = f"{day:02d}/{month:02d}/{year}"
                    description = DTC_DESCRIPTIONS.get(dtc_code, "Unknown error")
                    dtc_entry = f"{dtc_code:08X} - {description} on {date_str}"
                    dtc_log.append(dtc_entry)
                    try:
                        with open(DTC_LOG_FILE, 'w') as f:
                            json.dump(dtc_log, f)
                    except Exception as e:
                        print(f"Error saving DTC log: {e}")
                    dtc_q.put(json.dumps(dtc_log))
                
                elif data[0] == 0x7F and len(data) >= 3:
                    sid = data[1]
                    nrc = data[2]
                    error_msg = "Unknown error"
                    if sid in UDS_NEG_MESSAGES and nrc in UDS_NEG_MESSAGES[sid]:
                        error_msg = UDS_NEG_MESSAGES[sid][nrc]
                    diag_q.put(f"Operation failed: {error_msg} (NRC=0x{nrc:02X})")
            
            elif arb_id == UPDATE_ACK_ID and data:
                ack_code = data[0]
                if ack_code == START_UPDATE and not ota_active and ota_firmware_path:
                    threading.Thread(
                        target=start_firmware_upload,
                        args=(ota_firmware_path, ota_q, ota_ack_queue),
                        daemon=True
                    ).start()
                    ota_active = True
                elif ack_code in [CODE_NEXT, CODE_ERROR]:
                    try:
                        ota_ack_queue.put(ack_code, block=False)
                    except queue.Full:
                        pass
        
        if time.time() % 1 < 0.1:
            current_time, current_date = get_time(lat, lon) if lat and lon else (None, None)
            temperature = get_weather()
            if not current_time or not current_date:
                now = datetime.datetime.now()
                current_time = now.strftime('%H:%M:%S')
                current_date = now.strftime('%Y-%m-%d')
            if not location:
                location = "Cairo, EG"
            
            meta_data = {
                'time': current_time,
                'date': current_date,
                'temp': f"{temperature:.1f}°C",
                'loc': location
            }
            meta_q.put(json.dumps(meta_data))
            
            if time.time() % 300 < 0.1:
                lat, lon, location = get_location()

threading.Thread(target=main_thread, daemon=True).start()

def sse_gen(q):
    while True:
        data = q.get()
        yield f"data: {data}\n\n"

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/speed-stream')
def speed_stream():
    return Response(stream_with_context(sse_gen(speed_q)), mimetype='text/event-stream')

@app.route('/meta-stream')
def meta_stream():
    return Response(stream_with_context(sse_gen(meta_q)), mimetype='text/event-stream')

@app.route('/diag-stream')
def diag_stream():
    return Response(stream_with_context(sse_gen(diag_q)), mimetype='text/event-stream')

@app.route('/dtc-stream')
def dtc_stream():
    return Response(stream_with_context(sse_gen(dtc_q)), mimetype='text/event-stream')

@app.route('/ota-progress')
def ota_progress():
    return Response(stream_with_context(sse_gen(ota_q)), mimetype='text/event-stream')

@app.route('/can-stream')
def can_stream():
    return Response(stream_with_context(sse_gen(can_q)), mimetype='text/event-stream')

@app.route('/diagnostics', methods=['POST'])
def diagnostics():
    data = request.json or {}
    sid = data.get('sid')
    param = data.get('param', '')
    
    try:
        sid_int = int(sid, 16)
        sub_int = int(param, 16) if param else 0
    except:
        diag_q.put('Invalid SID or parameter')
        return '', 400
    
    diag_q.put("\n--- New Diagnostic Request ---")
    diag_q.put(f"Sending request: SID=0x{sid_int:02X}, Param=0x{sub_int:02X}")
    
    if sid_int == SID_DIAG_SESSION_CTRL:
        data_bytes = bytes([sid_int, sub_int])
    elif sid_int == SID_SECURITY_ACCESS:
        try:
            password = int(param, 16) if param else 0
            high, low = (password >> 8) & 0xFF, password & 0xFF
            data_bytes = bytes([sid_int, 0x00, high, low])
        except:
            diag_q.put("Invalid password format")
            return '', 400
    elif sid_int == SID_READ_DATA_BY_ID:
        data_bytes = bytes([sid_int, sub_int])
    elif sid_int == SID_READ_DTC:
        data_bytes = bytes([sid_int, 0x00])
    elif sid_int == SID_CLEAR_DTC:
        data_bytes = bytes([sid_int, 0x00])
    elif sid_int == SID_REQUEST_DOWNLOAD:
        data_bytes = bytes([sid_int, 0x00])
    else:
        diag_q.put(f"Unsupported SID 0x{sid_int:02X}")
        return '', 400
    
    if can_bus.send(DIAG_REQUEST_ID, data_bytes):
        diag_q.put("Request sent successfully")
    else:
        diag_q.put("Failed to send request")
    
    return '', 204

@app.route('/ota/fetch')
def ota_fetch():
    server_client = ServerClient()
    available, version, url, message = server_client.check_for_update()
    return jsonify({
        'available': available,
        'version': version,
        'url': url,
        'message': message
    })

@app.route('/ota/download')
def ota_download():
    global ota_firmware_path
    
    version = request.args.get('version')
    url = request.args.get('url')
    
    if not version or not url:
        return 'Missing parameters', 400
    
    server_client = ServerClient()
    path, error = None, None
    for progress in server_client.download_firmware(version, url):
        if isinstance(progress, tuple):  # Finished download
            path, error = progress
        else:
            ota_q.put(progress)
    
    if path:
        ota_firmware_path = path
        return jsonify({
            'success': True,
            'path': path,
            'version': version
        })
    return jsonify({
        'success': False,
        'error': error
    }), 500

@app.route('/ota/start', methods=['POST'])
def ota_start():
    global ota_active
    
    if ota_active:
        return jsonify({
            'success': False,
            'error': "Update already in progress"
        }), 400
    
    data_bytes = bytes([SID_REQUEST_DOWNLOAD, 0x00])
    if not can_bus.send(DIAG_REQUEST_ID, data_bytes):
        return jsonify({
            'success': False,
            'error': "Failed to send request download"
        }), 500
    
    return jsonify({
        'success': True,
        'message': "Update initiated, waiting for vehicle readiness"
    })

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, threaded=True, debug=True)