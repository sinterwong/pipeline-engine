scp -P 22 -r build/aarch64/bin \
               build/aarch64/include \
               build/aarch64/lib \
               conf \
               tests/data/data \
               tests/data/models \
               root@192.168.0.188:/userdata/flowengine

scp -P 9202 -r build/aarch64/bin \
               build/aarch64/include \
               build/aarch64/lib \
               conf \
               tests/data/data \
               tests/data/models \
               root@114.242.23.39:/root/flowengine
