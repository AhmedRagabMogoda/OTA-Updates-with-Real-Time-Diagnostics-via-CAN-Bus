# server_client.py

import os
import requests
from Utils import VERSION_JSON_URL, FIRMWARE_DIR

class ServerClient:
    """
    Client for interacting with the firmware server.
    Provides methods to check for updates and download firmware binaries.
    """
    def __init__(self, version_url=VERSION_JSON_URL, firmware_dir=FIRMWARE_DIR):
        self.version_url = version_url
        self.firmware_dir = firmware_dir
        os.makedirs(self.firmware_dir, exist_ok=True)

    def check_for_update(self):
        """
        Retrieves the latest firmware version info from the server.

        :return: (available: bool, version: str, url: str)
        """
        try:
            resp = requests.get(self.version_url, timeout=5)
            resp.raise_for_status()
            data = resp.json()
            latest = data.get('latest')
            url = data.get('url')
        except Exception:
            return False, None, None

        # Read current version if exists
        current_file = os.path.join(self.firmware_dir, 'current_version.txt')
        try:
            with open(current_file, 'r') as f:
                current = f.read().strip()
        except FileNotFoundError:
            current = None

        if latest and latest != current:
            return True, latest, url
        return False, current, None

    def download_firmware(self, version, url):
        """
        Downloads the firmware binary from the given URL and saves it locally.

        :param version: version string e.g. '1.1.0'
        :param url: direct URL to .bin file
        :return: filepath on success, None on failure
        """
        filename = f"firmware_v{version}.bin"
        filepath = os.path.join(self.firmware_dir, filename)
        try:
            r = requests.get(url, stream=True, timeout=10)
            r.raise_for_status()
            with open(filepath, 'wb') as f:
                for chunk in r.iter_content(chunk_size=4096):
                    if chunk:
                        f.write(chunk)
            # update current_version.txt
            current_file = os.path.join(self.firmware_dir, 'current_version.txt')
            with open(current_file, 'w') as vf:
                vf.write(version)
            return filepath
        except Exception:
            return None
