#!/usr/bin/env python3
"""
Upload a local firmware file to S3, then bump and update a JSON config.
Only the 'latest' (integer) and 'url' (string) fields are maintained in the JSON.
Edit the DEFINE_* constants below before running.
"""

import os
import json
import boto3
from botocore.exceptions import ClientError

# -------------------------------
# Configuration Constants (#define)
# -------------------------------
DEFINE_S3_BUCKET     = 'my-vehicle-firmware'    # S3 bucket name
DEFINE_REGION        = 'eu-north-1'              # AWS region for the bucket
DEFINE_JSON_KEY      = 'firmware/version.json'  # JSON config path inside the bucket
DEFINE_UPLOAD_FOLDER = 'firmware'               # Folder in the bucket to upload firmware files
DEFINE_AWS_PROFILE   = 'default'                # AWS CLI profile name

# Full path to the local firmware file on your machine:
DEFINE_LOCAL_FILE    = r'd:\studying\GitHub\Graduation Project\firmware_updater\Binary.txt'

# Initialize AWS session and S3 client
session = boto3.Session(profile_name=DEFINE_AWS_PROFILE, region_name=DEFINE_REGION)
s3 = session.client('s3')


def fetch_current_config(bucket: str, key: str) -> dict:
    """
    Download and parse the current JSON configuration from S3.
    Returns defaults {'latest': '0.0.0', 'url': ''} if the object does not exist.
    """
    try:
        response = s3.get_object(Bucket=bucket, Key=key)
        config_data = json.loads(response['Body'].read())
        latest = str(config_data.get('latest', '0.0.0'))
        url = str(config_data.get('url', ''))
        return {'latest': latest, 'url': url}
    except ClientError as e:
        if e.response['Error']['Code'] == 'NoSuchKey':
            return {'latest': '0.0.0', 'url': ''}
        else:
            raise


def upload_file_to_s3(local_path: str, bucket: str, folder: str) -> str:
    """
    Upload a local file to the specified folder in S3 and return its public URL.
    """
    filename = os.path.basename(local_path)
    s3_key = f"{folder}/{filename}"
    s3.upload_file(local_path, bucket, s3_key)
    return f"https://{bucket}.s3.{DEFINE_REGION}.amazonaws.com/{s3_key}"


def update_and_upload_config(bucket: str, key: str, new_latest: str, new_url: str):
    """
    Create and upload a JSON object with only 'latest' and 'url' keys.
    """
    payload = {'latest': new_latest, 'url': new_url}
    s3.put_object(
        Bucket=bucket,
        Key=key,
        Body=json.dumps(payload, indent=2),
        ContentType='application/json'
    )


def main():
    try:
        # 1. Retrieve current configuration
        config = fetch_current_config(DEFINE_S3_BUCKET, DEFINE_JSON_KEY)
        print(f"Current config -> latest: {config['latest']}, url: {config['url']}")

        # 2. Upload the local firmware file
        print(f"Uploading: {DEFINE_LOCAL_FILE}")
        new_url = upload_file_to_s3(DEFINE_LOCAL_FILE, DEFINE_S3_BUCKET, DEFINE_UPLOAD_FOLDER)
        print(f"Upload complete. New URL: {new_url}")

        # 3. Parse and increment semantic version MAJOR.MINOR.PATCH
        parts = config['latest'].split('.')
        major, minor, patch = (int(p) for p in (parts + ['0', '0', '0'])[:3])
        patch += 1
        new_latest = f"{major}.{minor}.{patch}"

        # 4. Update and upload the JSON config
        print(f"Updating JSON config at {DEFINE_JSON_KEY} to version {new_latest}")
        update_and_upload_config(DEFINE_S3_BUCKET, DEFINE_JSON_KEY, new_latest, new_url)

        print(f"Success: Config updated to latest={new_latest}")
    except Exception as error:
        print(f"Error during operation: {error}")


if __name__ == '__main__':
    main()
