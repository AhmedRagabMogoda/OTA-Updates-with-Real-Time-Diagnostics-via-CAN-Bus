document.addEventListener('DOMContentLoaded', () => {
  // --- Element references ---
  const speedDisplay    = document.getElementById('speed-display');
  const speedMeta       = document.getElementById('speed-meta');
  const diagService     = document.getElementById('diag-service');
  const diagParamSelect = document.getElementById('diag-param-select');
  const diagParamInput  = document.getElementById('diag-param-input');
  const btnDiagnostics  = document.getElementById('btn-diagnostics');
  const diagResults     = document.getElementById('diag-results');
  const btnCheck        = document.getElementById('btn-check-update');
  const btnDownload     = document.getElementById('btn-download');
  const btnStart        = document.getElementById('btn-start-update');
  const otaProgress     = document.getElementById('ota-progress');
  const otaStatus       = document.getElementById('ota-status');
  const btnDtcToggle    = document.getElementById('btn-dtc-toggle');
  const dtcPanel        = document.getElementById('dtc-panel');
  const btnDtcClose     = document.getElementById('btn-dtc-close');
  const dtcEntries      = document.getElementById('dtc-entries');

  // --- Diagnostics sub-function maps ---
  const subOptions = {
    '0x10': [
      { value: '0x00', text: 'Session Default' },
      { value: '0x01', text: 'Session Sensor' },
      { value: '0x02', text: 'Session Control' },
      { value: '0x03', text: 'Session Programming' }
    ],
    '0x22': [
      { value: '0xF190', text: 'Read Temperature DID' },
      { value: '0xF191', text: 'Read Distance DID' }
    ]
  };

  // Show or hide the param widget
  function refreshParamWidget() {
    const svc = diagService.value;
    if (svc === '0x27') {
      diagParamInput.type = 'password';
      diagParamInput.placeholder = 'Enter password';
      diagParamInput.classList.remove('hidden');
      diagParamSelect.classList.add('hidden');
    } else if (subOptions[svc]) {
      diagParamSelect.classList.remove('hidden');
      diagParamInput.classList.add('hidden');
      diagParamSelect.innerHTML = '';
      subOptions[svc].forEach(opt => {
        const o = document.createElement('option');
        o.value = opt.value;
        o.textContent = opt.text;
        diagParamSelect.appendChild(o);
      });
    } else {
      diagParamSelect.classList.add('hidden');
      diagParamInput.classList.add('hidden');
    }
  }
  diagService.value = '';
  refreshParamWidget();
  diagService.addEventListener('change', refreshParamWidget);

  // Send Diagnostics request
  btnDiagnostics.addEventListener('click', () => {
    const sid = diagService.value;
    if (!sid) {
      alert('Please select a diagnostic service');
      return;
    }
    let param = '';
    if (!diagParamInput.classList.contains('hidden')) {
      param = diagParamInput.value;
    } else if (!diagParamSelect.classList.contains('hidden')) {
      param = diagParamSelect.value;
    }
    fetch('/diagnostics', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ sid, param })
    })
    .then(() => {
      // clear previous results
      diagResults.textContent = '';
      diagService.value = '';
      refreshParamWidget();
    })
    .catch(err => console.error(err));
  });

  // DTC Panel controls
  btnDtcToggle.addEventListener('click', () => dtcPanel.classList.toggle('hidden'));
  btnDtcClose .addEventListener('click', () => dtcPanel.classList.add('hidden'));
  const dtcLog = [];
  function renderDtcEntries() {
    dtcEntries.innerHTML = '';
    if (dtcLog.length === 0) {
      dtcEntries.innerHTML = '<div class="dtc-entry">No DTC entries yet.</div>';
    } else {
      dtcLog.forEach(msg => {
        const d = document.createElement('div');
        d.className = 'dtc-entry';
        d.textContent = msg;
        dtcEntries.appendChild(d);
      });
    }
  }

  // SSE: DTC stream
  new EventSource('/dtc-stream').onmessage = e => {
    const arr = JSON.parse(e.data);
    arr.forEach(m => dtcLog.push(m));
    if (dtcLog.length > 10) dtcLog.splice(0, dtcLog.length - 10);
    renderDtcEntries();
    dtcPanel.classList.remove('hidden');
  };

  // --- OTA Update workflow (unchanged) ---
  let firmwareReady = false;

  btnCheck.addEventListener('click', async () => {
    otaStatus.textContent = 'Checking...';
    try {
      const res = await fetch('/ota/fetch');
      const data = await res.json();
      if (data.available) {
        otaStatus.textContent = `Firmware v${data.version} available`;
        btnDownload.disabled = false;
        btnDownload.dataset.url = data.url;
        btnDownload.dataset.version = data.version;
      } else {
        otaStatus.textContent = 'No update available';
        btnDownload.disabled = true;
      }
    } catch {
      otaStatus.textContent = 'Check failed';
    }
  });

  btnDownload.addEventListener('click', async () => {
    otaStatus.textContent = 'Downloading...';
    btnDownload.disabled = true;
    try {
      const v = btnDownload.dataset.version;
      const u = btnDownload.dataset.url;
      const r = await fetch(`/ota/download?version=${v}&url=${encodeURIComponent(u)}`);
      if (!r.ok) throw new Error();
      otaStatus.textContent = `Downloaded v${v}`;
      firmwareReady = true;
      btnStart.disabled = false;
    } catch {
      otaStatus.textContent = 'Download failed';
      btnDownload.disabled = false;
    }
  });

  btnStart.addEventListener('click', () => {
    if (!firmwareReady) return;
    otaStatus.textContent = 'Requesting download mode...';
    btnStart.disabled = true;
    fetch('/diagnostics', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ sid: '0x34', param: '' })
    });
  });

new EventSource('/diag-stream').onmessage = e => {
  console.log('[SSE diag]', e.data);

  // أنشئ عنصر <div> جديد لكل رسالة
  const msgDiv = document.createElement('div');
  msgDiv.textContent = e.data;
  msgDiv.className = 'diag-entry';  // يمكنك تخصيص CSS لاحقًا

  // أضف العنصر إلى مربع النتائج
  diagResults.appendChild(msgDiv);

  // مرّر شريط التمرير إلى الأسفل
  diagResults.scrollTop = diagResults.scrollHeight;

  // تابع ACK للبدء بالرفع
  if (e.data.startsWith('Download mode: True')) {
    otaStatus.textContent = 'Uploading firmware...';
    fetch('/ota-start', {/*…*/});
  }
};


  new EventSource('/ota-progress').onmessage = e => {
    const p = parseInt(e.data, 10);
    otaProgress.value = p;
    otaStatus.textContent = `Update: ${p}%`;
    if (p >= 100) {
      otaStatus.textContent = 'Update completed successfully!';
    }
  };

  // --- Speed & Meta updates ---
  new EventSource('/speed-stream').onmessage = e => {
    speedDisplay.textContent = `${e.data} km/h`;
  };

  // Geolocation & reverse-geocoding
  let locLat=null, locLon=null, city='Unknown', country='';
  if (navigator.geolocation) {
    navigator.geolocation.getCurrentPosition(pos => {
      locLat = pos.coords.latitude.toFixed(4);
      locLon = pos.coords.longitude.toFixed(4);
      fetch(`https://nominatim.openstreetmap.org/reverse?lat=${locLat}&lon=${locLon}&format=json`)
        .then(r => r.json())
        .then(j => {
          city    = j.address.city || j.address.town || j.address.village || city;
          country = j.address.country || country;
        })
        .catch(() => {});
    });
  }

  // Weather fetch
  let weatherTemp = null;
  async function updateWeather() {
    if (locLat===null) return;
    try {
      const r = await fetch(
        `https://api.open-meteo.com/v1/forecast?latitude=${locLat}&longitude=${locLon}&current_weather=true`
      );
      const jd = await r.json();
      weatherTemp = jd.current_weather.temperature;
    } catch {
      weatherTemp = null;
    }
  }
  updateWeather();
  setInterval(updateWeather, 60000);

  // Update meta every second
  setInterval(()=>{
    const now = new Date();
    const t   = now.toLocaleTimeString('en-GB');
    const d   = now.toISOString().slice(0,10);
    const icon= (weatherTemp!==null && weatherTemp>=25) ? '🔥' : '❄️';
    const tempStr = (weatherTemp!==null) ? `${weatherTemp}°C` : '--';
    speedMeta.textContent = `${t} | ${d} | ${icon} ${tempStr} | ${city}, ${country}`;
  }, 1000);
});
