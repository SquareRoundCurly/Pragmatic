import auto_graph
import os

def delete_file(file_path):
    if os.path.exists(file_path):
        os.remove(file_path)

def execute_command(command):
    import subprocess
    try:
        return subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True)
    
    except subprocess.CalledProcessError as e:
        return e
    
dir = 'tests/3_graph'

delete_file(f'{dir}/HelloWorld.exe')

t1 = auto_graph.Task(execute_command, f'clang {dir}/HelloWorld.cpp -o {dir}/HelloWorld.exe')
result = t1.exec()
print(result.stdout)

t2 = auto_graph.Task(execute_command, f'{dir}/HelloWorld.exe')
result = t2.exec()
print(result.stdout)