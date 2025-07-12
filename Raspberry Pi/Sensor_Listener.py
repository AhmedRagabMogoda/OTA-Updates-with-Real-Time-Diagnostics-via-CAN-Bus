import requests
import datetime
import pytz
import timezonefinder

def get_weather():
    """Get weather data from wttr.in without API key"""
    try:
        url = f"https://wttr.in/?format=%t"
        response = requests.get(url, timeout=3)
        if response.status_code == 200:
            temp_str = response.text.strip()
            if temp_str.endswith('°C'):
                return float(temp_str.replace('°C', ''))
            elif temp_str.endswith('°F'):
                f_temp = float(temp_str.replace('°F', ''))
                return (f_temp - 32) * 5/9
            else:
                try:
                    return float(temp_str)
                except:
                    return 25.0
        return 25.0
    except Exception as e:
        print(f"Weather API error: {e}")
        return 25.0

def get_location():
    """Get approximate location based on public IP"""
    try:
        response = requests.get('https://ipinfo.io', timeout=2)
        data = response.json()
        loc = data.get('loc', '').split(',')
        city = data.get('city', 'Cairo')
        country = data.get('country', 'EG')
        
        if loc and len(loc) == 2:
            return float(loc[0]), float(loc[1]), f"{city}, {country}"
        return 30.0444, 31.2357, "Cairo, EG"
    except:
        return 30.0444, 31.2357, "Cairo, EG"

def get_time(lat, lon):
    """Get local time based on coordinates"""
    try:
        tf = timezonefinder.TimezoneFinder()
        timezone_str = tf.timezone_at(lat=lat, lng=lon)
        tz = pytz.timezone(timezone_str)
        now = datetime.datetime.now(tz)
        return now.strftime('%H:%M:%S'), now.strftime('%Y-%m-%d')
    except:
        now = datetime.datetime.now()
        return now.strftime('%H:%M:%S'), now.strftime('%Y-%m-%d')