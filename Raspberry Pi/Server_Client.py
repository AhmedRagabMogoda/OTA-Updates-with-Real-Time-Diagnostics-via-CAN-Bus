import os
import requests

VERSION_JSON_URL = "https://my-vehicle-firmware.s3.eu-north-1.amazonaws.com/firmware/version.json"
FIRMWARE_DIR = "/home/ahmed-mogoda/OTA_firmware"

class ServerClient:
    """Class for communicating with firmware server"""
    def __init__(self, version_url=VERSION_JSON_URL, firmware_dir=FIRMWARE_DIR):
        self.version_url = version_url
        self.firmware_dir = firmware_dir
        os.makedirs(self.firmware_dir, exist_ok=True)
    
    def get_current_version(self):
        """Get current firmware version"""
        current_file = os.path.join(self.firmware_dir, 'current_version.txt')
        try:
            with open(current_file, 'r') as f:
                return f.read().strip()
        except FileNotFoundError:
            return "0.0.0"
    
    def check_for_update(self):
        """Check for firmware updates"""
        try:
            resp = requests.get(self.version_url, timeout=5)
            resp.raise_for_status()
            data = resp.json()
            latest = data.get('latest')
            url = data.get('url')
            
            if not latest or not url:
                return False, None, None, "Invalid version.json format"
            
            current = self.get_current_version()
            
            if latest != current:
                return True, latest, url, f"Update available: v{latest}"
            return False, current, None, f"Already on latest version: v{current}"
        except Exception as e:
            return False, None, None, f"Update check error: {str(e)}"
    
    def download_firmware(self, version, url):
        """Download firmware"""
        filename = f"firmware_v{version}.bin"
        filepath = os.path.join(self.firmware_dir, filename)
        
        try:
            r = requests.get(url, stream=True, timeout=30)
            r.raise_for_status()
            
            total_size = int(r.headers.get('content-length', 0))
            downloaded = 0
            
            with open(filepath, 'wb') as f:
                for chunk in r.iter_content(chunk_size=8192):
                    if chunk:
                        f.write(chunk)
                        downloaded += len(chunk)
                        progress = int((downloaded / total_size) * 100)
                        # Progress will be handled by main queue
                        yield progress
            
            current_file = os.path.join(self.firmware_dir, 'current_version.txt')
            with open(current_file, 'w') as vf:
                vf.write(version)
            
            return filepath, None
        except Exception as e:
            return None, f"Download error: {str(e)}"