import os
import json
import boto3
from botocore.exceptions import ClientError

# ----------------------------
# File paths and AWS settings
# ----------------------------
INPUT_FILE_PATH = r"d:\\studying\\GitHub\\Graduation Project\\firmware_updater\\Application.txt"
OUTPUT_FILE_PATH = r"d:\\studying\\GitHub\\Graduation Project\\firmware_updater\\Binary.txt"

DEFINE_S3_BUCKET     = 'my-vehicle-firmware'    # S3 bucket name */
DEFINE_REGION        = 'eu-north-1'             # AWS region */
DEFINE_JSON_KEY      = 'firmware/version.json'  # Path for JSON config in S3 */
DEFINE_UPLOAD_FOLDER = 'firmware'               # Folder in S3 for firmware files */
DEFINE_AWS_PROFILE   = 'default'                # AWS CLI profile name */

DEFINE_LOCAL_FILE    = OUTPUT_FILE_PATH          # Local path to binary file */

# ----------------------------
# Initialize AWS session
# ----------------------------
session = boto3.Session(profile_name=DEFINE_AWS_PROFILE, region_name=DEFINE_REGION)
s3 = session.client('s3')

def remove_spaces_and_newlines(input_path: str, output_path: str) -> None:
    """
    Read input file, remove spaces and newline characters, write result to output file.
    """
    with open(input_path, 'r') as infile:
        content = infile.read()
    processed = content.replace(' ', '').replace('\n', '')
    with open(output_path, 'w') as outfile:
        outfile.write(processed)
    print("Spaces and newlines removed successfully.")


def fetch_current_config(bucket: str, key: str) -> dict:
    """
    Download current JSON configuration from S3.
    Return default if the object does not exist.
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
    Upload a local file to the specified S3 folder and return its public URL.
    """
    filename = os.path.basename(local_path)
    s3_key = f"{folder}/{filename}"
    s3.upload_file(local_path, bucket, s3_key)
    return f"https://{bucket}.s3.{DEFINE_REGION}.amazonaws.com/{s3_key}"


def update_and_upload_config(bucket: str, key: str, new_latest: str, new_url: str) -> None:
    """
    Create and upload JSON payload containing only 'latest' and 'url'.
    """
    payload = {'latest': new_latest, 'url': new_url}
    s3.put_object(
        Bucket=bucket,
        Key=key,
        Body=json.dumps(payload, indent=2),
        ContentType='application/json'
    )


def main():
    """
    Process input file, upload firmware, bump version, and update JSON config.
    """
    remove_spaces_and_newlines(INPUT_FILE_PATH, OUTPUT_FILE_PATH)

    try:
        config = fetch_current_config(DEFINE_S3_BUCKET, DEFINE_JSON_KEY)
        print(f"Current config -> latest: {config['latest']}, url: {config['url']}")

        print(f"Uploading: {DEFINE_LOCAL_FILE}")
        new_url = upload_file_to_s3(DEFINE_LOCAL_FILE, DEFINE_S3_BUCKET, DEFINE_UPLOAD_FOLDER)
        print(f"Upload complete. New URL: {new_url}")

        parts = config['latest'].split('.')
        major, minor, patch = (int(p) for p in (parts + ['0', '0', '0'])[:3])
        patch += 1
        new_latest = f"{major}.{minor}.{patch}"

        print(f"Updating JSON config at {DEFINE_JSON_KEY} to version {new_latest}")
        update_and_upload_config(DEFINE_S3_BUCKET, DEFINE_JSON_KEY, new_latest, new_url)

        print(f"Success: Config updated to latest={new_latest}")
    except Exception as error:
        print(f"Error during operation: {error}")

if __name__ == '__main__':
    main()
