import os
from pathlib import Path

src_path = Path('src')

for path, dir, files in os.walk(src_path):
    for file in files:

        with open(os.path.join(path, file), 'r') as f:
            lines = f.readlines()

            for i, line in enumerate(lines):
                if not line.startswith("#include <pckpp"):
                    continue
                
                include_path = os.path.join(Path(line[line.find('<pckpp/') + 7: line.find('>')]))

                lines[i] = (f'#include "{include_path}"\n')
        
        with open(os.path.join(path, file), 'w') as f:
            f.writelines(lines)