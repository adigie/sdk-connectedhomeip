clean:
  rm -rf out/nrf-native-sim-tests/nrfconnect/zephyr/zephyr.exe
  rm -rf out/nrf-native-sim-tests/nrfconnect/zephyr/zephyr.elf

build: clean
  ./scripts/build/build_examples.py --target nrf-native-sim-tests build

gen-compile-commands:
  jq -s add out/nrf-native-sim-tests/**/compile_commands.json > compile_commands.json
