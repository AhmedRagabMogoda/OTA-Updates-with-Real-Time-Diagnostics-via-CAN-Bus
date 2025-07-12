document.addEventListener('DOMContentLoaded', () => {
  // Element references
  const speedDisplay = document.getElementById('speed-display');
  const timeDisplay = document.getElementById('time-display');
  const dateDisplay = document.getElementById('date-display');
  const tempDisplay = document.getElementById('temp-display');
  const locDisplay = document.getElementById('loc-display');
  const diagService = document.getElementById('diag-service');
  const diagParamSelect = document.getElementById('diag-param-select');
  const diagParamInput = document.getElementById('diag-param-input');
  const btnDiagnostics = document.getElementById('btn-diagnostics');
  const diagResults = document.getElementById('diag-results');
  const diagStatus = document.getElementById('diag-status');
  const btnCheck = document.getElementById('btn-check-update');
  const btnDownload = document.getElementById('btn-download');
  const btnStart = document.getElementById('btn-start-update');
  const otaProgress = document.getElementById('ota-progress');
  const otaStatus = document.getElementById('ota-status');
  const updateStatus = document.getElementById('update-status');
  const btnDtcToggle = document.getElementById('btn-dtc-toggle');
  const dtcPanel = document.getElementById('dtc-panel');
  const btnDtcClose = document.getElementById('btn-dtc-close');
  const dtcEntries = document.getElementById('dtc-entries');
  const internalTemp = document.getElementById('internal-temp');
  const distDisplay = document.getElementById('dist-display');

  // Diagnostics sub-function options
  const subOptions = {
    '0x10': [ // Session Control
      { value: '0x00', text: 'DEFAULT' },
      { value: '0x01', text: 'SENSOR' },
      { value: '0x02', text: 'CONTROL' },
      { value: '0x03', text: 'PROGRAMMING' }
    ],
    '0x22': [ // Read Data by ID
      { value: '0x01', text: 'TEMPERATURE' },
      { value: '0x02', text: 'DISTANCE' }
    ],
    '0x27': [],     // Security Access
    '0x19': [],     // Read DTC
    '0x14': []      // Clear DTC
  };

  // OTA states
  let otaState = {
    ready: false,
    firmwarePath: '',
    version: null
  };

  function refreshParamWidget() {
    const svc = diagService.value;
    // Clear param controls
    diagParamSelect.innerHTML = '';
    diagParamInput.value = '';
    diagParamSelect.classList.add('hidden');
    diagParamInput.classList.add('hidden');
    // If security access, show input
    if (svc === '0x27') {
      diagParamInput.placeholder = 'Enter password (hex)';
      diagParamInput.type = 'text';
      diagParamInput.classList.remove('hidden');
    }
    // If has sub-options
    else if (subOptions[svc] && subOptions[svc].length > 0) {
      diagParamSelect.appendChild(new Option('Select...', ''));  
      subOptions[svc].forEach(opt => {
        diagParamSelect.appendChild(new Option(opt.text, opt.value));
      });
      diagParamSelect.classList.remove('hidden');
    }
  }

  // Initialize
  diagService.value = '';
  refreshParamWidget();
  diagService.addEventListener('change', refreshParamWidget);

  // Send Diagnostics request
  btnDiagnostics.addEventListener('click', () => {
    const sid = diagService.value;
    if (!sid) return;
    
    let param = '';
    if (!diagParamInput.classList.contains('hidden')) {
      param = diagParamInput.value;
    } else if (!diagParamSelect.classList.contains('hidden')) {
      param = diagParamSelect.value;
    }
    
    diagStatus.textContent = "Sending request...";
    diagStatus.className = "status-message info";
    
    fetch('/diagnostics', {
      method: 'POST',
      headers: {'Content-Type':'application/json'},
      body: JSON.stringify({ sid, param })
    })
    .catch(error => {
      diagStatus.textContent = `Error: ${error.message}`;
      diagStatus.className = "status-message error";
    });
  });

  // DTC Panel
  let dtcLog = [];
  function renderDtc() {
    dtcEntries.innerHTML = dtcLog.length
      ? dtcLog.map(m => `<div class="dtc-entry">${m}</div>`).join('')
      : '<div class="dtc-entry">No DTC entries.</div>';
  }
  btnDtcToggle.addEventListener('click', () => dtcPanel.classList.toggle('hidden'));
  btnDtcClose.addEventListener('click', () => dtcPanel.classList.add('hidden'));

  // SSE helper
  function listenSSE(url, onData) {
    const es = new EventSource(url);
    es.onmessage = e => onData(e.data);
  }

  // Streams
  listenSSE('/speed-stream', d => speedDisplay.textContent = `${d} km/h`);
  
  listenSSE('/meta-stream', d => {
    try {
      const data = JSON.parse(d);
      timeDisplay.textContent = data.time || '--:--:--';
      dateDisplay.textContent = data.date || '----/--/--';
      tempDisplay.textContent = data.temp || '--°C';
      locDisplay.textContent = data.loc || 'Unknown';
    } catch {
      // Fallback if parsing fails
      timeDisplay.textContent = '--:--:--';
      dateDisplay.textContent = '----/--/--';
      tempDisplay.textContent = '--°C';
      locDisplay.textContent = 'Unknown';
    }
  });
  
  listenSSE('/diag-stream', d => {
    // Always display the message
    diagResults.textContent += d + '\n';
    diagResults.scrollTop = diagResults.scrollHeight;
    
    // Check for special data messages
    if (d.startsWith("TEMPERATURE_DATA:")) {
        const value = d.split(':')[1];
        internalTemp.textContent = `${value} °C`;
        diagStatus.textContent = "Read temperature succeeded";
        diagStatus.className = "status-message success";
    } else if (d.startsWith("DISTANCE_DATA:")) {
        const value = d.split(':')[1];
        distDisplay.textContent = `${value} cm`;
        diagStatus.textContent = "Read distance succeeded";
        diagStatus.className = "status-message success";
    } else if (d.includes("Operation failed")) {
        diagStatus.textContent = d;
        diagStatus.className = "status-message error";
    } else if (d.includes("succeeded")) {
        diagStatus.textContent = d;
        diagStatus.className = "status-message success";
    }
  });
  
  listenSSE('/dtc-stream', d => {
    try {
      dtcLog = JSON.parse(d);
      renderDtc(); 
      dtcPanel.classList.remove('hidden');
    } catch (e) {
      console.error('Error parsing DTC data:', e);
    }
  });
  
  listenSSE('/ota-progress', p => { 
    if (typeof p === 'string') {
        if (p.includes("Vehicle ready for update")) {
            updateStatus.textContent = "Vehicle ready for update - Starting firmware upload";
            updateStatus.className = "status-message info";
        }
        else if (p === "UPDATE_COMPLETE") {
            otaProgress.value = 100;
            otaStatus.textContent = 'Update completed successfully!';
            otaStatus.className = "status-message success";
            updateStatus.textContent = "Firmware update completed successfully";
            updateStatus.className = "status-message success";
            btnStart.disabled = true;
        }
        else if (p === "UPDATE_FAILED") {
            otaStatus.textContent = "Update failed on vehicle side";
            otaStatus.className = "status-message error";
            updateStatus.textContent = "Firmware update failed";
            updateStatus.className = "status-message error";
            btnStart.disabled = false;
        }
        else if (p.includes("ACK received")) {
            // Just log, no special handling
            otaStatus.textContent = p;
        }
        else if (p.includes("error")) {
            otaStatus.textContent = p;
            otaStatus.className = "status-message error";
            updateStatus.textContent = "Firmware update failed";
            updateStatus.className = "status-message error";
            btnStart.disabled = false;
        }
        else {
            otaStatus.textContent = p;
        }
    } else {
        const pr = parseInt(p, 10);
        otaProgress.value = pr;
        otaStatus.textContent = `Update: ${pr}%`;
    }
  });

  // OTA actions
  btnCheck.addEventListener('click', async () => {
    otaStatus.textContent = 'Checking for updates...';
    otaStatus.className = "status-message info";
    try {
      const res = await fetch('/ota/fetch'); 
      const js = await res.json();
      
      if (js.available) { 
        otaState.updateAvailable = true;
        otaState.version = js.version;
        btnDownload.disabled = false; 
        otaStatus.textContent = js.message; 
        otaStatus.className = "status-message success";
        btnDownload.dataset.ver = js.version; 
        btnDownload.dataset.url = js.url;
      } else { 
        otaStatus.textContent = js.message; 
        otaStatus.className = "status-message warning";
        btnDownload.disabled = true; 
      }
    } catch (error) { 
      otaStatus.textContent = `Check error: ${error.message}`; 
      otaStatus.className = "status-message error";
    }
  });
  
  btnDownload.addEventListener('click', async () => {
    otaStatus.textContent = 'Downloading firmware...'; 
    otaStatus.className = "status-message info";
    btnDownload.disabled = true;
    
    try { 
      const v = btnDownload.dataset.ver, 
            u = btnDownload.dataset.url; 
      
      const r = await fetch(`/ota/download?version=${v}&url=${encodeURIComponent(u)}`); 
      const result = await r.json();
      
      if (result.success) { 
        otaStatus.textContent = `Downloaded v${v}`; 
        otaStatus.className = "status-message success";
        updateStatus.textContent = "Firmware downloaded successfully";
        updateStatus.className = "status-message success";
        otaState.ready = true; 
        otaState.firmwarePath = result.path;
        btnStart.disabled = false;
      } else {
        otaStatus.textContent = result.error; 
        otaStatus.className = "status-message error";
        updateStatus.textContent = "Firmware download failed";
        updateStatus.className = "status-message error";
        btnDownload.disabled = false; 
      }
    } catch (error) { 
      otaStatus.textContent = `Download failed: ${error.message}`; 
      otaStatus.className = "status-message error";
      updateStatus.textContent = "Firmware download failed";
      updateStatus.className = "status-message error";
      btnDownload.disabled = false; 
    }
  });
  
  btnStart.addEventListener('click', () => {
    if (!otaState.ready) return;
    
    otaStatus.textContent = 'Starting OTA update...'; 
    otaStatus.className = "status-message info";
    btnStart.disabled = true;
    
    // Send start command
    fetch('/ota/start', {
      method: 'POST',
      headers: {'Content-Type': 'application/json'},
      body: JSON.stringify({ path: otaState.firmwarePath })
    })
    .then(response => response.json())
    .then(data => {
      if (data.success) {
        updateStatus.textContent = data.message;
        updateStatus.className = "status-message success";
      } else {
        updateStatus.textContent = data.error;
        updateStatus.className = "status-message error";
        btnStart.disabled = false;
      }
    })
    .catch(error => {
      updateStatus.textContent = `Error: ${error.message}`;
      updateStatus.className = "status-message error";
      btnStart.disabled = false;
    });
  });
});
