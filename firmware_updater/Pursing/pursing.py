# Specify the input and output file paths
input_file_path = r"d:\studying\GitHub\Graduation Project\firmware_updater\Pursing\test.txt"
output_file_path = r"d:\studying\GitHub\Graduation Project\firmware_updater\Binary.txt"

# Open the input file for reading
with open(input_file_path, 'r') as input_file:
    content = input_file.read()
    content_without_spaces_or_newlines = content.replace(' ', '').replace('\n', '')
    print(content_without_spaces_or_newlines)

# Open the output file for writing
with open(output_file_path, 'w') as output_file:
    output_file.write(content_without_spaces_or_newlines)

print("Spaces and newlines removed successfully.")
