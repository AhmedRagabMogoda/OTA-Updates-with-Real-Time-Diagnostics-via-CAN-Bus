from flask import Flask, render_template, request, jsonify, Response, stream_with_context
from threading import Thread
import queue, datetime, json

from CAN_Bus import CanBus
from Sensor_Listener import SensorListener
from Diagnostics_Client import DiagnosticsClient
from OTA_Client import OTAClient
from Server_Client import ServerClient
from Utils import (
    SPEED_ID,
    DTC_ID_TEMP,
    DTC_ID_DIST,
    START_UPDATE,
    UPDATE_ACK_ID,
    UPDATE_FRAME_ID,
    DIAG_RESPONSE_ID,
    SID_DIAG_SESSION_CTRL,
    SID_SECURITY_ACCESS,
    SID_READ_DATA_BY_ID,
    SID_READ_DTC,
    SID_CLEAR_DTC,
    SID_REQUEST_DOWNLOAD,
    POS_NEG_MESSAGES,
    DTC_DESCRIPTIONS
)

app = Flask(__name__)

# Initialize components
can            = CanBus()
listener       = SensorListener(channel='can0', bitrate=500000, queue_size=100)
diag_client    = DiagnosticsClient(can)
ota_client     = OTAClient(can)
server_client  = ServerClient()
listener.start()

# Queues for Server-Sent Events
speed_q = queue.Queue()
temp_q  = queue.Queue()
dist_q  = queue.Queue()
diag_q  = queue.Queue()   
dtc_q   = queue.Queue()
ota_q   = queue.Queue()

latest_firmware_path = '/home/ahmed-mogoda/OTA_firmware/firmware_v1.0.38.txt'
ota_client.watch_for_start(latest_firmware_path)

# start UDS response monitor so everything goes into diag_queue
diag_client.start_response_monitor(diag_q)

# Dispatch real-time speed into speed_q
def sensor_dispatcher():
    while True:
        msg = listener.can.recv()
        if not msg:
            continue
        arb_id, data = msg
        if arb_id == SPEED_ID:
            speed = data[0] | (data[1] << 8)
            speed_q.put(speed)
Thread(target=sensor_dispatcher, daemon=True).start()

# Generic SSE generator
def sse_gen(q):
    while True:
        yield f"data: {q.get()}\n\n"

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/speed-stream')
def speed_stream():
    return Response(stream_with_context(sse_gen(speed_q)),
                    mimetype='text/event-stream')

@app.route('/temp-stream')
def temp_stream():
    return Response(stream_with_context(sse_gen(temp_q)),
                    mimetype='text/event-stream')

@app.route('/dist-stream')
def dist_stream():
    return Response(stream_with_context(sse_gen(dist_q)),
                    mimetype='text/event-stream')

@app.route('/diag-stream')
def diag_stream():
    return Response(stream_with_context(sse_gen(diag_q)),
                    mimetype='text/event-stream')

@app.route('/dtc-stream')
def dtc_stream():
    def gen():
        while True:
            entries = dtc_q.get()
            lines = [
                f"{e['description']} at {e['time']} on {e['date']}"
                for e in entries
            ]
            yield f"data: {json.dumps(lines)}\n\n"
    return Response(stream_with_context(gen()),
                    mimetype='text/event-stream')

@app.route('/ota-progress')
def ota_progress():
    return Response(stream_with_context(sse_gen(ota_q)),
                    mimetype='text/event-stream')

@app.route('/diagnostics', methods=['POST'])
def diagnostics():
    body  = request.json or {}
    try:
        sid = int(body.get('sid', '0'), 16)
    except ValueError:
        diag_q.put("Invalid SID")
        return ('', 400)

    raw_param = body.get('param', '').strip()
    param_int = None
    if sid in {SID_DIAG_SESSION_CTRL, SID_SECURITY_ACCESS, SID_READ_DATA_BY_ID}:
        try:
            param_int = int(raw_param, 16)
        except ValueError:
            diag_q.put(f"Invalid param for SID 0x{sid:02X}")
            return ('', 400)

    ok = False

    if sid == SID_DIAG_SESSION_CTRL:
        ok = diag_client.start_session(param_int)
    elif sid == SID_SECURITY_ACCESS:
        ok = diag_client.send_password(param_int)
    elif sid == SID_READ_DATA_BY_ID:
        data = diag_client.read_data_by_id(param_int) or b'\x00\x00'
        if param_int == DTC_ID_TEMP:
            temp_q.put(data[0] | (data[1] << 8))
        elif param_int == DTC_ID_DIST:
            dist_q.put(data[0] | (data[1] << 8))
        else:
            diag_q.put(f"Read Data 0x{param_int:04X}: {data.hex()}")
        ok = True
    elif sid == SID_READ_DTC:
        codes = diag_client.read_dtc() or []
        now   = datetime.datetime.now()
        tstr  = now.strftime("%H:%M:%S")
        dstr  = now.strftime("%Y-%m-%d")
        entries = [
            {
                'description': DTC_DESCRIPTIONS.get(c, "Unknown fault"),
                'time': tstr,
                'date': dstr
            }
            for c in codes
        ]
        dtc_q.put(entries)
        ok = True
    elif sid == SID_CLEAR_DTC:
        ok = diag_client.clear_dtc()
    elif sid == SID_REQUEST_DOWNLOAD:
        ok = diag_client.request_download()
    else:
        diag_q.put(f"Unsupported SID 0x{sid:02X}")
        return ('', 400)

    msg = POS_NEG_MESSAGES.get(sid, {}).get(
        ok, f"Service 0x{sid:02X} {'succeeded' if ok else 'failed'}"
    )
    diag_q.put(msg)
    return ('', 204)

@app.route('/ota/fetch')
def ota_fetch():
    available, version, url = server_client.check_for_update()
    return jsonify(available=available, version=version, url=url)

@app.route('/ota/download')
def ota_download():
    global latest_firmware_path
    version = request.args.get('version')
    url     = request.args.get('url')
    if not version or not url:
        return ('Missing version or url', 400)
    path = server_client.download_firmware(version, url)
    if not path:
        ota_q.put("Download failed")
        return ('', 500)
    latest_firmware_path = path
    ota_q.put(f"Downloaded {version}")
    return ('', 204)

@app.route('/ota/start', methods=['POST'])
def ota_start():
    if not latest_firmware_path:
        ota_q.put("No firmware downloaded")
        return ('', 400)
    ota_q.put(f"Ready to upload {latest_firmware_path}")
    return ('', 204)


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, threaded=True)
