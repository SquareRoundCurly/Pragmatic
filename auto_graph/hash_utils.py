import hashlib
import json
import os

def calculate_hash(file_path: str) -> str:
	"""Calculate SHA256 hash of the file."""
	sha256_hash = hashlib.sha256()
	
	with open(file_path, "rb") as f:
		for byte_block in iter(lambda: f.read(4096), b""):
			sha256_hash.update(byte_block)
	
	return sha256_hash.hexdigest()

def store_hash_in_json(file_path: str, json_file: str) -> None:
	"""Store filename and its hash in a JSON file. Create it if doesn't exist."""
	file_hash = calculate_hash(file_path)
	filename = os.path.basename(file_path)
	
	if os.path.exists(json_file):
		with open(json_file, 'r') as f:
			data = json.load(f)
	else:
		data = {}
	
	data[filename] = file_hash
	
	with open(json_file, 'w') as f:
		json.dump(data, f)

def verify_file_hash(file_path: str, json_file: str) -> bool:
	"""Verify the file's hash with the stored hash in the JSON file."""
	if not os.path.exists(json_file):
		return False
	
	filename = os.path.basename(file_path)
	
	with open(json_file, 'r') as f:
		data = json.load(f)
	
	if filename not in data:
		return False
	
	current_hash = calculate_hash(file_path)
	stored_hash = data[filename]
	
	return current_hash == stored_hash