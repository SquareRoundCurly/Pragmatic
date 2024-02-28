import hashlib
import json
import os

def hash_file(filename):
	"""Compute the SHA-256 hash of a file's content."""
	sha256_hash = hashlib.sha256()
	with open(filename, "rb") as f:
		# Read and update hash string value in blocks of 4K
		for byte_block in iter(lambda: f.read(4096), b""):
			sha256_hash.update(byte_block)
	return sha256_hash.hexdigest()

def store_and_verify_hash(filename, json_filename="file_hashes.json"):
	if not os.path.exists(filename):
		return False
	
	new_hash = hash_file(filename)
	
	if os.path.exists(json_filename):
		with open(json_filename, "r") as json_file:
			try:
				data = json.load(json_file)
			except json.JSONDecodeError:
				data = {}
	else:
		data = {}
	
	if filename in data and data[filename] == new_hash:
		# The file exists, and the hash matches
		return True
	else:
		# Update the JSON file with the new hash
		data[filename] = new_hash
		with open(json_filename, "w") as json_file:
			json.dump(data, json_file, indent=4)
		return False