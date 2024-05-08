import sys
import os
import re

bundle_file = f'{sys.argv[1]}/include/atomic/spirv.h'
writer = open(bundle_file, 'w')

try:
    writer.write("#ifndef ATOMIC_SPIRV_HEADER\n#define ATOMIC_SPIRV_HEADER\n\n")

    for file in sys.argv[2:]:
        reader = open(file, 'rb')

        try:
            reader.seek(0, os.SEEK_END)
            size = reader.tell()
            reader.seek(0, os.SEEK_SET)

            content = ''.join('\\x{:02x}'.format(x) for x in reader.read())
            
            file_name = file.replace(sys.argv[1] + '/build/', '').replace('.', '_').replace('/', '_')

            writer.write(f'static const unsigned *{file_name} = (const unsigned *) \"{content}\";\nstatic const unsigned {file_name}_length = {size};\n\n')
        finally:
            reader.close()

    writer.write("#endif")
finally:
    writer.close()
